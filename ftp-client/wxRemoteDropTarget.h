#pragma once
#include "wxRemoteTreeCtrl.h"
#include "wxLocalDirCtrl.h"
#include <wx/dnd.h>

class wxRemoteDropTarget : public wxTextDropTarget
{
private:
	wxLocalDirCtrl* m_clientCtrl;
	wxRemoteTreeCtrl* m_serverTree;

	wxTreeItemId m_targetItem;
	wxTreeItemId m_remoteItem;
public:
	wxRemoteDropTarget(wxRemoteTreeCtrl* serverTree, wxLocalDirCtrl* clientTree);
	virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data) override;
	virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult defResult) override;

	wxTreeItemId GetTargetID();
	wxTreeItemId GetSourceID();
};