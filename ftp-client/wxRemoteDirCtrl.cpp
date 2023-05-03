#include "wxRemoteDirCtrl.h"

wxBEGIN_EVENT_TABLE(wxRemoteDirCtrl, wxPanel)
EVT_TREE_SEL_CHANGED(wxID_ANY, wxRemoteDirCtrl::ItemSelected)
EVT_TREE_ITEM_RIGHT_CLICK(wxID_ANY, wxRemoteDirCtrl::ItemContext)
EVT_TREE_ITEM_MENU(wxID_ANY, wxRemoteDirCtrl::OnItemMenu)
EVT_COMMAND_RIGHT_CLICK(wxID_ANY, wxRemoteDirCtrl::RightClickTree)
EVT_TREE_BEGIN_LABEL_EDIT(wxID_ANY, wxRemoteDirCtrl::OnBeginEditItem)
EVT_LEFT_DOWN(wxRemoteDirCtrl::MouseEvents)
EVT_RIGHT_DOWN(wxRemoteDirCtrl::MouseEvents)
EVT_LEFT_DCLICK(wxRemoteDirCtrl::MouseEvents)
EVT_RIGHT_DCLICK(wxRemoteDirCtrl::MouseEvents)
EVT_KEY_DOWN(wxRemoteDirCtrl::KeyDown)
wxEND_EVENT_TABLE()

wxDEFINE_EVENT(wxEVT_TREE_EXTRACTION_THREAD_COMPLETED, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_TREE_DELETE_FILE, wxThreadEvent);

wxRemoteDirCtrl::wxRemoteDirCtrl(std::shared_ptr<wxFTPWrapper> pclient, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxPanel(parent, id, pos, size, style), m_owner(pclient) {
    m_tree = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT);
    wxBoxSizer* serverPanelSizer = new wxBoxSizer(wxVERTICAL);
    serverPanelSizer->SetMinSize(wxSize(350, 300));
    serverPanelSizer->Add(m_tree, 1, wxEXPAND | wxALL);
    SetSizer(serverPanelSizer);
    SetMinSize(wxSize(350, 300));

    m_questionImageID = 0;
    m_warningImageID = 0;
    m_errorImageID = 0;

    m_bTreeEnabled = true;
    m_shutdown = true;

    Bind(wxEVT_TREE_EXTRACTION_THREAD_COMPLETED, &wxRemoteDirCtrl::DoEndExtractionThread, this);
    Bind(wxEVT_TREE_DELETE_FILE, &wxRemoteDirCtrl::DoDeleteFile, this);

    m_owner->Bind(wxEVT_COMMAND_CONNECT_SOCKET, &wxRemoteDirCtrl::OnConnectSocket, this);
    m_owner->Bind(wxEVT_COMMAND_CLOSE_SOCKET, &wxRemoteDirCtrl::OnCloseSocket, this);
}

