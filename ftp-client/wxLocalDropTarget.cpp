#include "wxLocalDropTarget.h"

wxLocalDropTarget::wxLocalDropTarget(wxRemoteDirCtrl* serverTree, wxLocalDirCtrl* clientTree) : wxTreeDropTarget(serverTree, clientTree) {}

bool wxLocalDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data) {
    if (isActive) {
        int nHitTest = wxTREE_HITTEST_ONITEMLABEL;
        wxTreeCtrl* clientTree = m_clientCtrl->GetTreeCtrl();
        m_targetItem = clientTree->HitTest(wxPoint(x, y), nHitTest);
        if (!m_targetItem) return false;
        if (!wxDir::Exists(m_clientCtrl->GetPath(m_targetItem)) && clientTree->GetItemParent(m_targetItem) != m_clientCtrl->GetRootId())
            m_targetItem = clientTree->GetItemParent(m_targetItem);
        m_sourceItem = m_serverCtrl->GetTreeCtrl()->GetFocusedItem();
    }
    else return false;

    return true;
}

wxDragResult wxLocalDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult defResult) {
    if (isActive) {
        int nHitTest = wxTREE_HITTEST_ONITEMLABEL;
        wxTreeItemId previousItem = m_targetItem;
        wxTreeCtrl* clientTree = m_clientCtrl->GetTreeCtrl();
        m_targetItem = clientTree->HitTest(wxPoint(x, y), nHitTest);
        if (!m_targetItem) {
            if (previousItem) clientTree->SetItemDropHighlight(previousItem, false);
            return wxDragNone;
        }

        if (m_targetItem != previousItem) {
            wxString path = m_clientCtrl->GetPath(m_targetItem);
            if (wxDir::Exists(path) || clientTree->GetItemParent(m_targetItem) == m_clientCtrl->GetRootId()) {
                clientTree->SetItemDropHighlight(m_targetItem);
                clientTree->SelectItem(m_targetItem);
            }

            if (previousItem) clientTree->SetItemDropHighlight(previousItem, false);
        }
    }
    else return wxDragNone;

    return defResult;
}

wxTreeItemId wxLocalDropTarget::GetSourceID() {
    return m_sourceItem;
}

