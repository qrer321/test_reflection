#include "Parser.h"

#define DIRECTORY_PATH "C:\\test_workspace\\test_reflection\\TestReflection\\"

int main()
{
	Parser parser;
	parser.ClearParser();
	parser.SettingParser(DIRECTORY_PATH, DIRECTORY_PATH);
	parser.RunParser();

	return 0;
}