void wxRemoteDirCtrl::CreateContext(wxRemoteTreeCtrlItemData* pdata) {
    bool isFile = false;
    if (dynamic_cast<wxRemoteFileData*>(pdata)) isFile = true;
    wxMenu mnu;
    mnu.SetClientData(pdata);

    if (!isFile) {
        wxMenuItem* item_create = new wxMenuItem(&mnu, wxID_TREE_CREATE_DIR, _("Create directory"));
        item_create->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW_DIR, wxART_MENU, wxSize(16, 16)));
        mnu.Append(item_create);
        m_tree->Bind(wxEVT_COMMAND_MENU_SELECTED, &wxRemoteDirCtrl::OnPopupCreateDir, this, wxID_TREE_CREATE_DIR);
    }
    if (m_tree->GetItemParent(m_tree->GetFocusedItem()) != m_tree->GetRootItem()) {
        if (!isFile) mnu.AppendSeparator();
        wxMenuItem* item_rename = new wxMenuItem(&mnu, wxID_TREE_RENAME_ITEM, _("Rename"));
        item_rename->SetBitmap(wxArtProvider::GetBitmap(wxART_REDO, wxART_MENU, wxSize(16, 16)));
        mnu.Append(item_rename);
        Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {wxTreeItemId item = m_tree->GetFocusedItem();
        m_owner->SwitchDirectory(dynamic_cast<wxRemoteDirData*>(m_tree->GetItemData(m_tree->GetItemParent(item)))->GetPath());
        if (item.IsOk()) m_tree->EditLabel(item); }, wxID_TREE_RENAME_ITEM);

        wxMenuItem* item_delete = new wxMenuItem(&mnu, wxID_TREE_DELETE_ITEM, _("Delete"));
        item_delete->SetBitmap(wxArtProvider::GetBitmap(wxART_DELETE, wxART_MENU, wxSize(16, 16)));
        mnu.Append(item_delete);
        Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event)
        {m_delThread = new wxDeletionThread(this, m_tree->GetFocusedItem());
        m_tree->UnselectAll();
        if (m_delThread->Create() != wxTHREAD_NO_ERROR) return;
        wxCriticalSectionLocker enter(m_critsect);
        if (m_delThread->Run() != wxTHREAD_NO_ERROR) return; }, wxID_TREE_DELETE_ITEM);
    }

    if (!isFile) {
        mnu.AppendSeparator();
        wxMenuItem* item_refresh = new wxMenuItem(&mnu, wxID_TREE_REFRESH_ITEM, _("Refresh"));
        item_refresh->SetBitmap(wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_MENU, wxSize(16, 16)));
        mnu.Append(item_refresh);
        Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {wxTreeItemId item = m_tree->GetFocusedItem();
        RefreshTree(item); }, wxID_TREE_REFRESH_ITEM);
    }

    PopupMenu(&mnu);
}

void wxRemoteDirCtrl::OnConnectSocket(wxCommandEvent& event) {
    {
        wxCriticalSectionLocker lock(m_critsect);
        m_shutdown = false;
    }

    event.Skip();
}

void wxRemoteDirCtrl::OnCloseSocket(wxCommandEvent& event) {
    {
        wxCriticalSectionLocker lock(m_critsect);
        m_shutdown = true;
    }

    if (m_extThread && m_extThread->IsRunning()) m_extThread->Wait();
    if (m_delThread && m_delThread->IsRunning()) m_delThread->Wait();

    event.Skip();
}

void wxRemoteDirCtrl::ItemSelected(wxTreeEvent& event) {
    wxTreeItemId item = event.GetItem();
    try {
        wxRemoteDirData* pdata = dynamic_cast<wxRemoteDirData*>(m_tree->GetItemData(item));
        if (pdata) {
            if (!pdata->isExtracted()) RefreshTree(item);
            else if (!m_owner->SwitchDirectory(pdata->GetPath())) {
                m_tree->DeleteChildren(item);
                m_tree->SetItemImage(item, m_warningImageID);
                m_tree->SetItemHasChildren(item, false);
                return;
            }
        }
    }
    catch (std::bad_cast* e) {
        event.Skip();
    }
}

void wxRemoteDirCtrl::TreeEvents(wxTreeEvent& event) {
    if (WidgetBusy() || !m_bTreeEnabled) event.Veto();
}

void wxRemoteDirCtrl::ItemContext(wxTreeEvent& event) {
    if (m_bTreeEnabled && !WidgetBusy()) {
        wxTreeItemId item = event.GetItem();
        m_tree->SelectItem(item);
    }
    else event.Veto();
}

void wxRemoteDirCtrl::OnItemMenu(wxTreeEvent& event) {
    if (m_bTreeEnabled && !WidgetBusy()) {
        wxTreeItemId item = event.GetItem();
        void* pdata = reinterpret_cast<void*>(m_tree->GetItemData(item));

        if (m_extThread && m_extThread->IsRunning()) SetClientData(pdata);
        else CreateContext(static_cast<wxRemoteTreeCtrlItemData*>(pdata));
    }
    else event.Veto();
}

