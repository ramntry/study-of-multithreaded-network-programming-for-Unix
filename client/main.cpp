#include <iostream>
#include <boost/thread/thread.hpp>
#include "testClient.hpp"

const char *port = "7789";

class ClientsEmulator
{
public:
	ClientsEmulator(char const *host);
	void clientThread(int id);
	void start();

private:
	boost::asio::io_service io_service_;
	tcp::endpoint server_point_;
	boost::thread_group clients_;
};

ClientsEmulator::ClientsEmulator(char const *host)
{
	tcp::resolver resolver(io_service_);
	tcp::resolver::query resolve_query(host, port);
	server_point_ = *resolver.resolve(resolve_query);
}

void ClientsEmulator::start()
{
	std::cout << "START!" << std::endl;
	for (int i = 0; i < kNumberOfClients; ++i)
	{
		clients_.create_thread(boost::bind(&ClientsEmulator::clientThread, this, i));
	}
	clients_.join_all();
	std::cout << "\r\n\r\n\tBye!\r\n" << std::endl;
}

void ClientsEmulator::clientThread(int id)
{
	Client(io_service_, server_point_, id).start();
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage: client <host>" << std::endl;
		exit(1);
	}

	ClientsEmulator emulator(argv[1]);
	emulator.start();

	return 0;
}

