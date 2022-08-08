#pragma once
#include "../TestReflection/global.h"
#include "../TestReflection/Reflection.h"
#include "../TestReflection/GarbageCollector.h"
#include "../TestReflection/delta_timer.h"
#include "../TestReflection/SomeTestClass.h"

#pragma comment(lib, "TestReflection.lib")

#include "Client.h"

constexpr size_t buf_size = 1024;
inline char buffer[buf_size] = {};

inline void copy_and_send(const std::string& input_string, const SOCKET& target_socket)
{
	memset(&(buffer[0]), 0, buf_size);
	memcpy_s(buffer, buf_size, input_string.c_str(), input_string.size());

	send(target_socket, buffer, static_cast<int>(buf_size), 0);
}

static void SetPropertyValue(UObject* object, const std::string& prop_name, const std::string& value)
{
	// should function be generated automatically..?
	const auto find_iter = object->GetProperties().find(prop_name);
	if (find_iter == object->GetProperties().end())
	{
		std::cout << "Invalid Property Name entered..." << std::endl << std::endl;
		return;
	}

	const std::string& prop_type = find_iter->second->GetType();
	if ("int" == prop_type)
	{
		object->SetPropertyValue(prop_name, std::stoi(value));
		return;
	}

	if ("SomeTestClass*" == prop_type)
	{
		UObject* target_object = Reflection::GetInstance()->FindObjectBasedOnName(value);
		if (nullptr != target_object)
		{
			object->SetPropertyValue(prop_name, target_object);
			return;
		}
	}
}

static std::string GetPropertyValue(UObject* object, const std::string& prop_name)
{
	const auto find_iter = object->GetProperties().find(prop_name);
	const std::string& prop_type = find_iter->second->GetType();

	if ("int" == prop_type)
	{
		return std::to_string(object->GetPropertyValue<int>(prop_name));
	}

	if ("SomeTestClass*" == prop_type)
	{
		SomeTestClass* target_object = object->GetPropertyValue<SomeTestClass*>(prop_name);
		if (nullptr == target_object)
		{
			return "nullptr";
		}

		return target_object->GetName();
	}

	return std::string();
}

inline bool PrintProperty(const std::string& object_name, UObject*& find_object)
{
	find_object = Reflection::GetInstance()->FindObjectBasedOnName(object_name);
	if (nullptr == find_object)
	{
		std::cout << object_name << " object does not exist..." << std::endl << std::endl;
		return false;
	}

	std::cout << "Owned Session				 : " << find_object->GetOwnerSession() << std::endl;
	std::cout << "Object Name				 : " << object_name << std::endl;
	std::cout << "Number of member variables : " << find_object->GetProperties().size() << std::endl;

	for (const auto& elem : find_object->GetProperties())
	{
		std::cout << elem.second->GetType() << " " << elem.first << "\t" << GetPropertyValue(find_object, elem.first) << std::endl;
	}

	std::cout << std::endl << std::endl;

	return true;
}