#include "wxLogMessageItem.h"

wxLogMessageItem::wxLogMessageItem(const wxString& msg, Message type) {
	date = wxDateTime::Now().Format(wxT("%d.%m.%y %H:%M:%S"), wxDateTime::CET);

	switch (type) {
        case Message::STATUS:
            stype = "Status";
            colour.Set(0, 0, 0);
            break;

        case Message::REQUEST:
            stype = "Request";
            colour.Set(42, 82, 190);
            break;

        case Message::RESPONSE:
        {
            stype = "Response";

            std::string responseCode = boost::regex_replace(msg.ToStdString(), boost::regex("^(?<code>[0-9]{3}).*\r"), "$+{code}", boost::format_all);
            int responseCodeInt = responseCode[0] - '0';

            switch (responseCodeInt) {
            case 2:
                colour.Set(23, 114, 69);
                break;
            case 1: case 3:
                colour.Set(255, 79, 0);
                break;
            case 4: case 5:
                colour.Set(255, 0, 0);
                break;
            }

            break;
        }
        case Message::EC:
            stype = "Error";
            colour.Set(255, 0, 0);
            break;
    }

    message = wxString::FromUTF8(msg);
}