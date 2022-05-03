#include "wxFTPGenericDirCtrl.h"

wxBEGIN_EVENT_TABLE(wxFTPGenericDirCtrl, wxTreeCtrl)
EVT_TREE_SEL_CHANGED(wxID_ANY, wxFTPGenericDirCtrl::ServerTreeItemClicked)
EVT_COMMAND_RIGHT_CLICK(wxID_ANY, wxFTPGenericDirCtrl::RightClickTree)
EVT_TREE_ITEM_RIGHT_CLICK(wxID_ANY, wxFTPGenericDirCtrl::RightClickElem)
wxEND_EVENT_TABLE()

wxFTPGenericDirCtrl::wxFTPGenericDirCtrl(wxFTPWrapper& wrapper, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) 
	: wxTreeCtrl(parent, id, pos, size, wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS), client(wrapper) {}

bool wxFTPGenericDirCtrl::RefreshServerTree() {
    DeleteAllItems();
    Refresh();
    wxImageList* serverTreeImageList = wxTheFileIconsTable->GetSmallImageList();
    questionImageID = serverTreeImageList->Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, wxSize(16, 16)));
    warningImageID = serverTreeImageList->Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, wxSize(16, 16)));
    errorImageID = serverTreeImageList->Add(wxArtProvider::GetIcon(wxART_ERROR, wxART_OTHER, wxSize(16, 16)));
    SetImageList(serverTreeImageList);
    wxTreeItemId root = AddRoot("root");
    try
    {
        client.PrintMessage("Extracting root directory...");
        auto PWDresponse = client.SendRequest("PWD");
        PWDresponse->message = boost::regex_replace(PWDresponse->message, boost::regex("^257 \"/(.*)\" .*"), "$1", boost::format_all);
        wxTreeItemId rootId = AppendItem(root, wxString::FromUTF8((boost::format("/ %d") % PWDresponse->message).str()), -1, -1, new wxServerTreeCtrlDirData(PWDresponse->message));

        if (RefreshServerTree(rootId)) {
            client.PrintMessage("Root directory extracted.");
            dynamic_cast<wxServerTreeCtrlDirData*>(GetItemData(rootId))->Deactivate();
            SetItemImage(rootId, wxFileIconsTable::drive);
        }
        else throw std::exception("Couldnt't extract list of contents from root dircetory.");
    }
    catch (std::exception& e)
    {
        client.PrintMessage(e.what(), Message::EC);
        return false;
    }

    return true;
}

bool wxFTPGenericDirCtrl::RefreshServerTree(const wxTreeItemId& rootItem) {
    DeleteChildren(rootItem);

    wxServerTreeCtrlDirData* itemData = dynamic_cast<wxServerTreeCtrlDirData*>(GetItemData(rootItem));
    
    std::string rootDirectory = itemData ? itemData->GetPath() : "";
    if (auto list = client.ExtractList(rootDirectory)) {
         while (!list->empty()) {
            std::string& sresponse = list->front();
            std::string fileName = boost::regex_replace(sresponse, boost::regex(".+; (?<file>.+)\r"), "$+{file}");
            std::string type = boost::regex_replace(sresponse, boost::regex(".*type\=(\\w+);.*"), "$1", boost::format_all);
            std::string path = rootDirectory + "/" + fileName;
            if (type == "file") {
                wxTreeItemId currentItem = AppendItem(rootItem, wxString::FromUTF8(fileName), -1, -1, new wxServerTreeCtrlFileData(path));
                wxServerTreeCtrlFileData* itemData = dynamic_cast<wxServerTreeCtrlFileData*>(GetItemData(currentItem));
                SetItemImage(currentItem, wxTheFileIconsTable->GetIconID(itemData->GetExtension()));
            }
            else if (type == "dir") {
                wxTreeItemId currentItem = AppendItem(rootItem, wxString::FromUTF8(fileName), -1, -1, new wxServerTreeCtrlDirData(path));
                SetItemImage(currentItem, questionImageID);
            }

            list->pop_front();
         }
         Expand(rootItem);
    }
    else {
        SetItemImage(rootItem, errorImageID);
        client.PrintMessage("Missing directory", Message::EC);
        return false;
    } 

    return true;
}

void wxFTPGenericDirCtrl::RightClickTree(wxCommandEvent& event) {
    void* data = static_cast<wxMenu*>(event.GetEventObject())->GetClientData();
    wxMenu mnu;
    mnu.SetClientData(data);
    mnu.Append(wxID_TREE_REFRESH, "Refresh");
    Bind(wxEVT_COMMAND_MENU_SELECTED, &wxFTPGenericDirCtrl::OnPopupClickRefreshTree, this, wxID_TREE_REFRESH);
    PopupMenu(&mnu);
}

void wxFTPGenericDirCtrl::ServerTreeItemClicked(wxTreeEvent& event) {
    try {
        wxTreeItemId selectedItem = event.GetItem();
        SelectItem(selectedItem);
        wxServerTreeCtrlDirData* itemData = dynamic_cast<wxServerTreeCtrlDirData*>(GetItemData(selectedItem));
        if (itemData) {
            std::string root;
            root = itemData->GetPath();
            if (root == "") root = "/";

            client.PrintMessage("Switching directory... ");
            if (client.SendRequest((boost::format("CWD %d") % root).str())->isFine) {
                client.SendRequest("PWD");
                if (!itemData->isExtracted()) {
                    if (RefreshServerTree(selectedItem)) {
                        itemData->Deactivate();
                        SetItemImage(selectedItem, wxFileIconsTable::folder);
                    }
                } 
            }
            else {
                client.PrintMessage("Cant't change directory.", Message::EC);
                DeleteChildren(selectedItem);
                SetItemImage(selectedItem, warningImageID);
            }
        } else event.Skip();
    }
    catch (std::bad_cast* e)
    {
        event.Skip();
    }
}

void wxFTPGenericDirCtrl::RightClickElem(wxTreeEvent& event) {
    try {
        wxTreeItemId selectedItem = event.GetItem();
        SelectItem(selectedItem);
        wxServerTreeCtrlItemData* itemData = dynamic_cast<wxServerTreeCtrlFileData*>(GetItemData(selectedItem));

        if (!itemData) {
            itemData = static_cast<wxServerTreeCtrlItemData*>(GetItemData(selectedItem));
            wxMenu mnu;
            mnu.SetClientData(itemData);
            mnu.Append(wxID_TREE_REFRESH_ITEM, "Refresh");
            Bind(wxEVT_COMMAND_MENU_SELECTED, &wxFTPGenericDirCtrl::OnPopupClickItem, this, wxID_TREE_REFRESH_ITEM);
            PopupMenu(&mnu);
        }
        else event.Skip();
    }
    catch (std::bad_cast* e)
    {
        event.Skip();
    }
}

void wxFTPGenericDirCtrl::OnPopupClickRefreshTree(wxCommandEvent& event) {
    RefreshServerTree();
}

void wxFTPGenericDirCtrl::OnPopupClickItem(wxCommandEvent& event) {
    wxTreeItemId selectedItem = GetSelection();
    RefreshServerTree(selectedItem);
}