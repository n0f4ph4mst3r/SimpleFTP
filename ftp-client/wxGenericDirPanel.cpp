#include "wxGenericDirPanel.h"

wxBEGIN_EVENT_TABLE(wxGenericDirPanel, wxSplitterWindow)
EVT_SET_CURSOR(wxGenericDirPanel::SetCursor)
EVT_COMMAND(wxID_ANY, wxEVT_DIRPANEL_MAINTHREAD_COMPLETED, wxGenericDirPanel::DoEndThread)
wxEND_EVENT_TABLE()

wxDEFINE_EVENT(wxEVT_DIRPANEL_MAINTHREAD_COMPLETED, wxCommandEvent);

wxGenericDirPanel::wxGenericDirPanel(std::shared_ptr<wxFTPWrapper> pclient, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) : wxSplitterWindow(parent, id, pos, size, style, name), m_owner(pclient) {
    m_serverCtrl = new wxRemoteDirCtrl(pclient, this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME | wxTAB_TRAVERSAL);
    m_remoteDropTarget = new wxRemoteDropTarget(m_serverCtrl, m_clientCtrl);
    m_serverCtrl->GetTreeCtrl()->Bind(wxEVT_TREE_BEGIN_DRAG, &wxGenericDirPanel::RemoteTreeOnDragInit, this);
    m_serverCtrl->GetTreeCtrl()->SetDropTarget(m_remoteDropTarget);
    m_serverCtrl->GetTreeCtrl()->Enable(false);

    m_clientCtrl = new wxLocalDirCtrl(this, wxID_ANY, wxDirDialogDefaultFolderStr, wxDefaultPosition, wxSize(350, 300));
    m_localDropTarget = new wxLocalDropTarget(m_serverCtrl, m_clientCtrl);
    m_clientCtrl->GetTreeCtrl()->SetDropTarget(m_localDropTarget);
    m_clientCtrl->GetTreeCtrl()->Bind(wxEVT_TREE_BEGIN_DRAG, &wxGenericDirPanel::LocalTreeOnDragInit, this);

    SplitVertically(m_clientCtrl, m_serverCtrl);
    SetSashGravity(0.5);
    SetMinimumPaneSize(20);

    m_shutdown = true;

    m_owner->Bind(wxEVT_COMMAND_TRANSFER_COMPLETED, &wxGenericDirPanel::DoEndTransferThread, this);
    m_owner->Bind(wxEVT_COMMAND_CONNECT_SOCKET, &wxGenericDirPanel::OnConnectSocket, this);
    m_owner->Bind(wxEVT_COMMAND_CLOSE_SOCKET, &wxGenericDirPanel::OnCloseSocket, this);
}

void wxGenericDirPanel::SetCursor(wxSetCursorEvent& event) {
    if (m_serverCtrl->WidgetBusy()) {
        if (wxFindWindowAtPoint(wxGetMousePosition()) == m_serverCtrl->GetTreeCtrl()) {
            event.SetCursor(wxCURSOR_WAIT);
            return;
        }
    }

    if ((GetThread() && GetThread()->IsRunning())) {
        if (GetScreenRect().Contains(wxGetMousePosition())) {
            event.SetCursor(wxCURSOR_WAIT);
            return;
        }
    }

    if ((m_owner->GetThread() && m_owner->GetThread()->IsRunning())) {
        if (GetScreenRect().Contains(wxGetMousePosition())) {
            event.SetCursor(wxCURSOR_WAIT);
            return;
        }
    }

    event.Skip();
}

