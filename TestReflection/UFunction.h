#pragma once
#include "global.h"

class UFunction
{
public:
	UFunction()
		: function_return_type_()
		, function_params_()
	{
	}
	UFunction(std::string return_type, std::vector<std::string> params)
		: function_return_type_(std::move(return_type))
		, function_params_(std::move(params))
	{
	}

	//UFunction(const UFunction& other) = delete;
	//UFunction(UFunction&& other) = delete;
	//UFunction& operator= (const UFunction& other) = delete;
	//UFunction& operator= (UFunction&& other) = delete;

	~UFunction() = default;

private:
	std::string function_return_type_;
	std::vector<std::string> function_params_;

	//std::function<>
};