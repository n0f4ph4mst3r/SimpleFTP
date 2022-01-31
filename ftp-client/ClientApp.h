#pragma once
#include <wx/wx.h>
#include <wx/dirctrl.h>
#include <wx/protocol/ftp.h>
#include <wx/event.h>
#include <wx/textctrl.h>
#include <map>

class ClientApp : public wxFrame
{
public:
    ClientApp(const wxString& title);
    wxDECLARE_EVENT_TABLE();
private:
    wxFTP* ftpClient;

    //login, port and etc.
    std::map <int, wxString> accessData;


    /*
    *
    *
    *
    * ----------------------------header-------------------------------------
    *
    *
    *
    */
    //panels
    wxPanel* headerPanel;
    wxPanel* controlPanel;
    wxPanel* accessPanelMain;

    //sizers
    wxGridSizer* controlPanelSizer;
    wxFlexGridSizer* accessPanelMainSizer;
    wxBoxSizer* headerPanelSizer;

    //buttons
    wxButton* connectionButton;
    wxButton* disconectionButton;

    //TextCtrl
    wxTextCtrl* hostCtrl;
    wxTextCtrl* userCtrl;
    wxTextCtrl* passwordCtrl;
    wxTextCtrl* portCtrl;
    /*
    *
    *
    *
    * ----------------------------center----------------------------------
    *
    *
    *
    */
    //panels
    wxPanel* centerPanel;
    wxPanel* switchPanel;

    //sizers
    wxFlexGridSizer* centerSizer;
    wxGridSizer* switchSizer;

    //GenericDir
    wxGenericDirCtrl* clientDirs;
    wxGenericDirCtrl* serverDirs;

    //buttons
    wxButton* rightshift;
    wxButton* leftshift;
    /*
   *
   *
   *
   * ----------------------------footer-------------------------------------
   *
   *
   *
   */
    //panels
    wxPanel* footer;

    //ListBoxs
    wxListBox* footerListBox;

    //sizers
    wxBoxSizer* footerSizer;




    //events ID
    enum { 
           ID_CONNECT_SERV = wxID_HIGHEST + 1,
           ID_DISCONNECT_SERV,
           ID_HOST_DATA,
           ID_LOGIN_DATA,
           ID_PORT_DATA,
           ID_PASSWORD_DATA
         };

    //events
    void connectionClicked(wxCommandEvent& event);
    void disconnectionClicked(wxCommandEvent& event);
    void accessDataChanged(wxCommandEvent& event);
 };
