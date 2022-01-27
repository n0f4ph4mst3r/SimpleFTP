#pragma once
#include <wx/wx.h>
#include <wx/dirctrl.h>

class ClientApp : public wxFrame
{
public:
    ClientApp(const wxString& title);
    wxDECLARE_EVENT_TABLE();
};