void wxGenericDirPanel::LocalTreeOnDragInit(wxTreeEvent& event) {
    wxTreeCtrl* clientTree = m_clientCtrl->GetTreeCtrl();
    wxTreeCtrl* serverCtrl = m_serverCtrl->GetTreeCtrl();

    serverCtrl->SetItemDropHighlight(m_serverCtrl->GetTreeCtrl()->GetFocusedItem(), false);
    serverCtrl->UnselectAll();

    wxTreeItemId item = event.GetItem();
    wxTreeItemId parent = clientTree->GetItemParent(item);
    m_remoteDropTarget->SetPath(m_clientCtrl->GetPath(item));

    if (parent == m_clientCtrl->GetRootId()) event.Veto();
    else {
        wxString fileName = clientTree->GetItemText(item);
        m_localDropTarget->Deactivate();
        m_remoteDropTarget->Activate();

        wxTextDataObject tdo(fileName);
        wxDropSource tds(tdo, clientTree);
        wxDragResult dragResult = tds.DoDragDrop(wxDrag_CopyOnly);

        if (dragResult != wxDragCopy) {
            clientTree->SetItemDropHighlight(clientTree->GetFocusedItem(), false);
            m_clientCtrl->UnselectAll();
            event.Skip();
        }
        else {
            if (!m_remoteDropTarget->GetTargetID()) return;

            m_bDirectionFlag = false;
            if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR) return;
            if (GetThread()->Run() != wxTHREAD_NO_ERROR) return;

            m_clientCtrl->DisableTree();
            m_serverCtrl->DisableTree();
        }
    }
}

void wxGenericDirPanel::RemoteTreeOnDragInit(wxTreeEvent& event) {
    wxTreeCtrl* clientTree = m_clientCtrl->GetTreeCtrl();
    wxTreeCtrl* serverCtrl = m_serverCtrl->GetTreeCtrl();

    clientTree->SetItemDropHighlight(m_clientCtrl->GetTreeCtrl()->GetFocusedItem(), false);
    clientTree->UnselectAll();

    wxTreeItemId item = event.GetItem();
    wxTreeItemId parent = m_serverCtrl->GetTreeCtrl()->GetItemParent(item);

    if (parent == serverCtrl->GetRootItem()) event.Veto();
    else {
        if (dynamic_cast<wxRemoteFileData*>(serverCtrl->GetItemData(item))) {
            wxRemoteDirData* parentData = dynamic_cast<wxRemoteDirData*>(serverCtrl->GetItemData(serverCtrl->GetItemParent(item)));
            std::string root = parentData->GetPath() == "" ? "/" : parentData->GetPath();
            m_owner->SwitchDirectory(root);
        }
        else m_owner->SwitchDirectory(dynamic_cast<wxRemoteDirData*>(serverCtrl->GetItemData(item))->GetPath());
        serverCtrl->SetFocusedItem(item);

        wxString fileName = serverCtrl->GetItemText(item);
        m_localDropTarget->Activate();
        m_remoteDropTarget->Deactivate();

        wxTextDataObject tdo(fileName);
        wxDropSource tds(tdo, serverCtrl);
        wxDragResult dragResult = tds.DoDragDrop(wxDrag_CopyOnly);

        if (dragResult != wxDragCopy) {
            clientTree->SetItemDropHighlight(m_clientCtrl->GetTreeCtrl()->GetFocusedItem(), false);
            m_clientCtrl->UnselectAll();
        }
        else {
            if (!m_localDropTarget->GetTargetID()) return;

            std::string source = dynamic_cast<wxRemoteTreeCtrlItemData*>(serverCtrl->GetItemData(m_localDropTarget->GetSourceID()))->GetPath();
            std::string target = m_clientCtrl->GetPath(m_localDropTarget->GetTargetID()).ToStdString();
            if (!wxEndsWithPathSeparator(target)) target += wxFILE_SEP_PATH;
            target += wxString::FromUTF8(wxString(source).AfterLast('/'));

            if (dynamic_cast<wxRemoteFileData*>(serverCtrl->GetItemData(m_localDropTarget->GetSourceID()))) {
                if (wxFileName::FileExists(target)) {
                    wxDialog* popupWindow = new wxDialog(m_clientCtrl, wxID_ANY, "File replacement");
                    popupWindow->SetSize(250, 100);
                    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
                    frameSizer->Add(popupWindow->CreateTextSizer("The file you are about to upload already exists. Replace?"), 0, wxEXPAND | wxALL, 5);
                    frameSizer->Add(popupWindow->CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 5);
                    popupWindow->SetSizerAndFit(frameSizer);
                    if (popupWindow->ShowModal() != wxID_OK) return;
                }
            }
            else {
                if (wxDir::Exists(target)) {
                    wxDialog* popupWindow = new wxDialog(m_clientCtrl, wxID_ANY, "Directory replacement");
                    popupWindow->SetSize(250, 100);
                    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
                    frameSizer->Add(popupWindow->CreateTextSizer("The directory you are about to upload already exists. Replace?"), 0, wxEXPAND | wxALL, 5);
                    frameSizer->Add(popupWindow->CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 5);
                    popupWindow->SetSizerAndFit(frameSizer);
                    if (popupWindow->ShowModal() != wxID_OK) return;
                    else wxFileName::Rmdir(target, wxPATH_RMDIR_RECURSIVE);
                }
            }

            m_bDirectionFlag = true;
            if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR) return;
            if (GetThread()->Run() != wxTHREAD_NO_ERROR) return;

            m_clientCtrl->DisableTree();
            m_serverCtrl->DisableTree();
        }
    }
}

