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
    ID_FOOTER,
    ID_LOGGER,
    ID_SERVER_DIRECTORIES,
    ID_SERVER_DIRECTORIES_ITEM
};

wxBEGIN_EVENT_TABLE(ClientApp, wxFrame)
wxEND_EVENT_TABLE()

ClientApp::ClientApp(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize), resolver(io_context), commandSocket(io_context) {
    wxInitAllImageHandlers();

    accessData = { {ID_HOST_DATA, "127.0.0.1"},
                   {ID_LOGIN_DATA, wxEmptyString},
                   {ID_PORT_DATA, wxEmptyString},
                   {ID_PASSWORD_DATA, wxEmptyString}, };
    /*
    * 
    * 
    * 
    * ----------------------------header-----------------------------------
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

    wxButton* connectionButton = new wxButton(controlPanel, ID_CONNECT_SERV, wxT("Connect"));
    wxButton* disconectionButton = new wxButton(controlPanel, ID_DISCONNECT_SERV, wxT("Disconnect"));

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::connectionClicked, this, ID_CONNECT_SERV);

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

    wxTextCtrl* hostCtrl = new wxTextCtrl(accessPanelMain, ID_HOST_DATA, wxT("127.0.0.1"), wxDefaultPosition, wxSize(125, 20));
    wxTextCtrl* userCtrl = new wxTextCtrl(accessPanelMain, ID_LOGIN_DATA, wxEmptyString, wxDefaultPosition, wxSize(125, 20));
    wxTextCtrl* portCtrl = new wxTextCtrl(accessPanelMain, ID_PORT_DATA, wxEmptyString, wxDefaultPosition, wxSize(125, 20));
    wxTextCtrl* passwordCtrl = new wxTextCtrl(accessPanelMain, ID_PASSWORD_DATA, wxEmptyString, wxDefaultPosition, wxSize(125, 20), wxTE_PASSWORD);

    accessPanelMainSizer->Add(hostCtrl, 0, wxALIGN_TOP | wxTOP | wxLEFT, 5);
    accessPanelMainSizer->Add(userCtrl, 0, wxALIGN_TOP | wxTOP | wxLEFT, 5);
    accessPanelMainSizer->Add(passwordCtrl, 0, wxALIGN_TOP | wxTOP | wxLEFT, 5);
    accessPanelMainSizer->Add(portCtrl, 0, wxALIGN_TOP | wxTOP | wxLEFT, 5);

    Bind(wxEVT_TEXT, &ClientApp::accessDataChanged, this, ID_HOST_DATA);
    Bind(wxEVT_TEXT, &ClientApp::accessDataChanged, this, ID_LOGIN_DATA);
    Bind(wxEVT_TEXT, &ClientApp::accessDataChanged, this, ID_PORT_DATA);
    Bind(wxEVT_TEXT, &ClientApp::accessDataChanged, this, ID_PASSWORD_DATA);

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
    wxPanel* centerPanel = new wxPanel(this, -1, wxDefaultPosition, wxSize(700, 275));
    centerPanel->SetBackgroundColour(wxColor(100, 100, 200)); //need delete

    wxFlexGridSizer* centerSizer = new wxFlexGridSizer (1, 3, 0, 0);

    clientTree = new wxGenericDirCtrl(centerPanel, -1, wxDirDialogDefaultFolderStr, wxDefaultPosition, wxSize(300, -1));
    serverTree = new wxTreeCtrl(centerPanel, ID_SERVER_DIRECTORIES, wxDefaultPosition, wxSize(300, -1));
    serverTree->SetImageList(clientTree->GetTreeCtrl()->GetImageList());
    serverTree->Bind(wxEVT_LEFT_DOWN, &ClientApp::serverDirItemClicked, this);
    serverTree->Enable(false);

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
    centerSizer->Add(serverTree, 1, wxEXPAND | wxALL, 5);
    centerSizer->AddGrowableCol(0, 1);
    centerSizer->AddGrowableCol(2, 1);
    centerSizer->AddGrowableRow(0, 1);

    centerPanel->SetSizer(centerSizer);
    /*
    *
    *
    *
    * ----------------------------footer----------------------------------
    *
    *
    *
    */
    wxPanel* footer = new wxPanel(this, ID_FOOTER, wxDefaultPosition, wxSize(700, 150));
    footer->SetBackgroundColour(wxColor(128, 0, 128)); //need delete

    logCtrl = new wxListView(footer, ID_LOGGER, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    logCtrl->InsertColumn(0, "Date");
    logCtrl->InsertColumn(1, "Type");
    logCtrl->InsertColumn(2, "Message");
    PrintMessage("Welcome!", Message::STATUS);
    
    wxBoxSizer* footerSizer = new wxBoxSizer(wxVERTICAL);
    footerSizer->Add(logCtrl, 1, wxEXPAND | wxALL, 5);
    logCtrl->SetColumnWidth(0, 75);
    logCtrl->SetColumnWidth(1, 75);
    logCtrl->SetColumnWidth(2, 540);
    footer->SetSizer(footerSizer);

    //Set sizers for main frame
    wxBoxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
    frameSizer->Add(headerPanel, 0, wxEXPAND | wxLEFT | wxRIGHT);
    frameSizer->Add(centerPanel, 1 , wxEXPAND | wxALL);
    frameSizer->Add(footer, 0, wxEXPAND | wxLEFT | wxRIGHT);

    this->SetSizerAndFit(frameSizer);

    Bind(wxEVT_LIST_INSERT_ITEM, &ClientApp::LoggerItemInsert, this, wxID_ANY);
    Bind(wxEVT_LIST_COL_BEGIN_DRAG, &ClientApp::LoggerColumnBeginDragged, this, ID_LOGGER);
    Bind(wxEVT_LIST_COL_DRAGGING, &ClientApp::LoggerColumnDragged, this, ID_LOGGER);
    Bind(wxEVT_LIST_COL_END_DRAG, &ClientApp::LoggerColumnEndDragged, this, ID_LOGGER);
    Bind(wxEVT_SIZE, &ClientApp::FrameSizeChanged, this, wxID_ANY);

    Centre();
}

void ClientApp::connectionClicked(wxCommandEvent&) {
    try
    {
        if (!accessData[ID_PORT_DATA]) accessData[ID_PORT_DATA] = "ftp";
        tcp::resolver::query query(accessData[ID_HOST_DATA].ToStdString(), accessData[ID_PORT_DATA].ToStdString());

        tcp::resolver::results_type endpoints = resolver.resolve(accessData[ID_HOST_DATA].ToStdString(), "ftp");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::resolver::iterator end;

        boost::system::error_code error = boost::asio::error::host_not_found;
        while (error && endpoint_iterator != end)
        {
            commandSocket.close();
            commandSocket.connect(*endpoint_iterator++, error);
        }
        if (error) throw boost::system::system_error(error);

        PrintResponse();
        SendRequest((boost::format("USER %d") % accessData[ID_LOGIN_DATA]).str());
        SendRequest((boost::format("PASS %d") % accessData[ID_PASSWORD_DATA]).str());
        PrintMessage("Command socket connected.");

        std::string sresponse = SendRequest("PWD");
        sresponse = boost::regex_replace(sresponse, boost::regex("^257 \"/(.*)\" .*"), "$1", boost::format_all);
        wxTreeItemId rootId = serverTree->AddRoot((boost::format("/ %d") % sresponse).str(), -1, -1, new ServerTreeCtrlItemData(sresponse));
        serverTree->SetItemImage(rootId, wxFileIconsTable::drive);

        ExtractFiles(rootId);
        serverTree->Enable();

        Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::disconnectionClicked, this, ID_DISCONNECT_SERV);
        Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::connectionClicked, this, ID_CONNECT_SERV);
    }
    catch (std::exception& e)
    {
        Shutdown();

        PrintMessage(e.what(), Message::EC);
    }
}

