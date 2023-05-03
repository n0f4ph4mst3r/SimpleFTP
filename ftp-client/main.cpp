#include "ClientApp.h"
#include "main.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
    ClientApp* app = new ClientApp(wxT("SimpleFTP"));
    app->Show(true);

    return true;
}