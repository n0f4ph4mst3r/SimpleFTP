#include "wxRemoteDropTarget.h"

wxRemoteDropTarget::wxRemoteDropTarget(wxRemoteDirCtrl* serverCtrl, wxLocalDirCtrl* clientTree) : wxTreeDropTarget(serverCtrl, clientTree) {}

bool wxRemoteDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data) {
    if (isActive) {
        int nHitTest = wxTREE_HITTEST_ONITEMLABEL;
        wxTreeCtrl* serverTree = m_serverCtrl->GetTreeCtrl();
        m_targetItem = serverTree->HitTest(wxPoint(x, y), nHitTest);
        if (!m_targetItem) return false;
        if (dynamic_cast<wxRemoteFileData*>(serverTree->GetItemData(m_targetItem))) m_targetItem = serverTree->GetItemParent(m_targetItem);
        serverTree->SelectItem(m_targetItem);
    }
    else return false;

    return true;
}

wxDragResult wxRemoteDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult defResult) {
    if (isActive) {
        int nHitTest = wxTREE_HITTEST_ONITEMLABEL;
        wxTreeItemId previousItem = m_targetItem;
        wxTreeCtrl* serverTree = m_serverCtrl->GetTreeCtrl();
        m_targetItem = serverTree->HitTest(wxPoint(x, y), nHitTest);
        if (!m_targetItem) {
            if (previousItem) serverTree->SetItemDropHighlight(previousItem, false);
            return wxDragNone;
        }

        if (m_targetItem != previousItem) {
            if (dynamic_cast<wxRemoteDirData*>(serverTree->GetItemData(m_targetItem)) || serverTree->GetItemParent(m_targetItem) == serverTree->GetRootItem()) {
                serverTree->SetItemDropHighlight(m_targetItem);
            }

            if (previousItem) serverTree->SetItemDropHighlight(previousItem, false);
        }
    }
    else return wxDragNone;

    return defResult;
}

void wxRemoteDropTarget::SetPath(const wxString& path) {
    m_path = path;
}

wxString wxRemoteDropTarget::GetPath() {
    return m_path;
}

