#include "ClientApp.h"

wxBEGIN_EVENT_TABLE(ClientApp, wxFrame)
   EVT_COMBOBOX(-1, dirComboBoxOnCreate)
wxEND_EVENT_TABLE()

ClientApp::ClientApp(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize) {
    /*
    * 
    * 
    * 
    * ----------------------------header-------------------------------------
    * 
    * 
    * 
    */
    wxPanel* headerPanel = new wxPanel(this, -1, wxDefaultPosition, wxSize(700, 100));
    headerPanel->SetBackgroundColour(wxColor(200, 100, 100));

    //Connect/Disconnect buttons
    wxPanel* controlPanel = new wxPanel(headerPanel, -1, wxDefaultPosition, wxSize(100, 100));
    controlPanel->SetBackgroundColour(wxColor(255, 165, 0)); //need delete

    wxGridSizer* controlPanelSizer = new wxGridSizer(2, 1, 0, 0);
    wxButton* conButton = new wxButton(controlPanel, -1, wxT("Connect"));
    wxButton* discButton = new wxButton(controlPanel, -1, wxT("Disconnect"));
    controlPanelSizer->Add(conButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM | wxBOTTOM, 5);
    controlPanelSizer->Add(discButton, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxTOP, 5);

    controlPanel->SetSizer(controlPanelSizer);

    wxPanel* accessPanelMain = new wxPanel(headerPanel, -1, wxDefaultPosition, wxSize(600, 100));
    accessPanelMain->SetBackgroundColour(wxColor(255, 203, 219)); //need delete

    //Login panel
    wxPanel* accessPanelTop = new wxPanel(accessPanelMain, -1, wxDefaultPosition, wxSize(600, 50));
    accessPanelTop->SetBackgroundColour(wxColor(0, 255, 0)); //need delete

    wxFlexGridSizer* accessPanelTopSizer = new wxFlexGridSizer(2, 4, 1, 1);
    accessPanelTopSizer->Add(new wxStaticText(accessPanelTop, -1, wxT("Host")), 0,  wxALIGN_BOTTOM | wxTOP | wxLEFT, 5);
    accessPanelTopSizer->Add(new wxStaticText(accessPanelTop, -1, wxT("Login")), 0, wxALIGN_BOTTOM | wxTOP | wxLEFT, 5);
    accessPanelTopSizer->Add(new wxStaticText(accessPanelTop, -1, wxT("Port")), 0, wxALIGN_BOTTOM | wxTOP | wxLEFT, 5);
    accessPanelTopSizer->Add(new wxStaticText(accessPanelTop, -1, wxT("Password")), 0, wxALIGN_BOTTOM | wxTOP | wxLEFT, 5);
    accessPanelTopSizer->Add(new wxTextCtrl(accessPanelTop, -1, wxEmptyString, wxDefaultPosition, wxSize(125, 20)), 0, wxTOP | wxLEFT, 5);
    accessPanelTopSizer->Add(new wxTextCtrl(accessPanelTop, -1, wxEmptyString, wxDefaultPosition, wxSize(125, 20)), 0, wxTOP | wxLEFT, 5);
    accessPanelTopSizer->Add(new wxTextCtrl(accessPanelTop, -1, wxEmptyString, wxDefaultPosition, wxSize(125, 20)), 0, wxTOP | wxLEFT, 5);
    accessPanelTopSizer->Add(new wxTextCtrl(accessPanelTop, -1, wxEmptyString, wxDefaultPosition, wxSize(125, 20), wxTE_PASSWORD), 0, wxTOP | wxLEFT, 5);
    accessPanelTopSizer->AddGrowableCol(3, 1);

    //Directory selector
    wxPanel* accessPanelBottom = new wxPanel(accessPanelMain, -1, wxDefaultPosition, wxSize(600, 50));
    accessPanelBottom->SetBackgroundColour(wxColor(255, 255, 0)); //need delete

    wxFlexGridSizer* accessPanelBottomSizer = new wxFlexGridSizer(2, 3, 0, 0);
    
    wxComboBox* dirComboBox = new wxComboBox(accessPanelBottom, -1, wxEmptyString, wxDefaultPosition, wxSize(300, 20));
    wxStaticText* currentPath = new wxStaticText(accessPanelBottom, -1, wxT("path/to/file"));
    wxButton* createDirButton = new wxButton(accessPanelBottom, -1, wxT("Create directory"), wxDefaultPosition, wxSize(125, 22));

    accessPanelBottomSizer->Add(dirComboBox, 0, wxLEFT | wxTOP, 5);
    accessPanelBottomSizer->Add(new wxPanel(accessPanelBottom, -1, wxDefaultPosition, wxSize(100, 25)), 0);
    accessPanelBottomSizer->Add(new wxPanel(accessPanelBottom, -1, wxDefaultPosition, wxSize(100, 25)), 1, wxEXPAND);
    accessPanelBottomSizer->Add(currentPath, 1, wxLEFT | wxBOTTOM, 5);
    accessPanelBottomSizer->Add(new wxPanel(accessPanelBottom, -1, wxDefaultPosition, wxSize(100, 25)), 0);
    accessPanelBottomSizer->Add(createDirButton, 0, wxLEFT | wxBOTTOM, 5);
    accessPanelBottomSizer->AddGrowableCol(0, 1);
    accessPanelBottomSizer->AddGrowableCol(2, 1);

    //Set sizers
    wxBoxSizer* accessPanelMainSizer = new wxBoxSizer(wxVERTICAL);
    accessPanelMainSizer->Add(accessPanelTop, 1, wxEXPAND | wxLEFT | wxRIGHT);
    accessPanelMainSizer->Add(accessPanelBottom, 1, wxEXPAND | wxLEFT | wxRIGHT);
    
    accessPanelMain->SetSizer(accessPanelMainSizer);
    accessPanelTop->SetSizer(accessPanelTopSizer);
    accessPanelBottom->SetSizer(accessPanelBottomSizer);

    wxBoxSizer* headerPanelSizer = new wxBoxSizer(wxHORIZONTAL);
    headerPanelSizer->Add(accessPanelMain, 1, wxEXPAND | wxLEFT | wxRIGHT);
    headerPanelSizer->Add(controlPanel, 0, wxEXPAND);
    headerPanel->SetSizer(headerPanelSizer);
    /*
    *
    *
    *
    * ----------------------------center-------------------------------------
    *
    *
    *
    */
    wxPanel* centerPanel = new wxPanel(this, -1, wxDefaultPosition, wxSize(700, 275));
    centerPanel->SetBackgroundColour(wxColor(100, 100, 200)); //need delete

    wxFlexGridSizer* centerSizer = new wxFlexGridSizer (1, 3, 0, 0);
    wxListBox* userDirs = new wxListBox(centerPanel, -1, wxDefaultPosition, wxSize(300, -1));
    wxListBox* serverDirs = new wxListBox(centerPanel, -1, wxDefaultPosition, wxSize(300, -1));

    //Switch
    wxPanel* switchPanel = new wxPanel(centerPanel, -1, wxDefaultPosition, wxSize(95, -1));
    wxButton* rightshift = new wxButton(switchPanel, -1, wxT(">>>"));
    wxButton* leftshift = new wxButton(switchPanel, -1, wxT("<<<"));
    wxGridSizer* switchSizer = new wxGridSizer(2, 1, 0, 0);
    switchSizer->Add(rightshift, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_BOTTOM | wxBOTTOM, 15);
    switchSizer->Add(leftshift, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP | wxTOP, 15);

    switchPanel->SetSizer(switchSizer);

    //Set sizer
    centerSizer->Add(userDirs, 1, wxEXPAND | wxALL, 5);
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
    wxPanel* footer = new wxPanel(this, -1, wxDefaultPosition, wxSize(700, 150));
    footer->SetBackgroundColour(wxColor(128, 0, 128)); //need delete

    wxListBox* footerListBox = new wxListBox(footer, -1);

    wxBoxSizer* footerSizer = new wxBoxSizer(wxVERTICAL);
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

void ClientApp::dirComboBoxOnCreate(wxCommandEvent& event) {

}