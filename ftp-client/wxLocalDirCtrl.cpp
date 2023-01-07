#include "wxLocalDirCtrl.h"

wxBEGIN_EVENT_TABLE(wxLocalDirCtrl, wxGenericDirCtrl)
EVT_TREE_ITEM_MENU(wxID_ANY, wxLocalDirCtrl::OnPopupMenu)
EVT_TREE_ITEM_ACTIVATED(wxID_ANY, wxLocalDirCtrl::ItemActivated)
EVT_COMMAND_RIGHT_CLICK(wxID_ANY, wxLocalDirCtrl::RightClickTree)
EVT_TREE_DELETE_ITEM(wxID_ANY, wxLocalDirCtrl::ItemDelete)
wxEND_EVENT_TABLE()

wxLocalDirCtrl::wxLocalDirCtrl(wxWindow* parent, const wxWindowID id, const wxString& dir, const wxPoint& pos, const wxSize& size)
    : wxGenericDirCtrl(parent, id, dir, pos, size, wxDIRCTRL_3D_INTERNAL | wxDIRCTRL_EDIT_LABELS) {
    GetTreeCtrl()->Bind(wxEVT_LEFT_DOWN, &wxLocalDirCtrl::MouseEvents, this);
    GetTreeCtrl()->Bind(wxEVT_RIGHT_DOWN, &wxLocalDirCtrl::MouseEvents, this);
    GetTreeCtrl()->Bind(wxEVT_LEFT_DCLICK, &wxLocalDirCtrl::MouseEvents, this);
    GetTreeCtrl()->Bind(wxEVT_RIGHT_DCLICK, &wxLocalDirCtrl::MouseEvents, this);
    GetTreeCtrl()->Bind(wxEVT_KEY_DOWN, &wxLocalDirCtrl::KeyDown, this);
    m_bTreeEnabled = true;
}

std::optional<wxString> wxLocalDirCtrl::ShowDialogForCreateDir() {
    wxDialog* popupWindow = new wxDialog(this, wxID_ANY, "Create directory");
    popupWindow->SetSize(250, 100);

    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(popupWindow->CreateTextSizer("Please enter the name of the directory which should be created:"), 0, wxEXPAND | wxALL, 5);

    wxBoxSizer* pathCtrlSizer = new wxBoxSizer(wxVERTICAL);

    auto defaultPath = [=, this]() {
        wxString path = GetPath();
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
    wxTextCtrl* pathCtrl = new wxTextCtrl(popupWindow, wxID_ANY, defaultPath(), wxDefaultPosition, wxSize(300, -1));
    pathCtrlSizer->Add(pathCtrl, 0, wxEXPAND | wxALL, 5);

    frameSizer->Add(pathCtrlSizer);
    frameSizer->Add(popupWindow->CreateSeparatedButtonSizer(wxOK | wxCANCEL), 0, wxEXPAND | wxALL, 5);
    popupWindow->SetSizerAndFit(frameSizer);

    wxString path;
    wxTreeItemId rootId = GetTreeCtrl()->GetFocusedItem();
    try {
        if (popupWindow->ShowModal() == wxID_OK) {
            path = pathCtrl->GetValue();
            bool dirExists = wxDir::Exists(path);
            MakeDirectory(path, rootId, dirExists);
            SelectPath(path);
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

    wxMenuItem* openItem = new wxMenuItem(&mnu, wxID_OPEN_FILE_OR_DIR, _("Open"));
    openItem->SetBitmap(wxArtProvider::GetBitmap(wxART_GO_DIR_UP, wxART_MENU, wxSize(16, 16)));
    Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {wxLaunchDefaultApplication(GetPath()); }, wxID_OPEN_FILE_OR_DIR);
    mnu.Append(openItem);
    mnu.AppendSeparator();

    wxMenuItem* createItem = new wxMenuItem(&mnu, wxID_CREATE_DIR, _("Create directory"));
    createItem->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW_DIR, wxART_MENU, wxSize(16, 16)));
    mnu.Append(createItem);
    mnu.Append(wxID_CREATE_DIR_CREATE_N_ENTER, "Create directory and enter it");
    Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {ShowDialogForCreateDir(); }, wxID_CREATE_DIR);
    Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {auto path = ShowDialogForCreateDir();
    if (path) wxLaunchDefaultApplication(*path); }, wxID_CREATE_DIR_CREATE_N_ENTER);
    mnu.AppendSeparator();

    if (GetTreeCtrl()->GetItemParent(item) != GetRootId()) {
        wxMenuItem* renameItem = new wxMenuItem(&mnu, wxID_RENAME_FILE_OR_DIR, _("Rename"));
        renameItem->SetBitmap(wxArtProvider::GetBitmap(wxART_REDO, wxART_MENU, wxSize(16, 16)));
        mnu.Append(renameItem);
        Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
        if (item.IsOk()) GetTreeCtrl()->EditLabel(item); }, wxID_RENAME_FILE_OR_DIR);

        wxMenuItem* deleteItem = new wxMenuItem(&mnu, wxID_DELETE_FILE_OR_DIR, _("Delete"));
        deleteItem->SetBitmap(wxArtProvider::GetBitmap(wxART_DELETE, wxART_MENU, wxSize(16, 16)));
        mnu.Append(deleteItem);
        Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) { wxSetCursor(wxCURSOR_WAIT);
            wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
        wxString path = GetPath(item);
        if (wxFileName::Rmdir(path, wxPATH_RMDIR_RECURSIVE)) GetTreeCtrl()->Delete(item);
        wxSetCursor(wxCURSOR_ARROW); }, wxID_DELETE_FILE_OR_DIR);
        mnu.AppendSeparator();
    }
    wxMenuItem* refreshItem = new wxMenuItem(&mnu, wxID_TREE_REFRESH, _("Refresh"));
    refreshItem->SetBitmap(wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_MENU, wxSize(16, 16)));
    mnu.Append(refreshItem);
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
    wxMenuItem* refreshItem = new wxMenuItem(&mnu, wxID_TREE_REFRESH, _("Refresh"));
    refreshItem->SetBitmap(wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_MENU, wxSize(16, 16)));
    mnu.Append(refreshItem);
    Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {UnselectAll();  ReCreateTree(); }, wxID_TREE_REFRESH);
    PopupMenu(&mnu);
}

void wxLocalDirCtrl::ItemDelete(wxTreeEvent& event) {
    wxTreeItemId itemParent = GetTreeCtrl()->GetItemParent(event.GetItem());
    if (GetTreeCtrl()->GetChildrenCount(itemParent) <= 1)
        GetTreeCtrl()->SetItemHasChildren(itemParent, false);
}

void wxLocalDirCtrl::MakeDirectory(const wxString& path, const wxTreeItemId& rootId, bool bdirExists) {
    wxMkDir(path);

    if (wxDir::Exists(path)) {
        UnselectAll();

        wxFileName dir(path);
        if (!bdirExists) {
            wxDirItemData* dirData = new wxDirItemData(path, dir.GetName(), true);
            if (rootId.IsOk()) {
                wxTreeItemId item = AppendItem(rootId, dir.GetName(), wxFileIconsTable::folder, -1, dirData);
                GetTreeCtrl()->SetItemImage(item, wxFileIconsTable::folder_open, wxTreeItemIcon_Expanded);
                if (!GetTreeCtrl()->ItemHasChildren(rootId)) GetTreeCtrl()->SetItemHasChildren(rootId);
                GetTreeCtrl()->SetItemHasChildren(item);
            }
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
