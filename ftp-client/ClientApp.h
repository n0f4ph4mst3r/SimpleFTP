#pragma once
#include <wx/wx.h>

class ClientApp : public wxFrame
{
public:
    ClientApp(const wxString& title);
    wxDECLARE_EVENT_TABLE();

private:
    void dirComboBoxOnCreate(wxCommandEvent& event);
};
