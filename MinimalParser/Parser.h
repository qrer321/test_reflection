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
	Parser() = default;
	~Parser() = default;

	Parser(const Parser& other) = delete;
	Parser(Parser&& other) = delete;
	Parser operator=(const Parser& other) = delete;
	Parser operator=(Parser&& other) = delete;

	void SettingParser(const std::string& directory_path, const std::string& path_for_generated_file);
	void RunParser();
	void ClearParser();

private:
	void GetFilesNameFromDirectory(const std::string& path, std::vector<std::string>& files) const;
	void ExtractDataFromFile(const std::string& file_path);
	void GenerateHeaderFile(const std::string& class_name, const std::vector<PropertyInfo>& properties_info, const std::vector<FunctionInfo>& functions_info, const std::string& target_path) const;

	void CheckErrorPos(size_t find_pos);
	size_t GetClosestEndPos(const std::string& source, const size_t start_offset, bool& is_end_of_line) const;

	std::vector<std::string> files_;
	std::string path_for_generated_file_;
};