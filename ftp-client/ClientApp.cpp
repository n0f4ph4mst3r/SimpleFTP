#include "ClientApp.h"

using boost::asio::ip::tcp;

wxBEGIN_EVENT_TABLE(ClientApp, wxFrame)
EVT_TEXT(wxID_CONNECTION_HOST, ClientApp::AccessDataChanged)
EVT_TEXT(wxID_CONNECTION_LOGIN, ClientApp::AccessDataChanged)
EVT_TEXT(wxID_CONNECTION_PORT, ClientApp::AccessDataChanged)
EVT_TEXT(wxID_CONNECTION_PASSWORD, ClientApp::AccessDataChanged)
EVT_TREE_BEGIN_DRAG(wxID_TREE_SERVER, ClientApp::RemoteTreeOnDragInit)
wxEND_EVENT_TABLE()

ClientApp::ClientApp(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize) {
    wxInitAllImageHandlers();

    m_accessData = { {wxID_CONNECTION_HOST, "localhost"},
                   {wxID_CONNECTION_LOGIN, wxEmptyString},
                   {wxID_CONNECTION_PORT, wxEmptyString},
                   {wxID_CONNECTION_PASSWORD, wxEmptyString}, };

    wxSplitterWindow* splittermain = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D);
    wxPanel* topPanel = new wxPanel(splittermain, wxID_ANY, wxDefaultPosition, wxSize(700, 400));
    wxPanel* headerPanel = new wxPanel(topPanel, wxID_ANY, wxDefaultPosition, wxSize(700, 100));
    wxSplitterWindow* splitterCenter = new wxSplitterWindow(topPanel, wxID_ANY, wxDefaultPosition, wxSize(700, 295), wxSP_3D);
    wxPanel* bottomPanel = new wxPanel(splittermain, wxTASK_PANEL, wxDefaultPosition, wxSize(700, 300));
    m_taskManager = new wxTaskManager(bottomPanel, wxTASK_MANAGER, wxDefaultPosition, wxSize(700, 300));

    m_client = std::make_unique<wxFTPWrapper>(m_taskManager);
    wxPanel* serverPanel = new wxPanel(splitterCenter, wxID_ANY, wxDefaultPosition, wxSize(300, 350), wxBORDER_THEME);
    m_serverTree = new wxRemoteTreeCtrl(*m_client, serverPanel, wxID_TREE_SERVER, wxDefaultPosition, wxSize(300, -1));
    wxBoxSizer* serverPanelSizer = new wxBoxSizer(wxVERTICAL);
    serverPanelSizer->Add(m_serverTree, 1, wxEXPAND | wxALL);
    serverPanel->SetSizer(serverPanelSizer);
    m_serverTree->Enable(false);
    //
    //  header
    //
    wxPanel* controlPanel = new wxPanel(headerPanel, wxID_ANY, wxDefaultPosition, wxSize(100, 100));
    wxGridSizer* controlPanelSizer = new wxGridSizer(2, 1, 0, 0);

    wxButton* connectionButton = new wxButton(controlPanel, wxID_BUTTON_CONNECT, wxT("Connect"));
    wxButton* disconectionButton = new wxButton(controlPanel, wxID_BUTTON_DISCONNECT, wxT("Disconnect"));

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::ConnectionClicked, this, wxID_BUTTON_CONNECT);

    controlPanelSizer->Add(connectionButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM | wxBOTTOM, 5);
    controlPanelSizer->Add(disconectionButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxTOP, 5);

    controlPanel->SetSizer(controlPanelSizer);
    //
    //  Login panel
    //
    wxPanel* accessPanelMain = new wxPanel(headerPanel, -1, wxDefaultPosition, wxSize(600, 100));
    wxFlexGridSizer* accessPanelMainSizer = new wxFlexGridSizer(2, 4, 0, 0);
    accessPanelMainSizer->Add(new wxStaticText(accessPanelMain, -1, wxT("Host")), 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 5);
    accessPanelMainSizer->Add(new wxStaticText(accessPanelMain, -1, wxT("Login")), 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 5);
    accessPanelMainSizer->Add(new wxStaticText(accessPanelMain, -1, wxT("Password")), 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 5);
    accessPanelMainSizer->Add(new wxStaticText(accessPanelMain, -1, wxT("Port")), 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 5);

    wxTextCtrl* hostCtrl = new wxTextCtrl(accessPanelMain, wxID_CONNECTION_HOST, wxT("localhost"), wxDefaultPosition, wxSize(125, 20));
    wxTextCtrl* userCtrl = new wxTextCtrl(accessPanelMain, wxID_CONNECTION_LOGIN, wxEmptyString, wxDefaultPosition, wxSize(125, 20));
    wxTextCtrl* portCtrl = new wxTextCtrl(accessPanelMain, wxID_CONNECTION_PORT, wxEmptyString, wxDefaultPosition, wxSize(125, 20));
    wxTextCtrl* passwordCtrl = new wxTextCtrl(accessPanelMain, wxID_CONNECTION_PASSWORD, wxEmptyString, wxDefaultPosition, wxSize(125, 20), wxTE_PASSWORD);

    accessPanelMainSizer->Add(hostCtrl, 0, wxALIGN_TOP | wxTOP | wxLEFT, 5);
    accessPanelMainSizer->Add(userCtrl, 0, wxALIGN_TOP | wxTOP | wxLEFT, 5);
    accessPanelMainSizer->Add(passwordCtrl, 0, wxALIGN_TOP | wxTOP | wxLEFT, 5);
    accessPanelMainSizer->Add(portCtrl, 0, wxALIGN_TOP | wxTOP | wxLEFT, 5);

    accessPanelMainSizer->AddGrowableRow(0, 1);
    accessPanelMainSizer->AddGrowableRow(1, 1);

    //Set sizers
    accessPanelMain->SetSizer(accessPanelMainSizer);

    wxBoxSizer* headerPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    headerPanelSizer->Add(accessPanelMain, 1, wxEXPAND | wxLEFT | wxRIGHT);
    headerPanelSizer->Add(controlPanel, 0, wxEXPAND);
    headerPanel->SetSizer(headerPanelSizer);
    //
    //  center
    //
    m_clientTree = new wxLocalDirCtrl(splitterCenter, wxID_ANY, wxDirDialogDefaultFolderStr, wxDefaultPosition, wxSize(300, 350));
    m_clientDropTarget = new wxRemoteDropTarget(m_serverTree, m_clientTree);
    m_clientTree->GetTreeCtrl()->SetDropTarget(m_clientDropTarget);

    //
    //  footer
    //
    wxBoxSizer* bottomPanelSizer = new wxBoxSizer(wxVERTICAL);
    bottomPanelSizer->Add(m_taskManager, 1, wxEXPAND | wxALL);
    bottomPanel->SetSizer(bottomPanelSizer);

    //Set sizers
    wxBoxSizer* topPanelSizer = new wxBoxSizer(wxVERTICAL);
    topPanelSizer->Add(headerPanel, 0, wxEXPAND | wxALL);
    topPanelSizer->Add(splitterCenter, 1, wxEXPAND | wxALL);
    topPanel->SetSizer(topPanelSizer);

    //Set sizers for main frame
    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(splittermain, 1, wxEXPAND | wxLEFT | wxRIGHT);

    splittermain->SplitHorizontally(topPanel, bottomPanel);
    splitterCenter->SplitVertically(m_clientTree, serverPanel);

    splittermain->SetMinimumPaneSize(300);
    splitterCenter->SetSashGravity(0.5);
    splitterCenter->SetMinimumPaneSize(350);

    this->SetSizerAndFit(frameSizer);

    Centre();
}

