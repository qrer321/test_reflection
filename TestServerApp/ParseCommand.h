#pragma once
#include "global.h"

inline bool ParsePropertyCall(const std::string& recv_string,
							  std::string& object_name, std::string& property_name,
							  std::string& value, UObject*& find_object)
{
	size_t name_pos = recv_string.find(":") + 2;
	size_t end_pos = recv_string.find("\\");
	object_name = recv_string.substr(name_pos, end_pos - name_pos);

	find_object = Reflection::GetInstance()->FindObjectBasedOnName(object_name);
	if (nullptr == find_object)
	{
		std::cout << "object does not exist" << std::endl;

		string_copy("property setting denied...");
		send(recv_data->socket_, buffer, static_cast<int>(buf_size), 0);
		return false;
	}

	size_t temp_pos = end_pos + 1;
	end_pos = recv_string.find("\\", temp_pos);
	property_name = recv_string.substr(temp_pos, end_pos - temp_pos);
	value = recv_string.substr(end_pos + 1, recv_string.size() - (end_pos + 1));

	return true;
}

inline bool ParseFunctionCall(const std::string& recv_string,
							  std::string& target, std::string& object_name,
							  std::string& function_name, std::string& input_params,
							  UObject*& find_object, UFunction*& find_function)
{
	size_t name_pos = recv_string.find(":") + 2;
	size_t end_pos = recv_string.find("\\");
	target = recv_string.substr(name_pos, end_pos - name_pos);

	size_t temp_pos = end_pos + 1;
	end_pos = recv_string.find("\\", temp_pos);
	object_name = recv_string.substr(temp_pos, end_pos - temp_pos);

	find_object = Reflection::GetInstance()->FindObjectBasedOnName(object_name);
	if (nullptr == find_object)
	{
		std::cout << "object does not exist" << std::endl;

		string_copy("function call denied...");
		send(recv_data->socket_, buffer, static_cast<int>(buf_size), 0);
		return false;
	}

	temp_pos = end_pos + 1;
	end_pos = recv_string.find("\\", temp_pos);
	function_name = recv_string.substr(temp_pos, end_pos - temp_pos);
	input_params = recv_string.substr(end_pos + 1, recv_string.size() - (end_pos + 1));

	find_function = find_object->GetFunction(function_name);
	if (nullptr == find_function)
	{
		std::cout << "function does not exist" << std::endl;

		string_copy("function call denied...");
		send(recv_data->socket_, buffer, static_cast<int>(buf_size), 0);
		return false;
	}

	return true;
}

inline void ParseParams(const std::string& input_params, std::vector<int>& input_vector)
{
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
}

inline void ResendCommand(const std::string& command, Resend_Command type)
{
	if (Resend_Command::OTHER_CLIENT == type)
	{
		lock.lock();
		for (const auto& session : server_instance->GetClientSessions())
		{
			string_copy(command);
			send(session, buffer, static_cast<int>(buf_size), 0);
		}
		lock.unlock();
	}
	else if (Resend_Command::OTHER_CLIENT_NOT_ME == type)
	{
		lock.lock();
		for (const auto& session : server_instance->GetClientSessions())
		{
			if (session == recv_data->socket_)
			{
				continue;
			}

			string_copy(command);
			send(session, buffer, static_cast<int>(buf_size), 0);
		}
		lock.unlock();
	}
}