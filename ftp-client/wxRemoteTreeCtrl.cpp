#include "wxRemoteTreeCtrl.h"

wxBEGIN_EVENT_TABLE(wxRemoteTreeCtrl, wxTreeCtrl)
EVT_TREE_SEL_CHANGED(wxID_ANY, wxRemoteTreeCtrl::ServerTreeItemSelected)
EVT_COMMAND_RIGHT_CLICK(wxID_ANY, wxRemoteTreeCtrl::RightClickTree)
EVT_TREE_ITEM_RIGHT_CLICK(wxID_ANY, wxRemoteTreeCtrl::RightClickElem)
EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_REFRESHTHREAD_COMPLETED, wxRemoteTreeCtrl::DoEndRefreshThread)
EVT_ENTER_WINDOW(wxRemoteTreeCtrl::MouseEnter)
EVT_LEAVE_WINDOW(wxRemoteTreeCtrl::MouseLeave)
wxEND_EVENT_TABLE()

wxDEFINE_EVENT(wxEVT_COMMAND_REFRESHTHREAD_COMPLETED, wxCommandEvent);

wxRemoteTreeCtrl::wxRemoteTreeCtrl(wxFTPWrapper& wrapper, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) 
	: wxTreeCtrl(parent, id, pos, size, wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS), m_owner(wrapper) {
    m_busyFlag = false;
    m_questionImageID = 0;
    m_warningImageID = 0;
    m_errorImageID = 0;
}

void wxRemoteTreeCtrl::RightClickTree(wxCommandEvent& event) {
    void* pdata = static_cast<wxMenu*>(event.GetEventObject())->GetClientData();
    wxMenu mnu;
    mnu.SetClientData(pdata);
    mnu.Append(wxID_TREE_REFRESH, "Refresh");
    Bind(wxEVT_COMMAND_MENU_SELECTED, &wxRemoteTreeCtrl::OnPopupClickRefreshTree, this, wxID_TREE_REFRESH);
    PopupMenu(&mnu);
}

void wxRemoteTreeCtrl::ServerTreeItemSelected(wxTreeEvent& event) {
    wxTreeItemId selectedItem = event.GetItem();
    try 
    {
        wxRemoteDirData* pdata = dynamic_cast<wxRemoteDirData*>(GetItemData(selectedItem));
        if (pdata) {
            if (!pdata->isExtracted())
                RefreshTree(selectedItem);
            else if (!m_owner.SwitchDirectory(pdata->GetPath())) {
                DeleteChildren(selectedItem);
                SetItemImage(selectedItem, m_warningImageID);
                event.Skip();
                return;
            }
        } 
    }
    catch (std::bad_cast* e)
    {
        event.Skip();
    }
}

void wxRemoteTreeCtrl::RightClickElem(wxTreeEvent& event) {
    try
    {
        wxTreeItemId selectedItem = event.GetItem();
        SelectItem(selectedItem);
        wxRemoteTreeCtrlItemData* pdata = dynamic_cast<wxRemoteFileData*>(GetItemData(selectedItem));

        if (pdata) 
            event.Skip();
        else {
            pdata = static_cast<wxRemoteTreeCtrlItemData*>(GetItemData(selectedItem));
            wxMenu mnu;
            mnu.SetClientData(pdata);
            mnu.Append(wxID_TREE_REFRESH_ITEM, "Refresh");
            Bind(wxEVT_COMMAND_MENU_SELECTED, &wxRemoteTreeCtrl::OnPopupClickItem, this, wxID_TREE_REFRESH_ITEM);
            PopupMenu(&mnu);
        }
    }
    catch (std::bad_cast* e)
    {
        event.Skip();
    }
}

void wxRemoteTreeCtrl::TreeIdle(wxIdleEvent& event) {
    if (wxFindWindowAtPoint(wxGetMousePosition()) == this && m_busyFlag && !wxIsBusy) wxSetCursor(wxCURSOR_WAIT);
}

void wxRemoteTreeCtrl::OnPopupClickRefreshTree(wxCommandEvent& event) {
    RefreshTree();
}

void wxRemoteTreeCtrl::OnPopupClickItem(wxCommandEvent& event) {
    wxTreeItemId item = GetSelection();
    RefreshTree(item);
}

void wxRemoteTreeCtrl::DoEndRefreshThread(wxCommandEvent&) {
    Unbind(wxEVT_IDLE, &wxRemoteTreeCtrl::TreeIdle, this, wxID_ANY);
    if (IsFrozen()) Thaw();
    m_busyFlag = false;
    if (wxFindWindowAtPoint(wxGetMousePosition()) == this) wxSetCursor(wxCURSOR_ARROW);
}

void wxRemoteTreeCtrl::MouseEnter(wxMouseEvent&) {
    if (!wxIsBusy) {
        if (m_busyFlag) wxSetCursor(wxCursor(wxCURSOR_WAIT));
        else wxSetCursor(wxNullCursor);
    }
}

void wxRemoteTreeCtrl::MouseLeave(wxMouseEvent&) {
    if (!wxIsBusy) wxSetCursor(wxNullCursor);
}

