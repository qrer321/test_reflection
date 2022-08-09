#include "Parser.h"

int main()
{
	// 간단한 경로설정
	std::string path = std::filesystem::current_path().string();
	std::string upper_string = "test_reflection";
	size_t upper_path = path.find(upper_string);
	path = path.substr(0, upper_path + upper_string.size());
	path += "\\TestReflection\\";

	Parser parser;
	parser.ClearParser();
	parser.SettingParser(path, path);
	parser.RunParser();

	return 0;
}