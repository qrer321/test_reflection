#pragma once

#include "..\TestReflection\global.h"
#include "..\TestReflection\Reflection.h"
#include "..\TestReflection\GarbageCollector.h"
#include "..\TestReflection\SomeTestClass.h"

#pragma comment(lib, "TestReflection.lib")

#include "Server.h"

const size_t buf_size = 1024;
char buffer[buf_size] = {};
void string_copy(const std::string& input)
{
	memset(&(buffer[0]), 0, buf_size);
	memcpy_s(buffer, buf_size, input.c_str(), input.size());
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

enum Resend_Command
{
	TO_SERVER,
	OTHER_CLIENT,
	OTHER_CLIENT_NOT_ME,
};

std::mutex lock;
static Server* server_instance = nullptr;
RecvOverlapped* recv_data = nullptr;
std::unordered_map<SOCKET, SessionStruct> client_map;

static void SetPropertyValue(UObject* object, const std::string& prop_name, const std::string& value)
{
	// should function be generated automatically..?
	auto find_iter = object->GetProperties().find(prop_name);
	if (find_iter == object->GetProperties().end())
	{
		std::cout << "잘못된 Property 명을 입력하였습니다..." << std::endl << std::endl;
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