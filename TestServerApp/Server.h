#pragma once
#include "..\TestReflection\global.h"

struct RecvOverlapped
{
	RecvOverlapped()
	{
		memset(this, 0, sizeof(RecvOverlapped));

		buf_.len = 1024;
		buf_.buf = arr_buf_;
	}

	SOCKET		socket_;
	WSABUF		buf_;
	DWORD		buf_count_;
	DWORD		number_of_bytes_recvd_;
	DWORD		flags_;
	char		arr_buf_[1024];
	OVERLAPPED	overlapped_;
};

class Server
{
public:
	Server();
	~Server();

	Server(const Server& other) = delete;
	Server(Server&& other) = delete;
	Server& operator= (const Server& other) = delete;
	Server& operator= (Server&& other) = delete;

	void ServerRun();

	std::set<SOCKET> GetClientSessions() const { return client_sessions_; }

public:
	void IocpThreadFunction();
	void ServerInitialize();

	HANDLE handle_;
	std::mutex server_lock_;
	std::set<SOCKET> client_sessions_;
	std::vector<std::shared_ptr<std::thread>> thread_list_;

	SOCKET accept_socket_;
};