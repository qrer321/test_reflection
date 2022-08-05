#pragma once
#include "global.h"

void ConnectServer(int port)
{
	network_role = RPC_ROLE::RPC_SERVER;
}

void ConnectClient(std::string ip, int port)
{
	network_role = RPC_ROLE::RPC_CLIENT;

}