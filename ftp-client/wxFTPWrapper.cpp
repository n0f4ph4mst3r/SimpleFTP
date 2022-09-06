#include "wxFTPWrapper.h"

wxFTPWrapper::wxFTPWrapper(wxTaskManager* taskManager) : IAsioClientFTP(),  m_taskManager(taskManager) {}

void wxFTPWrapper::PrintMessage(const std::string& msg, Message type) {
	if (msg != "") m_taskManager->PrintMessage(msg, type);
}

void wxFTPWrapper::UpdateTask(const size_t data) {
	m_taskManager->UpdateTask(data);
}

void wxFTPWrapper::TaskCompleted() {
	wxMutexGuiLocker lockGui = wxMutexGuiLocker();
	m_taskManager->TaskCompleted();
}

void wxFTPWrapper::TaskFailed(const std::string& reason) {
	wxMutexGuiLocker lockGui = wxMutexGuiLocker();
	m_taskManager->TaskFailed(reason);
}