void wxGenericDirPanel::OnConnectSocket(wxCommandEvent& event) {
    {
        wxCriticalSectionLocker lock(m_critsect);
        m_shutdown = false;
    }

    m_serverCtrl->RefreshTree();
    m_serverCtrl->GetTreeCtrl()->Enable();

    event.Skip();
}

void wxGenericDirPanel::OnCloseSocket(wxCommandEvent& event) {
    {
        wxCriticalSectionLocker lock(m_critsect);
        m_shutdown = true;
    }
    if (GetThread() && GetThread()->IsRunning()) GetThread()->Wait();

    m_serverCtrl->GetTreeCtrl()->Enable(false);
    m_serverCtrl->GetTreeCtrl()->DeleteAllItems();
    m_clientCtrl->EnableTree();

    event.Skip();
}

void wxGenericDirPanel::DoEndThread(wxCommandEvent&) {
    {
        wxCriticalSectionLocker lock(m_critsect);
        if (m_shutdown) return;
    }

    if (m_owner->CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR) return;
    if (m_owner->GetThread()->Run() != wxTHREAD_NO_ERROR) return;
}

void wxGenericDirPanel::DoEndTransferThread(wxCommandEvent&) {
    m_clientCtrl->EnableTree();
    m_serverCtrl->EnableTree();
}

