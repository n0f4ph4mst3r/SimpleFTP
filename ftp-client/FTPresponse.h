#pragma once
#include <string>
#include <boost/regex.hpp>

struct FTPresponse {
	FTPresponse();
	FTPresponse(const std::string& str);
	std::string message;
	int code;
	bool isOk;
	bool isFine;
};