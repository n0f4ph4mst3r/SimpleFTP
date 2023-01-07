#pragma once
#include <wx/event.h>
#include <wx/thread.h>

#include "IAsioClientFTP.h"
#include "wxTransferTransaction.h"
#include "wxLogMessageItem.h"

wxDECLARE_EVENT(wxEVT_COMMAND_CONNECT_SOCKET, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_CLOSE_SOCKET, wxCommandEvent);
wxDECLARE_EVENT(wxTASK_ADDED, wxCommandEvent);
wxDECLARE_EVENT(wxTASK_UPDATED, wxThreadEvent);
wxDECLARE_EVENT(wxTASK_COMPLETED, wxThreadEvent);
wxDECLARE_EVENT(wxTASK_FAILED, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_TRANSFER_COMPLETED, wxCommandEvent);
wxDECLARE_EVENT(wxPRINT_MESSAGE, wxCommandEvent);

class wxFTPWrapper : public IAsioClientFTP, public wxEvtHandler, public wxThreadHelper
{
private:
	std::list<wxTransferTransaction> m_queuededTasks;
	wxCriticalSection m_critsect;
	bool m_shutdown;

	void TaskPopFront();
	virtual wxThread::ExitCode Entry();

	void OnCloseSocket(wxCommandEvent& event);
protected:
	virtual void UpdateTask(const int offset) override;
	virtual void TaskCompleted() override;
	virtual void TaskFailed(const std::string& reason) override;
public:
	wxFTPWrapper();
	virtual void PrintMessage(const std::string& msg, Message type = Message::STATUS) override;

	virtual bool Connect(const std::string& host, const std::string& user, const std::string& pass, const std::string& port = "21") override;
	virtual void Shutdown() override;

	void AddTask(const wxTransferTransaction& task);
	std::list<wxTransferTransaction> GetTasks();
};
