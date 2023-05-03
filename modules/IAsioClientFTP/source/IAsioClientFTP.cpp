#include "IAsioClientFTP.h"

using boost::asio::ip::tcp;

IAsioClientFTP::IAsioClientFTP() : m_resolver(m_context), m_commandSocket(m_context) {}

bool IAsioClientFTP::Connect() {
    try {
        PrintMessage((boost::format("Connecting to %d...") % m_host).str()); 

        tcp::resolver::query query(m_host, m_port);

        tcp::resolver::results_type endpoints = m_resolver.resolve(m_host, "ftp");
        tcp::resolver::iterator endpoint_iterator = m_resolver.resolve(query);
        tcp::resolver::iterator end;

        boost::system::error_code error = boost::asio::error::host_not_found;
        while (error && endpoint_iterator != end) {
            m_commandSocket.close();
            m_commandSocket.connect(*endpoint_iterator++, error);
        }
        if (error) throw boost::system::system_error(error);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        PrintResponse();

        FTPresponse USERresponse = SendRequest((boost::format("USER %d") % m_user).str());
        if ((USERresponse.ncode == 331 || USERresponse.ncode == 332 || USERresponse.IsFine) && SendRequest((boost::format("PASS %d") % m_password).str()).IsFine) {
            while (m_commandSocket.available() > 0) PrintResponse();
            PrintMessage("Command socket connected.");
        }
        else throw std::runtime_error("Couldnt't connect to command socket.");
    }
    catch (std::exception& e) {
        PrintMessage(e.what(), Message::EC);
        Shutdown();
        return false;
    }

    return true;
}

void IAsioClientFTP::Connect_Data(tcp::socket& socket) {
    PrintMessage("Connecting to data socket...");
    std::string sport = boost::regex_replace(SendRequest("EPSV").message, boost::regex(".+ \\([[:print:]]{3}(\\d{1,5})[[:print:]]\\)\r", boost::regex::perl), "$1", boost::format_all);

    tcp::resolver::query query_trsans(m_host, sport);
    tcp::resolver::results_type endpoints_trsans = m_resolver.resolve(query_trsans);
    tcp::resolver::iterator end;

    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoints_trsans != end) {
        socket.close();
        socket.connect(*endpoints_trsans++, error);
    }
    if (error) throw boost::system::system_error(error);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (socket.is_open()) PrintMessage("Data socket connected.");
    else throw std::runtime_error("Couldnt't establish connection to data socket!");
}


bool IAsioClientFTP::Connect(const std::string& host, const std::string& user, const std::string& pass, const std::string& port) {
    m_host = host;
    m_user = user;
    m_password = pass;
    m_port = port;

    return Connect();
}

void IAsioClientFTP::Shutdown() {
    m_commandSocket.close();
    m_currentDir = "";
}

FTPresponse IAsioClientFTP::SendRequest(const std::string& srequest) {
    if (!m_commandSocket.is_open()) Connect();
    if (!m_commandSocket.is_open()) throw std::runtime_error("Couldn't connect to command socket.");

    PrintMessage(srequest, Message::REQUEST);
    boost::asio::streambuf request;
    std::ostream request_stream(&request);

    request_stream << srequest << "\r\n";
    boost::asio::write(m_commandSocket, request);
    m_commandSocket.wait(m_commandSocket.wait_read);
    request_stream.flush();

    return PrintResponse();
}

FTPresponse IAsioClientFTP::PrintResponse() {
    boost::asio::streambuf responsebuf;
    boost::asio::read_until(m_commandSocket, responsebuf, boost::regex("\r\n"));

    std::istream response_stream(&responsebuf);
    std::string sresponse;
    FTPresponse response;
    while (response_stream.peek() != EOF && std::getline(response_stream, sresponse)) {
        response = FTPresponse(sresponse);
        if (response.ncode != 0) PrintMessage(response.message, Message::RESPONSE); 
        else throw std::runtime_error("Missing response.");
    }
    response_stream.clear();

    return response;
}

std::optional<std::list<std::string>> IAsioClientFTP::ExtractList(const std::string& path) {
    std::list<std::string> extractedList;
    try {
        if (m_currentDir != path) {
            if (!SwitchDirectory(path)) return std::nullopt;
        } 

        tcp::socket dataSocket(m_context);
        Connect_Data(dataSocket);

        PrintMessage((boost::format("Retrieving directory \"%d\"...") % path).str());
        FTPresponse MLSDresponse = SendRequest("MLSD");
        if (MLSDresponse.IsFine || MLSDresponse.ncode == 150 || MLSDresponse.ncode == 125) {
            dataSocket.wait(dataSocket.wait_read);

            boost::asio::streambuf responsebuf;
            std::istream response_stream(&responsebuf);
            std::string sresponse;
            for (;;) {
                 boost::system::error_code error;
                 boost::asio::read(dataSocket, responsebuf, boost::asio::transfer_at_least(1), error);
                 if (error == boost::asio::error::eof) break; // Connection closed cleanly by peer.
                 else if (error) {
                     if (dataSocket.is_open()) dataSocket.close();
                     throw boost::system::system_error(error); // Some other error.
                 }
            }
            if (dataSocket.is_open()) dataSocket.close();

            if (!MLSDresponse.IsFine) {
                while (m_commandSocket.available() == 0) m_commandSocket.wait(m_commandSocket.wait_read);
                PrintResponse();
            }

            while (response_stream.peek() != EOF && std::getline(response_stream, sresponse))
                extractedList.push_back(sresponse);
            response_stream.clear();

            PrintMessage((boost::format("Directory listing of \"%d\" successful.") % path).str());
        }
        else throw std::runtime_error("Couldnt't extract list of contents.");
    }
    catch (std::exception& e) {
        PrintMessage(e.what(), Message::EC);
        return std::nullopt;
    }

    return extractedList;
}

