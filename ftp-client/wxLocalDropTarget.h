#pragma once
#include "wxTreeDropTarget.h"

class wxLocalDropTarget : public wxTreeDropTarget
{
private:
	wxTreeItemId m_sourceItem;
public:
	wxLocalDropTarget(wxRemoteDirCtrl* serverCtrl, wxLocalDirCtrl* clientCtrl);
	virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data) override;
	virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult defResult) override;

	wxTreeItemId GetSourceID();
};
