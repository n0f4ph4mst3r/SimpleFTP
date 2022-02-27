#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include "ClientApp.h"

using boost::asio::ip::tcp;

//eventID
enum {
    ID_CONNECT_SERV = wxID_HIGHEST + 1,
    ID_DISCONNECT_SERV,
    ID_HOST_DATA,
    ID_LOGIN_DATA,
    ID_PORT_DATA,
    ID_PASSWORD_DATA,
    ID_LOGGER
};

wxBEGIN_EVENT_TABLE(ClientApp, wxFrame)
wxEND_EVENT_TABLE()

ClientApp::ClientApp(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize) {

    accessData = { {ID_HOST_DATA, "127.0.0.1"},
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

    hostCtrl = new wxTextCtrl(accessPanelMain, ID_HOST_DATA, wxT("127.0.0.1"), wxDefaultPosition, wxSize(125, 20));
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
    serverDirs = new wxListBox(centerPanel, -1, wxDefaultPosition, wxSize(300, -1));
    serverDirs->Enable(false);

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

    wxTextCtrl* footerTextCtrl = new wxTextCtrl(footer, ID_LOGGER, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH);

    footerTextCtrl->Bind(wxEVT_SET_FOCUS, [footerTextCtrl](wxFocusEvent&) {
        HideCaret(footerTextCtrl->GetHWND());
        });

    footerSizer = new wxBoxSizer(wxVERTICAL);
    footerSizer->Add(footerTextCtrl, 1, wxEXPAND | wxALL, 5);
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
    wxTextCtrl* footerTextCtrl = dynamic_cast<wxTextCtrl*>(FindWindowById(ID_LOGGER));
    footerTextCtrl->SetDefaultStyle(wxTextAttr(*wxBLACK, *wxWHITE, *wxNORMAL_FONT));
    try
    {
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve(accessData[ID_HOST_DATA].ToStdString(), "ftp");

        tcp::socket commandSocket(io_context);
        boost::asio::connect(commandSocket, endpoints);

        boost::asio::streambuf response;
        std::istream response_stream_login(&response);
        std::istream response_stream_pasv(&response);

        boost::asio::streambuf request;
        std::ostream request_stream(&request);

        std::string sipaddress, sport;

        if (commandSocket.is_open()) {
            //send request
            request_stream << "USER " << accessData[ID_LOGIN_DATA] << "\r\n";
            request_stream << "PASS " << accessData[ID_PASSWORD_DATA] << "\r\n";
            boost::asio::write(commandSocket, request);

            //wait one second
            boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
            boost::asio::read_until(commandSocket, response, boost::regex("\r\n"));
            
            std::string sresponse;
            while(response_stream_login.peek() != EOF && std::getline(response_stream_login, sresponse)) {
                footerTextCtrl->SetDefaultStyle(wxTextAttr(*wxBLACK));
                footerTextCtrl->AppendText(std::move(sresponse));
            }
            response_stream_login.clear();

            request_stream.flush();
            request_stream << "PASV" << "\r\n";
            boost::asio::write(commandSocket, request);

            boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
            boost::asio::read_until(commandSocket, response, boost::regex("\r\n"));
            std::getline(response_stream_pasv, sresponse);
            
            boost::regex regex_ip(".+\\(([0-9]{1,}),([0-9]{1,}),([0-9]{1,}),([0-9]{1,}),([0-9]{1,}),([0-9]{1,})\\).*");
            //Get a IP address string
            sipaddress = boost::regex_replace(sresponse, regex_ip, "$1.$2.$3.$4", boost::format_all);
            unsigned int itmp1 = boost::lexical_cast<unsigned int>(boost::regex_replace(sresponse, regex_ip, "$5", boost::format_all));
            unsigned int itmp2 = boost::lexical_cast<unsigned int>(boost::regex_replace(sresponse, regex_ip, "$6", boost::format_all));

            //Get a Port number(16bit) string
            sport = boost::lexical_cast<std::string>(itmp1 * 256 + itmp2);

            footerTextCtrl->SetDefaultStyle(wxTextAttr(*wxBLUE));
            footerTextCtrl->AppendText("Command socket connected\r\n");
        }
        else {
            footerTextCtrl->SetDefaultStyle(wxTextAttr(*wxRED));
            footerTextCtrl->AppendText("ERROR: Connection to command socket is fail\r\n");
        }

        tcp::resolver::query query_trsans(sipaddress, sport);
        tcp::resolver::results_type endpoints_trsans = resolver.resolve(query_trsans);

        tcp::socket dataSocket(io_context);
        boost::asio::connect(dataSocket, endpoints_trsans);

        std::istream response_stream_list(&response);

        if (dataSocket.is_open()) {
            request_stream.flush();
            request_stream << "LIST" << "\r\n";
            boost::asio::write(commandSocket, request);

            boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
            boost::asio::read_until(dataSocket, response, boost::regex("\r\n"));

            std::string sresponse;
            while (response_stream_list.peek() != EOF && std::getline(response_stream_list, sresponse)) {
                serverDirs->Append(std::move(sresponse));
            }
 
            footerTextCtrl->SetDefaultStyle(wxTextAttr(*wxBLUE));
            footerTextCtrl->AppendText("Data socket connected\r\n");
            serverDirs->Enable(true);
        }
        else {
            footerTextCtrl->SetDefaultStyle(wxTextAttr(*wxRED));
            footerTextCtrl->AppendText("ERROR: Connection to data socket is fail\r\n");
        }
    }
    catch (std::exception& e)
    {
        footerTextCtrl->SetDefaultStyle(wxTextAttr(*wxRED));
        std::ostream streamCtrl(footerTextCtrl);
        streamCtrl << e.what() << "\r\n";
        streamCtrl.flush();
    }
}

void ClientApp::disconnectionClicked(wxCommandEvent& event) {
    wxTextCtrl* footerTextCtrl = dynamic_cast<wxTextCtrl*>(FindWindowById(ID_LOGGER));
    const wxFont* font = new wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, true);
    footerTextCtrl->SetDefaultStyle(wxTextAttr(*wxBLACK, *wxBLUE, *font));
    footerTextCtrl->AppendText("Disconneted!\r\n");
    serverDirs->Enable(false);
    serverDirs->Clear();
}

void ClientApp::accessDataChanged(wxCommandEvent& event) {
    wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
    accessData[event.GetId()] = textCtrl->GetValue();
}

