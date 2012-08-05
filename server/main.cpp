#include "testServer.hpp"

int main()
{
	Server server(7789, 100);
	server.start();

	return 0;
}

