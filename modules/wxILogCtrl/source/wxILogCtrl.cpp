#include "wxILogCtrl.h"

wxBEGIN_EVENT_TABLE(wxILogCtrl, wxListCtrl)
EVT_LIST_COL_BEGIN_DRAG(wxID_ANY, wxILogCtrl::ColumnBeginDragged)
EVT_LIST_COL_DRAGGING(wxID_ANY, wxILogCtrl::ColumnDragged)
EVT_LIST_COL_END_DRAG(wxID_ANY, wxILogCtrl::ColumnEndDragged)
EVT_LIST_INSERT_ITEM(wxID_ANY, wxILogCtrl::ItemInsert)
EVT_LIST_ITEM_RIGHT_CLICK(wxID_ANY, wxILogCtrl::ItemContext)
wxEND_EVENT_TABLE()

wxILogCtrl::wxILogCtrl() : wxListCtrl() {};

wxILogCtrl::wxILogCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
    : wxListCtrl(parent, id, pos, size, wxLC_REPORT | wxLC_VIRTUAL | wxLC_SINGLE_SEL) {
}

void wxILogCtrl::ColumnBeginDragged(wxListEvent& event) {
    #if defined(__WXMSW__)
    if (event.GetColumn() == GetColumnCount() - 1 && wxGetMouseState().LeftIsDown()) {
        Freeze();
        m_IsFreezed = true;
    }
    #endif
}

void wxILogCtrl::ColumnDragged(wxListEvent& event) {
    #if defined(__WXMSW__)
    if (event.GetColumn() >= 0 && event.GetColumn() < GetColumnCount() - 1) {
        SetColumnWidth(GetColumnCount() - 1, wxLIST_AUTOSIZE_USEHEADER);
    }
    else if (!m_IsDragged) {
        m_IsDragged = true;
        Bind(wxEVT_IDLE, &wxILogCtrl::ListIdle, this, wxID_ANY);
    }
    #endif
}

void wxILogCtrl::ColumnEndDragged(wxListEvent& event) {
    #if defined(__WXMSW__)
    SetColumnWidth(GetColumnCount() - 1, wxLIST_AUTOSIZE_USEHEADER);
    m_IsDragged = false;
    if (m_IsFreezed) {
        Thaw();
        m_IsFreezed = false;
    }
    #endif
}

void wxILogCtrl::ItemInsert(wxListEvent& event) {
    #if defined(__WXMSW__)
    SetColumnWidth(GetColumnCount() - 1, wxLIST_AUTOSIZE_USEHEADER);
    #endif
}

void wxILogCtrl::ItemContext(wxListEvent& event) {
    void* pdata = reinterpret_cast<void*>(event.GetItem().GetData());
    wxMenu mnu;
    mnu.SetClientData(pdata);
    wxMenuItem* deleteItem = new wxMenuItem(&mnu, wxID_LOGGER_CLEAR, _("Clear"));
    deleteItem->SetBitmap(wxArtProvider::GetBitmap(wxART_DELETE, wxART_MENU, wxSize(16, 16)));
    mnu.Append(deleteItem);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &wxILogCtrl::OnPopupClick, this, wxID_LOGGER_CLEAR);
    PopupMenu(&mnu);
}

void wxILogCtrl::OnPopupClick(wxCommandEvent& event) {
    void* pdata = static_cast<wxMenu*>(event.GetEventObject())->GetClientData();
    DeleteAllItems();
}

void wxILogCtrl::SizeChanged(wxSizeEvent& event) {
    SetColumnWidth(GetColumnCount() - 1, wxLIST_AUTOSIZE_USEHEADER);
    if (!m_IsDragged) {
        m_IsDragged = true;
        Bind(wxEVT_IDLE, &wxILogCtrl::ListIdle, this, wxID_ANY);
    }
    event.Skip();
}

void wxILogCtrl::ListIdle(wxIdleEvent& event) {
    if (m_IsDragged && !wxGetMouseState().LeftIsDown()) {
        SetColumnWidth(GetColumnCount() - 1, wxLIST_AUTOSIZE_USEHEADER);
        m_IsDragged = false;
        Unbind(wxEVT_IDLE, &wxILogCtrl::ListIdle, this, wxID_ANY);
    }
    event.Skip();
}

void wxILogCtrl::InitLog() {
    #if defined(__WXMSW__)
    Bind(wxEVT_SIZE, &wxILogCtrl::SizeChanged, this);
    #endif
}