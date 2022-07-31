#pragma once

// ��ü ����, ��ü ������Ƽ ����, GC ����

#include "SomeTestClass.h"
#include "Reflection.h"

void TypenameConverter()
{
	// int
	// SomeTestClass*
}

void CreateObject()
{
	
	std::cout << "�������� Ŭ������ �����մϴ�." << std::endl;
	std::cout << "��ü �� �Է�..." << std::endl;

	std::string object_name;
	std::cin >> object_name;
	std::cout << std::endl;

	auto test_object = NewObject<SomeTestClass>();
	test_object->SetName(object_name);

}

void ActivateGarbageCollector()
{
	
}

void GetProperty(const std::string& object_name)
{
	UObject* object = Reflection::GetInstance()->FindObjectBasedOnName(object_name);
	if (nullptr == object)
	{
		std::cout << "something wrong..." << std::endl;
		system("pause");
		return;
	}

	std::cout << "��ü �� : " << object_name << std::endl;
	std::cout << "��� ���� ���� : " << object->GetProperties().size() << std::endl;

	for (const auto& elem : object->GetProperties())
	{
		std::cout << elem.second->GetType() << " " << elem.first << " " << object->GetPropertyValue<int>(elem.first) << std::endl;
	}

	std::cout << std::endl << std::endl;
}

void GetAllProperty()
{
	for (const auto& elem : Reflection::GetInstance()->GetAllObject())
	{
		GetProperty(elem->GetName());
	}

	system("pause");
}

void SetProperty()
{
	
}

void menu_output()
{
	while (true)
	{
		std::string output_string;
		std::string input_value;

		output_string = output_string +
			"1. ��ü ����                    2. ��� ��ü ���� Ȯ��\n" +
			"3. ������Ƽ ����                4. GC ����";
		std::cout << output_string;
		std::cout << std::endl << std::endl;

		std::cin >> input_value;
		switch (std::stoi(input_value))
		{
		case 1:
			CreateObject();
			break;
		case 2:
			GetAllProperty();
			break;
		case 3:
			SetProperty();
			break;
		case 4:
			ActivateGarbageCollector();
			break;
		default:
			break;
		}

		system("cls");
	}
}