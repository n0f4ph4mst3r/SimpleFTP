#include <boost/regex.hpp>
#include "wxServerTreeCtrltemData.h"

wxServerTreeCtrlItemData::wxServerTreeCtrlItemData(const std::string& path) {
    fullPath = path;
};

std::string wxServerTreeCtrlItemData::GetPath() {
    return fullPath;
}

wxServerTreeCtrlFileData::wxServerTreeCtrlFileData(const std::string& path) : wxServerTreeCtrlItemData (path) {
    extension = boost::regex_replace(fullPath, boost::regex("(\.[^.^/]+)+$"), "$1", boost::format_all);
}

std::string wxServerTreeCtrlFileData::GetExtension() {
    return extension;
}

wxServerTreeCtrlDirData::wxServerTreeCtrlDirData(const std::string& path) : wxServerTreeCtrlItemData(path) {
    dirExtracted = false;
}

bool wxServerTreeCtrlDirData::isExtracted() {
    return dirExtracted;
}

void wxServerTreeCtrlDirData::Deactivate() {
    dirExtracted = true;
}