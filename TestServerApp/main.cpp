#pragma once
#include "Server.h"

int main()
{
	Server* test_server = new Server;
	test_server->ServerRun();

	// server ended
	delete test_server;
}