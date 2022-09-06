#pragma once
#include "wxFTPWrapper.h"

#include <wx/wx.h>
#include <wx/event.h>
#include <wx/splitter.h>
#include <wx/thread.h>

#include <map>
#include <memory>

#include "wxRemoteDropTarget.h"
#include "wxTransferTransaction.h"

using boost::asio::ip::tcp;

class ClientApp : public wxFrame, public wxThreadHelper
{
private:
    std::map <int, wxString> m_accessData; //login, port and etc.
    std::unique_ptr<wxFTPWrapper> m_client; //work with FTP
    std::list<wxTransferTransaction> queuededTasks;

    //widgets
    wxTaskManager* m_taskManager;
    wxLocalDirCtrl* m_clientTree;
    wxRemoteTreeCtrl* m_serverTree;

    void Shutdown();
    void AddTask(const wxTransferTransaction& task);

    //events
    void ConnectionClicked(wxCommandEvent& event);
    void DisconnectionClicked(wxCommandEvent& event);
    void AccessDataChanged(wxCommandEvent& event);

    void RemoteTreeOnDragInit(wxTreeEvent& event);

    wxRemoteDropTarget* m_clientDropTarget;

    enum {
        wxID_BUTTON_CONNECT = wxID_HIGHEST + 1,
        wxID_BUTTON_DISCONNECT,

        wxID_CONNECTION_HOST,
        wxID_CONNECTION_LOGIN,
        wxID_CONNECTION_PORT,
        wxID_CONNECTION_PASSWORD,

        wxTASK_PANEL,
        wxTASK_MANAGER,

        wxID_TREE_CLIENT,
        wxID_TREE_SERVER,
        wxID_TREE_SERVER_ITEM
    };
protected:
    virtual wxThread::ExitCode Entry();
    wxCriticalSection m_pThreadCS;

    wxDECLARE_EVENT_TABLE();
public:
    ClientApp(const wxString& title);
};




 

