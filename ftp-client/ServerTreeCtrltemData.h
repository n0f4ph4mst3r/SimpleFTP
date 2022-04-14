#pragma once
#include <wx/treebase.h>

enum class ItemType {
    FILE,
    DIR,
    ITEM
};

class ServerTreeCtrlItemData : public wxTreeItemData {
public:
    ServerTreeCtrlItemData(const std::string& path);
    ItemType GetType();
    std::string GetPath();
protected:
    virtual void SetType();
    ItemType type;
    std::string fullPath;
};

class ServerTreeCtrlFileData : public ServerTreeCtrlItemData {
public:
    ServerTreeCtrlFileData(const std::string& path);
    std::string GetExtension();
protected:
    void SetType() override;
private:
    std::string extension;
};

class ServerTreeCtrlDirData : public ServerTreeCtrlItemData {
public:
    ServerTreeCtrlDirData(const std::string& path);
    bool isExtracted();
    void Deactivate();
protected:
    void SetType() override;
private:
    bool dirExtracted;
};