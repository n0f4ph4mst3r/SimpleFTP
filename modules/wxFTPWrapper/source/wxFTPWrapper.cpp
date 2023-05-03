#include "wxFTPWrapper.h"

wxDEFINE_EVENT(wxEVT_COMMAND_CONNECT_SOCKET, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_CLOSE_SOCKET, wxCommandEvent);
wxDEFINE_EVENT(wxTASK_ADDED, wxCommandEvent);
wxDEFINE_EVENT(wxTASK_UPDATED, wxThreadEvent);
wxDEFINE_EVENT(wxTASK_COMPLETED, wxThreadEvent);
wxDEFINE_EVENT(wxTASK_FAILED, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_TRANSFER_COMPLETED, wxCommandEvent);
wxDEFINE_EVENT(wxPRINT_MESSAGE, wxCommandEvent);

wxFTPWrapper::wxFTPWrapper() : IAsioClientFTP() {
    m_shutdown = false;
    Bind(wxEVT_COMMAND_CLOSE_SOCKET, &wxFTPWrapper::OnCloseSocket, this);
}

void wxFTPWrapper::PrintMessage(const std::string& msg, Message type) {
    if (msg != "") {
        wxCommandEvent evt = wxCommandEvent(wxPRINT_MESSAGE);
    #if defined(__WXMSW__)
        evt.SetClientData(new wxLogMessageItem(msg, type));
    #else
        evt.SetClientData(new wxLogMessageItem(wxString::FromUTF8(msg), type));
    #endif
        wxQueueEvent(this, evt.Clone());
    }
}

void wxFTPWrapper::UpdateTask(const int offset) {
    {
        wxCriticalSectionLocker lock(m_critsect);
        m_queuededTasks.front().progress += offset;
    }

    wxThreadEvent evt = wxThreadEvent(wxTASK_UPDATED);
    auto div = double(m_queuededTasks.front().progress) / double(m_queuededTasks.front().size) * 100;
    if (div > 100) div = 100;
    evt.SetExtraLong(div);
    wxQueueEvent(this, evt.Clone());
}

void wxFTPWrapper::TaskCompleted() {
    wxTransferTransaction task = m_queuededTasks.front();
    wxThreadEvent evt = wxThreadEvent(wxTASK_COMPLETED);
    evt.SetPayload(task);
    TaskPopFront();
    wxQueueEvent(this, evt.Clone());
}

void wxFTPWrapper::TaskFailed(const std::string& reason) {
    wxTransferTransaction task = m_queuededTasks.front();
    wxThreadEvent evt = wxThreadEvent(wxTASK_FAILED);
    evt.SetPayload(std::make_pair(task, reason));
    TaskPopFront();
    wxQueueEvent(this, evt.Clone());
}

bool wxFTPWrapper::Connect(const std::string& host, const std::string& user, const std::string& pass, const std::string& port) {
    if (IAsioClientFTP::Connect(host, user, pass, port)) {
        m_shutdown = false;
        wxQueueEvent(this, new wxThreadEvent(wxEVT_COMMAND_CONNECT_SOCKET));
        return true;
    }
    else return false;
}

void wxFTPWrapper::Shutdown() {
    wxQueueEvent(this, new wxThreadEvent(wxEVT_COMMAND_CLOSE_SOCKET));
}

void wxFTPWrapper::AddTask(const wxTransferTransaction& task) {
    {
        wxCriticalSectionLocker lock(m_critsect);
        m_queuededTasks.push_back(task);
    }
    wxMilliSleep(100);

    if (task.isFile) {
        PrintMessage((boost::format("File \"%d\" added to the queue.") % (task.bDirection ? task.local : task.remote)).str());
        wxCommandEvent* evt = new wxCommandEvent(wxTASK_ADDED);
        evt->SetClientData(new wxTransferTransaction(task));
        wxQueueEvent(this, evt);
    }
    else PrintMessage((boost::format("Directory \"%d\" added to the queue.") % (task.bDirection ? task.local : task.remote)).str());
}

std::list<wxTransferTransaction> wxFTPWrapper::GetTasks() {
    return m_queuededTasks;
}

void wxFTPWrapper::TaskPopFront() {
    wxCriticalSectionLocker lock(m_critsect);
    m_queuededTasks.pop_front();
}

wxThread::ExitCode wxFTPWrapper::Entry() {
    while (!GetThread()->TestDestroy() && !m_queuededTasks.empty()) {
        {
            wxCriticalSectionLocker lock(m_critsect);
            if (m_shutdown) return static_cast<wxThread::ExitCode>(NULL);
        }

        wxTransferTransaction task = m_queuededTasks.front();
        if (task.bDirection) {
            if (task.isFile) DownloadFile(task.local, task.remote);
            else {

                #if defined(__WXMSW__)
                    wxMkdir(task.local);
                #else
                    wxMkdir(wxString::FromUTF8(task.local));
                #endif
                TaskPopFront();
            }
        }
        else {
            if (task.isFile) LoadFile(task.remote, task.local);
            else {
                CreateDir(task.remote);
                TaskPopFront();
            }
        }
        wxMilliSleep(100);
    }

    wxQueueEvent(this, new wxThreadEvent(wxEVT_COMMAND_TRANSFER_COMPLETED));
    return static_cast<wxThread::ExitCode>(NULL);
}

void wxFTPWrapper::OnCloseSocket(wxCommandEvent& event) {
    {
        wxCriticalSectionLocker lock(m_critsect);
        m_shutdown = true;
    }
    if (GetThread() && GetThread()->IsRunning()) GetThread()->Wait();

    while (!m_queuededTasks.empty()) {
        if (m_queuededTasks.front().isFile) TaskFailed("Connection lost.");
        else TaskPopFront();
    } 

    if (m_commandSocket.is_open()) SendRequest("QUIT");
    IAsioClientFTP::Shutdown();
}




