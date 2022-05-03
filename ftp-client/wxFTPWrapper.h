#pragma once
#include "AsioClientFTP.h"
#include "wxLogCtrl.h"

class wxFTPWrapper : public AsioClientFTP
{
public: 
	wxFTPWrapper(wxLogCtrl& logCtrl);
	virtual void PrintMessage(const std::string& msg, Message type = Message::STATUS) override;
private:
	wxLogCtrl& logCtrl;
};