#include "ClientApp.h"

using boost::asio::ip::tcp;

wxBEGIN_EVENT_TABLE(ClientApp, wxFrame)
   EVT_TEXT(wxID_CONNECTION_HOST, ClientApp::AccessDataChanged)
   EVT_TEXT(wxID_CONNECTION_LOGIN, ClientApp::AccessDataChanged)
   EVT_TEXT(wxID_CONNECTION_PORT, ClientApp::AccessDataChanged)
   EVT_TEXT(wxID_CONNECTION_PASSWORD, ClientApp::AccessDataChanged)
wxEND_EVENT_TABLE()

ClientApp::ClientApp(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize) {
    wxInitAllImageHandlers();

    accessData = { {wxID_CONNECTION_HOST, "127.0.0.1"},
                   {wxID_CONNECTION_LOGIN, wxEmptyString},
                   {wxID_CONNECTION_PORT, wxEmptyString},
                   {wxID_CONNECTION_PASSWORD, wxEmptyString}, };

    wxSplitterWindow* splittermain = new wxSplitterWindow(this, wxID_ANY);
    splittermain->SetMinimumPaneSize(150); 

    wxPanel* topPanel = new wxPanel(splittermain, -1, wxDefaultPosition, wxSize(700, 375));

    wxPanel* headerPanel = new wxPanel(topPanel, -1, wxDefaultPosition, wxSize(700, 100));
    headerPanel->SetBackgroundColour(wxColor(200, 100, 100)); //need delete

    wxPanel* centerPanel = new wxPanel(topPanel, -1, wxDefaultPosition, wxSize(700, 275));
    centerPanel->SetBackgroundColour(wxColor(100, 100, 200)); //need delete

    wxPanel* footer = new wxPanel(splittermain, wxID_FOOTER, wxDefaultPosition, wxSize(700, 150));
    footer->SetBackgroundColour(wxColor(128, 0, 128)); //need delete

    logCtrl = new wxLogCtrl();
    logCtrl->EnableSystemTheme(false);
    logCtrl->Create(footer, wxID_LOGGER, wxDefaultPosition, wxSize(700, -1), wxLC_REPORT | wxLC_VIRTUAL | wxLC_SINGLE_SEL);
    logCtrl->InitLog();

    client = std::make_unique<wxFTPWrapper>(*logCtrl);
    wxPanel* serverPanel = new wxPanel(centerPanel, -1, wxDefaultPosition, wxSize(300, -1), wxBORDER_THEME);
    serverTree = new wxFTPGenericDirCtrl(*client, serverPanel, wxID_TREE_SERVER, wxDefaultPosition, wxSize(300, -1));
    wxBoxSizer* serverPanelSizer = new wxBoxSizer(wxVERTICAL);
    serverPanelSizer->Add(serverTree, 1, wxEXPAND | wxALL);
    serverPanel->SetSizer(serverPanelSizer);
    serverTree->Enable(false);
    
    /*
    * 
    * 
    * 
    * ----------------------------header-----------------------------------
    * 
    * 
    * 
    */
    wxPanel* controlPanel = new wxPanel(headerPanel, -1, wxDefaultPosition, wxSize(100, 100));
    controlPanel->SetBackgroundColour(wxColor(255, 165, 0)); //need delete

    wxGridSizer* controlPanelSizer = new wxGridSizer(2, 1, 0, 0);

    wxButton* connectionButton = new wxButton(controlPanel, wxID_BUTTON_CONNECT, wxT("Connect"));
    wxButton* disconectionButton = new wxButton(controlPanel, wxID_BUTTON_DISCONNECT, wxT("Disconnect"));

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::ConnectionClicked, this, wxID_BUTTON_CONNECT);

    controlPanelSizer->Add(connectionButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM | wxBOTTOM, 5);
    controlPanelSizer->Add(disconectionButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxTOP, 5);

    controlPanel->SetSizer(controlPanelSizer);

    //Login panel
    wxPanel* accessPanelMain = new wxPanel(headerPanel, -1, wxDefaultPosition, wxSize(600, 100));
    accessPanelMain->SetBackgroundColour(wxColor(255, 203, 219)); //need delete

    wxFlexGridSizer* accessPanelMainSizer = new wxFlexGridSizer(2, 4, 0, 0);
    accessPanelMainSizer->Add(new wxStaticText(accessPanelMain, -1, wxT("Host")), 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 5);
    accessPanelMainSizer->Add(new wxStaticText(accessPanelMain, -1, wxT("Login")), 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 5);
    accessPanelMainSizer->Add(new wxStaticText(accessPanelMain, -1, wxT("Password")), 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 5);
    accessPanelMainSizer->Add(new wxStaticText(accessPanelMain, -1, wxT("Port")), 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 5);

    wxTextCtrl* hostCtrl = new wxTextCtrl(accessPanelMain, wxID_CONNECTION_HOST, wxT("127.0.0.1"), wxDefaultPosition, wxSize(125, 20));
    wxTextCtrl* userCtrl = new wxTextCtrl(accessPanelMain, wxID_CONNECTION_LOGIN, wxEmptyString, wxDefaultPosition, wxSize(125, 20));
    wxTextCtrl* portCtrl = new wxTextCtrl(accessPanelMain, wxID_CONNECTION_PORT, wxEmptyString, wxDefaultPosition, wxSize(125, 20));
    wxTextCtrl* passwordCtrl = new wxTextCtrl(accessPanelMain, wxID_CONNECTION_PASSWORD, wxEmptyString, wxDefaultPosition, wxSize(125, 20), wxTE_PASSWORD);

    accessPanelMainSizer->Add(hostCtrl, 0, wxALIGN_TOP | wxTOP | wxLEFT, 5);
    accessPanelMainSizer->Add(userCtrl, 0, wxALIGN_TOP | wxTOP | wxLEFT, 5);
    accessPanelMainSizer->Add(passwordCtrl, 0, wxALIGN_TOP | wxTOP | wxLEFT, 5);
    accessPanelMainSizer->Add(portCtrl, 0, wxALIGN_TOP | wxTOP | wxLEFT, 5);

    accessPanelMainSizer->AddGrowableRow(0, 1);
    accessPanelMainSizer->AddGrowableRow(1, 1);

    //Set sizers
    accessPanelMain->SetSizer(accessPanelMainSizer);

    wxBoxSizer* headerPanelSizer = new wxBoxSizer(wxHORIZONTAL);
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
    wxFlexGridSizer* centerSizer = new wxFlexGridSizer (1, 3, 0, 0);


    clientTree = new wxGenericDirCtrl(centerPanel, -1, wxDirDialogDefaultFolderStr, wxDefaultPosition, wxSize(300, -1));
    
    //Switch
    wxPanel* switchPanel = new wxPanel(centerPanel, -1, wxDefaultPosition, wxSize(95, -1));
    wxButton* rightshift = new wxButton(switchPanel, -1, wxT(">>>"));
    wxButton* leftshift = new wxButton(switchPanel, -1, wxT("<<<"));
    wxGridSizer* switchSizer = new wxGridSizer(2, 1, 0, 0);
    switchSizer->Add(rightshift, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM | wxBOTTOM, 15);
    switchSizer->Add(leftshift, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxTOP, 15);

    switchPanel->SetSizer(switchSizer);

    //Set sizer
    centerSizer->Add(clientTree, 1, wxEXPAND | wxALL, 5);
    centerSizer->Add(switchPanel, 1, wxEXPAND);
    centerSizer->Add(serverPanel, 1, wxEXPAND | wxALL, 5);
    centerSizer->AddGrowableCol(0, 1);
    centerSizer->AddGrowableCol(2, 1);
    centerSizer->AddGrowableRow(0, 1);

    centerPanel->SetSizer(centerSizer);

    wxBoxSizer* topPanelSizer = new wxBoxSizer(wxVERTICAL);
    topPanelSizer->Add(headerPanel, 0, wxEXPAND | wxLEFT | wxRIGHT);
    topPanelSizer->Add(centerPanel, 1, wxEXPAND | wxALL);
    topPanel->SetSizer(topPanelSizer);
    /*
    *
    *
    *
    * ----------------------------footer----------------------------------
    *
    *
    *
    */
    
    wxBoxSizer* footerSizer = new wxBoxSizer(wxVERTICAL);
    footerSizer->Add(logCtrl, 1, wxEXPAND | wxALL, 5);
    logCtrl->SetColumnWidth(0, 75);
    logCtrl->SetColumnWidth(1, 75);
    logCtrl->SetColumnWidth(2, 540);
    footer->SetSizer(footerSizer);

    //Set sizers for main frame
    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(splittermain, 1, wxEXPAND | wxLEFT | wxRIGHT);

    splittermain->SplitHorizontally(topPanel, footer);

    this->SetSizerAndFit(frameSizer);

    Centre();
}

