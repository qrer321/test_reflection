#pragma once
#include "../TestReflection/global.h"
#include "../TestReflection/delta_timer.h"

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
	void GCFunction();

	SOCKET session_socket_;
	std::mutex client_lock_;
	std::thread recv_thread_;
	std::thread gc_thread_;

	delta_timer dc_;
	float		dc_time_;
};