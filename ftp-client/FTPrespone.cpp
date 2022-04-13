#include "FTPresponse.h"

FTPresponse::FTPresponse() {};

FTPresponse::FTPresponse (const std::string& str) {
	try {
		std::string scode = boost::regex_replace(str, boost::regex("^(?<code>[0-9]{3}).*\r"), "$+{code}", boost::format_all);
		if (scode.length() != 3) throw std::exception();

		code = std::stoi(scode);

		switch (scode[0] - '0') {
		case 2:
			isFine = true;
			isOk = true;
			break;
		case 1: case 3: 
			isFine = false;
			isOk = true;
			break;
		case 4: case 5:
			isFine = false;
			isOk = false;
			break;
		default:
			throw std::exception();
		}

		message = str;
	}
	catch (std::exception& e)
	{
		code = 0;
		message = "";
		isFine = false;
		isOk = false;
	}
}