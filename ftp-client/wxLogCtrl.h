#pragma once
#include "wxILogCtrl.h"
#include "wxLogMessageItem.h"

class wxLogCtrl : public wxILogCtrl
{
private:
	wxVector<wxLogMessageItem> m_list;
protected:
	virtual void OnPopupClick(wxCommandEvent& event) override;
public:
	wxLogCtrl();
	wxLogCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	virtual wxString OnGetItemText(long item, long column) const override;
	virtual wxListItemAttr* OnGetItemAttr(long item) const override;

	void PrintMessage(const wxLogMessageItem& msg);

	enum Columns {
		wxCOL_DATE = 0,
		wxCOL_TYPE,
		wxCOL_MSG
	};

	void InitLog();
};