#pragma once
#include "..\TestReflection\global.h"

class Client
{
public:
	Client();
	~Client();

	Client(const Client& other) = delete;
	Client(Client&& other) = delete;
	Client& operator= (const Client& other) = delete;
	Client& operator= (Client&& other) = delete;

	void ClientRun();

	SOCKET GetSessionSocket() const { return session_socket_; }

private:
	void ClientInitialize();
	void RecvFunction();

	std::mutex client_lock_;
	std::thread recv_thread_;

	SOCKET session_socket_;
};