void wxRemoteTreeCtrl::RefreshTree() {
    wxSetCursor(wxCursor(wxCURSOR_WAIT));
    m_busyFlag = true;
    Bind(wxEVT_IDLE, &wxRemoteTreeCtrl::TreeIdle, this, wxID_ANY);
    DeleteAllItems();
    Refresh();

    wxImageList* pImageList = wxTheFileIconsTable->GetSmallImageList();
    m_questionImageID = pImageList->Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, wxSize(16, 16)));
    m_warningImageID = pImageList->Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, wxSize(16, 16)));
    m_errorImageID = pImageList->Add(wxArtProvider::GetIcon(wxART_ERROR, wxART_OTHER, wxSize(16, 16)));
    SetImageList(pImageList);
    wxTreeItemId root = AddRoot("root");
    Freeze();

    try
    {
        FTPresponse PWDresponse = m_owner.SendRequest("PWD");
        PWDresponse.message = boost::regex_replace(PWDresponse.message, boost::regex("^257 \"/(.*)\" .*"), "$1", boost::format_all);
        wxTreeItemId rootId = AppendItem(root, wxString::FromUTF8((boost::format("/ %d") % PWDresponse.message).str()), -1, -1, new wxRemoteDirData(PWDresponse.message));
        SelectItem(rootId);
    }
    catch (std::exception& e)
    {
        m_owner.PrintMessage(e.what(), Message::EC);
        DeleteAllItems();
        return;
    }
}

void wxRemoteTreeCtrl::RefreshTree(const wxTreeItemId& rootItem) {
     wxSetCursor(wxCursor(wxCURSOR_WAIT));
     m_busyFlag = true;
     Bind(wxEVT_IDLE, &wxRemoteTreeCtrl::TreeIdle, this, wxID_ANY);

     m_refreshthread = new wxRefreshThread(this, rootItem);
     if (m_refreshthread->Create() != wxTHREAD_NO_ERROR) return;
     wxCriticalSectionLocker enter(m_critsect);
     if (m_refreshthread->Run() != wxTHREAD_NO_ERROR) return;
}

wxRefreshThread* wxRemoteTreeCtrl::GetRefreshThread() {
    return m_refreshthread;
}

wxRefreshThread::wxRefreshThread(wxRemoteTreeCtrl* tree, const wxTreeItemId& item) : wxThread(wxTHREAD_JOINABLE) {
    m_tree = tree;
    m_item = item;
}

wxThread::ExitCode wxRefreshThread::Entry() {
    wxRemoteDirData* pdata = dynamic_cast<wxRemoteDirData*>(m_tree->GetItemData(m_item));
    pdata->Deactivate();
    std::string rootDir = pdata ? pdata->GetPath() : "";

    std::optional<std::list<std::string>> result;
    bool listExtracted = false;
    std::thread extractionThread([&]() {
        result = m_tree->m_owner.ExtractList(rootDir);
        listExtracted = true;
    });
    extractionThread.detach();
    while (!TestDestroy()) {
        if (listExtracted) break;
    } 

    wxMutexGuiLocker lockGui = wxMutexGuiLocker();
    wxWindowUpdateLocker noUpdates(m_tree);
    m_tree->DeleteChildren(m_item);
    m_tree->SetItemHasChildren(m_item);
    if (result != std::nullopt) {
        auto list = std::move(result);
        if (list->empty()) {
            m_tree->SetItemHasChildren(m_item, false);
        }
        else {
            while (!list->empty() && !TestDestroy()) {
                std::string& sresponse = list->front();
                std::string fileName = boost::regex_replace(sresponse, boost::regex(".+; (?<file>.+)\r"), "$+{file}");
                std::string type = boost::regex_replace(sresponse, boost::regex(".*type\=(\\w+);.*"), "$1", boost::format_all);
                std::string path = rootDir + "/" + fileName;
                if (type == "file") {
                    std::stringstream sstream(boost::regex_replace(sresponse, boost::regex(".*size\=(\\w+);.*"), "$1"));
                    size_t size;
                    sstream >> size;

                    wxTreeItemId item = m_tree->AppendItem(m_item, wxString::FromUTF8(fileName), -1, -1, new wxRemoteFileData(path, size));
                    wxRemoteFileData* itemData = dynamic_cast<wxRemoteFileData*>(m_tree->GetItemData(item));
                    m_tree->SetItemImage(item, wxTheFileIconsTable->GetIconID(itemData->GetExtension()));
                }
                else if (type == "dir") {
                    wxTreeItemId item = m_tree->AppendItem(m_item, wxString::FromUTF8(fileName), -1, -1, new wxRemoteDirData(path));
                    m_tree->SetItemImage(item, m_tree->m_questionImageID);
                    m_tree->SetItemHasChildren(item);
                }

                list->pop_front();
            }
            if (m_tree->GetChildrenCount(m_item) == 0) m_tree->SetItemHasChildren(m_item, false);
        }
    }
    else {
        m_tree->SetItemImage(m_item, m_tree->m_errorImageID);
        m_tree->SetItemHasChildren(m_item, false);
        m_tree->m_owner.PrintMessage("Missing directory", Message::EC);
        wxQueueEvent(m_tree, new wxThreadEvent(wxEVT_COMMAND_REFRESHTHREAD_COMPLETED));
        return static_cast<wxThread::ExitCode>(NULL);
    }

    m_tree->GetItemParent(m_item) == m_tree->GetRootItem() ? m_tree->SetItemImage(m_item, wxFileIconsTable::drive) : m_tree->SetItemImage(m_item, wxFileIconsTable::folder);
    if (m_tree->IsSelected(m_item) && m_tree->GetChildrenCount(m_item) > 0) m_tree->Expand(m_item);
    wxQueueEvent(m_tree, new wxThreadEvent(wxEVT_COMMAND_REFRESHTHREAD_COMPLETED));
    return static_cast<wxThread::ExitCode>(NULL);
}