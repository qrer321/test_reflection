#pragma once
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>

struct PropertyInfo
{
	std::string type;
	std::string name;
};

struct FunctionInfo
{
	std::string					return_type;
	std::string					name;
	std::vector<std::string>	params;
};

class Parser
{
public:
	Parser();
	~Parser() = default;

	void SettingParser(const std::string& _directory_path, const std::string& _path_for_generated_file);
	void RunParser();
	void ClearParser();

private:
	void GetFilesNameFromDirectory(const std::string& _path, std::vector<std::string>& _files);
	void ExtractDataFromFile(const std::string& _file_path);
	void GenerateHeaderFile(const std::string& _class_name, const std::vector<PropertyInfo>& _properties_info, const std::vector<FunctionInfo>& _functions_info, const std::string& _target_path);

	void CheckErrorPos(size_t find_pos);
	size_t GetClosestEndPos(const std::string& source, const size_t start_offset, bool& is_end_of_line);

	std::vector<std::string> files;
	std::string path_for_generated_file;
};