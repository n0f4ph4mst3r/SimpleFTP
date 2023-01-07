#pragma once
#include "wxFTPWrapper.h"

#include <wx/splitter.h>
#include <wx/notebook.h>
#include <wx/dataview.h>

#include <sstream>

#include "wxLogCtrl.h"

class wxTaskManager : public wxSplitterWindow
{
private:
	std::shared_ptr<wxFTPWrapper> m_owner;

	wxDataViewListCtrl* m_queuededFilesCtrl;
	wxDataViewListCtrl* m_failedTransfersCtrl;
	wxDataViewListCtrl* m_succesfulTransfersCtrl;

	wxLogCtrl* m_logCtrl;

	void TaskToVariant(const wxTransferTransaction& task, wxVector<wxVariant>& data);

	//events
	void ItemContext(wxDataViewEvent& event);

	void TaskAdded(wxCommandEvent& event);
	void TaskUpdated(wxThreadEvent& event);
	void TaskCompleted(wxThreadEvent& event);
	void TaskFailed(wxThreadEvent& event);

	void PrintMessage(wxCommandEvent& event);

	enum {
		wxID_LOGGER_CLEAR = wxID_HIGHEST + 1
	};
public:
	wxTaskManager(std::shared_ptr<wxFTPWrapper> pclient, wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(700, 300));
};