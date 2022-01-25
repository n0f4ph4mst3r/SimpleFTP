#include "main.h"
#include "ClientApp.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
    ClientApp* app = new ClientApp(wxT("FTP-client"));
    app->Show(true);

    return true;
}