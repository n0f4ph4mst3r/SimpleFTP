#include "wxLogCtrl.h"

wxLogCtrl::wxLogCtrl() : wxILogCtrl() {};

wxLogCtrl::wxLogCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
	: wxILogCtrl(parent, id, pos, size) {
    InitLog();
}

void wxLogCtrl::OnPopupClick(wxCommandEvent& event) {
	wxILogCtrl::OnPopupClick(event);
	m_list.clear();
}

void wxLogCtrl::InitLog() {
    m_IsFreezed = false;
    m_IsDragged = false;

    InsertColumn(wxCOL_DATE, "Date");
    InsertColumn(wxCOL_TYPE, "Type");
    InsertColumn(wxCOL_MSG, "Message");

    PrintMessage(wxLogMessageItem("Welcome!"));
	wxILogCtrl::InitLog();
}

wxString wxLogCtrl::OnGetItemText(long item, long column) const {
	wxCHECK_MSG(item >= 0 && item <= m_list.size(), "", "Invalid item index in wxLogCtrl::OnGetItemText");

	switch (column) {
		case wxCOL_DATE:
			return m_list[item].m_date;
		case wxCOL_TYPE:
			return m_list[item].m_stype;
		case wxCOL_MSG:
			return m_list[item].m_message;
		default:
			wxFAIL_MSG("Invalid column index in wxLogCtrl::OnGetItemText");
	}
	
	return "";
}

wxListItemAttr* wxLogCtrl::OnGetItemAttr(long item) const {
    wxCHECK_MSG(item >= 0 && item <= m_list.size(), nullptr, "Invalid item index in wxLogCtrl::OnGetItemText");

    wxListItemAttr* plist = new wxListItemAttr();
    plist->SetTextColour(m_list[item].m_colour);
    return plist;
}

void wxLogCtrl::PrintMessage(const wxLogMessageItem& msg) {
	m_list.push_back(msg);
	SetItemCount(m_list.size());
	EnsureVisible(GetItemCount()-1);
}



