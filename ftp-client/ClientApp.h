#pragma once
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>

#include <wx/wx.h>
#include <wx/dirctrl.h>
#include <wx/event.h>
#include <wx/mimetype.h>
#include <wx/datetime.h>
#include <wx/listctrl.h>
#include <wx/arrstr.h>
#include <wx/artprov.h>

#include <map>

#include "enumMessage.h"
#include "ServerTreeCtrltemData.h"

using boost::asio::ip::tcp;

class ClientApp : public wxFrame
{
 public:
    ClientApp(const wxString& title);
    wxDECLARE_EVENT_TABLE();
 private:
    //login, port and etc.
    std::map <int, wxString> accessData;

    boost::asio::io_context io_context;
    tcp::resolver resolver;
    tcp::socket commandSocket;

    wxListView* logCtrl;
    bool frameDragged = false;
    bool logCtrlFreezed = false;

    wxTreeCtrl* serverTree;
    wxGenericDirCtrl* clientTree;

    std::string SendRequest(const std::string request);
    std::string PrintResponse();
    void ExtractFiles(const wxTreeItemId& root);
    void PrintMessage(const std::string& msg, Message type = Message::STATUS);
    void Shutdown();

    //events
    void connectionClicked(wxCommandEvent& event);
    void disconnectionClicked(wxCommandEvent& event);
    void accessDataChanged(wxCommandEvent& event);
    void serverDirItemClicked(wxMouseEvent& event);
    void LoggerColumnBeginDragged(wxListEvent& event);
    void LoggerColumnDragged(wxListEvent& event);
    void LoggerColumnEndDragged(wxListEvent& event);
    void LoggerItemInsert(wxListEvent& event);
    void FrameSizeChanged(wxSizeEvent& event);
    void FrameIdle(wxIdleEvent& event);
};

