#include "Parser.h"
#include <iostream>

void Parser::SettingParser(const std::string& directory_path, const std::string& path_for_generated_file)
{
	path_for_generated_file_ = path_for_generated_file;
	GetFilesNameFromDirectory(directory_path, files_);
}

void Parser::RunParser()
{
	for (const auto& each_file : files_)
	{
		ExtractDataFromFile(each_file);
	}
}

void Parser::ClearParser()
{
	if (0 == files_.capacity())
	{
		return;
	}

	files_.clear();
	std::vector<std::string>().swap(files_);
}

void Parser::GetFilesNameFromDirectory(const std::string& path, std::vector<std::string>& files) const
{
	if (false == std::filesystem::exists(path))
	{
		assert(false);
		return;
	}

	for (const std::filesystem::directory_entry& elem : std::filesystem::recursive_directory_iterator(path))
	{
		if (".h" == elem.path().extension())
		{
			files.push_back(elem.path().string());
		}
	}
}

// 추후 find 함수의 성능 개선을 위해 Boyer-Moore 알고리즘 혹은 Knuth-Morris-Pratt 알고리즘을 고려해본다.
void Parser::ExtractDataFromFile(const std::string& file_path)
{
	// read file
	std::string file_string;
	std::ostringstream ss = {};
	std::ifstream input_file(file_path);
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
		// UCLASS가 없는 class의 경우 reflection이 정상적으로 동작하는지?
		return;
	}

	find_start_pos += find_string.length();
	find_end_pos = file_string.find(' ', find_start_pos);
	class_name = file_string.substr(find_start_pos, find_end_pos - find_start_pos);



	////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// find UPROPERTY() ///////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	find_string = "UPROPERTY()\n\t";
	while (true)
	{
		// 파일의 끝에 도달할 때 까지 검색

		// find UPROPERTY()\n\t
		find_start_pos = file_string.find(find_string, find_end_pos);
		if (std::string::npos == find_start_pos)
		{
			break;
		}

		// find property type
		find_start_pos += find_string.length();
		find_end_pos = file_string.find(' ', find_start_pos);
		CheckErrorPos(find_end_pos);

		property_type = file_string.substr(find_start_pos, find_end_pos - find_start_pos);

		// find property name
		find_start_pos = find_end_pos + 1;
		find_end_pos = file_string.find(';', find_start_pos);
		CheckErrorPos(find_end_pos);

		property_name = file_string.substr(find_start_pos, find_end_pos - find_start_pos);

		// input property info to vector
		PropertyInfo p_info = { property_type, property_name };
		properties_info.push_back(p_info);
	}


	// pos 초기화
	find_start_pos = 0;
	find_end_pos = 0;

	////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// find UFUNCTION() ///////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	find_string = "UFUNCTION()\n\t";
	while (true)
	{
		// 파일의 끝에 도달할 때 까지 검색

		// find UFUNCTION()\n\t
		find_start_pos = file_string.find(find_string, find_end_pos);
		if (std::string::npos == find_start_pos)
		{
			break;
		}

		// find function return type
		find_start_pos += find_string.length();
		find_end_pos = file_string.find(' ', find_start_pos);
		CheckErrorPos(find_end_pos);

		function_return_type = file_string.substr(find_start_pos, find_end_pos - find_start_pos);

		// find function name
		find_start_pos = find_end_pos + 1;
		find_end_pos = file_string.find('(', find_start_pos);
		CheckErrorPos(find_end_pos);

		function_name = file_string.substr(find_start_pos, find_end_pos - find_start_pos);

		// find function parameters
		size_t function_param_end_pos = 0;
		function_param_end_pos = file_string.find(')', find_end_pos);
		if (1 < function_param_end_pos - find_end_pos)
		{
			std::string param;
			bool end_of_line = false;
			while (false == end_of_line)
			{
				find_start_pos = find_end_pos + 1;
				find_end_pos = file_string.find(' ', find_start_pos);
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

	GenerateHeaderFile(class_name, properties_info, functions_info, path_for_generated_file_);
}

void Parser::GenerateHeaderFile(const std::string& class_name, const std::vector<PropertyInfo>& properties_info, const std::vector<FunctionInfo>& functions_info, const std::string& target_path) const
{
	std::string gen_file_path;
	if (true == target_path.empty())
	{
		gen_file_path = std::filesystem::current_path().string() + "\\" + class_name + ".generated.h";
	}
	else
	{
		gen_file_path = target_path + class_name + ".generated.h";
	}

	if (true == std::filesystem::exists(gen_file_path))
	{
		std::filesystem::remove({gen_file_path});
	}

	std::ofstream gen_file(gen_file_path);

	std::string generated_string =
		"#ifdef " + class_name + "_generated_h\n" +
		"#error " + class_name + ".generated.h already included, missing '#pragma once' in " + class_name + ".h\n" +
		"#endif\n" +
		"#define " + class_name + "_generated_h\n" +
		"\n" +
		"#ifdef UCLASS\n" +
		"#undef UCLASS\n" +
		"#define UCLASS()\n" +
		"#endif\n\n" +
		"#ifndef UCLASS\n" +
		"#define UCLASS() \\\n" +
		"\t\tstatic std::unordered_map<std::string, class UProperty*> reflection_properties; \\\n" +
		"\t\tstatic std::unordered_map<std::string, class UFunction*> reflection_functions;\n" +
		"#endif" +
		"\n\n" +
		"#ifdef GENERATED_BODY\n" +
		"#undef GENERATED_BODY\n" +
		"#endif\n\n" +
		"#define GENERATED_BODY() \\\n" +
		"private: \\\n" +
		"\t\tvoid AddReflectionInfo() \\\n" +
		"\t\t{ \\\n" + 
		"\t\t\treflection_properties = \\\n" +
		"\t\t\t{ \\\n";

	for (const PropertyInfo& p_info : properties_info)
	{
		generated_string += "\t\t\t\t{\"" + p_info.name + "\", new UProperty(\"" + p_info.type + "\", "
		"typeid(" + p_info.type + ").hash_code(), &" + p_info.name +", sizeof(" + p_info.name + ")) }, \\\n";
	}

	generated_string = generated_string +
		"\t\t\t}; \\\n \t\t\t\\\n" +
		"\t\t\treflection_functions = \\\n" +
		"\t\t\t{ \\\n";

	for (const FunctionInfo& f_info : functions_info)
	{
		generated_string += "\t\t\t\t{\"" + f_info.name + "\", new UFunction(\"" + f_info.return_type + "\", { ";

		for (const std::string& param_string : f_info.params)
		{
			generated_string = generated_string +
				"\"" + param_string + "\", ";
		}

		generated_string = generated_string +
		"})}, \\\n";
	}
		
	generated_string = generated_string +
		"\t\t\t}; \\\n \t\t\t\\\n" +
		"\t\t\tSetProperties(reflection_properties); \\\n" +
		"\t\t\tSetFunctions(reflection_functions); \\\n" +
		"\t\t} \\\n" +
		"public: \\\n" +
		"\t\t" + class_name + "() \\\n" +
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

size_t Parser::GetClosestEndPos(const std::string& source, const size_t start_offset, bool& is_end_of_line) const
{
	// check closest end point
	static std::string check_string_arr[] = { ",", ");", ";", " {" };

	size_t closest_point = UINT_MAX;
	size_t temp_pos = 0;
	std::string check_string;
	size_t arr_size = std::size(check_string_arr);

	for (size_t i = 0; i < arr_size; ++i)
	{
		temp_pos = source.find(check_string_arr[i], start_offset);
		if (std::string::npos != temp_pos &&
			closest_point > temp_pos)
		{
			closest_point = temp_pos + check_string_arr[i].length();
			check_string = check_string_arr[i];
		}
	}

	if (check_string == ");" || check_string == ";" || check_string == " {")
	{
		is_end_of_line = true;
	}

	return closest_point;
}
