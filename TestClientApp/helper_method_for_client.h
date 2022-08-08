#pragma once
#include "global.h"
#include "ParseCommand.h"

inline Client* client_instance = nullptr;

inline void Call_CreateObject()
{
	if (nullptr == client_instance)
	{
		return;
	}

	std::cout << "Create as an 'SomeTestClass' class." << std::endl;
	std::cout << "Enter object name... : ";

	std::string object_name;
	std::getline(std::cin, object_name);
	object_name = "NewObject : " + object_name;
	std::cout << std::endl;

	copy_and_send(object_name, client_instance->GetSessionSocket());
}

inline void Call_GetAllProperty()
{
	if (nullptr == client_instance)
	{
		return;
	}

	std::cout << "Server? or Own Client?" << std::endl;
	std::string target_session;
	std::getline(std::cin, target_session);

	if ("Server" == target_session)
	{
		std::cout << "Get all object properties...to Server" << std::endl;
		copy_and_send("GetAllProperty", client_instance->GetSessionSocket());
	}
	else
	{
		std::cout << "Get all object properties...to Own Client" << std::endl;

		for (const auto& object : Reflection::GetInstance()->GetAllObject())
		{
			if (true == object->IsPendingKill())
			{
				continue;
			}

			std::string object_name = object->GetName();
			std::cout << "Object Name : " << object_name << std::endl;
			std::cout << "member variable count : " << object->GetProperties().size() << std::endl;

			for (const auto& elem : object->GetProperties())
			{
				std::cout << elem.second->GetType() << " " << elem.first << "\t" << GetPropertyValue(object, elem.first) << std::endl;
			}

			std::cout << std::endl;
		}
	}
}

inline void Call_SetProperty()
{
	if (nullptr == client_instance)
	{
		return;
	}

	std::cout << "Enter object name..." << std::endl;

	std::string object_name;
	std::getline(std::cin, object_name);

	UObject* find_object = nullptr;
	if (false == PrintProperty(object_name, find_object))
	{
		return;
	}

	std::cout << "Enter the property name : ";
	std::string property_name;
	std::getline(std::cin, property_name);

	if (nullptr == find_object->GetProperty(property_name))
	{
		std::cout << "There is no property named " << property_name << std::endl;
		return;
	}

	std::cout << "Enter Value : ";
	std::string value;
	std::getline(std::cin, value);

	std::string send_string = "SetProperty : ";
	send_string += object_name + "\\" + property_name + "\\" + value;
	copy_and_send(send_string, client_instance->GetSessionSocket());
}

inline void Call_FunctionCall()
{
	if (nullptr == client_instance)
	{
		return;
	}

	// Input Object Name
	std::cout << "Enter object name... : ";
	std::string object_name;
	std::getline(std::cin, object_name);

	UObject* find_object = Reflection::GetInstance()->FindObjectBasedOnName(object_name);
	if (nullptr == find_object)
	{
		std::cout << "There is no object named \"" << object_name << "\"" << std::endl;
		return;
	}

	const std::unordered_map<std::string, UFunction*> functions = find_object->GetFunctions();
	for (const auto& func : functions)
	{
		std::cout << func.first << std::endl;
	}

	// Input Function Name
	std::string function_name;
	std::cout << std::endl;
	std::cout << "Enter the name of the function : ";
	std::getline(std::cin, function_name);

	UFunction* find_function = find_object->GetFunction(function_name);
	if (nullptr == find_function)
	{
		std::cout << "There is no function named \"" << function_name << "\"" << std::endl;
		return;
	}

	// Print Function's Info
	std::cout << std::endl;
	std::cout << function_name << "'s Parameters " << find_function->GetReturnType() << "( ";
	const std::vector<std::string>& params = find_function->GetFunctionParams();
	for (const auto& param : params)
	{
		std::cout << param << ", ";
	}
	std::cout << " )" << std::endl << std::endl;

	std::string input_params;
	if (false == params.empty())
	{
		// 당장은 정수형 매개변수만이 가능함.
		std::cout << "Enter a value for the parameter (separated by spacing): ";
		std::getline(std::cin, input_params);
	}
	else
	{
		input_params = "none";
	}

	std::string target;
	std::cout << "server / client / netmulticast" << std::endl;
	std::getline(std::cin, target);

	std::string send_string = "Call Function : ";
	send_string += target + "\\" + object_name + "\\" + function_name + "\\" + input_params;
	copy_and_send(send_string, client_instance->GetSessionSocket());
}

