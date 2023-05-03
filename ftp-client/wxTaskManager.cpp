#include "wxTaskManager.h"

wxTaskManager::wxTaskManager(std::shared_ptr<wxFTPWrapper> pclient, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
	: wxSplitterWindow(parent, id, pos, size, wxSP_3D | wxSP_LIVE_UPDATE), m_owner(pclient) {
	//
	//     notebook
	//
	wxPanel* mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	wxNotebook* notebook = new wxNotebook(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);

	m_queuededFilesCtrl = new wxDataViewListCtrl(notebook, wxID_ANY);
	m_failedTransfersCtrl = new wxDataViewListCtrl(notebook, wxID_ANY);
	m_succesfulTransfersCtrl = new wxDataViewListCtrl(notebook, wxID_ANY);

	m_queuededFilesCtrl->AppendTextColumn("Server/Local file", wxDATAVIEW_CELL_INERT, 250);
	m_failedTransfersCtrl->AppendTextColumn("Server/Local file", wxDATAVIEW_CELL_INERT, 200);
	m_succesfulTransfersCtrl->AppendTextColumn("Server/Local file", wxDATAVIEW_CELL_INERT, 250);

	m_queuededFilesCtrl->AppendTextColumn("Direction");
	m_failedTransfersCtrl->AppendTextColumn("Direction");
	m_succesfulTransfersCtrl->AppendTextColumn("Direction");

	m_queuededFilesCtrl->AppendTextColumn("Remote file", wxDATAVIEW_CELL_INERT, 150);
	m_failedTransfersCtrl->AppendTextColumn("Remote file", wxDATAVIEW_CELL_INERT, 150);
	m_succesfulTransfersCtrl->AppendTextColumn("Remote file", wxDATAVIEW_CELL_INERT, 150);

	m_queuededFilesCtrl->AppendTextColumn("Size");
	m_failedTransfersCtrl->AppendTextColumn("Size");
	m_succesfulTransfersCtrl->AppendTextColumn("Size");

	m_queuededFilesCtrl->AppendProgressColumn("Progress");

	m_failedTransfersCtrl->AppendTextColumn("Date");
	m_succesfulTransfersCtrl->AppendTextColumn("Date");

	m_failedTransfersCtrl->AppendTextColumn("Reason", wxDATAVIEW_CELL_INERT, 75);

	notebook->AddPage(m_queuededFilesCtrl, "Queueded Files", true, -1);
	notebook->AddPage(m_failedTransfersCtrl, "Failed Transfers", false, -1);
	notebook->AddPage(m_succesfulTransfersCtrl, "Succesful Transfers", false, -1);

	mainSizer->Add(notebook, 1, wxEXPAND | wxALL);
	mainSizer->SetMinSize(wxSize(700, 150));
	mainPanel->SetSizer(mainSizer);
	mainPanel->SetMinSize(wxSize(700, 150));

	m_failedTransfersCtrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &wxTaskManager::ItemContext, this);
	m_succesfulTransfersCtrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &wxTaskManager::ItemContext, this);
	//
	//     logCtrl
	//
	wxPanel* logCtrlPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_logCtrl = new wxLogCtrl();
	m_logCtrl->EnableSystemTheme(false);
	m_logCtrl->Create(logCtrlPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_VIRTUAL | wxLC_SINGLE_SEL);
	m_logCtrl->InitLog();

	wxBoxSizer* logCtrlSizer = new wxBoxSizer(wxVERTICAL);
	logCtrlSizer->SetMinSize(wxSize(700, 150));
	logCtrlSizer->Add(m_logCtrl, 1, wxEXPAND | wxALL);

	m_logCtrl->SetColumnWidth(0, 75);
	m_logCtrl->SetColumnWidth(1, 75);
	m_logCtrl->SetColumnWidth(2, 540);

	logCtrlPanel->SetSizer(logCtrlSizer);
	logCtrlPanel->SetMinSize(wxSize(700, 150));

	//split
	SplitHorizontally(mainPanel, logCtrlPanel);
	SetMinimumPaneSize(20);
	SetSashGravity(0.5);

	m_owner->Bind(wxTASK_ADDED, &wxTaskManager::TaskAdded, this);
	m_owner->Bind(wxTASK_UPDATED, &wxTaskManager::TaskUpdated, this);
	m_owner->Bind(wxTASK_COMPLETED, &wxTaskManager::TaskCompleted, this);
	m_owner->Bind(wxTASK_FAILED, &wxTaskManager::TaskFailed, this);

	m_owner->Bind(wxPRINT_MESSAGE, &wxTaskManager::PrintMessage, this);
}

