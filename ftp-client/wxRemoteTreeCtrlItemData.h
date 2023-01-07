#pragma once
#include <wx/treebase.h>

class wxRemoteTreeCtrlItemData : public wxTreeItemData {
protected:
    wxString m_path;
public:
    wxRemoteTreeCtrlItemData(const std::string& path);
    void SetPath(const std::string& path);
    std::string GetPath();
};

class wxRemoteFileData : public wxRemoteTreeCtrlItemData {
private:
    wxString m_ext;
    size_t m_size;
public:
    wxRemoteFileData(const std::string& path, const size_t size);
    std::string GetExtension();
    size_t GetSize();
};

class wxRemoteDirData : public wxRemoteTreeCtrlItemData {
private:
    bool m_IsExtracted;
public:
    wxRemoteDirData(const std::string& path);
    bool isExtracted();
    void Deactivate();
};