#include "wxLocalDirCtrl.h"

wxBEGIN_EVENT_TABLE(wxLocalDirCtrl, wxGenericDirCtrl)
EVT_TREE_ITEM_MENU(wxID_ANY, wxLocalDirCtrl::OnPopupMenu)
EVT_TREE_ITEM_ACTIVATED(wxID_ANY, wxLocalDirCtrl::ItemActivated)
EVT_COMMAND_RIGHT_CLICK(wxID_ANY, wxLocalDirCtrl::RightClickTree)
wxEND_EVENT_TABLE()

wxLocalDirCtrl::wxLocalDirCtrl(wxWindow* parent, const wxWindowID id, const wxString& dir, const wxPoint& pos, const wxSize& size)
    : wxGenericDirCtrl(parent, id, dir, pos, size, wxDIRCTRL_3D_INTERNAL | wxDIRCTRL_EDIT_LABELS) {
    GetTreeCtrl()->Bind(wxEVT_LEFT_DOWN, &wxLocalDirCtrl::MouseEvents, this);
    GetTreeCtrl()->Bind(wxEVT_RIGHT_DOWN, &wxLocalDirCtrl::MouseEvents, this);
    GetTreeCtrl()->Bind(wxEVT_LEFT_DCLICK, &wxLocalDirCtrl::MouseEvents, this);
    GetTreeCtrl()->Bind(wxEVT_RIGHT_DCLICK, &wxLocalDirCtrl::MouseEvents, this);
    GetTreeCtrl()->Bind(wxEVT_KEY_DOWN, &wxLocalDirCtrl::KeyDown, this);
    #if defined(__WXMSW__)
        Bind(wxEVT_TREE_DELETE_ITEM, &wxLocalDirCtrl::ItemDelete, this);
    #endif
    m_bTreeEnabled = true;
}

std::optional<wxString> wxLocalDirCtrl::ShowDialogForCreateDir() {
    wxDialog* popupWindow = new wxDialog(this, wxID_ANY, "Create directory");
    popupWindow->SetSize(250, 100);

    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(popupWindow->CreateTextSizer("Please enter the name of the directory which should be created:"), 0, wxEXPAND | wxALL, 5);

    wxTreeItemId item = GetTreeCtrl()->GetFocusedItem(); 
    if (!wxDir::Exists(GetPath(item))) item = GetTreeCtrl()->GetItemParent(item);

    auto defaultPath = [=, this]() {
        wxString path = GetPath(item);
        if (!wxEndsWithPathSeparator(path)) path += wxFILE_SEP_PATH;
        path += "New directory";

        if (wxDir::Exists(path)) {
            wxString tmp = path;
            unsigned counter = 1;
            while (wxDir::Exists(tmp)) {
                tmp = path + std::to_string(counter);
                counter++;
            }
            path = std::move(tmp);
        }
        return path;
    };

    wxBoxSizer* pathCtrlSizer = new wxBoxSizer(wxVERTICAL);
    wxTextCtrl* pathCtrl = new wxTextCtrl(popupWindow, wxID_ANY, defaultPath(), wxDefaultPosition, wxSize(300, -1));
    pathCtrlSizer->Add(pathCtrl, 0, wxEXPAND | wxALL, 5);

    frameSizer->Add(pathCtrlSizer);
    frameSizer->Add(popupWindow->CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 5);
    popupWindow->SetSizerAndFit(frameSizer);

    wxString path;
    try {
        if (popupWindow->ShowModal() == wxID_OK) {
            path = pathCtrl->GetValue();
            if (!wxDir::Exists(path)) MakeDirectory(path, item);
            if (wxDir::Exists(path)) SelectPath(path);
        }
        else throw path;

        popupWindow->Destroy();
        return path;
    }
    catch (wxString) {
        popupWindow->Destroy();
        return std::nullopt;
    }
    catch (std::system_error e) {
        wxLogSysError(e.what());
        popupWindow->Destroy();
        return std::nullopt;
    }
    catch (std::runtime_error e) {
        wxLogError(e.what());
        popupWindow->Destroy();
        return std::nullopt;
    }
}

