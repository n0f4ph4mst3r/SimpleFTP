#include "ClientApp.h"

//eventID
enum {
    ID_CONNECT_SERV = wxID_HIGHEST + 1,
    ID_DISCONNECT_SERV,
    ID_HOST_DATA,
    ID_LOGIN_DATA,
    ID_PORT_DATA,
    ID_PASSWORD_DATA
};

wxBEGIN_EVENT_TABLE(ClientApp, wxFrame)
wxEND_EVENT_TABLE()

ClientApp::ClientApp(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize) {

    ftpClient = new wxFTP();

    accessData = { {ID_HOST_DATA, "localhost"},
                   {ID_LOGIN_DATA, wxEmptyString},
                   {ID_PORT_DATA, wxEmptyString},
                   {ID_PASSWORD_DATA, wxEmptyString}, };
    /*
    * 
    * 
    * 
    * ----------------------------header-------------------------------------
    * 
    * 
    * 
    */
    headerPanel = new wxPanel(this, -1, wxDefaultPosition, wxSize(700, 100));
    headerPanel->SetBackgroundColour(wxColor(200, 100, 100));

    //Connect/Disconnect buttons
    controlPanel = new wxPanel(headerPanel, -1, wxDefaultPosition, wxSize(100, 100));
    controlPanel->SetBackgroundColour(wxColor(255, 165, 0)); //need delete

    controlPanelSizer = new wxGridSizer(2, 1, 0, 0);

    connectionButton = new wxButton(controlPanel, ID_CONNECT_SERV, wxT("Connect"));
    disconectionButton = new wxButton(controlPanel, ID_DISCONNECT_SERV, wxT("Disconnect"));

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::connectionClicked, this, ID_CONNECT_SERV);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::disconnectionClicked, this, ID_DISCONNECT_SERV);

    controlPanelSizer->Add(connectionButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM | wxBOTTOM, 5);
    controlPanelSizer->Add(disconectionButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxTOP, 5);

    controlPanel->SetSizer(controlPanelSizer);

    //Login panel
    accessPanelMain = new wxPanel(headerPanel, -1, wxDefaultPosition, wxSize(600, 100));
    accessPanelMain->SetBackgroundColour(wxColor(255, 203, 219)); //need delete

    accessPanelMainSizer = new wxFlexGridSizer(2, 4, 0, 0);
    accessPanelMainSizer->Add(new wxStaticText(accessPanelMain, -1, wxT("Host")), 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 5);
    accessPanelMainSizer->Add(new wxStaticText(accessPanelMain, -1, wxT("Login")), 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 5);
    accessPanelMainSizer->Add(new wxStaticText(accessPanelMain, -1, wxT("Port")), 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 5);
    accessPanelMainSizer->Add(new wxStaticText(accessPanelMain, -1, wxT("Password")), 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 5);

    hostCtrl = new wxTextCtrl(accessPanelMain, ID_HOST_DATA, wxT("localhost"), wxDefaultPosition, wxSize(125, 20));
    userCtrl = new wxTextCtrl(accessPanelMain, ID_LOGIN_DATA, wxEmptyString, wxDefaultPosition, wxSize(125, 20));
    portCtrl = new wxTextCtrl(accessPanelMain, ID_PORT_DATA, wxEmptyString, wxDefaultPosition, wxSize(125, 20));
    passwordCtrl = new wxTextCtrl(accessPanelMain, ID_PASSWORD_DATA, wxEmptyString, wxDefaultPosition, wxSize(125, 20), wxTE_PASSWORD);

    accessPanelMainSizer->Add(hostCtrl, 0, wxALIGN_TOP | wxTOP | wxLEFT, 5);
    accessPanelMainSizer->Add(userCtrl, 0, wxALIGN_TOP | wxTOP | wxLEFT, 5);
    accessPanelMainSizer->Add(portCtrl, 0, wxALIGN_TOP | wxTOP | wxLEFT, 5);
    accessPanelMainSizer->Add(passwordCtrl, 0, wxALIGN_TOP | wxTOP | wxLEFT, 5);

    Bind(wxEVT_TEXT, &ClientApp::accessDataChanged, this, ID_HOST_DATA);
    Bind(wxEVT_TEXT, &ClientApp::accessDataChanged, this, ID_LOGIN_DATA);
    Bind(wxEVT_TEXT, &ClientApp::accessDataChanged, this, ID_PORT_DATA);
    Bind(wxEVT_TEXT, &ClientApp::accessDataChanged, this, ID_PASSWORD_DATA);

    accessPanelMainSizer->AddGrowableRow(0, 1);
    accessPanelMainSizer->AddGrowableRow(1, 1);

    //Set sizers
    accessPanelMain->SetSizer(accessPanelMainSizer);

    headerPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    headerPanelSizer->Add(accessPanelMain, 1, wxEXPAND | wxLEFT | wxRIGHT);
    headerPanelSizer->Add(controlPanel, 0, wxEXPAND);
    headerPanel->SetSizer(headerPanelSizer);
    /*
    *
    *
    *
    * ----------------------------center----------------------------------
    *
    *
    *
    */
    centerPanel = new wxPanel(this, -1, wxDefaultPosition, wxSize(700, 275));
    centerPanel->SetBackgroundColour(wxColor(100, 100, 200)); //need delete

    centerSizer = new wxFlexGridSizer (1, 3, 0, 0);

    clientDirs = new wxGenericDirCtrl(centerPanel, -1, wxDirDialogDefaultFolderStr, wxDefaultPosition, wxSize(300, -1));
    serverDirs = new wxGenericDirCtrl(centerPanel, -1, wxDirDialogDefaultFolderStr, wxDefaultPosition, wxSize(300, -1));

    //Switch
    switchPanel = new wxPanel(centerPanel, -1, wxDefaultPosition, wxSize(95, -1));
    rightshift = new wxButton(switchPanel, -1, wxT(">>>"));
    leftshift = new wxButton(switchPanel, -1, wxT("<<<"));
    switchSizer = new wxGridSizer(2, 1, 0, 0);
    switchSizer->Add(rightshift, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM | wxBOTTOM, 15);
    switchSizer->Add(leftshift, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxTOP, 15);

    switchPanel->SetSizer(switchSizer);

    //Set sizer
    centerSizer->Add(clientDirs, 1, wxEXPAND | wxALL, 5);
    centerSizer->Add(switchPanel, 1, wxEXPAND);
    centerSizer->Add(serverDirs, 1, wxEXPAND | wxALL, 5);
    centerSizer->AddGrowableCol(0, 1);
    centerSizer->AddGrowableCol(2, 1);
    centerSizer->AddGrowableRow(0, 1);

    centerPanel->SetSizer(centerSizer);
    /*
    *
    *
    *
    * ----------------------------footer-------------------------------------
    *
    *
    *
    */
    footer = new wxPanel(this, -1, wxDefaultPosition, wxSize(700, 150));
    footer->SetBackgroundColour(wxColor(128, 0, 128)); //need delete

    footerListBox = new wxListBox(footer, -1);

    footerSizer = new wxBoxSizer(wxVERTICAL);
    footerSizer->Add(footerListBox, 1, wxEXPAND | wxALL, 5);
    footer->SetSizer(footerSizer);

    //Set sizers for main frame
    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(headerPanel, 0, wxEXPAND | wxLEFT | wxRIGHT);
    frameSizer->Add(centerPanel, 1 , wxEXPAND | wxALL);
    frameSizer->Add(footer, 0, wxEXPAND | wxLEFT | wxRIGHT);

    this->SetSizerAndFit(frameSizer);

    Centre();
}

void ClientApp::connectionClicked(wxCommandEvent&) {
    ftpClient->SetUser(accessData[ID_LOGIN_DATA]);
    ftpClient->SetPassword(accessData[ID_PASSWORD_DATA]);
    if (!ftpClient->Connect(accessData[ID_HOST_DATA], wxAtoi(accessData[ID_PORT_DATA])))
    {
        wxLogError("Couldn't connect");
        return;
    }
    else {
        wxMessageBox(wxT("Connected"), wxT("Message"), wxOK | wxICON_INFORMATION, this);
    }
}

void ClientApp::disconnectionClicked(wxCommandEvent& event) {
    ftpClient->Close();
    wxMessageBox(wxT("Disconnected"), wxT("Message"), wxOK | wxICON_INFORMATION, this);
}

void ClientApp::accessDataChanged(wxCommandEvent& event) {
    wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
    accessData[event.GetId()] = textCtrl->GetValue();
}