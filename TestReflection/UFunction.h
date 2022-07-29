#pragma once
#include "global.h"

class UObject;
class UFunction
{
public:
	UFunction()
		: function_return_type()
		, function_params()
	{
	}
	UFunction(std::string _return_type, const std::vector<std::string>& _params)
		: function_return_type(_return_type)
		, function_params(_params)
	{
	}

	~UFunction() = default;

private:
	std::string function_return_type;
	std::vector<std::string> function_params;
};