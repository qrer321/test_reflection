#pragma once

// 객체 생성, 객체 프로퍼티 변경, GC 수행

#include "Reflection.h"
#include "GarbageCollector.h"
#include "SomeTestClass.h"

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
		std::cout << "잘못된 Property 명을 입력하였습니다..." << std::endl << std::endl;
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

void ParamFolds(std::vector<std::string>& output_params) { return; }

template <typename T, typename... Types>
void ParamFolds(std::vector<std::string>& output_params, const T& first_param, Types... other_params)
{
	output_params.push_back(std::to_string(first_param));
	ParamFolds(output_params, other_params...);
}

template <typename T, typename... Types>
void CallFunction(UObject* object, const std::string& func_name, Types... params)
{
	std::vector<std::string> type_vector;
	ParamFolds(type_vector, params...);

	UFunction* object_func = object->GetFunction(func_name);
	if (nullptr != object_func)
	{
		object_func->CallFunction<T>(object, type_vector);
	}
}

void CreateObject()
{
	std::cout << "임의적인 클래스로 생성합니다." << std::endl;
	std::cout << "객체 명 입력... : " << std::endl;

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

void GetProperty(const std::string& object_name)
{
	UObject* object = Reflection::GetInstance()->FindObjectBasedOnName(object_name);
	if (nullptr == object)
	{
		std::cout << object_name << " 이라는 오브젝트는 존재하지 않습니다..." << std::endl << std::endl;
		system("pause");
		return;
	}

	std::cout << "객체 명 : " << object_name << std::endl;
	std::cout << "멤버 변수 개수 : " << object->GetProperties().size() << std::endl;

	for (const auto& elem : object->GetProperties())
	{
		
		std::cout << elem.second->GetType() << " " << elem.first << "\t" << GetPropertyValue(object, elem.first) << std::endl;
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
	system("cls");

	while (true)
	{
		std::cout << "객체 명 입력 혹은 나가기(0)... : ";

		std::string object_name;
		std::cin >> object_name;
		std::cout << std::endl;

		if ("0" == object_name)
		{
			break;
		}

		GetProperty(object_name);
		UObject* object = Reflection::GetInstance()->FindObjectBasedOnName(object_name);
		
		std::cout << "변경하려는 Property 명을 입력하세요 : ";
		std::string property_name;
		std::cin >> property_name;

		// 해당 property의 타입이 포인터 형태인 경우 가리키려는 오브젝트 명을 입력...
		std::cout << "값 입력 : ";
		std::string value;
		std::cin >> value;

		SetPropertyValue(object, property_name, value);
	}
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
			"5. 종료";
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
			break_console = true;
			break;
		default:
			break;
		}

		system("cls");

		if (true == break_console)
		{
			break;
		}
	}
}