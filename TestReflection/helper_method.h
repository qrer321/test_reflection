#pragma once

// 객체 생성, 객체 프로퍼티 변경, GC 수행

#include "Reflection.h"
#include "GarbageCollector.h"
#include "SomeTestClass.h"

#include "property_method.h"

void CreateObject()
{
	std::cout << "임의적인 클래스로 생성합니다." << std::endl;
	std::cout << "객체 명 입력... : ";

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

void FunctionCall()
{
	system("cls");
	std::cout << "객체 명 입력... : ";
	std::string object_name;
	std::cin >> object_name;
	std::cout << std::endl;

	UObject* find_object = Reflection::GetInstance()->FindObjectBasedOnName(object_name);
	if (nullptr == find_object)
	{
		std::cout << object_name << " 객체는 존재하지 않습니다." << std::endl;
		system("pause");

		return;
	}

	std::unordered_map<std::string, UFunction*> functions = find_object->GetFunctions();
	for (const auto& func : functions)
	{
		std::cout << func.first << std::endl;
	}

	std::string function_name;
	std::cout << std::endl;
	std::cout << "호출하려는 함수 명을 입력하세요 : ";
	std::cin >> function_name;

	UFunction* find_function = find_object->GetFunction(function_name);
	if (nullptr == find_function)
	{
		std::cout << function_name << " 함수는 존재하지 않습니다." << std::endl;
		system("pause");

		return;
	}

	std::cout << std::endl;
	std::cout << function_name << "'s Parameters ( ";
	std::vector<std::string> params = find_function->GetFunctionParams();
	for (const auto& param : params)
	{
		std::cout << param << ", ";
	}
	std::cout << " )" << std::endl << std::endl;

	if (false == params.empty())
	{
		std::string input_params;
		std::cout << "매개변수에 값을 입력하세요.(띄어쓰기로 구분합니다) : ";
		std::cin.ignore();
		std::getline(std::cin, input_params);

		//find_function->CallFunction("1");
	}

	system("pause");
}

void menu_output()
{
	bool break_console = false;

	while (true)
	{
		std::string output_string;
		std::string input_value;

		output_string = output_string +
			"1. 객체 생성                    2. 모든 객체 정보 확인\n" +
			"3. 프로퍼티 변경                4. GC 수행\n" +
			"5. 함수 호출";
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