wxThread::ExitCode wxGenericDirPanel::Entry() {
    if (m_bDirectionFlag) {
        m_serverCtrl->DisableTree();

        std::string source_root = dynamic_cast<wxRemoteTreeCtrlItemData*>(m_serverCtrl->GetTreeCtrl()->GetItemData(m_localDropTarget->GetSourceID()))->GetPath();
        std::string target_root = m_clientCtrl->GetPath(m_localDropTarget->GetTargetID()).ToStdString();
        if (!wxEndsWithPathSeparator(target_root)) target_root += wxFILE_SEP_PATH;
        target_root += wxString::FromUTF8(wxString(source_root).AfterLast('/'));

        std::function<void(const wxTreeItemId&)> f_createTask = [&](const wxTreeItemId& item) {
            auto pItemData = dynamic_cast<wxRemoteTreeCtrlItemData*>(m_serverCtrl->GetTreeCtrl()->GetItemData(item));
            std::string source = pItemData->GetPath();
            std::string target = source;
            target.erase(0, source_root.length());
            for (int i = 0; i < target.length(); ++i) {
                if (target[i] == '/') target[i] = wxFILE_SEP_PATH;
            }
            target = wxString::FromUTF8(target);
            target = target_root + target;

            if (wxRemoteFileData* pfiledata = dynamic_cast<wxRemoteFileData*>(pItemData)) {
                {
                    wxCriticalSectionLocker lock(m_critsect);
                    if (m_shutdown) return;
                }

                wxTransferTransaction task = wxTransferTransaction(target, source, true, pfiledata->GetSize());
                m_owner->AddTask(task);
                wxMilliSleep(500);
            }
            else if (wxRemoteDirData* pdirdata = dynamic_cast<wxRemoteDirData*>(pItemData)) {
                auto f_refreshDir = [&](const wxTreeItemId& item) {
                    m_serverCtrl->RefreshTree(item);
                    wxListExtractionThread* rthrd = m_serverCtrl->GetExtractionThread();
                    if (rthrd && rthrd->IsRunning() && !GetThread()->TestDestroy()) rthrd->Wait();

                    if (m_serverCtrl->GetTreeCtrl()->GetChildrenCount(item) > 0) {
                        wxTreeItemIdValue cookie;
                        wxTreeItemId childId = m_serverCtrl->GetTreeCtrl()->GetFirstChild(item, cookie);

                        while (childId.IsOk() && !GetThread()->TestDestroy()) {
                            {
                                wxCriticalSectionLocker lock(m_critsect);
                                if (m_shutdown) return;
                            }

                            f_createTask(childId);
                            childId = m_serverCtrl->GetTreeCtrl()->GetNextSibling(childId);
                        }
                    }
                };

                wxTransferTransaction task = wxTransferTransaction(target, source, false);
                m_owner->AddTask(task);
                wxMilliSleep(500);
                f_refreshDir(item);
            }
        };

        f_createTask(m_localDropTarget->GetSourceID());
    }
    else {
        wxListExtractionThread* ethread = m_serverCtrl->GetExtractionThread();
        if (ethread && ethread->IsRunning() && !GetThread()->TestDestroy()) ethread->Wait();

        std::string source_root = m_remoteDropTarget->GetPath().ToStdString();
        std::string target_root = dynamic_cast<wxRemoteTreeCtrlItemData*>(m_serverCtrl->GetTreeCtrl()->GetItemData(m_remoteDropTarget->GetTargetID()))->GetPath();
        if (target_root == "") target_root += '/';
        else if (target_root[target_root.length() - 1] != '/') target_root += '/';
        target_root += wxString((wxFileName(source_root).GetFullName()).utf8_str()).ToStdString();

        std::function<void(const wxString&)> f_createTask = [&](const wxString& path) {
            std::string target = path.ToStdString();
            target.erase(0, source_root.length());
            for (int i = 0; i < target.length(); ++i) {
                if (target[i] == wxFILE_SEP_PATH) target[i] = '/';
            }
            target = wxString(target).utf8_str();
            target = target_root + target;

            if (wxFileName::FileExists(path)) {
                {
                    wxCriticalSectionLocker lock(m_critsect);
                    if (m_shutdown) return;
                }

                wxString ssize;
                ssize << wxFileName::GetSize(path);

                std::stringstream sstream(ssize.ToStdString());
                size_t size;
                sstream >> size;

                wxTransferTransaction task = wxTransferTransaction(path.ToStdString(), target, true, size, false);
                m_owner->AddTask(task);
                wxMilliSleep(500);
            }
            else if (wxDir::Exists(path)) {
                wxDir dir(path);
                wxTransferTransaction task = wxTransferTransaction(path.ToStdString(), target, false, 0, false);
                m_owner->AddTask(task);
                wxMilliSleep(500);

                wxString file;
                if (dir.GetFirst(&file, "*", wxDIR_DEFAULT)) {
                    {
                        wxCriticalSectionLocker lock(m_critsect);
                        if (m_shutdown) return;
                    }

                    if (!wxEndsWithPathSeparator(path)) file = wxFILE_SEP_PATH + file;
                    file = path + file;
                    f_createTask(file);
                }

                while (dir.GetNext(&file)) {
                    {
                        wxCriticalSectionLocker lock(m_critsect);
                        if (m_shutdown) return;
                    }

                    if (!wxEndsWithPathSeparator(path)) file = wxFILE_SEP_PATH + file;
                    file = path + file;
                    f_createTask(file);
                }
            }
        };

        f_createTask(source_root);
    }

    {
        wxCriticalSectionLocker lock(m_critsect);
        if (!m_shutdown) wxQueueEvent(this, new wxThreadEvent(wxEVT_DIRPANEL_MAINTHREAD_COMPLETED));
    }

    return static_cast<wxThread::ExitCode>(NULL);
}