void ClientApp::ConnectionClicked(wxCommandEvent&) {
    Shutdown();
    Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::ConnectionClicked, this, wxID_BUTTON_CONNECT);
    if (m_accessData[wxID_CONNECTION_PORT] == "") m_accessData[wxID_CONNECTION_PORT] = "ftp";
    if (m_client->Connect(m_accessData[wxID_CONNECTION_HOST].ToStdString(), m_accessData[wxID_CONNECTION_LOGIN].ToStdString(), m_accessData[wxID_CONNECTION_PASSWORD].ToStdString(), m_accessData[wxID_CONNECTION_PORT].ToStdString())) {
        m_serverTree->RefreshTree();
        m_serverTree->Enable();

        Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::DisconnectionClicked, this, wxID_BUTTON_DISCONNECT);
        Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::ConnectionClicked, this, wxID_BUTTON_CONNECT);
    }
    else Shutdown();
}

void ClientApp::DisconnectionClicked(wxCommandEvent& event) {
    m_client->SendRequest("QUIT");
    Shutdown();
    m_client->PrintMessage("Disconneted.");
}

void ClientApp::AccessDataChanged(wxCommandEvent& event) {
    wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
    m_accessData[event.GetId()] = textCtrl->GetValue();
}

void ClientApp::RemoteTreeOnDragInit(wxTreeEvent& event) {
    m_clientTree->GetTreeCtrl()->SetItemDropHighlight(m_clientTree->GetTreeCtrl()->GetFocusedItem(), false);
    m_clientTree->UnselectAll();

    wxTreeItemId item = event.GetItem();
    wxTreeItemId parent = m_serverTree->GetItemParent(item);

    if (parent == m_serverTree->GetRootItem()) {
        event.Veto();
    }
    else {
        if (dynamic_cast<wxRemoteFileData*>(m_serverTree->GetItemData(item))) {
            wxRemoteDirData* parentData = dynamic_cast<wxRemoteDirData*>(m_serverTree->GetItemData(m_serverTree->GetItemParent(item)));
            std::string root = parentData->GetPath() == "" ? "/" : parentData->GetPath();
            m_client->SwitchDirectory(root);
        }
        else m_client->SwitchDirectory(dynamic_cast<wxRemoteDirData*>(m_serverTree->GetItemData(item))->GetPath());

        wxString fileName = m_serverTree->GetItemText(item);
        m_serverTree->SetFocusedItem(item);
        wxTextDataObject tdo(fileName);
        wxDropSource tds(tdo, m_serverTree);
        wxDragResult dragResult = tds.DoDragDrop(wxDrag_CopyOnly);
        if (dragResult != wxDragCopy) {
            m_clientTree->GetTreeCtrl()->SetItemDropHighlight(m_clientTree->GetTreeCtrl()->GetFocusedItem(), false);
            m_clientTree->UnselectAll();
        }
        else {
             if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
             {
                return;
             }

             if (GetThread()->Run() != wxTHREAD_NO_ERROR)
             {
                return;
             }
        }
    }
}


