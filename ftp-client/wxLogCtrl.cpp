#include "wxLogCtrl.h"

wxBEGIN_EVENT_TABLE(wxLogCtrl, wxListCtrl)
EVT_LIST_COL_BEGIN_DRAG(wxID_ANY, wxLogCtrl::ColumnBeginDragged)
EVT_LIST_COL_DRAGGING(wxID_ANY, wxLogCtrl::ColumnDragged)
EVT_LIST_COL_END_DRAG(wxID_ANY, wxLogCtrl::ColumnEndDragged)
EVT_LIST_INSERT_ITEM(wxID_ANY, wxLogCtrl::ItemInsert)
EVT_SIZE(wxLogCtrl::SizeChanged)
EVT_LIST_ITEM_RIGHT_CLICK(wxID_ANY, wxLogCtrl::RightClickElem)
wxEND_EVENT_TABLE()

wxLogCtrl::wxLogCtrl() : wxListCtrl() {};

wxLogCtrl::wxLogCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
	: wxListCtrl(parent, id, pos, size, wxLC_REPORT | wxLC_VIRTUAL | wxLC_SINGLE_SEL) {
    InitLog();
}

void wxLogCtrl::InitLog() {
    isFreezed = false;
    isDragged = false;

    InsertColumn(wxCOL_DATE, "Date");
    InsertColumn(wxCOL_TYPE, "Type");
    InsertColumn(wxCOL_MSG, "Message");


    PrintMessage("Welcome!");
}

wxString wxLogCtrl::OnGetItemText(long item, long column) const {
	wxCHECK_MSG(item >= 0 && item <= messageList.size(), "", "Invalid item index in wxLogCtrl::OnGetItemText");

	switch (column) {
		case wxCOL_DATE:
			return messageList[item].date;
		case wxCOL_TYPE:
			return messageList[item].stype;
		case wxCOL_MSG:
			return messageList[item].message;
		default:
			wxFAIL_MSG("Invalid column index in wxLogCtrl::OnGetItemText");
	}
	
	return "";
}

wxListItemAttr* wxLogCtrl::OnGetItemAttr(long item) const {
    wxCHECK_MSG(item >= 0 && item <= messageList.size(), nullptr, "Invalid item index in wxLogCtrl::OnGetItemText");

    wxListItemAttr* itemAtrr = new wxListItemAttr();
    itemAtrr->SetTextColour(messageList[item].colour);
    return itemAtrr;
}

void wxLogCtrl::PrintMessage(const wxString& msg, Message type) {
	if (msg != "") {
		wxLogMessageItem logMessage = wxLogMessageItem(msg, type);
		messageList.push_back(logMessage);
		SetItemCount(messageList.size());
		EnsureVisible(GetItemCount()-1);
	}
}


void wxLogCtrl::ColumnBeginDragged(wxListEvent& event) {
    if (event.GetColumn() == GetColumnCount() - 1 && wxGetMouseState().LeftIsDown()) {
        Freeze();
        isFreezed = true;
    }
}

void wxLogCtrl::ColumnDragged(wxListEvent& event) {
    if (event.GetColumn() >= 0 && event.GetColumn() < GetColumnCount() - 1)
    {
        SetColumnWidth(GetColumnCount() - 1, wxLIST_AUTOSIZE_USEHEADER);
    }
    else if (!isDragged) {
        isDragged = true;
        Bind(wxEVT_IDLE, &wxLogCtrl::ListIdle, this, wxID_ANY);
    }
}

void wxLogCtrl::ColumnEndDragged(wxListEvent& event) {
    SetColumnWidth(GetColumnCount() - 1, wxLIST_AUTOSIZE_USEHEADER);
    isDragged = false;
    if (isFreezed) {
        Thaw();
        isFreezed = false;
    }
}

void wxLogCtrl::ItemInsert(wxListEvent& event) {
    SetColumnWidth(GetColumnCount() - 1, wxLIST_AUTOSIZE_USEHEADER);
}

void wxLogCtrl::RightClickElem(wxListEvent& event) {
    void* data = reinterpret_cast<void*>(event.GetItem().GetData());
    wxMenu mnu;
    mnu.SetClientData(data);
    mnu.Append(wxID_LOGGER_CLEAR, "Clear");
    Bind(wxEVT_COMMAND_MENU_SELECTED, &wxLogCtrl::OnPopupClick, this, wxID_LOGGER_CLEAR);
    PopupMenu(&mnu);
}

void wxLogCtrl::OnPopupClick(wxCommandEvent& event) {
    void* data = static_cast<wxMenu*>(event.GetEventObject())->GetClientData();
    DeleteAllItems();
    messageList.clear();
}

void wxLogCtrl::SizeChanged(wxSizeEvent& event) {
    SetColumnWidth(GetColumnCount() - 1, wxLIST_AUTOSIZE_USEHEADER);
    if (!isDragged) {
        isDragged = true;
        Bind(wxEVT_IDLE, &wxLogCtrl::ListIdle, this, wxID_ANY);
    }
    event.Skip();
}

void wxLogCtrl::ListIdle(wxIdleEvent& event) {
    if (isDragged && !wxGetMouseState().LeftIsDown()) {
        SetColumnWidth(GetColumnCount() - 1, wxLIST_AUTOSIZE_USEHEADER);
        isDragged = false;
        Unbind(wxEVT_IDLE, &wxLogCtrl::ListIdle, this, wxID_ANY);
    }
}