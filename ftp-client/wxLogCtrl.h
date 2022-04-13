#pragma once
#include "wx/wx.h"
#include "wx/listctrl.h"
#include "wx/vector.h"

#include "wxLogMessageItem.h"

class wxLogCtrl : public wxListCtrl
{
public:
	wxLogCtrl();
	wxLogCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	
	virtual wxString OnGetItemText(long item, long column) const override;
	virtual wxListItemAttr* OnGetItemAttr(long item) const override;

	void PrintMessage(const wxString& msg, Message type = Message::STATUS);

	enum Columns {
		wxCOL_DATE = 0,
		wxCOL_TYPE,
		wxCOL_MSG
	};

	void InitLog();
private:
	wxVector<wxLogMessageItem> messageList;

	//events
	void ColumnBeginDragged(wxListEvent& event);
	void ColumnDragged(wxListEvent& event);
	void ColumnEndDragged(wxListEvent& event);
	void ItemInsert(wxListEvent& event);
	void SizeChanged(wxSizeEvent& event);
	void RightClickElem(wxListEvent& event);
	void OnPopupClick(wxCommandEvent& event);
	void ListIdle(wxIdleEvent& event);

	//eventFlags
	bool isFreezed;
	bool isDragged;

	enum {
		wxID_LOGGER_CLEAR = wxID_HIGHEST + 1
	};

	wxDECLARE_EVENT_TABLE();
};