void wxRemoteDirCtrl::OnBeginEditItem(wxTreeEvent& event) {
    wxTreeItemId item = event.GetItem();
    std::string path = dynamic_cast<wxRemoteTreeCtrlItemData*>(m_tree->GetItemData(item))->GetPath();
    std::string name = path.substr(path.find_last_of('/') + 1);
    FTPresponse RNFRresponse = m_owner->SendRequest((boost::format("RNFR %d") % name).str());
    if (RNFRresponse.ncode != 350) m_tree->EndEditLabel(item, false);
    else m_tree->Bind(wxEVT_TREE_END_LABEL_EDIT, &wxRemoteDirCtrl::OnEndEditItem, this, wxID_ANY);
}

void wxRemoteDirCtrl::OnEndEditItem(wxTreeEvent& event) {
    wxTreeItemId item = event.GetItem();
    std::string newname = wxString(event.GetLabel().utf8_str()).ToStdString();
    FTPresponse RNTOresponse = m_owner->SendRequest((boost::format("RNTO %d") % newname).str());
    if (!RNTOresponse.IsFine) event.Veto();
    else {
        m_tree->SelectItem(item);
        std::string root = dynamic_cast<wxRemoteTreeCtrlItemData*>(m_tree->GetItemData(m_tree->GetItemParent(item)))->GetPath();
        char lastChar = root != wxEmptyString ? root[root.length() - 1] : root[0];
        std::string newpath = lastChar == '/' ? std::move(root) + newname : std::move(root) + '/' + newname;
        wxRemoteTreeCtrlItemData* pdata = dynamic_cast<wxRemoteTreeCtrlItemData*>(m_tree->GetItemData(item));
        pdata->SetPath(newpath);
        if (dynamic_cast<wxRemoteDirData*>(pdata)) RefreshTree(item);
    }
    m_tree->Unbind(wxEVT_TREE_END_LABEL_EDIT, &wxRemoteDirCtrl::OnEndEditItem, this, wxID_ANY);
}

void wxRemoteDirCtrl::RightClickTree(wxCommandEvent& event) {
    void* pdata = static_cast<wxMenu*>(event.GetEventObject())->GetClientData();
    wxMenu mnu;
    mnu.SetClientData(pdata);
    wxMenuItem* refreshItem = new wxMenuItem(&mnu, wxID_TREE_REFRESH, _("Refresh"));
    refreshItem->SetBitmap(wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_MENU, wxSize(16, 16)));
    mnu.Append(refreshItem);
    Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {RefreshTree(); }, wxID_TREE_REFRESH);
    PopupMenu(&mnu);
}

void wxRemoteDirCtrl::OnPopupCreateDir(wxCommandEvent& event) {
    wxDialog* popupWindow = new wxDialog(this, wxID_ANY, "Create directory");
    popupWindow->SetSize(250, 100);

    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(popupWindow->CreateTextSizer("Please enter the name of the directory which should be created:"), 0, wxEXPAND | wxALL, 5);

    wxBoxSizer* pathCtrlSizer = new wxBoxSizer(wxVERTICAL);
    wxTreeItemId rootId = m_tree->GetFocusedItem();
    wxRemoteDirData* pRootData = dynamic_cast<wxRemoteDirData*>(m_tree->GetItemData(rootId));

    auto defaultPath = [=, this]() {
        wxString root = pRootData->GetPath();
        wxChar lastChar = root != wxEmptyString ? root[root.length() - 1] : '0';
        wxString path = lastChar == '/' ? std::move(root) + "New directory" : std::move(root) + "/New directory";

        return path;
    };
    wxTextCtrl* pathCtrl = new wxTextCtrl(popupWindow, wxID_ANY, defaultPath(), wxDefaultPosition, wxSize(300, -1));
    pathCtrlSizer->Add(pathCtrl, 0, wxEXPAND | wxALL, 5);

    frameSizer->Add(pathCtrlSizer);
    frameSizer->Add(popupWindow->CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 5);
    popupWindow->SetSizerAndFit(frameSizer);

    wxString path;
    try {
        if (popupWindow->ShowModal() == wxID_OK) {
            path = pathCtrl->GetValue();
            if (path == "" || path == "/") throw path;
            if (!CreateDir(path.ToStdString(), rootId)) throw path;
        }
        else throw path;

        popupWindow->Destroy();
    }
    catch (wxString) {
        popupWindow->Destroy();
    }
    catch (std::system_error e) {
        wxLogSysError(e.what());
        popupWindow->Destroy();
    }
    catch (std::runtime_error e) {
        m_owner->PrintMessage(e.what(), Message::EC);
        popupWindow->Destroy();
    }
}

