#pragma once
#include "global.h"
#include "ParseCommand.h"

inline void Call_CreateObject(Client* client)
{
	if (nullptr == client)
	{
		return;
	}

	std::cout << "임의적인 클래스로 생성합니다." << std::endl;
	std::cout << "객체 명 입력... : ";

	std::string object_name;
	std::getline(std::cin, object_name);
	object_name = "NewObject : " + object_name;
	std::cout << std::endl;

	SOCKET session_socket = client->GetSessionSocket();
	string_copy(object_name);
	send(session_socket, buffer, static_cast<int>(buf_size), 0);
}

inline void Call_GetAllProperty(Client* client)
{
	if (nullptr == client)
	{
		return;
	}

	std::cout << "모든 객체 정보를 받아옵니다..." << std::endl;
	SOCKET session_socket = client->GetSessionSocket();

	std::string packet_string = "GetAllProperty";
	string_copy(packet_string);
	send(session_socket, buffer, static_cast<int>(buf_size), 0);
}

inline void Call_SetProperty(Client* client)
{
	if (nullptr == client)
	{
		return;
	}

	std::cout << "객체 명 입력..." << std::endl;

	std::string object_name;
	std::getline(std::cin, object_name);

	if (false == PrintProperty(object_name))
	{
		return;
	}

	std::cout << "변경하려는 Property 명을 입력하세요 : ";
	std::string property_name;
	std::getline(std::cin, property_name);

	std::cout << "값 입력 : ";
	std::string value;
	std::getline(std::cin, value);

	std::string send_string = "SetProperty : ";
	send_string += object_name + "\\" + property_name + "\\" + value;
	string_copy(send_string);
	send(client->GetSessionSocket(), buffer, static_cast<int>(buf_size), 0);
}

inline void Call_FunctionCall(Client* client)
{
	if (nullptr == client)
	{
		return;
	}

	// Input Object Name
	std::cout << "객체 명 입력... : ";
	std::string object_name;
	std::getline(std::cin, object_name);

	UObject* find_object = Reflection::GetInstance()->FindObjectBasedOnName(object_name);
	if (nullptr == find_object)
	{
		std::cout << object_name << " 객체는 존재하지 않습니다." << std::endl;
		return;
	}

	std::unordered_map<std::string, UFunction*> functions = find_object->GetFunctions();
	for (const auto& func : functions)
	{
		std::cout << func.first << std::endl;
	}

	// Input Function Name
	std::string function_name;
	std::cout << std::endl;
	std::cout << "호출하려는 함수 명을 입력하세요 : ";
	std::getline(std::cin, function_name);

	UFunction* find_function = find_object->GetFunction(function_name);
	if (nullptr == find_function)
	{
		std::cout << function_name << " 함수는 존재하지 않습니다." << std::endl;
		return;
	}

	// Print Function's Info
	std::cout << std::endl;
	std::cout << function_name << "'s Parameters " << find_function->GetReturnType() << "( ";
	std::vector<std::string> params = find_function->GetFunctionParams();
	for (const auto& param : params)
	{
		std::cout << param << ", ";
	}
	std::cout << " )" << std::endl << std::endl;

	std::string input_params;
	if (false == params.empty())
	{
		// 당장은 정수형 매개변수만이 가능함.
		std::cout << "매개변수에 값을 입력하세요.(띄어쓰기로 구분합니다) : ";
		std::getline(std::cin, input_params);
	}
	else
	{
		input_params = "none";
	}

	std::string target;
	std::cout << "Server Or Other(Client)" << std::endl;
	std::getline(std::cin, target);

	std::string send_string = "Call Function : ";
	send_string += target + "\\" + object_name + "\\" + function_name + "\\" + input_params;
	string_copy(send_string);
	send(client->GetSessionSocket(), buffer, static_cast<int>(buf_size), 0);
}

inline void Call_Disconnect(Client* client)
{
	std::cout << "접속을 종료합니다." << std::endl;

	SOCKET session_socket = client->GetSessionSocket();

	std::string disconnect_string = "Disconnect Client";
	string_copy(disconnect_string);
	send(session_socket, buffer, static_cast<int>(buf_size), 0);
}

inline void ToHelperMethod(const std::string& recv_string)
{
	if (std::string::npos != recv_string.find("Create Object : "))
	{
		size_t name_pos = recv_string.find(":") + 2;
		size_t name_end_pos = recv_string.find("\\");
		std::string object_name = recv_string.substr(name_pos, name_end_pos - name_pos);
		std::string owner_name = recv_string.substr(name_end_pos + 1, recv_string.size() - (name_end_pos + 1));
	
		auto new_object = NewObject<SomeTestClass>();
		new_object->SetName(object_name);
		new_object->SetOwnerSession(owner_name);
	}
	else if (std::string::npos != recv_string.find("SetProperty : "))
	{
		std::string object_name;
		std::string property_name;
		std::string value;

		size_t name_pos = recv_string.find(":") + 2;
		size_t end_pos = recv_string.find("\\");
		object_name = recv_string.substr(name_pos, end_pos - name_pos);

		UObject* find_object = Reflection::GetInstance()->FindObjectBasedOnName(object_name);
		if (nullptr == find_object)
		{
			std::cout << "object does not exist" << std::endl;
			return;
		}

		size_t temp_pos = end_pos + 1;
		end_pos = recv_string.find("\\", temp_pos);
		property_name = recv_string.substr(temp_pos, end_pos - temp_pos);
		value = recv_string.substr(end_pos + 1, recv_string.size() - (end_pos + 1));

		SetPropertyValue(find_object, property_name, value);
	}
	else if (std::string::npos != recv_string.find("Call Function : "))
	{
		std::string target;
		std::string object_name;
		std::string function_name;
		std::string input_params;
		UObject* find_object = nullptr;
		UFunction* find_function = nullptr;

		bool success_check = true;
		success_check = ParseFunctionCall(recv_string, target, object_name,
			function_name, input_params, find_object, find_function);

		if (false == success_check)
		{
			return;
		}

		std::cout << find_object->GetOwnerSession() << " client call function " 
			<< function_name <<  std::endl;
		if ("none" == input_params)
		{
			find_function->CallFunction();
		}
		else
		{
			std::vector<int> input_vector;
			ParseParams(input_params, input_vector);

			find_function->CallFunction(input_vector);
		}
	}
	else
	{
		std::cout << "Recv : " << recv_string << std::endl;
	}
}