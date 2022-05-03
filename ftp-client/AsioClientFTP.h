#pragma once
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread/thread.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>

#include <list>
#include <optional>

#include "FTPresponse.h"
#include "enumMessage.h"

using boost::asio::ip::tcp;

class AsioClientFTP {
public:
	AsioClientFTP();
	bool Connect(const std::string& host, const std::string& user, const std::string& pass, const std::string& port = "21");
	std::optional<FTPresponse> SendRequest(const std::string& request);
	std::optional<std::list<std::string>> ExtractList(const std::string& directory = "");
	void Shutdown();

	virtual void PrintMessage(const std::string& msg, Message type = Message::STATUS) = 0;
protected:
	boost::asio::io_context io_context;
	tcp::resolver resolver;
	tcp::socket socket_command;

	std::string host;
	std::string user;
	std::string password;
	std::string port;

	bool Connect();
	std::optional<FTPresponse> PrintResponse();
};