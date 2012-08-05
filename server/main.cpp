#include "testServer.hpp"

int main()
{
	Server server(7789, 10);
	server.start();

	return 0;
}

