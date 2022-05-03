#include "AsioClientFTP.h"

using boost::asio::ip::tcp;

AsioClientFTP::AsioClientFTP() : resolver(io_context), socket_command(io_context) {}

bool AsioClientFTP::Connect(const std::string& hostName, const std::string& usr, const std::string& pass, const std::string& prt) {
    host = hostName;
    user = usr;
    password = pass;
    port = prt;

    return Connect();
}

bool AsioClientFTP::Connect() {
    try
    {
        PrintMessage((boost::format("Connecting to %d...") % host).str()); 

        tcp::resolver::query query(host, port);

        tcp::resolver::results_type endpoints = resolver.resolve(host, "ftp");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::resolver::iterator end;

        boost::system::error_code error = boost::asio::error::host_not_found;
        while (error && endpoint_iterator != end)
        {
            socket_command.close();
            socket_command.connect(*endpoint_iterator++, error);
        }
        if (error) throw boost::system::system_error(error);

        PrintResponse();

        if (SendRequest((boost::format("USER %d") % user).str())->isOk && SendRequest((boost::format("PASS %d") % password).str())->isFine)
            PrintMessage("Command socket connected.");
        else throw std::exception("Couldnt't connect to command socket.");
    }
    catch (std::exception& e)
    {
        PrintMessage(e.what(), Message::EC);
        Shutdown();
        return false;
    }

    return true;
}

std::optional<FTPresponse> AsioClientFTP::SendRequest(const std::string& srequest) {
    if (!socket_command.is_open()) Connect();

    if (socket_command.is_open()) {
        PrintMessage(srequest, Message::REQUEST);
        boost::asio::streambuf request;
        std::ostream request_stream(&request);

        request_stream << srequest << "\r\n";
        boost::asio::write(socket_command, request);
        socket_command.wait(socket_command.wait_read);
        request_stream.flush();

        return PrintResponse();
    }
    else return std::nullopt;
}

std::optional<FTPresponse> AsioClientFTP::PrintResponse() {
    boost::asio::streambuf responsebuf;
    boost::asio::read_until(socket_command, responsebuf, boost::regex("\r\n"));

    std::istream response_stream(&responsebuf);
    std::string sresponse;
    FTPresponse response;
    while (response_stream.peek() != EOF && std::getline(response_stream, sresponse))
    {
        response = FTPresponse(sresponse);
        if (response.code == 0) return std::nullopt;
        else PrintMessage(response.message, Message::RESPONSE);
    }
    response_stream.clear();

    return response;
}

std::optional<std::list<std::string>> AsioClientFTP::ExtractList(const std::string& root) {
    std::list<std::string> extractedList;
    try
    {
        PrintMessage("Connecting to data socket...");
        tcp::socket dataSocket(io_context);
        std::string sport = boost::regex_replace(SendRequest("EPSV")->message, boost::regex(".+ \\([[:print:]]{3}(\\d{1,5})[[:print:]]\\)\r", boost::regex::perl), "$1", boost::format_all);

        tcp::resolver::query query_trsans(host, sport);
        tcp::resolver::results_type endpoints_trsans = resolver.resolve(query_trsans);
        tcp::resolver::iterator end;

        boost::system::error_code error = boost::asio::error::host_not_found;
        while (error && endpoints_trsans != end) {
            dataSocket.close();
            dataSocket.connect(*endpoints_trsans++, error);
        }
        if (error) throw boost::system::system_error(error);

        if (dataSocket.is_open()) {
            PrintMessage("Data socket connected.");

            PrintMessage((boost::format("Extracting list of contents from \"%d\"...") % root).str());
            auto MLSDresponse = SendRequest("MLSD");
            if (MLSDresponse->isOk) {

                boost::asio::streambuf responsebuf;
                std::istream response_stream(&responsebuf);
                std::string sresponse;

                dataSocket.wait(dataSocket.wait_read);
                for (;;) {
                    boost::asio::read(dataSocket, responsebuf, boost::asio::transfer_at_least(1), error);
                    if (error == boost::asio::error::eof) break;
                    else if (error) throw error;
                }

                while (response_stream.peek() != EOF && std::getline(response_stream, sresponse))
                    extractedList.push_back(sresponse);
                response_stream.clear();

                if (socket_command.available() > 0) {
                    boost::asio::read_until(socket_command, responsebuf, boost::regex("\r\n"));
                    std::getline(response_stream, sresponse);
                    MLSDresponse = FTPresponse(sresponse);
                    PrintMessage(MLSDresponse->message, Message::RESPONSE);
                }

                if (MLSDresponse->isFine) PrintMessage((boost::format("List of contents from \"%d\" extracted.") % root).str());
                else throw std::exception("Broken list of contents.");

                dataSocket.close();
            }
            else throw std::exception("Couldnt't extract list of contents.");
        }
        else throw std::exception("Connection to data socket is fail!");
    }
    catch (std::exception& e)
    {
        PrintMessage(e.what(), Message::EC);
        return std::nullopt;
    }

    return extractedList;
}

void AsioClientFTP::Shutdown() {
    socket_command.close();
    PrintMessage("Disconneted.");
}
