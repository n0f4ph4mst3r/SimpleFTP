#include <boost/regex.hpp>
#include "wxRemoteTreeCtrlItemData.h"

wxRemoteTreeCtrlItemData::wxRemoteTreeCtrlItemData(const std::string& path) {
    m_path = path;
};

std::string wxRemoteTreeCtrlItemData::GetPath() {
    return m_path.ToStdString();
}

wxRemoteFileData::wxRemoteFileData(const std::string& path, const size_t size) : wxRemoteTreeCtrlItemData (path), m_size(size) {
    m_ext = m_path.AfterLast(wxT('.'));
}

std::string wxRemoteFileData::GetExtension() {
    return m_ext.ToStdString();
}

size_t wxRemoteFileData::GetSize() {
    return m_size;
}

wxRemoteDirData::wxRemoteDirData(const std::string& path) : wxRemoteTreeCtrlItemData(path) {
    m_IsExtracted = false;
}

bool wxRemoteDirData::isExtracted() {
    return m_IsExtracted;
}

void wxRemoteDirData::Deactivate() {
    m_IsExtracted = true;
}