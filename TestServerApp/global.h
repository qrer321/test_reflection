#pragma once

#include "../TestReflection/global.h"
#include "../TestReflection/Reflection.h"
#include "../TestReflection/GarbageCollector.h"
#include "../TestReflection/delta_timer.h"
#include "../TestReflection/SomeTestClass.h"

#pragma comment(lib, "TestReflection.lib")

#include "Server.h"

constexpr size_t buf_size = 1024;
inline char buffer[buf_size] = {};

inline void copy_and_send(const std::string& input_string, const SOCKET& target_socket)
{
	memset(&(buffer[0]), 0, buf_size);
	memcpy_s(buffer, buf_size, input_string.c_str(), input_string.size());

	send(target_socket, buffer, static_cast<int>(buf_size), 0);
}

struct SessionStruct
{
	std::string client_name;
};

struct PropertyStruct
{
	std::string					object_name_;
	std::vector<std::string>	properties_info;
};

enum RPC_TYPE
{
	RPC_Server,
	RPC_Client,
	RPC_NetMulticast,
};

std::mutex helper_lock;
static Server* server_instance = nullptr;
RecvOverlapped* recv_data = nullptr;
std::unordered_map<SOCKET, SessionStruct> client_map;

static void SetPropertyValue(UObject* object, const std::string& prop_name, const std::string& value)
{
	// should function be generated automatically..?
	auto find_iter = object->GetProperties().find(prop_name);
	if (find_iter == object->GetProperties().end())
	{
		std::cout << "There is no property named " << prop_name << std::endl;
		return;
	}

	if (true == value.empty())
	{
		std::cout << "Please enter a value..." << std::endl;
		return;
	}

	std::string prop_type = find_iter->second->GetType();

	if ("int" == prop_type)
	{
		object->SetPropertyValue(prop_name, std::stoi(value));
		return;
	}
	else if ("SomeTestClass*" == prop_type)
	{
		UObject* other = Reflection::GetInstance()->FindObjectBasedOnName(value);
		if (nullptr != other)
		{
			object->SetPropertyValue(prop_name, other);
			return;
		}
	}
}

static std::string GetPropertyValue(UObject* object, const std::string& prop_name)
{
	auto find_iter = object->GetProperties().find(prop_name);
	std::string prop_type = find_iter->second->GetType();

	if ("int" == prop_type)
	{
		return std::to_string(object->GetPropertyValue<int>(prop_name));
	}

	if ("SomeTestClass*" == prop_type)
	{
		SomeTestClass* target_object = object->GetPropertyValue<SomeTestClass*>(prop_name);
		if (nullptr != target_object)
		{
			return target_object->GetName();
		}
		else
		{
			return "nullptr";
		}
	}

	return std::string();
}