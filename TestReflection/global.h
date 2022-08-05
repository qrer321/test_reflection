#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <unordered_map>
#include <set>
#include <queue>
#include <atomic>
#include <any>
#include <functional>
#include <conio.h>
#include <iostream>
#include <cassert>
#include "msgpack/msgpack.hpp"

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <thread>
#include <mutex>

#pragma comment(lib, "ws2_32")

enum RPC_ROLE
{
	RPC_SERVER,
	RPC_CLIENT,
};

enum RPC_CODE
{
	RPC_SUCCESS,
	RPC_BIND_FUNCTION_ERR,
	RPC_TIMEOUT,
};

static RPC_ROLE network_role = RPC_SERVER;
static RPC_CODE network_code = RPC_SUCCESS;