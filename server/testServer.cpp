#include <iostream>
#include <iomanip>
#include "testServer.hpp"

Server::Server(int port, int number_of_clients)
	: acceptor_(io_service_, tcp::endpoint(tcp::v4(), port))
	, connect_counters_(number_of_clients, 0)
	, stop_flag_(false)
	, global_counter_(0)
{
	std::cerr << "Server is created. Port " << port << "; "
			<< "number of clients " << number_of_clients << "." << std::endl;
}

void Server::start()
{
	std::cerr << "S : Mainloop start." << std::endl;
	while (!stop_flag_)
	{
		tcp::socket *client_socket = new tcp::socket(io_service_);
		std::cerr << "S : Wait for incoming connection..." << std::endl;
		acceptor_.accept(*client_socket);
		std::cerr << "S : Connection established." << std::endl;
		boost::thread(boost::bind(&Server::clientThread, this, client_socket)).detach();
	}
	std::cerr << "S : OK.\r\n\tBye!\r\n" << std::endl;
}

void Server::clientThread(tcp::socket *client_socket)
{
	ConnectionTerminator ct(client_socket);

	int &answer = connect_counters_[readClientId(client_socket)];
	sendAnswer(client_socket, answer);
	++answer;
	incGlobalCounter();
	printStatIfNeeded();

	std::cerr << "Tm: Connect counters are updated, connection is correctly closed." << std::endl;
}

void Server::incGlobalCounter()
{
	mutex_.lock();
	++global_counter_;
	mutex_.unlock();
}

int Server::readClientId(tcp::socket *client_socket)
{
	boost::asio::streambuf buf;
	std::cerr << "Tc: Read client id..." << std::endl;
	boost::asio::read_until(*client_socket, buf, kSeparator);
	std::istream in(&buf);

	int client_id = -1;
	in >> client_id;
	if (!in)
	{
		failInClientThread("Tc: Incorrect client id: bad type, must be integer.");
	}
	if (client_id == kHaltId)
	{
		failInClientThread("Tc: root client id: server halt.", kSetupStopFlag);
	}
	if (client_id < 0 || client_id >= connect_counters_.size())
	{
		failInClientThread("Tc: Incorrect client id: out of bounds.");
	}
	std::cerr << "Tc: Success: client id " << client_id << "." << std::endl;
	return client_id;
}

void Server::sendAnswer(tcp::socket *client_socket, int const &answer)
{
	boost::asio::streambuf buf;
	std::ostream os(&buf);
	os << answer << kSeparator << std::endl;
	std::cerr << "Tc: Write answer: " << answer << "..." << std::endl;
	boost::asio::write(*client_socket, buf);
}

void Server::failInClientThread(const char *message, bool set_up_stop_flag)
{
	std::cerr << message << std::endl;
	if (set_up_stop_flag)
	{
		stop_flag_ = true;
	}
	throw boost::thread_interrupted();
}

Server::ConnectionTerminator::ConnectionTerminator(tcp::socket *client_socket)
	: client_socket_(client_socket)
{
}

Server::ConnectionTerminator::~ConnectionTerminator()
{
	std::cerr << "Tt: Closing connection..." << std::endl;
	boost::system::error_code error_collector;
	client_socket_->shutdown(tcp::socket::shutdown_both, error_collector);
	client_socket_->close(error_collector);
	if (error_collector)
	{
		std::cerr << error_collector.message() << std::endl;
	}
	delete client_socket_;
	std::cerr << "Tt: Connection closed." << std::endl;
}

void Server::printStatIfNeeded()
{
	if (global_counter_ % kUpdatePeriod)
	{
		return;
	}
	std::cout << "\r\n\tPrint Stat\r\n" << std::endl;
	size_t len = connect_counters_.size();
	for (size_t i = 0; i < len; ++i)
	{
		std::cout << std::setw(5) << i << " | " << connect_counters_[i] << std::endl;
	}
	std::cout << std::endl;
}

