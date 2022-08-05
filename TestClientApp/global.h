#pragma once
#include "..\TestReflection\global.h"
#include "..\TestReflection\Reflection.h"
#include "..\TestReflection\GarbageCollector.h"
#include "..\TestReflection\SomeTestClass.h"

#pragma comment(lib, "TestReflection.lib")

#include "Client.h"

const size_t buf_size = 1024;
char buffer[buf_size] = {};
void string_copy(const std::string& input)
{
	memset(&(buffer[0]), 0, buf_size);
	memcpy_s(buffer, buf_size, input.c_str(), input.size());
}

static void SetPropertyValue(UObject* object, const std::string& prop_name, const std::string& value)
{
	// should function be generated automatically..?
	auto find_iter = object->GetProperties().find(prop_name);
	if (find_iter == object->GetProperties().end())
	{
		std::cout << "�߸��� Property ���� �Է��Ͽ����ϴ�..." << std::endl << std::endl;
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

bool PrintProperty(const std::string& object_name)
{
	UObject* object = Reflection::GetInstance()->FindObjectBasedOnName(object_name);
	if (nullptr == object)
	{
		std::cout << object_name << " �̶�� ������Ʈ�� �������� �ʽ��ϴ�..." << std::endl << std::endl;
		return false;
	}

	std::cout << "���� Ŭ�� : " << object->GetOwnerSession() << std::endl;
	std::cout << "��ü �� : " << object_name << std::endl;
	std::cout << "��� ���� ���� : " << object->GetProperties().size() << std::endl;

	for (const auto& elem : object->GetProperties())
	{
		std::cout << elem.second->GetType() << " " << elem.first << "\t" << GetPropertyValue(object, elem.first) << std::endl;
	}

	std::cout << std::endl << std::endl;

	return true;
}