inline void Call_DestroyObject()
{
	if (nullptr == client_instance)
	{
		return;
	}

	copy_and_send("GetAllProperty", client_instance->GetSessionSocket());
	std::cout << std::endl;

	Sleep(5);
	std::cout << "select the object you want to destroy" << std::endl;

	std::string selected_object;
	std::getline(std::cin, selected_object);
	std::cout << std::endl;

	if (nullptr == Reflection::GetInstance()->FindObjectBasedOnName(selected_object))
	{
		std::cout << "There is no object named \"" << selected_object << "\"" << std::endl;
		return;
	}

	selected_object = "Destroy Object : " + selected_object;
	copy_and_send(selected_object, client_instance->GetSessionSocket());
}

inline void Call_Disconnect()
{
	std::cout << "Exit the connection." << std::endl;
	copy_and_send("Disconnect Client", client_instance->GetSessionSocket());

	GarbageCollector::GetInstance()->ClearGarbageCollector();
}

inline void DummyUpdate()
{
	if (nullptr == client_instance)
	{
		return;
	}

	std::cout << "Enter object name..." << std::endl;

	std::string object_name;
	std::getline(std::cin, object_name);

	UObject* find_object = nullptr;
	if (false == PrintProperty(object_name, find_object))
	{
		return;
	}

	std::cout << "Enter the property name : ";
	std::string property_name;
	std::getline(std::cin, property_name);

	if (nullptr == find_object->GetProperty(property_name))
	{
		std::cout << "There is no property named " << property_name << std::endl;
		return;
	}

	std::cout << "Enter Value : ";
	std::string value;
	std::getline(std::cin, value);

	SetPropertyValue(find_object, property_name, value);
	std::cout << "client call dummy updated" << std::endl;
}

inline void Call_SetGCMax()
{
	std::string time_string;
	std::getline(std::cin, time_string);
	int time_int = std::stoi(time_string);

	std::string gc_timer = "GCMax : " + std::to_string(time_int);
	copy_and_send(gc_timer, client_instance->GetSessionSocket());
}

inline void ToHelperMethod(const std::string& recv_string)
{
	if (std::string::npos != recv_string.find("Create Object : "))
	{
		size_t name_pos = recv_string.find(':') + 2;
		size_t name_end_pos = recv_string.find('\\');
		std::string object_name = recv_string.substr(name_pos, name_end_pos - name_pos);
		std::string owner_name = recv_string.substr(name_end_pos + 1, recv_string.size() - (name_end_pos + 1));
	
		auto new_object = NewObject<SomeTestClass>();
		new_object->SetName(object_name);
		new_object->SetOwnerSession(owner_name);
	}
	else if (std::string::npos != recv_string.find("SetProperty : "))
	{
		std::string object_name;
		std::string property_name;
		std::string value;

		size_t name_pos = recv_string.find(':') + 2;
		size_t end_pos = recv_string.find('\\');
		object_name = recv_string.substr(name_pos, end_pos - name_pos);

		UObject* find_object = Reflection::GetInstance()->FindObjectBasedOnName(object_name);
		if (nullptr == find_object)
		{
			std::cout << "object does not exist" << std::endl;
			return;
		}

		size_t temp_pos = end_pos + 1;
		end_pos = recv_string.find('\\', temp_pos);
		property_name = recv_string.substr(temp_pos, end_pos - temp_pos);
		value = recv_string.substr(end_pos + 1, recv_string.size() - (end_pos + 1));

		SetPropertyValue(find_object, property_name, value);
	}
	else if (std::string::npos != recv_string.find("Call Function : "))
	{
		std::string target;
		std::string object_name;
		std::string function_name;
		std::string input_params;
		UObject* find_object = nullptr;
		UFunction* find_function = nullptr;

		bool success_check = true;
		success_check = ParseFunctionCall(recv_string, target, object_name,
			function_name, input_params, find_object, find_function);

		if (false == success_check)
		{
			return;
		}

		std::cout << find_object->GetOwnerSession() << " client call function " 
			<< function_name <<  std::endl;
		if ("none" == input_params)
		{
			find_function->CallFunction();
		}
		else
		{
			std::vector<int> input_vector;
			ParseParams(input_params, input_vector);

			find_function->CallFunction(input_vector);
		}
	}
	else if (std::string::npos != recv_string.find("Destroy Object : "))
	{
		UObject* find_object = nullptr;
		ParseDestroyCall(recv_string, find_object);

		SetPendingKill(find_object);
	}
	else if (std::string::npos != recv_string.find("GCMax : "))
	{
		const size_t name_pos = recv_string.find(':') + 2;
		const std::string time = recv_string.substr(name_pos, recv_string.size() - name_pos);

		client_instance->SetGCTimer(std::stof(time));
	}
	else
	{
		std::cout << "Recv : " << recv_string << std::endl;
	}
}