void wxRemoteDirCtrl::MouseEvents(wxMouseEvent& event) {
    if (m_bTreeEnabled && !WidgetBusy()) event.Skip();
}

void wxRemoteDirCtrl::KeyDown(wxKeyEvent& event) {
    if (m_bTreeEnabled && !WidgetBusy()) event.Skip();
}

void wxRemoteDirCtrl::DoEndExtractionThread(wxThreadEvent& event) {
    {
        wxTreeItemId rootItem = (event.GetPayload<std::pair<wxTreeItemId, std::optional<std::list<std::string>>>>()).first;
        wxRemoteDirData* pdata = dynamic_cast<wxRemoteDirData*>(m_tree->GetItemData(rootItem));
        pdata->Deactivate();
        std::string rootDir = pdata ? pdata->GetPath() : "";

        wxWindowUpdateLocker noUpdates(this);
        m_tree->DeleteChildren(rootItem);
        m_tree->SetItemHasChildren(rootItem);

        auto list = std::move((event.GetPayload<std::pair<wxTreeItemId, std::optional<std::list<std::string>>>>()).second);
        if (list != std::nullopt) {
            if (list->empty()) {
                m_tree->SetItemHasChildren(rootItem, false);
            }
            else {
                while (!list->empty()) {
                    std::string& sresponse = list->front();
                    std::string fileName = boost::regex_replace(sresponse, boost::regex(".+; (?<file>.+)\r"), "$+{file}");
                    std::string type = boost::regex_replace(sresponse, boost::regex(".*type\=(\\w+);.*"), "$1", boost::format_all);
                    std::string path = rootDir + "/" + fileName;
                    if (type == "file") {
                        std::stringstream sstream(boost::regex_replace(sresponse, boost::regex(".*size\=(\\w+);.*"), "$1"));
                        size_t size;
                        sstream >> size;

                        wxTreeItemId item = m_tree->AppendItem(rootItem, wxString::FromUTF8(fileName), -1, -1, new wxRemoteFileData(path, size));
                        wxRemoteFileData* itemData = dynamic_cast<wxRemoteFileData*>(m_tree->GetItemData(item));
                        m_tree->SetItemImage(item, wxTheFileIconsTable->GetIconID(itemData->GetExtension()));
                    }
                    else if (type == "dir") {
                        wxTreeItemId item = m_tree->AppendItem(rootItem, wxString::FromUTF8(fileName), -1, -1, new wxRemoteDirData(path));
                        m_tree->SetItemImage(item, m_questionImageID);
                        m_tree->SetItemHasChildren(item);
                    }

                    list->pop_front();
                }
                if (m_tree->GetChildrenCount(rootItem) == 0) m_tree->SetItemHasChildren(rootItem, false);
            }
        }
        else {
            if (m_tree->GetItemParent(rootItem) != m_tree->GetRootItem()) {
                m_tree->SetItemImage(rootItem, m_errorImageID);
                m_tree->SetItemHasChildren(rootItem, false);
                m_owner->PrintMessage("Missing directory", Message::EC);
            }
            else m_tree->DeleteAllItems();
        }

        m_tree->GetItemParent(rootItem) == m_tree->GetRootItem() ? m_tree->SetItemImage(rootItem, wxFileIconsTable::drive) : m_tree->SetItemImage(rootItem, wxFileIconsTable::folder);
        if (m_tree->IsSelected(rootItem) && m_tree->GetChildrenCount(rootItem) > 0) m_tree->Expand(rootItem);
    }

    if (IsFrozen()) Thaw();
    wxRemoteTreeCtrlItemData* pdata = static_cast<wxRemoteTreeCtrlItemData*>(GetClientData());
    if (pdata) {
        CreateContext(pdata);
        SetClientData(NULL);
    }
}

