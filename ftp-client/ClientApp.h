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
    wxFTP* ftpClient;

    //login, port and etc.
    std::map <wxString, wxString> accessData;


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
    wxButton* conButton;
    wxButton* discButton;

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
           ID_PASSWORD_DATA,
           ID_PORT_DATA
         };

    //events
    void conButtonClicked(wxCommandEvent& event);
    void dicsButtonClicked(wxCommandEvent& event);
 };