void ClientApp::Shutdown() {
    m_serverTree->Enable(false);
    m_serverTree->DeleteAllItems();

    m_client->Shutdown();
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::ConnectionClicked, this, wxID_BUTTON_CONNECT);
    Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::DisconnectionClicked, this, wxID_BUTTON_DISCONNECT);
}

void ClientApp::AddTask(const wxTransferTransaction& task) {
    wxMutexGuiLocker lockGui = wxMutexGuiLocker();
    queuededTasks.push_back(task);
    if (task.isFile) m_taskManager->AddTask(task.target, task.source, task.size);
}

wxThread::ExitCode ClientApp::Entry()
{
    wxBeginBusyCursor();
    std::string source_root = dynamic_cast<wxRemoteTreeCtrlItemData*>(m_serverTree->GetItemData(m_clientDropTarget->GetSourceID()))->GetPath();
    std::string target_root = m_clientTree->GetPath(m_clientDropTarget->GetTargetID()).ToStdString();
    if (!wxEndsWithPathSeparator(target_root))
        target_root += wxFILE_SEP_PATH;
    target_root += wxString::FromUTF8(wxString(source_root).AfterLast('/'));

    std::function<void(const wxTreeItemId&)> createTask;

    auto refreshDir = [&](const wxTreeItemId& item) {
        m_serverTree->RefreshTree(item);
        wxRefreshThread* t = m_serverTree->GetRefreshThread();
        if (t && t->IsRunning()) t->Wait();

        if (m_serverTree->GetChildrenCount(item) > 0) {
            wxTreeItemIdValue cookie;
            wxTreeItemId childId = m_serverTree->GetFirstChild(item, cookie);

            while (childId.IsOk() && !GetThread()->TestDestroy()) {
                createTask(childId);
                childId = m_serverTree->GetNextSibling(childId);
            }
        }
    };

    createTask = [&](const wxTreeItemId& item) {
        auto pitemdata = dynamic_cast<wxRemoteTreeCtrlItemData*>(m_serverTree->GetItemData(item));
        std::string source = pitemdata->GetPath();
        std::string target = source;
        target.erase(0, source_root.length());
        for (int i = 0; i < target.length(); ++i) {
            if (target[i] == '/') target[i] = wxFILE_SEP_PATH;
        }
        target = wxString::FromUTF8(target);
        target = target_root + target;

        if (wxRemoteFileData* pfiledata = dynamic_cast<wxRemoteFileData*>(pitemdata)) {
            wxTransferTransaction task = wxTransferTransaction(target, source, true, pfiledata->GetSize());
            AddTask(task);
        }
        else if (wxRemoteDirData* pdirdata = dynamic_cast<wxRemoteDirData*>(pitemdata)) {
            wxTransferTransaction task = wxTransferTransaction(target, source, false);
            AddTask(task);
            refreshDir(item);
        }
    };

    if (dynamic_cast<wxRemoteFileData*>(m_serverTree->GetItemData(m_clientDropTarget->GetSourceID()))) {
        bool fileExists = wxFileName::FileExists(target_root);
        if (fileExists) {
            wxDialog* popupWindow = new wxDialog(m_clientTree, wxID_ANY, "File replacement");
            popupWindow->SetSize(250, 100);
            wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
            frameSizer->Add(popupWindow->CreateTextSizer("The file you are about to upload already exists. Replace?"), 0, wxEXPAND | wxALL, 5);
            frameSizer->Add(popupWindow->CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 5);
            popupWindow->SetSizerAndFit(frameSizer);
            if (popupWindow->ShowModal() != wxID_OK) return static_cast<wxThread::ExitCode>(NULL);
            else fileExists = false;
        }

        if (fileExists) {
            queuededTasks.clear();
            return static_cast<wxThread::ExitCode>(NULL);
        }
    }
    else {
        bool dirExists = wxDir::Exists(target_root);
        if (dirExists) {
            wxDialog* popupWindow = new wxDialog(m_clientTree, wxID_ANY, "Directory replacement");
            popupWindow->SetSize(250, 100);
            wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
            frameSizer->Add(popupWindow->CreateTextSizer("The directory you are about to upload already exists. Replace?"), 0, wxEXPAND | wxALL, 5);
            frameSizer->Add(popupWindow->CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 5);
            popupWindow->SetSizerAndFit(frameSizer);
            if (popupWindow->ShowModal() != wxID_OK) return static_cast<wxThread::ExitCode>(NULL);
            else {
                if (!m_clientTree->RemoveFile(target_root)) return static_cast<wxThread::ExitCode>(NULL);
                dirExists = false;
            }
        }

        if (dirExists) {
            queuededTasks.clear();
            return static_cast<wxThread::ExitCode>(NULL);
        }
    }

    createTask(m_clientDropTarget->GetSourceID());

    while(!GetThread()->TestDestroy())
    {
        if (!queuededTasks.empty()) {
            wxTransferTransaction task = queuededTasks.front();
            if (task.isFile) {
                m_client->DownloadFile(task.target, task.source);
            }
            else wxMkDir(task.target);
            queuededTasks.pop_front();
        }
        else {
            wxEndBusyCursor();
            wxMutexGuiLocker lockGui = wxMutexGuiLocker();
            Refresh();
            return static_cast<wxThread::ExitCode>(NULL);
        } 
    }
    wxEndBusyCursor();

    return static_cast<wxThread::ExitCode>(NULL);    
}






