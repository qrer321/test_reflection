#pragma once
#include "Server.h"

int main()
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Server* test_server = new Server;
	test_server->ServerRun();

	// server ended
	delete test_server;
}