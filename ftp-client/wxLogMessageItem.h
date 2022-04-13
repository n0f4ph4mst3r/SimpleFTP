#pragma once
#include <boost/regex.hpp>
#include <wx/datetime.h>
#include <wx/colour.h>

#include "enumMessage.h"

struct wxLogMessageItem {
    wxLogMessageItem(const wxString& msg, Message type = Message::STATUS);
    wxString date;
    wxString stype;
    wxString message;
    wxColour colour;
};