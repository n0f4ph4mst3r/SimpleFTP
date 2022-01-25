#include "ClientApp.h"

ClientApp::ClientApp(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title) {
    wxInitAllImageHandlers();

    Centre();
}