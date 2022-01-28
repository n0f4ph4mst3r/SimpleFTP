#pragma once
#include <wx/wx.h>
#include <wx/dirctrl.h>
#include <wx/protocol/ftp.h>
#include <wx/event.h>
#include <map>

class ClientApp : public wxFrame
{
public:
    ClientApp(const wxString& title);
    wxDECLARE_EVENT_TABLE();
private:
    wxFTP* ftpClient = new wxFTP();
    //events ID
    enum { 
           ID_CONNECT_SERV = wxID_HIGHEST + 1,
           ID_DISCONNECT_SERV,
           ID_HOST_DATA,
           ID_LOGIN_DATA,
           ID_PASSWORD_DATA,
           ID_PORT_DATA
         };

    std::map <wxString, wxString> accessData = { {"host", "localhost"},
                                                 {"login", "admin"},
                                                 {"password", "test"},
                                                 {"port", "21"}};

    //events
    void conButtonClicked(wxCommandEvent& event);
    void dicsButtonClicked(wxCommandEvent& event);
 };
