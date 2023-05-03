#pragma once
#include <wx/wx.h>
#include <wx/dirctrl.h>
#include <wx/dialog.h>
#include <wx/filefn.h> 
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/artprov.h>

#include <optional>

class wxLocalDirCtrl : public wxGenericDirCtrl {
private:
	bool m_bTreeEnabled;

	std::optional<wxString> ShowDialogForCreateDir();

	//events
	void OnPopupMenu(wxTreeEvent& event);
	void ItemActivated(wxTreeEvent& event);
	void RightClickTree(wxCommandEvent& event);
	void ItemDelete(wxTreeEvent& event);

	wxDECLARE_EVENT_TABLE();
public:
	wxLocalDirCtrl(wxWindow* parent, const wxWindowID id = wxID_ANY, const wxString& dir = wxDirDialogDefaultFolderStr, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	void MakeDirectory(const wxString& path, const wxTreeItemId& rootId);

	void EnableTree();
	void DisableTree();

	void MouseEvents(wxMouseEvent& event);
	void KeyDown(wxKeyEvent& event);

	enum {
		wxID_OPEN_FILE_OR_DIR = wxID_HIGHEST + 1,
		wxID_TREE_REFRESH,
		wxID_CREATE_DIR,
		wxID_CREATE_DIR_CREATE_N_ENTER,
		wxID_RENAME_FILE_OR_DIR,
		wxID_DELETE_FILE_OR_DIR,
	};
};
