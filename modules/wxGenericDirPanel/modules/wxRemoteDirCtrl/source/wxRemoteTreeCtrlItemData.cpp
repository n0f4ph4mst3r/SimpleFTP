#include <boost/locale.hpp>
#include <sstream>
#include "wxRemoteTreeCtrlItemData.h"

wxRemoteTreeCtrlItemData::wxRemoteTreeCtrlItemData(const std::string& path) {
    SetPath(path);
};

void wxRemoteTreeCtrlItemData::SetPath(const std::string& path) {
#if defined(__WXMSW__) 
    m_path = path;
#else
    m_path = wxString::FromUTF8(path);
#endif
};

std::string wxRemoteTreeCtrlItemData::GetPath() {
#if defined(__WXMSW__) 
    return m_path.ToStdString();
#else
    boost::locale::generator gen;
    std::stringstream ss;
    ss.imbue(gen("he_IL.UTF-8"));
    ss << m_path;
    return ss.str();
#endif
}

wxRemoteFileData::wxRemoteFileData(const std::string& path, const size_t size) : wxRemoteTreeCtrlItemData (path), m_size(size) {
    m_ext = m_path.AfterLast(wxT('.'));
}

std::string wxRemoteFileData::GetExtension() {
#if defined(__WXMSW__) 
    return m_ext.ToStdString();
#else
    boost::locale::generator gen;
    std::stringstream ss;
    ss.imbue(gen("he_IL.UTF-8"));
    ss << m_ext;
    return ss.str();
#endif
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