bool IAsioClientFTP::LoadFile(const std::string& target, const std::string& source, unsigned encoding)
{
    try {
        std::ifstream in;
        if (encoding == ASCII) SendRequest("TYPE A").IsFine ? in.open(source, std::ios_base::in) : throw std::runtime_error("Encoding Error!");
        else SendRequest("TYPE I").IsFine ? in.open(source, std::ios::binary | std::ios_base::in) : throw std::runtime_error("Encoding Error!");
        if (!in.is_open()) throw std::runtime_error("Access denied.");

        tcp::socket dataSocket(m_context);
        Connect_Data(dataSocket);

        PrintMessage((boost::format("Uploading file \"%1%\"...") % source).str());
        FTPresponse STORresponse = SendRequest((boost::format("STOR %d") % target).str());
        if (STORresponse.IsFine || STORresponse.ncode == 125 || STORresponse.ncode == 150) {
            dataSocket.wait(dataSocket.wait_write);
            for (;;) {
                std::array<char, 4096> buf;
                in.read(buf.data(), sizeof(buf));
                int size = in.gcount();
                if (size == 0) break;
                else {
                    boost::system::error_code error;
                    dataSocket.write_some(boost::asio::buffer(buf, size), error);

                    if (error == boost::asio::error::eof) break; // Connection closed cleanly by peer.
                    else if (error) {
                        if (dataSocket.is_open()) dataSocket.close();
                        throw boost::system::system_error(error); // Some other error.
                    }
                    UpdateTask(size);
                }
                
            }
            if (dataSocket.is_open()) dataSocket.close();

            if (!STORresponse.IsFine) {
                while (m_commandSocket.available() == 0) m_commandSocket.wait(m_commandSocket.wait_read);
                PrintResponse();
            }
            TaskCompleted();

            std::size_t found = source.find_last_of(std::filesystem::path::preferred_separator);
            std::string name = source.substr(found + 1);
            PrintMessage((boost::format("File \"%1%\" uploaded to \"%2%\".") % name % target).str());
        }
        else throw std::runtime_error("Invalid response code");
    }
    catch (std::exception& e) {
        TaskFailed(e.what());
        PrintMessage(e.what(), Message::EC);
        return false;
    }
    return true;
}

bool IAsioClientFTP::DownloadFile(const std::string& target, const std::string& source, unsigned encoding) {
    try {
        std::ofstream out;
        if (encoding == ASCII) SendRequest("TYPE A").IsFine ? out.open(target, std::ios::trunc | std::ios_base::out) : throw std::runtime_error("Encoding Error!");
        else SendRequest("TYPE I").IsFine ? out.open(target, std::ios::binary | std::ios::trunc | std::ios_base::out) : throw std::runtime_error("Encoding Error!");
        if (!out.is_open()) throw std::runtime_error("Access denied.");

        tcp::socket dataSocket(m_context);
        Connect_Data(dataSocket);

        PrintMessage((boost::format("Downloading file \"%1%\"...") % source).str());
        FTPresponse RETRresponse = SendRequest((boost::format("RETR %d") % source).str());
        if (RETRresponse.IsFine || RETRresponse.ncode == 125 || RETRresponse.ncode == 150) {
            dataSocket.wait(dataSocket.wait_read);
            for (;;) {
                std::array<char, 4096> buf;
                boost::system::error_code error;
                int size = dataSocket.read_some(boost::asio::buffer(buf), error);

                if (error == boost::asio::error::eof) break; // Connection closed cleanly by peer.
                else if (error) {
                    if (dataSocket.is_open()) dataSocket.close();
                    throw boost::system::system_error(error); // Some other error.
                }

                out.write(reinterpret_cast<char*>(&buf), size);
                UpdateTask(size);
            }
            if (dataSocket.is_open()) dataSocket.close();

            if (!RETRresponse.IsFine) {
                while (m_commandSocket.available() == 0) m_commandSocket.wait(m_commandSocket.wait_read);
                PrintResponse();
            }
            TaskCompleted();

            std::size_t found = source.find_last_of("/");
            std::string name = source.substr(found + 1);
            PrintMessage((boost::format("File \"%1%\" downloaded to \"%2%\".") % name % target).str());
        }
        else throw std::runtime_error("Invalid response code");
    }
    catch (std::exception& e) {
        TaskFailed(e.what());
        PrintMessage(e.what(), Message::EC);
        return false;
    }
    return true;
}

bool IAsioClientFTP::SwitchDirectory(const std::string& path) {
    try {
        PrintMessage("Switching directory... ");
        std::string dir = boost::locale::conv::to_utf<char>(path, "UTF-8");
        if (dir == "") dir = "/";

        if (SendRequest((boost::format("CWD %d") % dir).str()).IsFine) {
            SendRequest("PWD");
            m_currentDir = dir;
        }
        else throw std::runtime_error("Cant't change directory.");
    }
    catch (std::exception& e) {
        PrintMessage(e.what(), Message::EC);
        return false;
    }
    return true;
}

bool IAsioClientFTP::CreateDir(const std::string& path) {
    try {
        FTPresponse MKDresponse = SendRequest((boost::format("MKD %d") % path).str());
        if (!MKDresponse.IsFine) return false;
    }
    catch (std::exception& e) {
        PrintMessage(e.what(), Message::EC);
        return false;
    }
    return true;
}






