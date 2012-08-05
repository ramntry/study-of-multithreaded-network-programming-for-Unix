#pragma once

#include <string>
#include <boost/asio.hpp>

int const kNumberOfConnectionsPerClient = 100;
int const kNumberOfClients = 10;
char const kSeparator = '$';
int const kBufferSize = 16;
int const kPauseLength = 1000;

using boost::asio::ip::tcp;

class Client
{
public:
	Client(boost::asio::io_service &io_service, tcp::endpoint const &server_point, int id);
	void start();
	void makeRequest();

private:
	boost::asio::io_service &io_service_;
	tcp::endpoint server_point_;
	int connection_counter_;
	std::string request_;
};

