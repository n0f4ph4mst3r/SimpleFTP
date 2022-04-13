#pragma once
#include "AsioClientFTP.h"

#include <wx/wx.h>
#include <wx/event.h>
#include <wx/splitter.h>

#include "wxLogCtrl.h"
#include "wxServerTreeCtrltemData.h"
#include "wxFTPGenericDirCtrl.h"

#include <map>
#include <memory>

using boost::asio::ip::tcp;

class ClientApp : public wxFrame
{
 public:
    ClientApp(const wxString& title);
 private:
    std::map <int, wxString> accessData; //login, port and etc.

    std::unique_ptr<wxFTPWrapper> client; //work with FTP

    //widgets
    wxLogCtrl* logCtrl;
    wxFTPGenericDirCtrl* serverTree;
    wxGenericDirCtrl* clientTree;

    void Shutdown();

    //events
    void ConnectionClicked(wxCommandEvent& event);
    void DisconnectionClicked(wxCommandEvent& event);
    void AccessDataChanged(wxCommandEvent& event);

    enum {
        wxID_BUTTON_CONNECT = wxID_HIGHEST + 1,
        wxID_BUTTON_DISCONNECT,
        wxID_CONNECTION_HOST,
        wxID_CONNECTION_LOGIN,
        wxID_CONNECTION_PORT,
        wxID_CONNECTION_PASSWORD,
        wxID_FOOTER,
        wxID_LOGGER,
        wxID_TREE_SERVER,
        wxID_TREE_SERVER_ITEM
    };

    wxDECLARE_EVENT_TABLE();
};

