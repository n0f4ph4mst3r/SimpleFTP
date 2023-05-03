#include "wxTreeDropTarget.h"

wxTreeDropTarget::wxTreeDropTarget(wxRemoteDirCtrl* serverCtrl, wxLocalDirCtrl* clientCtrl) {
    m_serverCtrl = serverCtrl;
    m_clientCtrl = clientCtrl;
};

bool wxTreeDropTarget::OnDrop(wxCoord x, wxCoord y) {
    if (!m_targetItem) return false;
    return true;
}

void wxTreeDropTarget::OnLeave() {
    if (m_targetItem) m_serverCtrl->GetTreeCtrl()->SetItemDropHighlight(m_targetItem, false);
}

void wxTreeDropTarget::Activate() {
    isActive = true;
}

void wxTreeDropTarget::Deactivate() {
    isActive = false;
}

wxTreeItemId wxTreeDropTarget::GetTargetID() {
    return m_targetItem;
}


