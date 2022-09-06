#include "wxTaskManager.h"

wxTaskManager::wxTaskManager(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
	: wxSplitterWindow(parent, id, pos, size, wxSP_3D) {
	//
	//     notebook
	//
	wxPanel* mainPanel = new wxPanel(this, wxWindow::NewControlId(), wxDefaultPosition, wxSize(700, 150));
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	wxNotebook* notebook = new wxNotebook(mainPanel, wxWindow::NewControlId(), wxDefaultPosition, size, wxNB_BOTTOM);
	
	m_queuededFilesCtrl = new wxDataViewListCtrl(notebook, wxWindow::NewControlId());
	m_failedTransfersCtrl = new wxDataViewListCtrl(notebook, wxWindow::NewControlId());
	m_succesfulTransfersCtrl = new wxDataViewListCtrl(notebook, wxWindow::NewControlId());

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
	mainPanel->SetSizer(mainSizer);

	//
	//     logCtrl
	//
	wxPanel* logCtrlPanel = new wxPanel(this, wxWindow::NewControlId(), wxDefaultPosition, wxSize(700, 150));

	m_logCtrl = new wxLogCtrl();
	m_logCtrl->EnableSystemTheme(false);
	m_logCtrl->Create(logCtrlPanel, wxWindow::NewControlId(), wxDefaultPosition, wxSize(700, -1), wxLC_REPORT | wxLC_VIRTUAL | wxLC_SINGLE_SEL);
	m_logCtrl->InitLog();

	wxBoxSizer* logCtrlSizer = new wxBoxSizer(wxVERTICAL);
	logCtrlSizer->Add(m_logCtrl, 1, wxEXPAND | wxALL);
	m_logCtrl->SetColumnWidth(0, 75);
	m_logCtrl->SetColumnWidth(1, 75);
	m_logCtrl->SetColumnWidth(2, 540);
	logCtrlPanel->SetSizer(logCtrlSizer);

	//split
	SplitHorizontally(mainPanel, logCtrlPanel);
	SetSashGravity(0.0);
	SetMinimumPaneSize(150);
}

void wxTaskManager::AddTask(const std::string& target, const std::string& source, const size_t size) {
	wxVector<wxVariant> data;
	data.push_back(wxVariant(target));
	data.push_back(wxVariant("<="));
	data.push_back(wxVariant(wxString::FromUTF8(source)));

	wxString ssize;
	ssize << size;
	data.push_back(wxVariant(ssize));

	data.push_back(wxVariant(long(0)));
	m_queuededFilesCtrl->AppendItem(data);
}

void wxTaskManager::UpdateTask(const size_t data) {
	std::stringstream sstream(m_queuededFilesCtrl->GetTextValue(0, m_queuededFilesCtrl->GetColumnCount() - 2).ToStdString());
	size_t totalSize;
	sstream >> totalSize;
	wxMutexGuiLocker lockGui = wxMutexGuiLocker();
	m_queuededFilesCtrl->SetValue(wxVariant((long)(data / totalSize * 100)), 0, m_queuededFilesCtrl->GetColumnCount() - 1);
}

void wxTaskManager::TaskCompleted() {
	wxVector<wxVariant> data;
	for (int i = 0; i < m_queuededFilesCtrl->GetColumnCount() - 1; ++i) {
		wxVariant var;
		m_queuededFilesCtrl->GetValue(var, 0, i);
		data.push_back(var);
	}
	data.push_back(wxVariant(wxDateTime::Now().Format(wxT("%d.%m.%y %H:%M:%S"), wxDateTime::CET)));
	m_queuededFilesCtrl->DeleteItem(0);

	m_succesfulTransfersCtrl->AppendItem(data);
}

void wxTaskManager::TaskFailed(const std::string& reason) {
	wxVector<wxVariant> data;
	for (int i = 0; i < m_queuededFilesCtrl->GetColumnCount() - 1; ++i) {
		wxVariant var;
		m_queuededFilesCtrl->GetValue(var, 0, i);
		data.push_back(var);
	}
	data.push_back(wxVariant(wxDateTime::Now().Format(wxT("%d.%m.%y %H:%M:%S"), wxDateTime::CET)));
	data.push_back(wxVariant(reason));
	m_queuededFilesCtrl->DeleteItem(0);

	m_failedTransfersCtrl->AppendItem(data);
}

void wxTaskManager::PrintMessage(const std::string& msg, Message type) {
	m_logCtrl->PrintMessage(msg, type);
}