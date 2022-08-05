#pragma once
#include "Client.h"

int main()
{
	// Create Client
	Client* test_client = new Client;
	test_client->ClientRun();

	// Client Disconnected
	delete test_client;
}