#pragma once
#include "wxFTPWrapper.h"

#include <wx/wx.h>
#include <wx/dirctrl.h>
#include <wx/mimetype.h>
#include <wx/arrstr.h>
#include <wx/artprov.h>

#include "wxServerTreeCtrltemData.h"

class wxFTPGenericDirCtrl : public wxTreeCtrl 
{
public:
	wxFTPGenericDirCtrl(wxFTPWrapper& client, wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	bool RefreshServerTree();
private:
	wxFTPWrapper& client;

	bool RefreshServerTree(const wxTreeItemId& item);

	//events
	void RightClickTree(wxCommandEvent& event);

	void ServerTreeItemClicked(wxTreeEvent& event);
	void RightClickElem(wxTreeEvent& event);

	void OnPopupClickRefreshTree(wxCommandEvent& event);
	void OnPopupClickItem(wxCommandEvent& event);

	//iconsID
	long questionImageID;
	long warningImageID;
	long errorImageID;

	enum {
		wxID_TREE_REFRESH = wxID_HIGHEST + 1,
		wxID_TREE_REFRESH_ITEM
	};
	 
	wxDECLARE_EVENT_TABLE();
};