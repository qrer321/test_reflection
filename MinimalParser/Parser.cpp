#include "Parser.h"
#include <iostream>

Parser::Parser()
	: files()
{
}

void Parser::SettingParser(const std::string& _directory_path, const std::string& _path_for_generated_file)
{
	path_for_generated_file = _path_for_generated_file;
	GetFilesNameFromDirectory(_directory_path, files);
}

void Parser::RunParser()
{
	for (const auto& each_file : files)
	{
		ExtractDataFromFile(each_file);
	}
}

void Parser::ClearParser()
{
	if (0 == files.capacity())
	{
		return;
	}

	files.clear();
	std::vector<std::string>().swap(files);
}

void Parser::GetFilesNameFromDirectory(const std::string& _path, std::vector<std::string>& _files)
{
	if (false == std::filesystem::exists(_path))
	{
		assert(false);
		return;
	}

	for (const std::filesystem::directory_entry& elem : std::filesystem::recursive_directory_iterator(_path))
	{
		std::filesystem::path file_path = elem.path();
		if (".h" == file_path.extension())
		{
			_files.push_back(file_path.string());
		}
	}
}

// ���� find �Լ��� ���� ������ ���� Boyer-Moore �˰����� Ȥ�� Knuth-Morris-Pratt �˰������� �����غ���.
void Parser::ExtractDataFromFile(const std::string& _file_path)
{
	// read file
	std::string file_string;
	std::ostringstream ss = {};
	std::ifstream input_file(_file_path);
	if (false == input_file.is_open())
	{
		std::cerr << "Could not open the file" << std::endl;
		exit(EXIT_FAILURE);
	}

	ss << input_file.rdbuf();
	file_string = ss.str();
	input_file.close();

	// initialize variable
	size_t find_start_pos = 0;
	size_t find_end_pos = 0;

	std::string find_string;
	std::string class_name;

	std::vector<PropertyInfo> properties_info;
	std::string property_type;
	std::string property_name;

	std::vector<FunctionInfo> functions_info;
	std::string function_return_type;
	std::string function_name;
	std::vector<std::string> function_params;



	////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// find UCLASS() //////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	find_string = "UCLASS()\nclass ";
	find_start_pos = file_string.find(find_string);
	if (std::string::npos == find_start_pos)
	{
		// UCLASS�� ���� class�� ��� reflection�� ���������� �����ϴ���?
		return;
	}

	find_start_pos += find_string.length();
	find_end_pos = file_string.find(" ", find_start_pos);
	class_name = file_string.substr(find_start_pos, find_end_pos - find_start_pos);



	////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// find UPROPERTY() ///////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	find_string = "UPROPERTY()\n\t";
	while (true)
	{
		// ������ ���� ������ �� ���� �˻�

		// find UPROPERTY()\n\t
		find_start_pos = file_string.find(find_string, find_end_pos);
		if (std::string::npos == find_start_pos)
		{
			break;
		}

		// find property type
		find_start_pos += find_string.length();
		find_end_pos = file_string.find(" ", find_start_pos);
		CheckErrorPos(find_end_pos);

		property_type = file_string.substr(find_start_pos, find_end_pos - find_start_pos);

		// find property name
		find_start_pos = find_end_pos + 1;
		find_end_pos = file_string.find(";", find_start_pos);
		CheckErrorPos(find_end_pos);

		property_name = file_string.substr(find_start_pos, find_end_pos - find_start_pos);

		// input property info to vector
		PropertyInfo p_info = { property_type, property_name };
		properties_info.push_back(p_info);
	}


	// pos �ʱ�ȭ
	find_start_pos = 0;
	find_end_pos = 0;

	////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// find UFUNCTION() ///////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	find_string = "UFUNCTION()\n\t";
	while (true)
	{
		// ������ ���� ������ �� ���� �˻�

		// find UFUNCTION()\n\t
		find_start_pos = file_string.find(find_string, find_end_pos);
		if (std::string::npos == find_start_pos)
		{
			break;
		}

		// find function return type
		find_start_pos += find_string.length();
		find_end_pos = file_string.find(" ", find_start_pos);
		CheckErrorPos(find_end_pos);

		function_return_type = file_string.substr(find_start_pos, find_end_pos - find_start_pos);

		// find function name
		find_start_pos = find_end_pos + 1;
		find_end_pos = file_string.find("(", find_start_pos);
		CheckErrorPos(find_end_pos);

		function_name = file_string.substr(find_start_pos, find_end_pos - find_start_pos);

		// find function parameters
		size_t function_param_end_pos = 0;
		function_param_end_pos = file_string.find(")", find_end_pos);
		if (1 < function_param_end_pos - find_end_pos)
		{
			std::string param;
			bool end_of_line = false;
			while (false == end_of_line)
			{
				find_start_pos = find_end_pos + 1;
				find_end_pos = file_string.find(" ", find_start_pos);
				CheckErrorPos(find_end_pos);
				
				param = file_string.substr(find_start_pos, find_end_pos - find_start_pos);
				function_params.push_back(param);

				// variable name skip
				end_of_line = false;
				find_end_pos = GetClosestEndPos(file_string, find_end_pos, end_of_line);
				CheckErrorPos(find_end_pos);
			}
		}
		
		FunctionInfo f_info = { function_return_type, function_name, function_params };
		functions_info.push_back(f_info);

		function_params.clear();
		std::vector<std::string>().swap(function_params);

	}

	GenerateHeaderFile(class_name, properties_info, functions_info, path_for_generated_file);
}

