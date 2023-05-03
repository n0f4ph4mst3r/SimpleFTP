#pragma once
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread/thread.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/locale.hpp>

#include <list>
#include <optional>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <chrono>

#include "FTPresponse.h"
#include "enumMessage.h"

using boost::asio::ip::tcp;

class IAsioClientFTP {
protected:
	boost::asio::io_context m_context;
	tcp::resolver m_resolver;

	tcp::socket m_commandSocket;

	std::string m_currentDir;

	std::string m_host;
	std::string m_user;
	std::string m_password;
	std::string m_port;

	//connection
	bool Connect();
	void Connect_Data(tcp::socket& socket);

	FTPresponse PrintResponse();
	virtual void UpdateTask(const int bytes) = 0;
	virtual void TaskCompleted() = 0;
	virtual void TaskFailed(const std::string& reason) = 0;

	enum DataEncoding {
		ASCII,
		BINARY
	};
public:
	IAsioClientFTP();

	virtual bool Connect(const std::string& host, const std::string& user, const std::string& pass, const std::string& port = "21");
	virtual void Shutdown();

	//input/output
	FTPresponse SendRequest(const std::string& request);
	virtual void PrintMessage(const std::string& msg, Message type = Message::STATUS) = 0;

	//work with data
	std::optional<std::list<std::string>> ExtractList(const std::string& path = "");

	bool LoadFile(const std::string& target, const std::string& source, unsigned encoding = BINARY);
	bool DownloadFile(const std::string& target, const std::string& source, unsigned encoding = BINARY);

	bool SwitchDirectory(const std::string& path);
	bool CreateDir(const std::string& path);
};