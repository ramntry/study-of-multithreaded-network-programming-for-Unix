#pragma once

#include <string>
#include <vector>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

using boost::asio::ip::tcp;

char const kSeparator = '$';
int const kAnswerBufferSize = 16;
int const kHaltId = -100;
int const kUpdatePeriod = 20;
bool const kSetupStopFlag = true;

class Server
{
public:
	Server(int port, int number_of_clients);
	void start();

private:
	struct ConnectionTerminator
	{
		ConnectionTerminator(tcp::socket *client_socket);
		~ConnectionTerminator();
		tcp::socket *client_socket_;
	};

	void clientThread(tcp::socket *client_socket);
	int readClientId(tcp::socket *client_socket);
	void failInClientThread(const char *message, bool set_up_stop_flag = false);
	void sendAnswer(tcp::socket *client_socket, int const &answer);
	void printStatIfNeeded();
	void incGlobalCounter();

	boost::asio::io_service io_service_;
	tcp::acceptor acceptor_;
	std::vector<int> connect_counters_;
	boost::thread_group client_threads_;
	volatile bool stop_flag_;
	volatile int global_counter_;
	boost::mutex mutex_;
};

