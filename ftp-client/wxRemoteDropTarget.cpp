#include "wxRemoteDropTarget.h"

wxRemoteDropTarget::wxRemoteDropTarget(wxRemoteTreeCtrl* serverTree, wxLocalDirCtrl* clientTree) {
    m_serverTree = serverTree;
    m_clientCtrl = clientTree;
};

bool wxRemoteDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString& data)
{
    wxTreeCtrl* clientTree = m_clientCtrl->GetTreeCtrl();

    int nHitTest = wxTREE_HITTEST_ONITEMLABEL;
    m_targetItem = clientTree->HitTest(wxPoint(x, y), nHitTest);
    if (!wxDir::Exists(m_clientCtrl->GetPath(m_targetItem)) && clientTree->GetItemParent(m_targetItem) != m_clientCtrl->GetRootId())
        m_targetItem = clientTree->GetItemParent(m_targetItem);

    m_remoteItem = m_serverTree->GetFocusedItem();
    return true;
}

wxDragResult wxRemoteDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult defResult)
{
    int nHitTest = wxTREE_HITTEST_ONITEMLABEL;
    wxTreeItemId previosItem;
    if (m_targetItem) previosItem = m_targetItem;
    wxTreeCtrl* tree = m_clientCtrl->GetTreeCtrl();
    m_targetItem = tree->HitTest(wxPoint(x, y), nHitTest);

    if (m_targetItem && (m_targetItem != previosItem))
    {
        wxString path = m_clientCtrl->GetPath(m_targetItem);
        if (wxDir::Exists(path) || tree->GetItemParent(m_targetItem) == m_clientCtrl->GetRootId()) {
            tree->SetItemDropHighlight(m_targetItem);
            tree->SelectItem(m_targetItem);
        } 
        if (previosItem) tree->SetItemDropHighlight(previosItem, false);
    }
    return defResult;
}

wxTreeItemId wxRemoteDropTarget::GetTargetID() {
    return m_targetItem;
}

wxTreeItemId wxRemoteDropTarget::GetSourceID() {
    return m_remoteItem;
}