void ClientApp::Shutdown() {
    serverTree->Enable(false);
    serverTree->DeleteAllItems();

    client->Shutdown();
}

void ClientApp::ConnectionClicked(wxCommandEvent&) {
    if (accessData[wxID_CONNECTION_PORT] == "") accessData[wxID_CONNECTION_PORT] = "ftp";
    if (client->Connect(accessData[wxID_CONNECTION_HOST].ToStdString(), accessData[wxID_CONNECTION_LOGIN].ToStdString(), accessData[wxID_CONNECTION_PASSWORD].ToStdString(), accessData[wxID_CONNECTION_PORT].ToStdString())) {
            serverTree->RefreshServerTree();
            serverTree->Enable();

            Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::DisconnectionClicked, this, wxID_BUTTON_DISCONNECT);
            Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::ConnectionClicked, this, wxID_BUTTON_CONNECT);
    }
    else Shutdown();
}

void ClientApp::DisconnectionClicked(wxCommandEvent& event) {
    client->SendRequest("QUIT");
    Shutdown();

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::ConnectionClicked, this, wxID_BUTTON_CONNECT);
    Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::DisconnectionClicked, this, wxID_BUTTON_DISCONNECT);
}

void ClientApp::AccessDataChanged(wxCommandEvent& event) {
    wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
    accessData[event.GetId()] = textCtrl->GetValue();
}
