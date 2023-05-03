#pragma once
#include "wxRemoteDirCtrl.h"
#include "wxLocalDirCtrl.h"
#include <wx/dnd.h>

class wxTreeDropTarget : public wxTextDropTarget
{
protected:
	wxLocalDirCtrl* m_clientCtrl;
	wxRemoteDirCtrl* m_serverCtrl;
	bool isActive;

	wxTreeItemId m_targetItem;
public:
	wxTreeDropTarget(wxRemoteDirCtrl* serverCtrl, wxLocalDirCtrl* clientCtrl);
	virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& data) = 0;
	virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult defResult) = 0;
	bool OnDrop(wxCoord x, wxCoord y) override;
	virtual void OnLeave() override;

	void Activate();
	void Deactivate();

	wxTreeItemId GetTargetID();
};