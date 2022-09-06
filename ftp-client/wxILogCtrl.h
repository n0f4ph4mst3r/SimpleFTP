#pragma once
#include "wx/wx.h"
#include "wx/listctrl.h"
#include "wx/vector.h"

class wxILogCtrl : public wxListCtrl
{
protected:
	//events
	void ColumnBeginDragged(wxListEvent& event);
	void ColumnDragged(wxListEvent& event);
	void ColumnEndDragged(wxListEvent& event);
	void ItemInsert(wxListEvent& event);
	void SizeChanged(wxSizeEvent& event);
	void RightClickElem(wxListEvent& event);
	virtual void OnPopupClick(wxCommandEvent& event);
	void ListIdle(wxIdleEvent& event);

	//eventFlags
	bool m_IsFreezed;
	bool m_IsDragged;

	enum {
		wxID_LOGGER_CLEAR = wxID_HIGHEST + 1
	};

	wxDECLARE_EVENT_TABLE();
public:
	wxILogCtrl();
	wxILogCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	virtual void InitLog() = 0;
};