void ClientApp::disconnectionClicked(wxCommandEvent& event) {
    SendRequest("QUIT");
    Shutdown();

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::connectionClicked, this, ID_CONNECT_SERV);
    Unbind(wxEVT_COMMAND_BUTTON_CLICKED, &ClientApp::disconnectionClicked, this, ID_DISCONNECT_SERV);

    PrintMessage("Disconneted.");
}

void ClientApp::accessDataChanged(wxCommandEvent& event) {
    wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
    accessData[event.GetId()] = textCtrl->GetValue();
}

void ClientApp::serverDirItemClicked(wxMouseEvent& event) {
    wxPoint mouse_position = event.GetPosition();
    int temp_num = wxTREE_HITTEST_ONITEMLABEL;

    wxTreeItemId selectedItem = serverTree->HitTest(mouse_position, temp_num);
    if (!serverTree->IsSelected(selectedItem)) serverTree->SelectItem(selectedItem, true);
    try {
        ServerTreeCtrlDirData* itemData = dynamic_cast<ServerTreeCtrlDirData*>(serverTree->GetItemData(selectedItem));
        if (!itemData) return;

        if (!itemData->isExtracted()) {
            std::string root;
            root = itemData->GetPath();

            SendRequest((boost::format("CWD %d") % root).str());
            SendRequest("PWD");

            ExtractFiles(selectedItem);
            itemData->Deactivate();
        }
        else event.Skip();
    }
    catch (std::bad_cast* e)
    {
        event.Skip();
    }
}

