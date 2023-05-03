#pragma once
#include <string>

struct wxTransferTransaction {
	wxTransferTransaction(const std::string& lcl, const std::string& rmt, const bool bfile, const size_t sz = 0, const bool direction = true) {
		local = lcl;
		remote = rmt;
		isFile = bfile;
		size = sz;
		bDirection = direction;
		progress = 0;
	};
	std::string local;
	std::string remote;
	bool isFile;
	bool bDirection;
	size_t size;
	size_t progress;
};

