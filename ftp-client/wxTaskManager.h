#pragma once
#include <wx/splitter.h>
#include <wx/notebook.h>
#include <wx/dataview.h>

#include <sstream>

#include "wxLogCtrl.h"

class wxTaskManager : public wxSplitterWindow
{
private:
	wxDataViewListCtrl* m_queuededFilesCtrl;
	wxDataViewListCtrl* m_failedTransfersCtrl;
	wxDataViewListCtrl* m_succesfulTransfersCtrl;

	wxLogCtrl* m_logCtrl;

public:
	wxTaskManager(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(700, 300));

	void AddTask(const std::string& target, const std::string& source, const size_t size);
	void UpdateTask(const size_t data);
	void TaskCompleted();
	void TaskFailed(const std::string& reason);

	void PrintMessage(const std::string& msg, Message type = Message::STATUS);
};