void wxTaskManager::TaskToVariant(const wxTransferTransaction& task, wxVector<wxVariant>& data) {
	data.push_back(wxVariant(task.local));
	if (task.bDirection) data.push_back(wxVariant("<="));
	else data.push_back(wxVariant("=>"));
	data.push_back(wxVariant(wxString::FromUTF8(task.remote)));

	wxString ssize;
	ssize << task.size;
	data.push_back(wxVariant(ssize));
}

void wxTaskManager::ItemContext(wxDataViewEvent& event) {
	void* pdata = reinterpret_cast<void*>(event.GetEventObject());
	wxMenu mnu;
	mnu.SetClientData(pdata);
	wxMenuItem* deleteItem = new wxMenuItem(&mnu, wxID_LOGGER_CLEAR, _("Clear"));
	deleteItem->SetBitmap(wxArtProvider::GetBitmap(wxART_DELETE, wxART_MENU, wxSize(16, 16)));
	mnu.Append(deleteItem);
	wxDataViewListCtrl* plist = dynamic_cast<wxDataViewListCtrl*>(event.GetEventObject());
	Bind(wxEVT_COMMAND_MENU_SELECTED, [&](wxCommandEvent& event) {plist->DeleteAllItems();}, wxID_LOGGER_CLEAR);
	PopupMenu(&mnu);
}

void wxTaskManager::TaskAdded(wxCommandEvent& event) {
	wxTransferTransaction* ptask = static_cast<wxTransferTransaction*>(event.GetClientData());

	wxVector<wxVariant> data;
	TaskToVariant(*ptask, data);
	data.push_back(wxVariant(long(0)));

	m_queuededFilesCtrl->AppendItem(data);
	event.SetClientData(NULL);
}

void wxTaskManager::TaskUpdated(wxThreadEvent& event) {
	m_queuededFilesCtrl->SetValue(wxVariant(event.GetExtraLong()), 0, m_queuededFilesCtrl->GetColumnCount() - 1);
}

void wxTaskManager::TaskCompleted(wxThreadEvent& event) {
	wxTransferTransaction task = event.GetPayload<wxTransferTransaction>();

	wxVector<wxVariant> data;
	TaskToVariant(task, data);
	data.push_back(wxVariant(wxDateTime::Now().Format(wxT("%d.%m.%y %H:%M:%S"), wxDateTime::Local)));

	m_queuededFilesCtrl->DeleteItem(0);
	m_succesfulTransfersCtrl->AppendItem(data);
}

void wxTaskManager::TaskFailed(wxThreadEvent& event) {
	auto task = event.GetPayload <std::pair<wxTransferTransaction, std::string>> ();

	wxVector<wxVariant> data;
	TaskToVariant(task.first, data);
	data.push_back(wxVariant(wxDateTime::Now().Format(wxT("%d.%m.%y %H:%M:%S"), wxDateTime::Local)));
	data.push_back(wxVariant(task.second));

	m_queuededFilesCtrl->DeleteItem(0);
	m_failedTransfersCtrl->AppendItem(data);
}

void wxTaskManager::PrintMessage(wxCommandEvent& event) {
	wxLogMessageItem* item = static_cast<wxLogMessageItem*>(event.GetClientData());
	m_logCtrl->PrintMessage(*item);
	event.SetClientData(NULL);
}