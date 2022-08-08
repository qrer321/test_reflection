#pragma once
#include "../TestReflection/global.h"
#include "../TestReflection/delta_timer.h"

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
	void SetGCTimer(float time) { dc_max_ = time; }
	const std::mutex& GetServerLock() { return server_lock_; }

	std::set<SOCKET> client_sessions_;
	const std::set<SOCKET>& GetClientSessions() const { return client_sessions_; }

private:
	void IocpThreadFunction();
	void GCFunction();
	void ServerInitialize();

	HANDLE handle_;
	SOCKET accept_socket_;
	std::mutex server_lock_;
	std::vector<std::shared_ptr<std::thread>> thread_list_;
	std::thread gc_thread_;

	delta_timer dc_;
	float		dc_time_;
	float		dc_max_;
};