void wxRemoteDirCtrl::DoDeleteFile(wxThreadEvent& event) {
    wxTreeItemId item = event.GetPayload<wxTreeItemId>();
    wxTreeItemId parentitem = m_tree->GetItemParent(item);
    m_tree->Delete(item);
    if (m_tree->GetChildrenCount(parentitem) == 0) m_tree->SetItemHasChildren(parentitem, false);
}

wxTreeCtrl* wxRemoteDirCtrl::GetTreeCtrl() {
	return m_tree;
}

void wxRemoteDirCtrl::EnableTree() {
    m_bTreeEnabled = true;
}

void wxRemoteDirCtrl::DisableTree() {
    m_bTreeEnabled = false;
}

void wxRemoteDirCtrl::RefreshTree() {
    m_tree->DeleteAllItems();
    if (!IsFrozen()) Freeze();

    wxImageList* pImageList = wxTheFileIconsTable->GetSmallImageList();
    m_questionImageID = pImageList->Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, wxSize(16, 16)));
    m_warningImageID = pImageList->Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, wxSize(16, 16)));
    m_errorImageID = pImageList->Add(wxArtProvider::GetIcon(wxART_ERROR, wxART_OTHER, wxSize(16, 16)));
    m_tree->SetImageList(pImageList);
    wxTreeItemId root = m_tree->AddRoot("root");

    try {
        FTPresponse PWDresponse = m_owner->SendRequest("PWD");
        PWDresponse.message = boost::regex_replace(PWDresponse.message, boost::regex("^257 \"/(.*)\" .*"), "$1", boost::format_all);
        wxTreeItemId rootId = m_tree->AppendItem(root, wxString::FromUTF8((boost::format("/ %d") % PWDresponse.message).str()), -1, -1, new wxRemoteDirData(PWDresponse.message));
        m_tree->SelectItem(rootId);
    }
    catch (std::exception& e) {
        m_owner->PrintMessage(e.what(), Message::EC);
        m_tree->DeleteAllItems();
        return;
    }
}

void wxRemoteDirCtrl::RefreshTree(const wxTreeItemId& item) {
    if (m_tree->IsExpanded(item)) m_tree->Collapse(item);
    m_extThread = new wxListExtractionThread(this, item);
    if (m_extThread->Create() != wxTHREAD_NO_ERROR) return;
    wxCriticalSectionLocker enter(m_critsect);
    if (m_extThread->Run() != wxTHREAD_NO_ERROR) return;
}

bool wxRemoteDirCtrl::CreateDir(const std::string& path, const wxTreeItemId& rootId) {
    if (m_owner->CreateDir(path)) {
        unsigned index = path.find_last_of('/') + 1;
        wxTreeItemId item = m_tree->AppendItem(rootId, wxString::FromUTF8(path.substr(index)), -1, -1, new wxRemoteDirData(path));
        m_tree->SetItemImage(item, wxFileIconsTable::folder);
        wxRemoteDirData* pdata = dynamic_cast<wxRemoteDirData*>(m_tree->GetItemData(item));
        pdata->Deactivate();
        m_tree->SelectItem(item);
        return true;
    }
    else return false;
}

bool wxRemoteDirCtrl::WidgetBusy() {
    if (m_extThread && m_extThread->IsRunning()) return true;
    if (m_delThread && m_delThread->IsRunning()) return true;
    return false;
}

wxListExtractionThread* wxRemoteDirCtrl::GetExtractionThread() {
    return m_extThread;
}

wxListExtractionThread::wxListExtractionThread(wxRemoteDirCtrl* handler, const wxTreeItemId& item) : wxThread(wxTHREAD_JOINABLE) {
    m_handler = handler;
    m_item = item;
}

