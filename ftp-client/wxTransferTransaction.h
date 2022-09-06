#pragma once
#include <string>

struct wxTransferTransaction {
	wxTransferTransaction(const std::string& trgt, const std::string& src, const bool bType, const size_t sz = 0, const bool direction = true) {
		target = trgt;
		source = src;
		isFile = bType;
		size = sz;
		bDirection = direction;
	};
	std::string target;
	std::string source;
	bool isFile;
	bool bDirection;
	size_t size;
};

