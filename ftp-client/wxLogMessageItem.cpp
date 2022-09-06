#include "wxLogMessageItem.h"

wxLogMessageItem::wxLogMessageItem(const wxString& msg, Message type) {
	m_date = wxDateTime::Now().Format(wxT("%d.%m.%y %H:%M:%S"), wxDateTime::CET);

	switch (type) {
        case Message::STATUS:
            m_stype = "Status";
            m_colour.Set(0, 0, 0);
            break;

        case Message::REQUEST:
            m_stype = "Request";
            m_colour.Set(42, 82, 190);
            break;

        case Message::RESPONSE:
        {
            m_stype = "Response";

            std::string sresponseCode = boost::regex_replace(msg.ToStdString(), boost::regex("^(?<code>[0-9]{3}).*\r"), "$+{code}", boost::format_all);
            int responseCodeType = sresponseCode[0] - '0';

            switch (responseCodeType) {
            case 2:
                m_colour.Set(23, 114, 69);
                break;
            case 1: case 3:
                m_colour.Set(255, 79, 0);
                break;
            case 4: case 5:
                m_colour.Set(255, 0, 0);
                break;
            }

            break;
        }
        case Message::EC:
            m_stype = "Error";
            m_colour.Set(255, 0, 0);
            break;
    }

    m_message = wxString::FromUTF8(msg);
}