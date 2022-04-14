#include <boost/regex.hpp>
#include "ServerTreeCtrltemData.h"

ServerTreeCtrlItemData::ServerTreeCtrlItemData(const std::string& path) {
    fullPath = std::move(path);
    SetType();
};

std::string ServerTreeCtrlItemData::GetPath() {
    return fullPath;
}

void ServerTreeCtrlItemData::SetType() {
    type = ItemType::ITEM;
}

ItemType ServerTreeCtrlItemData::GetType() {
    return type;
}


ServerTreeCtrlFileData::ServerTreeCtrlFileData(const std::string& path) : ServerTreeCtrlItemData (path) {
    extension = boost::regex_replace(fullPath, boost::regex("(\.[^.^/]+)+$"), "$1", boost::format_all);
}

void ServerTreeCtrlFileData::SetType() {
    type = ItemType::FILE;
}

std::string ServerTreeCtrlFileData::GetExtension() {
    return extension;
}

ServerTreeCtrlDirData::ServerTreeCtrlDirData(const std::string& path) : ServerTreeCtrlItemData(path) {
    dirExtracted = false;
}

void ServerTreeCtrlDirData::SetType() {
    type = ItemType::DIR;
}

bool ServerTreeCtrlDirData::isExtracted() {
    return dirExtracted;
}

void ServerTreeCtrlDirData::Deactivate() {
    dirExtracted = true;
}