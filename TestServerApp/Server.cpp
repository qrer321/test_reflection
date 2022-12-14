#include "Server.h"
#include "helper_method_for_server.h"

Server::Server()
	: dc_time_(0.f)
	, dc_max_(30.f)
	, thread_check_(true)
{
	ServerInitialize();
}

Server::~Server()
{
	thread_check_ = false;

	for (const auto& elem : thread_list_)
	{
		// GetQueuedCompletionStatue로 인해 대기상태로 들어간 thread 강제 활성화
		// 활성화 후 thread_check_ false로 thread while문 종료
		PostQueuedCompletionStatus(handle_, -1, 0, nullptr);
	}
	for (const auto& elem : thread_list_)
	{
		// while문 종료된 iocp thread들 join 처리
		elem->join();
	}
	
	gc_thread_.join();

	GarbageCollector::GetInstance()->Destroy();
	Reflection::GetInstance()->Destroy();

	std::cout << std::endl << std::endl;
	std::cout << "Server End" << std::endl;
}

void Server::IocpThreadFunction()
{
	while (thread_check_)
	{
		RecvOverlapped* data = {};
		DWORD number_of_bytes_transferred;
		ULONG_PTR completion_key;
		LPOVERLAPPED overlapped;
		GetQueuedCompletionStatus(handle_, &number_of_bytes_transferred, &completion_key, &overlapped, INFINITE);

		if (0 == number_of_bytes_transferred)
		{
			std::cout << "force disconnected" << std::endl;
			continue;
		}

		data = reinterpret_cast<RecvOverlapped*>(completion_key);
		if (nullptr == data)
		{
			continue;
		}

		ToHelperMethod(this, data);

		if (SOCKET_ERROR ==
				WSARecv(data->socket_, &data->buf_, 1, &data->number_of_bytes_recvd_,
				&data->flags_, &data->overlapped_, nullptr))
		{
			int error = static_cast<int>(GetLastError());
			if (WSAECONNRESET == error)
			{
				std::cout << "disconnected" << std::endl;
				continue;
			}

			if (WSA_IO_PENDING != error)
			{
				std::cout << "some other error occurred " << error << std::endl;
				continue;
			}
		}
	}
}

void Server::GCFunction()
{
	while (thread_check_)
	{
		dc_.Tick();
		dc_time_ += dc_.GetDelta();

		// GC 동작시키기
		if (dc_time_ > dc_max_)
		{
			dc_time_ = 0.f;
			std::cout << "Run Garbage Collector" << std::endl;
			GarbageCollector::GetInstance()->ActivateGarbageCollector();

			// For Property Replication
			std::cout << "Update All Object's Properties" << std::endl;
			for (const auto& object : Reflection::GetInstance()->GetAllObject())
			{
				Send_PropertyUpdate(object, object->GetProperties());
			}
		}
		Sleep(1);
	}
}

void Server::ServerInitialize()
{
	WSAData winsock;
	if (0 != WSAStartup(MAKEWORD(2, 2), &winsock))
	{
		std::cout << "WSAStartup Error" << std::endl;
		return;
	}

	handle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, 4);
	for (int i = 0; i < 4; ++i)
	{
		thread_list_.push_back(std::make_shared<std::thread>(&Server::IocpThreadFunction, this));
	}

	SOCKADDR_IN socket_address = {};
	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(30001);
	if (SOCKET_ERROR == inet_pton(AF_INET, "0.0.0.0", &socket_address.sin_addr))
	{
		std::cout << "Failed ip convert to binary form" << std::endl;
		return;
	}

	accept_socket_ = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == accept_socket_)
	{
		std::cout << "accept socket is an invaild socket" << std::endl;
		return;
	}

	unsigned long mode = 1;
	if (SOCKET_ERROR == ioctlsocket(accept_socket_, FIONBIO, &mode))
	{
		std::cout << "ioctlsocket error : " << GetLastError() << std::endl;
		return;
	}

	if (SOCKET_ERROR == bind(accept_socket_, reinterpret_cast<const sockaddr*>(&socket_address), sizeof(SOCKADDR_IN)))
	{
		std::cout << "socket bind error" << std::endl;
		return;
	}

	if (SOCKET_ERROR == listen(accept_socket_, SOMAXCONN))
	{
		std::cout << "failed create listen queue" << std::endl;
		return;
	}

	std::cout << "Server Started" << std::endl;
	std::cout << "Start Server Connection Function..." << std::endl;

	gc_thread_ = std::thread(&Server::GCFunction, this);
}

void Server::ServerRun()
{
	dc_.Reset();

	while (true)
	{
		SOCKADDR_IN user_address = {};
		user_address.sin_family = AF_INET;
		int len = static_cast<int>(sizeof(SOCKADDR_IN));

		SOCKET session_socket = accept(accept_socket_, reinterpret_cast<sockaddr*>(&user_address), &len);
		if (INVALID_SOCKET == session_socket)
		{
			int error = static_cast<int>(GetLastError());
			if (WSAEWOULDBLOCK == error)

			{
				Sleep(1);
				continue;
			}
			else
			{
				std::cout << "accpet to session socket error : " << error << std::endl;
			}
		}

		server_lock_.lock();
		client_sessions_.insert(session_socket);
		server_lock_.unlock();

		RecvOverlapped* new_data = new RecvOverlapped;
		new_data->socket_ = session_socket;

		// observing session socket
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(session_socket), handle_, reinterpret_cast<ULONG_PTR>(new_data), 0);

		if (SOCKET_ERROR ==
			WSARecv(new_data->socket_, &new_data->buf_, 1, &new_data->number_of_bytes_recvd_, &new_data->flags_, &new_data->overlapped_, nullptr))
		{
			int error = GetLastError();
			if (WSA_IO_PENDING != error)
			{
				std::cout << "some other error occurred" << error << std::endl;
				return;
			}
		}
	}
}