#include "wxFTPWrapper.h"

wxFTPWrapper::wxFTPWrapper(wxLogCtrl& log) : AsioClientFTP(), logCtrl(log) {}

void wxFTPWrapper::PrintMessage(const std::string& msg, Message type) {
	if (msg != "") logCtrl.PrintMessage(msg, type);
}

