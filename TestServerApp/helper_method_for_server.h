#pragma once
#include "global.h"
#include "ParseCommand.h"

inline void Recv_NewConnecter(const std::string& recv_string)
{
	if (nullptr == server_instance)
	{
		return;
	}

	const size_t name_pos = recv_string.find(':') + 2;
	const std::string client_name = recv_string.substr(name_pos, recv_string.size() - name_pos);

	helper_lock.lock();
	client_map[recv_data->socket_] = { client_name };
	helper_lock.unlock();

	std::cout << "New Connecter : " << client_name << " " << std::to_string(recv_data->socket_) << std::endl;
}

inline void Recv_CreateObject(const std::string& recv_string)
{
	if (nullptr == server_instance)
	{
		return;
	}

	const size_t name_pos = recv_string.find(':') + 2;
	const std::string object_name = recv_string.substr(name_pos, recv_string.size() - name_pos);
	if (nullptr != Reflection::GetInstance()->FindObjectBasedOnName(object_name))
	{
		std::cout << "object with the same name exists" << std::endl;

		copy_and_send("create denied...", recv_data->socket_);
		return;
	}

	helper_lock.lock();
	SomeTestClass* new_object = NewObject<SomeTestClass>();
	new_object->SetOwnerSession(client_map[recv_data->socket_].client_name);
	new_object->SetName(object_name);
	helper_lock.unlock();

	std::cout << "Created New Object : " << object_name << std::endl;

	const std::string send_string = object_name + " object created...";
	const std::string order_string = "Create Object : " + object_name + "\\" + client_map[recv_data->socket_].client_name;

	SendPacket(send_string, RPC_TYPE::RPC_NetMulticast);
	SendPacket(order_string, RPC_TYPE::RPC_NetMulticast);
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

	SendPacket(output_string, RPC_TYPE::RPC_Client);
}

inline void Recv_GetAllProperty()
{
	if (nullptr == server_instance)
	{
		return;
	}

	std::cout << client_map[recv_data->socket_].client_name << " Session Request All Property Info..." << std::endl;

	for (const auto& object : Reflection::GetInstance()->GetAllObject())
	{
		if (true == object->IsPendingKill())
		{
			continue;
		}

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
	helper_lock.lock();
	SetPropertyValue(find_object, property_name, value);
	helper_lock.unlock();

	// command print
	std::cout << client_map[recv_data->socket_].client_name << " Session Request " << object_name << "'s "
		<< property_name << " value to " << value << std::endl;

	// command send back
	// to set property on the client
	SendPacket(recv_string, RPC_TYPE::RPC_NetMulticast);
}

inline void Send_PropertyUpdate(UObject* object, const std::unordered_map<std::string, UProperty*>& properties)
{
	std::string object_name;
	std::string property_name;
	std::string value;
	std::string send_string;

	object_name = object->GetName();
	for (const auto& prop : properties)
	{
		property_name = prop.first;
		value = GetPropertyValue(object, property_name);

		send_string = "SetProperty : " + object_name + '\\' + property_name + '\\' + value;
		SendPacket(send_string, RPC_TYPE::RPC_NetMulticast);
	}
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

	if (false == ParseFunctionCall(recv_string, target, object_name,
		function_name, input_params, find_object, find_function))
	{
		return;
	}

	// if owner of the target object does not match the current session
	if ("server" == target)
	{
		std::string object_owner = find_object->GetOwnerSession();
		std::string current_session = client_map[recv_data->socket_].client_name;

		if (object_owner != current_session)
		{
			std::cout << object_name << " is not " << current_session << "'s object" << std::endl;
			std::cout << "Server Function Call Dropped" << std::endl;
			return;
		}
	}

	// check params & check target
	if ("none" == input_params)
	{
		if ("server" == target)
		{
			std::cout << client_map[recv_data->socket_].client_name << " Call Function : " << function_name << "\n";
			find_function->CallFunction();
		}
		else if ("client" == target)
		{
			SendPacket(recv_string, RPC_TYPE::RPC_Client);
		}
		else if ("netmulticast" == target)
		{
			SendPacket(recv_string, RPC_TYPE::RPC_NetMulticast);
		}
		else
		{
			std::cout << "wrong target" << std::endl;
			return;
		}
	}
	else
	{
		std::vector<int> input_vector;
		ParseParams(input_params, input_vector);

		if ("server" == target)
		{
			std::cout << client_map[recv_data->socket_].client_name << " Call Function : " << function_name << "\n";
			find_function->CallFunction(input_vector);
		}
		else if ("client" == target)
		{
			SendPacket(recv_string, RPC_TYPE::RPC_Client);
		}
		else if ("netmulticast" == target)
		{
			SendPacket(recv_string, RPC_TYPE::RPC_NetMulticast);
		}
		else
		{
			std::cout << "wrong target" << std::endl;
			return;
		}
	}
}

inline void Recv_DestroyObject(const std::string& recv_string)
{
	// Set Pending Kill
	UObject* find_object = nullptr;
	if (false == ParseDestroyCall(recv_string, find_object))
	{
		return;
	}

	SetPendingKill(find_object);
	SendPacket(recv_string, RPC_TYPE::RPC_NetMulticast);
}

inline void Recv_Disconnect()
{
	const SOCKET& target_socket = *server_instance->client_sessions_.find(recv_data->socket_);

	std::string disconnect_session = client_map[target_socket].client_name;
	std::cout << disconnect_session << " was disconnected..." << std::endl;

	client_map.erase(target_socket);
	server_instance->client_sessions_.erase(target_socket);

	// 연결을 끊은 Session의 객체들 Pending Kill 설정
	for (const auto& object : Reflection::GetInstance()->GetAllObject())
	{
		if (disconnect_session != object->GetOwnerSession()) 
		{
			continue;
		}

		std::string send_string = "Destroy Object : " + object->GetName();

		SetPendingKill(object);
		SendPacket(send_string, RPC_TYPE::RPC_NetMulticast);
	}
	server_instance->ForcedGCRun();
	
	shutdown(target_socket, SD_BOTH);

	if (INVALID_SOCKET == target_socket)
	{
		closesocket(target_socket);
	}
}

inline void Recv_SetGCMax(const std::string& recv_string)
{
	const size_t name_pos = recv_string.find(':') + 2;
	const std::string time = recv_string.substr(name_pos, recv_string.size() - name_pos);
	server_instance->SetGCTimer(std::stof(time));
	
	std::string send_string = "GCMax : " + time;
	SendPacket(send_string, RPC_TYPE::RPC_NetMulticast);
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
	else if (std::string::npos != recv_string.find("Destroy Object : "))
	{
		Recv_DestroyObject(recv_string);
	}
	else if (std::string::npos != recv_string.find("GCMax : "))
	{
		Recv_SetGCMax(recv_string);
	}
	else if ("Disconnect Client" == recv_string)
	{
		Recv_Disconnect();
	}
}