wxThread::ExitCode wxListExtractionThread::Entry() {
    wxRemoteDirData* pdata = dynamic_cast<wxRemoteDirData*>(m_handler->GetTreeCtrl()->GetItemData(m_item));
    std::string rootDir = pdata ? pdata->GetPath() : "";
    auto result = m_handler->m_owner->ExtractList(rootDir);

    {
        wxCriticalSectionLocker lock(m_handler->m_critsect);
        if (m_handler->m_shutdown) return static_cast<wxThread::ExitCode>(NULL);
    }

    wxThreadEvent evt = wxThreadEvent(wxEVT_TREE_EXTRACTION_THREAD_COMPLETED);
    evt.SetPayload(std::make_pair(m_item, result));
    wxQueueEvent(m_handler, evt.Clone());

    return static_cast<wxThread::ExitCode>(NULL);
}

wxListExtractionThread::~wxListExtractionThread() {
    wxCriticalSectionLocker enter(m_handler->m_critsect);
    m_handler->m_extThread = NULL;
}
wxDeletionThread::wxDeletionThread(wxRemoteDirCtrl* handler, const wxTreeItemId& item) {
    m_handler = handler;
    m_item = item;
}

wxThread::ExitCode wxDeletionThread::Entry() {
    wxTreeCtrl* tree = m_handler->GetTreeCtrl();
    std::function<void(const wxTreeItemId&)> deleteItem = [&](const wxTreeItemId& item) {
        wxRemoteTreeCtrlItemData* pdata = static_cast<wxRemoteTreeCtrlItemData*>(tree->GetItemData(item));
        std::string path = pdata->GetPath();
        if (dynamic_cast<wxRemoteFileData*>(pdata)) {
            {
                wxCriticalSectionLocker lock(m_handler->m_critsect);
                if (m_handler->m_shutdown) return;
            }

            FTPresponse DELEresponse = m_handler->m_owner->SendRequest((boost::format("DELE %d") % path).str());
            if (DELEresponse.IsFine) {
                wxThreadEvent evt = wxThreadEvent(wxEVT_TREE_DELETE_FILE);
                evt.SetPayload(item);
                wxQueueEvent(m_handler, evt.Clone());
            }
        }
        else {
            {
                wxCriticalSectionLocker lock(m_handler->m_critsect);
                if (m_handler->m_shutdown) return;
            }

            m_handler->RefreshTree(item);
            wxListExtractionThread* t = m_handler->GetExtractionThread();
            if (t && t->IsRunning() && !TestDestroy()) t->Wait();

            if (tree->GetItemImage(item) == m_handler->m_errorImageID) {
                wxThreadEvent evt = wxThreadEvent(wxEVT_TREE_DELETE_FILE);
                evt.SetPayload(item);
                wxQueueEvent(m_handler, evt.Clone());
            }
            else {
                if (tree->GetChildrenCount(item) > 0) {
                    std::deque<wxTreeItemId> nodes;
                    wxTreeItemIdValue cookie;
                    wxTreeItemId childId = tree->GetFirstChild(item, cookie);

                    while (childId.IsOk()) {
                        nodes.push_back(childId);
                        childId = tree->GetNextChild(item, cookie);
                    }

                    while (!nodes.empty() && !(TestDestroy())) {
                        {
                            wxCriticalSectionLocker lock(m_handler->m_critsect);
                            if (m_handler->m_shutdown) break;
                        }

                        wxTreeItemId node = nodes.front();
                        deleteItem(node);
                        nodes.pop_front();
                    }
                }

                FTPresponse RMDresponse = m_handler->m_owner->SendRequest((boost::format("RMD %d") % path).str());
                if (RMDresponse.IsFine) {
                    wxThreadEvent evt = wxThreadEvent(wxEVT_TREE_DELETE_FILE);
                    evt.SetPayload(item);
                    wxQueueEvent(m_handler, evt.Clone());
                }
            }
        }
    };

    deleteItem(m_item);
    return static_cast<wxThread::ExitCode>(NULL);
}

wxDeletionThread::~wxDeletionThread() {
    wxCriticalSectionLocker enter(m_handler->m_critsect);
    m_handler->m_delThread = NULL;
}