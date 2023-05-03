#pragma once
#include "wxTreeDropTarget.h"

class wxRemoteDropTarget : public wxTreeDropTarget
{
private:
	wxString m_path;
public:
	wxRemoteDropTarget(wxRemoteDirCtrl* serverCtrl, wxLocalDirCtrl* clientCtrl);
	virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data) override;
	virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult defResult) override;

	void SetPath(const wxString& path);
	wxString GetPath();
};