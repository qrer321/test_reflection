#pragma once
#include "global.h"
#include "ParseCommand.h"

inline void Recv_NewConnecter(std::string recv_string)
{
	if (nullptr == server_instance)
	{
		return;
	}

	size_t name_pos = recv_string.find(":") + 2;
	std::string client_name = recv_string.substr(name_pos, recv_string.size() - name_pos);

	client_map[recv_data->socket_] = { client_name };

	std::cout << "New Connecter : " << client_name << std::endl;
}

inline void Recv_CreateObject(std::string recv_string)
{
	if (nullptr == server_instance)
	{
		return;
	}

	size_t name_pos = recv_string.find(":") + 2;
	std::string object_name = recv_string.substr(name_pos, recv_string.size() - name_pos);
	if (nullptr != Reflection::GetInstance()->FindObjectBasedOnName(object_name))
	{
		std::cout << "object with the same name exists" << std::endl;

		string_copy("create denied...");
		send(recv_data->socket_, buffer, static_cast<int>(buf_size), 0);
		return;
	}

	auto new_object = NewObject<SomeTestClass>();
	new_object->SetOwnerSession(client_map[recv_data->socket_].client_name);
	new_object->SetName(object_name);

	std::cout << "Created New Object : " << object_name << std::endl;

	// 동일한 세션인 경우 만들어진 오브젝트에 대한 정보
	// 다른 세션에게는 어떤 세션이 오브젝트를 만들었다는 정보만을
	std::string owner_session;
	std::string other_session;

	owner_session = object_name + " object created...";
	other_session = client_map[recv_data->socket_].client_name + " was created " + object_name + " object...";

	std::string order_string;
	order_string += "Create Object : " + object_name + "\\" + client_map[recv_data->socket_].client_name;

	lock.lock();
	for (const auto& session : server_instance->GetClientSessions())
	{
		if (session == recv_data->socket_)
		{
			string_copy(owner_session);
			send(session, buffer, static_cast<int>(buf_size), 0);
		}
		else
		{
			string_copy(other_session);
			send(session, buffer, static_cast<int>(buf_size), 0);
		}

		string_copy(order_string);
		send(session, buffer, static_cast<int>(buf_size), 0);
	}
	lock.unlock();
}

inline void Recv_GetProperty(const std::string& object_name)
{
	UObject* object = Reflection::GetInstance()->FindObjectBasedOnName(object_name);
	if (nullptr == object)
	{
		return;
	}

	PropertyStruct p;
	p.object_name_ = object->GetName();
	
	for (const auto& prop : object->GetProperties())
	{
		std::string prop_type = prop.second->GetType();
		std::string prop_name = prop.first;
		std::string prop_value = GetPropertyValue(object, prop.first);

		std::string prop_string = prop_type + " " + prop_name + "\t" + prop_value + "\n";
		p.properties_info.push_back(prop_string);
	}

	std::string output_string =
		"Owner Client : " + object->GetOwnerSession() + "\n" +
		"Object Name : " + p.object_name_ + "\n";

	for (const auto& elem : p.properties_info)
	{
		output_string += elem;
	}

	string_copy(output_string);
	send(recv_data->socket_, buffer, static_cast<int>(buf_size), 0);
}

inline void Recv_GetAllProperty()
{
	for (const auto& object : Reflection::GetInstance()->GetAllObject())
	{
		Recv_GetProperty(object->GetName());
	}
}

inline void Recv_SetProperty(const std::string& recv_string)
{
	if (nullptr == server_instance)
	{
		return;
	}

	std::string object_name;
	std::string property_name;
	std::string value;
	UObject* find_object = nullptr;

	// string parse
	ParsePropertyCall(recv_string, object_name, property_name, value, find_object);

	// property setting
	SetPropertyValue(find_object, property_name, value);

	// command send back
	// to set property on the client
	ResendCommand(recv_string, Resend_Command::OTHER_CLIENT);
}

inline void Recv_FunctionCall(const std::string& recv_string)
{
	if (nullptr == server_instance)
	{
		return;
	}

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

	// check params & check target
	if ("none" == input_params)
	{
		if ("Server" == target)
		{
			std::cout << client_map[recv_data->socket_].client_name << " Call Function : " << function_name << "\n";
			find_function->CallFunction();
		}
		else
		{
			// send function to target
			ResendCommand(recv_string, Resend_Command::OTHER_CLIENT_NOT_ME);
		}
	}
	else
	{
		std::vector<int> input_vector;
		ParseParams(input_params, input_vector);

		if ("Server" == target)
		{
			std::cout << client_map[recv_data->socket_].client_name << " Call Function : " << function_name << "\n";
			find_function->CallFunction(input_vector);
		}
		else
		{
			// send function to target
			ResendCommand(recv_string, Resend_Command::OTHER_CLIENT_NOT_ME);
		}
	}
}

inline void Recv_Disconnect()
{
	SOCKET target_socket = *server_instance->client_sessions_.find(recv_data->socket_);

	std::string disconnect_session = client_map[target_socket].client_name;
	std::cout << disconnect_session << " was disconnected..." << std::endl;

	client_map.erase(target_socket);
	server_instance->client_sessions_.erase(target_socket);
	
	shutdown(target_socket, SD_BOTH);

	if (INVALID_SOCKET == target_socket)
	{
		closesocket(target_socket);
	}
}

inline void ToHelperMethod(Server* server, RecvOverlapped* data)
{
	server_instance = server;
	recv_data = data;
	
	std::string recv_string = data->arr_buf_;

	if (std::string::npos != recv_string.find("New Connecter : "))
	{
		Recv_NewConnecter(recv_string);
	}
	else if (std::string::npos != recv_string.find("NewObject"))
	{
		Recv_CreateObject(recv_string);
	}
	else if ("GetAllProperty" == recv_string)
	{
		Recv_GetAllProperty();
	}
	else if (std::string::npos != recv_string.find("SetProperty : "))
	{
		Recv_SetProperty(recv_string);
	}
	else if (std::string::npos != recv_string.find("Call Function : "))
	{
		Recv_FunctionCall(recv_string);
	}
	else if ("Disconnect Client" == recv_string)
	{
		Recv_Disconnect();
	}
}