void ClientApp::LoggerItemInsert(wxListEvent& event) {
    logCtrl->SetColumnWidth(logCtrl->GetColumnCount() - 1, wxLIST_AUTOSIZE_USEHEADER);
}

void ClientApp::LoggerColumnBeginDragged(wxListEvent& event) {
    if (event.GetColumn() == logCtrl->GetColumnCount() - 1 && wxGetMouseState().LeftIsDown()) {
        logCtrl->Freeze();
        logCtrlFreezed = true;
    }
}

void ClientApp::LoggerColumnDragged(wxListEvent& event) {
    if (event.GetColumn() >= 0 && event.GetColumn() < logCtrl->GetColumnCount() - 1)
    {
        logCtrl->SetColumnWidth(logCtrl->GetColumnCount() - 1, wxLIST_AUTOSIZE_USEHEADER);
    } else if (!frameDragged) {
        frameDragged = true;
        Bind(wxEVT_IDLE, &ClientApp::FrameIdle, this, wxID_ANY);
    }
}

void ClientApp::LoggerColumnEndDragged(wxListEvent& event) {
    logCtrl->SetColumnWidth(logCtrl->GetColumnCount() - 1, wxLIST_AUTOSIZE_USEHEADER);
    frameDragged = false;
    if (logCtrlFreezed) {
        logCtrl->Thaw();
        logCtrlFreezed = false;
    }
}

void ClientApp::FrameSizeChanged(wxSizeEvent& event) {
    logCtrl->SetColumnWidth(logCtrl->GetColumnCount() - 1, wxLIST_AUTOSIZE_USEHEADER);
    if (!frameDragged) {
        frameDragged = true;
        Bind(wxEVT_IDLE, &ClientApp::FrameIdle, this, wxID_ANY);
    }
    event.Skip();
}

void ClientApp::FrameIdle(wxIdleEvent& event) {
    if (frameDragged && !wxGetMouseState().LeftIsDown()) {
        logCtrl->SetColumnWidth(logCtrl->GetColumnCount() - 1, wxLIST_AUTOSIZE_USEHEADER);
        frameDragged = false;
        Unbind(wxEVT_IDLE, &ClientApp::FrameIdle, this, wxID_ANY);
    }
}

std::string ClientApp::SendRequest(const std::string srequest) {
        PrintMessage(srequest, Message::REQUEST);
        boost::asio::streambuf request;
        std::ostream request_stream(&request);

        request_stream << srequest << "\r\n";
        boost::asio::write(commandSocket, request);
        commandSocket.wait(commandSocket.wait_read);

        return PrintResponse();
}

