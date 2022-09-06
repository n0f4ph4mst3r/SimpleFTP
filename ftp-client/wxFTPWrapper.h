#pragma once
#include "IAsioClientFTP.h"
#include "wxTaskManager.h"

class wxFTPWrapper : public IAsioClientFTP
{
private:
	wxTaskManager* m_taskManager;
public:
	wxFTPWrapper(wxTaskManager* taskManger);
	virtual void PrintMessage(const std::string& msg, Message type = Message::STATUS) override;
	virtual void UpdateTask(const size_t data) override;
	virtual void TaskCompleted() override;
	virtual void TaskFailed(const std::string& reason) override;
};
