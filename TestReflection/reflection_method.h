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

void SetProperty()
{
	system("cls");

	GetAllProperty(false);

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
	std::cout << function_name << "'s Parameters " << find_function->GetReturnType() << "( ";
	std::vector<std::string> params = find_function->GetFunctionParams();
	for (const auto& param : params)
	{
		std::cout << param << ", ";
	}
	std::cout << " )" << std::endl << std::endl;

	if (false == params.empty())
	{
		// 당장은 정수형 매개변수만이 가능함.
		std::string input_params;
		std::cout << "매개변수에 값을 입력하세요.(띄어쓰기로 구분합니다) : ";
		std::cin.ignore();
		std::getline(std::cin, input_params);

		std::vector<int> input_vector;
		size_t sub_pos = 0;
		while (true)
		{
			size_t blank_pos = input_params.find(' ', sub_pos);
			if (std::string::npos == blank_pos)
			{
				size_t end_pos = input_params.length();
				std::string end_string = input_params.substr(sub_pos, end_pos - sub_pos);

				input_vector.push_back(std::stoi(end_string));
				break;
			}

			std::string sub_string = input_params.substr(sub_pos, blank_pos - sub_pos);
			sub_pos = blank_pos + 1;

			input_vector.push_back(std::stoi(sub_string));
		}

		find_function->CallFunction(input_vector);
	}
	else
	{
		find_function->CallFunction();
	}

	system("pause");
}