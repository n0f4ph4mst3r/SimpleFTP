#pragma once
#include "wxFTPWrapper.h"

#include <deque>
#include <sstream>

#include <wx/wx.h>
#include <wx/dirctrl.h>
#include <wx/mimetype.h>
#include <wx/arrstr.h>
#include <wx/artprov.h>
#include <wx/wupdlock.h>
#include <wx/imaglist.h>

#include "wxRemoteTreeCtrlItemData.h"

wxDECLARE_EVENT(wxEVT_TREE_EXTRACTION_THREAD_COMPLETED, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_TREE_DELETE_FILE, wxThreadEvent);

class wxRemoteDirCtrl;

class wxListExtractionThread : public wxThread
{
private:
	wxRemoteDirCtrl* m_handler;
	wxTreeItemId m_item;
public:
	wxListExtractionThread(wxRemoteDirCtrl* handler, const wxTreeItemId& item);
	virtual void* Entry() wxOVERRIDE;
	~wxListExtractionThread();
};

class wxDeletionThread : public wxThread
{
private:
	wxRemoteDirCtrl* m_handler;
	wxTreeItemId m_item;
public:
	wxDeletionThread(wxRemoteDirCtrl* handler, const wxTreeItemId& item);
	virtual void* Entry() wxOVERRIDE;
	~wxDeletionThread();
};

class wxRemoteDirCtrl : public wxPanel
{
private:
	std::shared_ptr<wxFTPWrapper> m_owner;
	bool m_shutdown;

	bool m_bTreeEnabled;
	wxTreeCtrl* m_tree;

	wxListExtractionThread* m_extThread;
	wxDeletionThread* m_delThread;
	wxCriticalSection m_critsect;

	void CreateContext(wxRemoteTreeCtrlItemData* pdata);

	//events
	void OnConnectSocket(wxCommandEvent& event);
	void OnCloseSocket(wxCommandEvent& event);

	void ItemSelected(wxTreeEvent& event);
	void OnBeginEditItem(wxTreeEvent& event);
	void OnEndEditItem(wxTreeEvent& event);
	void ItemContext(wxTreeEvent& event);
	void OnItemMenu(wxTreeEvent& event);
	
	void RightClickTree(wxCommandEvent& event);
	void OnPopupCreateDir(wxCommandEvent& event);

	void MouseEvents(wxMouseEvent& event);
	void KeyDown(wxKeyEvent& event);

	void DoEndExtractionThread(wxThreadEvent& event);
	void DoDeleteFile(wxThreadEvent& event);

	enum {
		wxID_TREE_REFRESH = wxID_HIGHEST + 1,
		wxID_TREE_REFRESH_ITEM,

		wxID_TREE_CREATE_DIR,

		wxID_TREE_RENAME_ITEM,
		wxID_TREE_DELETE_ITEM
	};

	//iconsID
	long m_questionImageID;
	long m_warningImageID;
	long m_errorImageID;

	friend class wxListExtractionThread;
	friend class wxDeletionThread;

	wxDECLARE_EVENT_TABLE();
public:
	wxRemoteDirCtrl(std::shared_ptr<wxFTPWrapper> pclient, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);

	wxTreeCtrl* GetTreeCtrl();
	void EnableTree();
	void DisableTree();
	void RefreshTree();
	void RefreshTree(const wxTreeItemId& item);

	bool CreateDir(const std::string& path, const wxTreeItemId& rootId);

	bool WidgetBusy();
	wxListExtractionThread* GetExtractionThread();
};