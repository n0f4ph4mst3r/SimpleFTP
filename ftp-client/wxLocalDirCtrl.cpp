#include "wxLocalDirCtrl.h"

wxBEGIN_EVENT_TABLE(wxLocalDirCtrl, wxGenericDirCtrl)
EVT_TREE_ITEM_MENU(wxID_ANY, wxLocalDirCtrl::OnPopupMenu)
EVT_TREE_ITEM_ACTIVATED(wxID_ANY, wxLocalDirCtrl::ItemActivated)
EVT_COMMAND_RIGHT_CLICK(wxID_ANY, wxLocalDirCtrl::RightClickTree)
EVT_TREE_DELETE_ITEM(wxID_ANY, wxLocalDirCtrl::ItemDelete)
wxEND_EVENT_TABLE()

wxLocalDirCtrl::wxLocalDirCtrl(wxWindow* parent, const wxWindowID id, const wxString& dir, const wxPoint& pos, const wxSize& size)
    : wxGenericDirCtrl(parent, id, dir, pos, size, wxDIRCTRL_3D_INTERNAL | wxDIRCTRL_EDIT_LABELS) {}

std::optional<wxString> wxLocalDirCtrl::ShowDialogForCreateDir() {
    wxDialog* popupWindow = new wxDialog(this, wxID_ANY, "Create directory");
    popupWindow->SetSize(250, 100);

    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(popupWindow->CreateTextSizer("Please enter the name of the directory which should be created:"), 0, wxEXPAND | wxALL, 5);

    wxBoxSizer* pathCtrlSizer = new wxBoxSizer(wxVERTICAL);

    auto newPath = [=, this]() {
        wxString root = GetPath();
        wxChar lastChar = root[root.Length() - 1];

        wxString path = lastChar == '\\' ? std::move(root) + "New directory" : std::move(root) + "\\New directory";
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
    wxTextCtrl* pathCtrl = new wxTextCtrl(popupWindow, wxID_ANY, newPath(), wxDefaultPosition, wxSize(300, -1));
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

    mnu.Append(wxID_OPEN_FILE_OR_DIR, "Open");
    Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {wxLaunchDefaultApplication(GetPath()); }, wxID_OPEN_FILE_OR_DIR);
    mnu.AppendSeparator();

    mnu.Append(wxID_CREATE_DIR, "Create directory");
    mnu.Append(wxID_CREATE_DIR_CREATE_N_ENTER, "Create directory and enter it");
    Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {ShowDialogForCreateDir(); }, wxID_CREATE_DIR);
    Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {std::optional<wxString> path = ShowDialogForCreateDir();
    if (path != std::nullopt) wxLaunchDefaultApplication(path.value()); }, wxID_CREATE_DIR_CREATE_N_ENTER);
    mnu.AppendSeparator();

    if (GetTreeCtrl()->GetItemParent(item) != GetRootId()) {
        mnu.Append(wxID_RENAME_FILE_OR_DIR, "Rename");
        Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) {wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
        if (item.IsOk()) GetTreeCtrl()->EditLabel(item); }, wxID_RENAME_FILE_OR_DIR);

        mnu.Append(wxID_DELETE_FILE_OR_DIR, "Delete");
        Bind(wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent& event) { wxSetCursor(wxCURSOR_WAIT);
            wxTreeItemId item = GetTreeCtrl()->GetFocusedItem();
        wxString path = GetPath(item);
        if (RemoveFile(path)) GetTreeCtrl()->Delete(item);
        wxSetCursor(wxCURSOR_ARROW); }, wxID_DELETE_FILE_OR_DIR);
        mnu.AppendSeparator();
    }
    mnu.Append(wxID_TREE_REFRESH, "Refresh");
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
    mnu.Append(wxID_TREE_REFRESH, "Refresh");
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

bool wxLocalDirCtrl::RemoveFile(const wxString& path) {
    if (wxFileName::FileExists(path)) return wxRemoveFile(path);
    else if (wxDir::Exists(path))
    {
        wxDir dir(path);
        if (!dir.HasFiles() && !dir.HasSubDirs())
        {
            dir.Close();
            return wxDir::Remove(path);
        }
        else
        {
            wxArrayString* files = new wxArrayString();
            wxDir::GetAllFiles(path, files, wxEmptyString, wxDIR_FILES | wxDIR_HIDDEN);
            for (size_t i = 0; i < files->GetCount(); ++i)
            {
                if (!wxRemoveFile(files->Item(i))) return false;
            }

            wxArrayString subDirs;
            wxString strFile;
            if (dir.GetFirst(&strFile, "*", wxDIR_HIDDEN | wxDIR_DIRS))
            {
                if (path[path.length() - 1] != '\\') strFile = '\\' + strFile;
                strFile = path + strFile;
                subDirs.Add(strFile);
            }

            while (dir.GetNext(&strFile))
            {
                if (path[path.length() - 1] != '\\') strFile = '\\' + strFile;
                strFile = path + strFile;
                subDirs.Add(strFile);
            }

            if (subDirs.GetCount() > 0)
            {
                for (int i = 0; i < subDirs.GetCount(); i++)
                {
                    if (!RemoveFile(subDirs.Item(i))) return false;
                }
            }

            dir.Close();
            return wxDir::Remove(path);
        }
    }

    return true;
}
