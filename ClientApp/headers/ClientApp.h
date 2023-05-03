#pragma once
#include "wxGenericDirPanel.h"
#include "wxTaskManager.h"
#include <wx/wx.h>
#include <wx/tooltip.h>

#include <map>
#include <memory>


using boost::asio::ip::tcp;

class ClientApp : public wxFrame
{
private:
    std::map<int, wxString> m_accessData; //login, port and etc.
    std::shared_ptr<wxFTPWrapper> m_client; //work with FTP

    //widgets
    wxTaskManager* m_taskManager;
    wxGenericDirPanel* m_dirCtrl;

    //events
    void ConnectionClicked(wxCommandEvent&);
    void DisconnectionClicked(wxCommandEvent&);
    void AccessDataChanged(wxCommandEvent& event);

    void DoEndTransferThread(wxCommandEvent& event);

    enum {
        wxID_SPLITTER_CENTER = wxID_HIGHEST + 1,

        wxID_BUTTON_CONNECT,
        wxID_BUTTON_DISCONNECT,

        wxID_CONNECTION_HOST,
        wxID_CONNECTION_LOGIN,
        wxID_CONNECTION_PORT,
        wxID_CONNECTION_PASSWORD
    };

    wxDECLARE_EVENT_TABLE();
public:
    ClientApp(const wxString& title);
};




 

