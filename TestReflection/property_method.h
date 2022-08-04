#pragma once
#include "global.h"
#include "UObject.h"

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

	if ("SomeTestClass*" == prop_type)
	{
		UObject* other = Reflection::GetInstance()->FindObjectBasedOnName(value);
		if (nullptr != other)
		{
			object->SetPropertyValue(prop_name, other);
			return;
		}
	}
}

void GetProperty(const std::string&);
void GetAllProperty(bool pause)
{
	for (const auto& elem : Reflection::GetInstance()->GetAllObject())
	{
		GetProperty(elem->GetName());
	}

	if (true == pause)
	{
		system("pause");
	}
}

void GetProperty(const std::string& object_name)
{
	UObject* object = Reflection::GetInstance()->FindObjectBasedOnName(object_name);
	if (nullptr == object)
	{
		std::cout << object_name << " �̶�� ������Ʈ�� �������� �ʽ��ϴ�..." << std::endl << std::endl;
		system("pause");
		return;
	}

	std::cout << "��ü �� : " << object_name << std::endl;
	std::cout << "��� ���� ���� : " << object->GetProperties().size() << std::endl;

	for (const auto& elem : object->GetProperties())
	{

		std::cout << elem.second->GetType() << " " << elem.first << "\t" << GetPropertyValue(object, elem.first) << std::endl;
	}

	std::cout << std::endl << std::endl;
}

void SetProperty()
{
	system("cls");

	GetAllProperty(false);

	while (true)
	{
		std::cout << "��ü �� �Է� Ȥ�� ������(0)... : ";

		std::string object_name;
		std::cin >> object_name;
		std::cout << std::endl;

		if ("0" == object_name)
		{
			break;
		}

		GetProperty(object_name);
		UObject* object = Reflection::GetInstance()->FindObjectBasedOnName(object_name);

		std::cout << "�����Ϸ��� Property ���� �Է��ϼ��� : ";
		std::string property_name;
		std::cin >> property_name;

		// �ش� property�� Ÿ���� ������ ������ ��� ����Ű���� ������Ʈ ���� �Է�...
		std::cout << "�� �Է� : ";
		std::string value;
		std::cin >> value;

		SetPropertyValue(object, property_name, value);
	}
}