void ClientApp::PrintMessage(const std::string& msg, Message type) {
    std::string stype;
    wxColour textColour;
    switch (type) {
        case Message::STATUS:
             stype = "Status";
             textColour = *wxBLACK;
             break;

        case Message::REQUEST:
             stype = "Request";
             textColour.Set(42, 82, 190);
             break;

        case Message::RESPONSE:
        {
            stype = "Response";

            std::string responseCode = boost::regex_replace(msg, boost::regex("^(?<code>[0-9]{3}).*\r"), "$+{code}", boost::format_all);;
            int responseCodeInt = responseCode[0] - '0';

            switch (responseCodeInt) {
            case 2:
                textColour.Set(23, 114, 69);
                break;
            case 1: case 3:
                textColour.Set(255, 79, 0);
                break;
            case 4: case 5:
                textColour = *wxRED;
                break;
            }

            break;
        }
        case Message::EC:
            stype = "Error";
            textColour = *wxRED;
            break;
    }
    long index = logCtrl->InsertItem(logCtrl->GetItemCount(), wxDateTime::Now().Format(wxT("%d.%m.%y %H:%M:%S"), wxDateTime::CET));
    logCtrl->SetItem(index, 1, stype);
    logCtrl->SetItem(index, 2, wxString::FromUTF8(msg));
    logCtrl->SetItemTextColour(index, textColour);
}

std::string ClientApp::PrintResponse() {
    boost::asio::streambuf response;
    boost::asio::read_until(commandSocket, response, boost::regex("\r\n"));

    std::istream response_stream(&response);
    std::string sresponse;
    while (response_stream.peek() != EOF && std::getline(response_stream, sresponse)) 
          PrintMessage(sresponse, Message::RESPONSE);

    return sresponse;
}

void ClientApp::ExtractFiles(const wxTreeItemId& rootItem) {
    try
    {
        tcp::socket dataSocket(io_context);
        std::string sport = boost::regex_replace(SendRequest("EPSV"), boost::regex(".+ \\([[:print:]]{3}(\\d{1,5})[[:print:]]\\)\r", boost::regex::perl), "$1", boost::format_all);

        tcp::resolver::query query_trsans(accessData[ID_HOST_DATA].ToStdString(), sport);
        tcp::resolver::results_type endpoints_trsans = resolver.resolve(query_trsans);
        tcp::resolver::iterator end;

        boost::system::error_code error = boost::asio::error::host_not_found;
        while (error && endpoints_trsans != end)
        {
            dataSocket.close();
            dataSocket.connect(*endpoints_trsans++, error);
        }
        if (error) throw boost::system::system_error(error);

        if (dataSocket.is_open()) {
            SendRequest("MLSD");
            
            ServerTreeCtrlItemData* itemData = (ServerTreeCtrlItemData*)serverTree->GetItemData(rootItem);
            std::string rootDirectory;
            rootDirectory = itemData->GetPath();

            boost::asio::streambuf response;
            std::istream response_stream(&response);
            std::string sresponse;

            for (;;) {
                boost::asio::read(dataSocket, response, boost::asio::transfer_at_least(1), error);
                if (error == boost::asio::error::eof) break;
                else if (error) throw error;
            }

            while (response_stream.peek() != EOF && std::getline(response_stream, sresponse))
            {
                std::string fileName = boost::regex_replace(sresponse, boost::regex(".+; (?<file>.+)\r"), "$+{file}");
                std::string type = boost::regex_replace(sresponse, boost::regex(".*type\=(\\w+);.*"), "$1", boost::format_all);
                std::string path = rootDirectory + "/" + fileName;
                if (type == "file") {
                    wxTreeItemId currentItem = serverTree->AppendItem(rootItem, wxString::FromUTF8(fileName), -1, -1, new ServerTreeCtrlFileData(path));
                    ServerTreeCtrlFileData* itemData = dynamic_cast<ServerTreeCtrlFileData*>(serverTree->GetItemData(currentItem));
                    serverTree->SetItemImage(currentItem, wxTheFileIconsTable->GetIconID(itemData->GetExtension()));
                }
                else if (type == "dir") {
                    wxTreeItemId currentItem = serverTree->AppendItem(rootItem, wxString::FromUTF8(fileName), -1, -1, new ServerTreeCtrlDirData(path));
                    serverTree->SetItemImage(currentItem, wxFileIconsTable::folder);
                }
            }
            response_stream.clear();
            serverTree->Expand(rootItem);
            dataSocket.close();
        }
        else throw std::exception("Connection to data socket is fail!");
    }
    catch (std::exception& e)
    {
        PrintMessage(e.what(), Message::EC);
    }
}

void ClientApp::Shutdown() {
    serverTree->Enable(false);
    serverTree->DeleteAllItems();

    commandSocket.close();
}