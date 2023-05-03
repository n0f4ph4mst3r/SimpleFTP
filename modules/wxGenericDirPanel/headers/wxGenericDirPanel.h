#pragma once
#include <wx/splitter.h>

#include "wxLocalDropTarget.h"
#include "wxRemoteDropTarget.h"

wxDECLARE_EVENT(wxEVT_DIRPANEL_MAINTHREAD_COMPLETED, wxCommandEvent);

class wxGenericDirPanel : public wxSplitterWindow, public wxThreadHelper
{
private:
	std::shared_ptr<wxFTPWrapper> m_owner;

	wxCriticalSection m_critsect;
	bool m_shutdown;

	bool m_bDirectionFlag;
	wxLocalDirCtrl* m_clientCtrl;
	wxRemoteDirCtrl* m_serverCtrl;

	wxLocalDropTarget* m_localDropTarget;
	wxRemoteDropTarget* m_remoteDropTarget;

	//events
	void SetCursor(wxSetCursorEvent& event);

	void LocalTreeOnDragInit(wxTreeEvent& event);
	void RemoteTreeOnDragInit(wxTreeEvent& event);

	void OnConnectSocket(wxCommandEvent& event);
	void OnCloseSocket(wxCommandEvent& event);

	void DoEndThread(wxCommandEvent&);
	void DoEndTransferThread(wxCommandEvent&);

	virtual wxThread::ExitCode Entry();
	wxCriticalSection m_pThreadCS;

	wxDECLARE_EVENT_TABLE();
public:
	wxGenericDirPanel(std::shared_ptr<wxFTPWrapper> pclient, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSP_3D, const wxString& name = "dirbuffer");
};