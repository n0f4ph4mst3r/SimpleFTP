#include "FTPresponse.h"

FTPresponse::FTPresponse() {};

FTPresponse::FTPresponse (const std::string& str) {
	try {
		std::string scode = boost::regex_replace(str, boost::regex("^(?<code>[0-9]{3}).*\r"), "$+{code}", boost::format_all);
		if (scode.length() != 3) throw std::exception();

		ncode = std::stoi(scode);

		switch (scode[0] - '0') {
		case 2:
			IsFine = true;
			break;
		case 1: case 3: case 4: case 5:
			IsFine = false;
			break;
		default:
			throw std::exception();
		}

		message = str;
	}
	catch (std::exception& e)
	{
		ncode = 0;
		message = "";
		IsFine = false;
	}
}