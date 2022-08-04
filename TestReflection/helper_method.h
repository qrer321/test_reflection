#pragma once

// ��ü ����, ��ü ������Ƽ ����, GC ����

#include "Reflection.h"
#include "GarbageCollector.h"
#include "SomeTestClass.h"

#include "reflection_method.h"

void CreateObject()
{
	std::cout << "�������� Ŭ������ �����մϴ�." << std::endl;
	std::cout << "��ü �� �Է�... : ";

	std::string object_name;
	std::cin >> object_name;
	std::cout << std::endl;

	SomeTestClass* test_object = NewObject<SomeTestClass>();
	test_object->SetName(object_name);
}

void ActivateGarbageCollector()
{
	GarbageCollector::GetInstance()->ActivateGarbageCollector();
}

void menu_output()
{
	bool break_console = false;

	while (true)
	{
		std::string output_string;
		std::string input_value;

		output_string = output_string +
			"1. ��ü ����                    2. ��� ��ü ���� Ȯ��\n" +
			"3. ������Ƽ ����                4. GC ����\n" +
			"5. �Լ� ȣ��";
		std::cout << output_string;
		std::cout << std::endl << std::endl;

		std::cin >> input_value;
		switch (std::stoi(input_value))
		{
		case 1:
			CreateObject();
			break;
		case 2:
			GetAllProperty(true);
			break;
		case 3:
			SetProperty();
			break;
		case 4:
			system("cls");
			std::cout << "Activated Garbage Collector" << std::endl << std::endl;
			ActivateGarbageCollector();
			system("pause");
			break;
		case 5:
			FunctionCall();
			break;
		default:
			break_console = true;
			break;
		}

		system("cls");

		if (true == break_console)
		{
			break;
		}
	}
}