void wxLocalDirCtrl::OnPopupMenu(wxTreeEvent& event) {
    wxTreeItemId item = event.GetItem();
    GetTreeCtrl()->SelectItem(item);
    wxMenu mnu;
    mnu.SetClientData(GetTreeCtrl()->GetItemData(item));

    wxMenuItem* item_open = new wxMenuItem(&mnu, wxID_OPEN_FILE_OR_DIR, _("Open"));
    item_open->SetBitmap(wxArtProvider::GetBitmap(wxART_GO_DIR_UP, wxART_MENU, wxSize(16, 16)));
    Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {wxLaunchDefaultApplication(GetPath()); }, wxID_OPEN_FILE_OR_DIR);
    mnu.Append(item_open);
    mnu.AppendSeparator();

    wxMenuItem* item_create = new wxMenuItem(&mnu, wxID_CREATE_DIR, _("Create directory"));
    item_create->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW_DIR, wxART_MENU, wxSize(16, 16)));
    mnu.Append(item_create);
    mnu.Append(wxID_CREATE_DIR_CREATE_N_ENTER, "Create directory and enter it");
    Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {ShowDialogForCreateDir(); }, wxID_CREATE_DIR);
    Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {auto path = ShowDialogForCreateDir();
    if (path) wxLaunchDefaultApplication(*path); }, wxID_CREATE_DIR_CREATE_N_ENTER);
    mnu.AppendSeparator();

    if (GetTreeCtrl()->GetItemParent(item) != GetRootId()) {
        wxMenuItem* item_rename = new wxMenuItem(&mnu, wxID_RENAME_FILE_OR_DIR, _("Rename"));
        item_rename->SetBitmap(wxArtProvider::GetBitmap(wxART_REDO, wxART_MENU, wxSize(16, 16)));
        mnu.Append(item_rename);
        Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
        if (item.IsOk()) GetTreeCtrl()->EditLabel(item); }, wxID_RENAME_FILE_OR_DIR);

        wxMenuItem* item_delete = new wxMenuItem(&mnu, wxID_DELETE_FILE_OR_DIR, _("Delete"));
        item_delete->SetBitmap(wxArtProvider::GetBitmap(wxART_DELETE, wxART_MENU, wxSize(16, 16)));
        mnu.Append(item_delete);
        Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) { wxSetCursor(wxCURSOR_WAIT);
        wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
        wxString path = GetPath(item);

        if (wxDir::Exists(path)) wxFileName::Rmdir(path, wxPATH_RMDIR_RECURSIVE);
        if (wxFileName::FileExists(path)) wxRemoveFile(path);
        if (!wxDir::Exists(path) && !wxFileName::FileExists(path)) GetTreeCtrl()->Delete(item);
        wxSetCursor(wxCURSOR_ARROW); }, wxID_DELETE_FILE_OR_DIR);
        mnu.AppendSeparator();
    }
    wxMenuItem* item_refresh = new wxMenuItem(&mnu, wxID_TREE_REFRESH, _("Refresh"));
    item_refresh->SetBitmap(wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_MENU, wxSize(16, 16)));
    mnu.Append(item_refresh);
    Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {UnselectAll();  ReCreateTree(); }, wxID_TREE_REFRESH);

    if (!GetTreeCtrl()->IsSelected(item)) GetTreeCtrl()->SelectItem(item);
    PopupMenu(&mnu);
}

void wxLocalDirCtrl::ItemActivated(wxTreeEvent& event) {
    if (GetTreeCtrl()->GetItemParent(event.GetItem()) != GetRootId()) GetTreeCtrl()->EditLabel(event.GetItem());
    else event.Skip();
}

void wxLocalDirCtrl::RightClickTree(wxCommandEvent& event) {
    void* pdata = static_cast<wxMenu*>(event.GetEventObject())->GetClientData();
    wxMenu mnu;
    mnu.SetClientData(pdata);
    wxMenuItem* item_refresh = new wxMenuItem(&mnu, wxID_TREE_REFRESH, _("Refresh"));
    item_refresh->SetBitmap(wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_MENU, wxSize(16, 16)));
    mnu.Append(item_refresh);
    Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {UnselectAll();  ReCreateTree(); }, wxID_TREE_REFRESH);
    PopupMenu(&mnu);
}

void wxLocalDirCtrl::ItemDelete(wxTreeEvent& event) {
    wxTreeItemId itemParent = GetTreeCtrl()->GetItemParent(event.GetItem());
    if (GetTreeCtrl()->GetChildrenCount(itemParent) <= 1)
        GetTreeCtrl()->SetItemHasChildren(itemParent, false);
}

void wxLocalDirCtrl::MakeDirectory(const wxString& path, const wxTreeItemId& rootId) {
    wxMkdir(path);

    if (wxDir::Exists(path) && rootId.IsOk()) {
        if (!GetTreeCtrl()->ItemHasChildren(rootId)) GetTreeCtrl()->SetItemHasChildren(rootId);
        if (GetTreeCtrl()->IsExpanded(rootId)) {
            GetTreeCtrl()->Collapse(rootId);
            GetTreeCtrl()->Expand(rootId);
        }
        else if (GetTreeCtrl()->GetChildrenCount(rootId) <= 1) {
            wxTreeItemId item = AppendItem(rootId, wxEmptyString, -1, -1);
            GetTreeCtrl()->Expand(rootId);
            GetTreeCtrl()->Collapse(rootId);
            GetTreeCtrl()->Expand(rootId);
        }
    }
}

void wxLocalDirCtrl::EnableTree() {
    m_bTreeEnabled = true;
}

void wxLocalDirCtrl::DisableTree() {
    m_bTreeEnabled = false;
}

void wxLocalDirCtrl::MouseEvents(wxMouseEvent& event) {
    if (m_bTreeEnabled) event.Skip();
}

void wxLocalDirCtrl::KeyDown(wxKeyEvent& event) {
    if (m_bTreeEnabled) event.Skip();
}
