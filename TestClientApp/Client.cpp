#include "Client.h"
#include "helper_method_for_client.h"

Client::Client()
	: dc_time_(0.f)
{
	ClientInitialize();
}

Client::~Client()
{
}

bool g_check = true;
void Client::RecvFunction()
{
	while (g_check)
	{
		if (INVALID_SOCKET == session_socket_)
		{
			std::cout << "close thread" << std::endl;
			break;
		}

		char buffer[1024] = {};
		recv(session_socket_, buffer, sizeof(buffer), 0);

		std::string recv_string = buffer;
		ToHelperMethod(recv_string);
	}
}

void Client::GCFunction()
{
	while (g_check)
	{
		dc_.Tick();
		dc_time_ += dc_.GetDelta();

		// GC 동작시키기
		if (dc_time_ > 10.f)
		{
			dc_time_ = 0.f;
			GarbageCollector::GetInstance()->ActivateGarbageCollector();
			std::cout << "Run Garbage Collector" << std::endl;
		}
		Sleep(1);
	}
}

void Client::ClientInitialize()
{
	WSAData winsock;
	if (0 != WSAStartup(MAKEWORD(2, 2), &winsock))
	{
		std::cout << "WSAStartup Error" << std::endl;
		return;
	}

	session_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == session_socket_)
	{
		std::cout << "session socket is invaild socket" << std::endl;
		return;
	}

	std::string ip = "127.0.0.1";
	std::string port = "30001";
	std::string client_name;

	std::cout << "Connecting the server at 127.0.0.1:30001" << std::endl;
	std::cout << "Write Client Name... : ";
	std::cin >> client_name;

	SOCKADDR_IN socket_address = {};
	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(30001);

	if (SOCKET_ERROR == inet_pton(AF_INET, ip.c_str(), &socket_address.sin_addr))
	{
		std::cout << "invalid pton socket" << std::endl;
		return;
	}

	if (SOCKET_ERROR == connect(session_socket_, reinterpret_cast<const sockaddr*>(&socket_address), sizeof(SOCKADDR_IN)))
	{
		int error = WSAGetLastError();
		if (WSAECONNREFUSED == error)
		{
			std::cout << "port number error" << std::endl;
			return;
		}
		else if (WSAEADDRNOTAVAIL == error)
		{
			std::cout << "ip error" << std::endl;
			return;
		}
		else
		{
			std::cout << "some other error : " << error << std::endl;
			return;
		}
	}

	// input client name
	client_name = "New Connecter : " + client_name;
	send(session_socket_, client_name.c_str(), static_cast<int>(client_name.size()), 0);
	recv_thread_ = std::thread(&Client::RecvFunction, this);
	gc_thread_ = std::thread(&Client::GCFunction, this);
}

void Client::ClientRun()
{
	bool break_console = false;
	std::cin.ignore();

	client_instance = this;
	dc_.Reset();

	while (true)
	{
		std::string output_string;
		std::string input_value;

		output_string = output_string +
			"1. 객체 생성                    2. 모든 객체 정보 확인\n" +
			"3. 프로퍼티 변경                4. 객체 삭제\n" +
			"5. 함수 호출";
		std::cout << output_string;
		std::cout << std::endl << std::endl;
		
		std::getline(std::cin, input_value);

		switch (std::stoi(input_value))
		{
		case 1:
			Call_CreateObject();
			break;
		case 2:
			Call_GetAllProperty();
			break;
		case 3:
			Call_SetProperty();
			break;
		case 4:
			Call_DestroyObject();
			break;
		case 5:
			Call_FunctionCall();
			break;
		default:
			Call_Disconnect();
			shutdown(session_socket_, SD_BOTH);
			closesocket(session_socket_);

			g_check = false;
			recv_thread_.join();
			gc_thread_.join();
			session_socket_ = INVALID_SOCKET;
			Sleep(1);

			WSACleanup();
			break_console = true;
			break;
		}

		system("pause");

		if (true == break_console)
		{
			break;
		}
	}
}