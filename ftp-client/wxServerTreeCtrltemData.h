#pragma once
#include <wx/treebase.h>

class wxServerTreeCtrlItemData : public wxTreeItemData {
public:
    wxServerTreeCtrlItemData(const std::string& path);
    std::string GetPath();
protected:
    std::string fullPath;
};

class wxServerTreeCtrlFileData : public wxServerTreeCtrlItemData {
public:
    wxServerTreeCtrlFileData(const std::string& path);
    std::string GetExtension();
private:
    std::string extension;
};

class wxServerTreeCtrlDirData : public wxServerTreeCtrlItemData {
public:
    wxServerTreeCtrlDirData(const std::string& path);
    bool isExtracted();
    void Deactivate();
private:
    bool dirExtracted;
};