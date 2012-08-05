#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include "testClient.hpp"

Client::Client(boost::asio::io_service &io_service, tcp::endpoint const &server_point, int id)
	: io_service_(io_service)
	, server_point_(server_point)
	, connection_counter_(0)
{
	std::ostringstream os;
	os << id << kSeparator;
	request_ = os.str();
}


void Client::start()
{
	for (int i = 0; i < kNumberOfConnectionsPerClient; ++i)
	{
		makeRequest();
	}
}

void Client::makeRequest()
{
	tcp::socket socket(io_service_);
	socket.connect(server_point_);

	int pause_length = kPauseLength / 2 + rand() % kPauseLength;
	boost::this_thread::sleep(boost::posix_time::milliseconds(pause_length));

	boost::asio::write(socket, boost::asio::buffer(request_));

	boost::array<char, kBufferSize> buf;
	boost::system::error_code error;
	boost::asio::read(socket, boost::asio::buffer(buf), error);
	if (error != boost::asio::error::eof)
	{
		std::cout << "EROOR while read answer from server" << std::endl;
	}
	int counter_on_server_side = -1;
	sscanf(buf.data(), "%d", &counter_on_server_side);
	
	if (counter_on_server_side != connection_counter_)
	{
		std::cout << "\r\n\t[FAIL]! request: " << request_
				<< "; expected: " << connection_counter_ << "; actual: " << counter_on_server_side << std::endl;
	}
	else
	{
		std::cout << "ok " << std::flush;
	}

	++connection_counter_;

	socket.shutdown(tcp::socket::shutdown_both);
	socket.close();
}