void Parser::GenerateHeaderFile(const std::string& _class_name, const std::vector<PropertyInfo>& _properties_info, const std::vector<FunctionInfo>& _functions_info, const std::string& _target_path)
{
	std::string gen_file_path;
	if ("" == _target_path)
	{
		gen_file_path = std::filesystem::current_path().string() + "\\" + _class_name + ".generated.h";
	}
	else
	{
		gen_file_path = _target_path + _class_name + ".generated.h";
	}

	if (true == std::filesystem::exists(gen_file_path))
	{
		std::filesystem::remove({gen_file_path});
	}

	std::ofstream gen_file(gen_file_path);

	std::string generated_string =
		"#ifdef " + _class_name + "_generated_h\n" +
		"#error " + _class_name + ".generated.h already included, missing '#pragma once' in " + _class_name + ".h\n" +
		"#endif\n" +
		"#define " + _class_name + "_generated_h\n" +
		"\n" +
		"#ifdef UCLASS\n" +
		"#undef UCLASS\n" +
		"#endif\n\n" +
		"#define UCLASS() \\\n" +
		"\t\tstatic std::unordered_map<std::string, UProperty*> reflection_properties; \\\n" +
		"\t\tstatic std::unordered_map<std::string, UFunction*> reflection_functions;\n" +
		"\n\n" +
		"#ifdef GENERATED_BODY\n" +
		"#undef GENERATED_BODY\n" +
		"#endif\n\n" +
		"#define GENERATED_BODY() \\\n" +
		"public: \\\n" +
		"\t\tvoid AddReflectionInfo() \\\n" +
		"\t\t{ \\\n" + 
		"\t\t\treflection_properties = \\\n" +
		"\t\t\t{ \\\n";

	for (PropertyInfo p_info : _properties_info)
	{
		generated_string = generated_string +
			"\t\t\t\t{\"" + p_info.name + "\", new UProperty(typeid(" + p_info.type + ").hash_code(), &" + p_info.name +", sizeof(" + p_info.name + "))}, \\\n";
	}

	generated_string = generated_string +
		"\t\t\t}; \\\n \t\t\t\\\n" +
		"\t\t\treflection_functions = \\\n" +
		"\t\t\t{ \\\n";

	for (FunctionInfo f_info : _functions_info)
	{
		generated_string = generated_string +
			"\t\t\t\t{\"" + f_info.name + "\", new UFunction(\"" + f_info.return_type + "\", { ";

		for (std::string param_string : f_info.params)
		{
			generated_string = generated_string +
				"\"" + param_string + "\", ";
		}

		generated_string = generated_string +
		"})}, \\\n";
	}
		
	generated_string = generated_string +
		"\t\t\t}; \\\n \t\t\t\\\n" +
		"\t\t\tSetProperties(std::move(reflection_properties)); \\\n" +
		"\t\t\tSetFunctions(std::move(reflection_functions)); \\\n" +
		"\t\t} \\\n\t\t\\\n" +
		"\t\t" + _class_name + "() \\\n" +
		"\t\t{ \\\n" +
		"\t\t\tAddReflectionInfo(); \\\n\t\t}" +
		"\n\n\n";

	generated_string = generated_string +
		"#ifdef UPROPERTY\n" +
		"#undef UPROPERTY\n" +
		"#endif\n\n" +
		"#define UPROPERTY()\n\n\n";

	generated_string = generated_string +
		"#ifdef UFUNCTION\n" +
		"#undef UFUNCTION\n" +
		"#endif\n\n" +
		"#define UFUNCTION()\n\n\n";

	gen_file.write(generated_string.c_str(), generated_string.size());
	gen_file.close();
}

void Parser::CheckErrorPos(size_t find_pos)
{
	if (std::string::npos == find_pos)
	{
		// Something Wrong.
		assert(false);
	}

	// None Error
	return;
}

size_t Parser::GetClosestEndPos(const std::string& _source, const size_t _start_offset, bool& _is_end_of_line)
{
	// check closest end point
	static std::string check_string_arr[] = { ",", ");", ";", " {" };

	size_t closest_point = UINT_MAX;
	size_t temp_pos = 0;
	std::string check_string;
	size_t arr_size = sizeof(check_string_arr) / sizeof(check_string_arr[0]);

	for (size_t i = 0; i < arr_size; ++i)
	{
		temp_pos = _source.find(check_string_arr[i], _start_offset);
		if (std::string::npos != temp_pos &&
			closest_point > temp_pos)
		{
			closest_point = temp_pos + check_string_arr[i].length();
			check_string = check_string_arr[i];
		}
	}

	if (check_string == ");" || check_string == ";" || check_string == " {")
	{
		_is_end_of_line = true;
	}

	return closest_point;
}