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

    m_accessData = {{wxID_CONNECTION_HOST, wxEmptyString},
                   {wxID_CONNECTION_LOGIN, wxEmptyString},
                   {wxID_CONNECTION_PORT, wxEmptyString},
                   {wxID_CONNECTION_PASSWORD, wxEmptyString}};

    wxSplitterWindow* splittermain = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(700, 750), wxSP_3D | wxSP_LIVE_UPDATE);
    wxPanel* topPanel = new wxPanel(splittermain, wxID_ANY, wxDefaultPosition, wxSize(700, 450));
    wxPanel* headerPanel = new wxPanel(topPanel, wxID_ANY, wxDefaultPosition, wxSize(700, 100));
    wxPanel* bottomPanel = new wxPanel(splittermain, wxID_ANY, wxDefaultPosition, wxSize(700, 300));

    m_client = std::make_shared<wxFTPWrapper>();
    m_taskManager = new wxTaskManager(m_client, bottomPanel, wxID_ANY, wxDefaultPosition, wxSize(700, 300));
    m_dirCtrl = new wxGenericDirPanel(m_client, topPanel, wxID_ANY, wxDefaultPosition, wxSize(700, 350), wxSP_3D | wxSP_LIVE_UPDATE);
    //
    //  header
    //
    wxPanel* controlPanel = new wxPanel(headerPanel, wxID_ANY, wxDefaultPosition, wxSize(100, 100));
    wxGridSizer* controlPanelSizer = new wxGridSizer(2, 1, 0, 0);

    wxButton* connectionButton = new wxButton(controlPanel, wxID_BUTTON_CONNECT, wxT("Connect"));
    wxButton* disconectionButton = new wxButton(controlPanel, wxID_BUTTON_DISCONNECT, wxT("Disconnect"));
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::ConnectionClicked, this, wxID_BUTTON_CONNECT);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::DisconnectionClicked, this, wxID_BUTTON_DISCONNECT);

    controlPanelSizer->Add(connectionButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM | wxBOTTOM, 5);
    controlPanelSizer->Add(disconectionButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxTOP, 5);
    controlPanel->SetSizer(controlPanelSizer);
    //
    //  Login panel
    //
    wxPanel* accessPanelMain = new wxPanel(headerPanel, -1, wxDefaultPosition, wxSize(600, 100));
    wxFlexGridSizer* accessPanelMainSizer = new wxFlexGridSizer(2, 4, 0, 0);
    accessPanelMainSizer->Add(new wxStaticText(accessPanelMain, -1, wxT("Host")), 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 5);
    accessPanelMainSizer->Add(new wxStaticText(accessPanelMain, -1, wxT("Login")), 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 5);
    accessPanelMainSizer->Add(new wxStaticText(accessPanelMain, -1, wxT("Password")), 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 5);
    accessPanelMainSizer->Add(new wxStaticText(accessPanelMain, -1, wxT("Port")), 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT, 5);

    wxTextCtrl* hostCtrl = new wxTextCtrl(accessPanelMain, wxID_CONNECTION_HOST, wxEmptyString, wxDefaultPosition, wxSize(125, 20));
    wxTextCtrl* userCtrl = new wxTextCtrl(accessPanelMain, wxID_CONNECTION_LOGIN, wxEmptyString, wxDefaultPosition, wxSize(125, 20));
    wxTextCtrl* passwordCtrl = new wxTextCtrl(accessPanelMain, wxID_CONNECTION_PASSWORD, wxEmptyString, wxDefaultPosition, wxSize(125, 20), wxTE_PASSWORD);
    wxTextCtrl* portCtrl = new wxTextCtrl(accessPanelMain, wxID_CONNECTION_PORT, wxEmptyString, wxDefaultPosition, wxSize(125, 20), 0, wxTextValidator(wxFILTER_DIGITS));
    portCtrl->SetMaxLength(5);
    portCtrl->SetToolTip(new wxToolTip(wxT("Enter the port which the server listens. The default is 21.")));

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

    wxBoxSizer* bottomPanelSizer = new wxBoxSizer(wxVERTICAL);
    bottomPanelSizer->Add(m_taskManager, 1, wxEXPAND | wxALL);
    bottomPanel->SetSizer(bottomPanelSizer);

    wxBoxSizer* topPanelSizer = new wxBoxSizer(wxVERTICAL);
    topPanelSizer->Add(headerPanel, 0, wxEXPAND | wxALL);
    topPanelSizer->Add(m_dirCtrl, 1, wxEXPAND | wxALL);
    topPanel->SetSizer(topPanelSizer);

    splittermain->SetMinimumPaneSize(300);
    splittermain->SplitHorizontally(topPanel, bottomPanel);

    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(splittermain, 1, wxEXPAND | wxALL);
    SetSizerAndFit(frameSizer);

    m_client->Bind(wxEVT_COMMAND_TRANSFER_COMPLETED, &ClientApp::DoEndTransferThread, this);
    SetIcon(wxICON(aaaaaaaa_logo));
    Centre();
}

void ClientApp::ConnectionClicked(wxCommandEvent&) {
    wxBusyCursor busyCursor = wxBusyCursor();
    wxTextCtrl* portCtrl = dynamic_cast<wxTextCtrl*>(FindWindowById(wxID_CONNECTION_PORT));
    long port;
    portCtrl->GetValue().ToLong(&port);
    if (port && port > 65535) {
        m_accessData[wxID_CONNECTION_PORT] = wxT("65535");
        portCtrl->SetValue(wxT("65535"));
    }

    if (m_accessData[wxID_CONNECTION_PORT] == "") m_accessData[wxID_CONNECTION_PORT] = "ftp";
    if (!m_client->wxFTPWrapper::Connect(m_accessData[wxID_CONNECTION_HOST].ToStdString(),
        m_accessData[wxID_CONNECTION_LOGIN].ToStdString(), 
        m_accessData[wxID_CONNECTION_PASSWORD].ToStdString(),
        m_accessData[wxID_CONNECTION_PORT].ToStdString()))
        m_client->Shutdown();
}

void ClientApp::DisconnectionClicked(wxCommandEvent&) {
    wxBusyCursor busyCursor = wxBusyCursor();
    m_client->Shutdown();
    m_client->PrintMessage("Disconneted.");
}

void ClientApp::AccessDataChanged(wxCommandEvent& event) {
    wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
    m_accessData[event.GetId()] = textCtrl->GetValue();
}

void ClientApp::DoEndTransferThread(wxCommandEvent& event) {
    wxSetCursor(wxCURSOR_ARROW);
    wxSetCursor(wxNullCursor);
    event.Skip();
}


