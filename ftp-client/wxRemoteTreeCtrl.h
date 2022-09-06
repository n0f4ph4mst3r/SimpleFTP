#pragma once
#include "wxFTPWrapper.h"

#include <wx/wx.h>
#include <wx/dirctrl.h>
#include <wx/mimetype.h>
#include <wx/arrstr.h>
#include <wx/artprov.h>
#include <wx/wupdlock.h>

#include "wxRemoteTreeCtrlItemData.h"

wxDECLARE_EVENT(wxEVT_COMMAND_REFRESHTHREAD_COMPLETED, wxCommandEvent);

class wxRemoteTreeCtrl;

class wxRefreshThread : public wxThread
{
private:
	wxRemoteTreeCtrl* m_tree;
	wxTreeItemId m_item;
public:
	wxRefreshThread(wxRemoteTreeCtrl* tree, const wxTreeItemId& item);

	virtual void* Entry() wxOVERRIDE;
};

class wxRemoteTreeCtrl : public wxTreeCtrl 
{
private:
	wxFTPWrapper& m_owner;
	bool m_busyFlag;

	//events
	void RightClickTree(wxCommandEvent& event);
	void ServerTreeItemSelected(wxTreeEvent& event);
	void RightClickElem(wxTreeEvent& event);

	void TreeIdle(wxIdleEvent& event);

	void OnPopupClickRefreshTree(wxCommandEvent& event);
	void OnPopupClickItem(wxCommandEvent& event);

	void DoEndRefreshThread(wxCommandEvent&);

	void MouseEnter(wxMouseEvent&);
	void MouseLeave(wxMouseEvent&);

	enum {
		wxID_TREE_REFRESH = wxID_HIGHEST + 1,
		wxID_TREE_REFRESH_ITEM
	};

	//iconsID
	long m_questionImageID;
	long m_warningImageID;
	long m_errorImageID;
	 
	wxDECLARE_EVENT_TABLE();
protected:
	wxRefreshThread* m_refreshthread;
	wxCriticalSection m_critsect;

	friend class wxRefreshThread;
public:
	wxRemoteTreeCtrl(wxFTPWrapper& client, wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	void RefreshTree();
	void RefreshTree(const wxTreeItemId& item);

	wxRefreshThread* GetRefreshThread();
};

