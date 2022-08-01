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

	UFunction(const UFunction& other) = delete;
	UFunction(UFunction&& other) = delete;
	UFunction& operator= (const UFunction& other) = delete;
	UFunction& operator= (UFunction&& other) = delete;

	~UFunction() = default;

	const std::string& GetReturnType() const { return function_return_type_; }
	const std::vector<std::string>& GetFunctionParams() const { return function_params_; }
	//const std::function<void(class UObject*)>& GetFunction() { return func_ptr; }

	template <typename T>
	void CallFunction(class UObject* object, std::vector<std::string>& params) 
	{
		if (nullptr == object)
		{
			return;
		}
	
		
	}

private:
	std::string function_return_type_;
	std::vector<std::string> function_params_;
	std::function<void(UObject*, std::vector<std::string>)> func_;
};