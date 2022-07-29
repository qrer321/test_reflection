#include "UObject.h"

UObject::~UObject()
{
	auto p_iter = properties.begin();
	while (properties.end() != p_iter)
	{
		delete p_iter++->second;
	}

	auto f_iter = functions.begin();
	while (functions.end() != f_iter)
	{
		delete f_iter++->second;
	}
}

UProperty* UObject::GetProperty(std::string _prop_name) const
{
	auto iter = properties.find(_prop_name);
	if (iter != properties.end())
	{
		return iter->second;
	}

	return nullptr;
}

UFunction* UObject::GetFunction(std::string _func_name) const
{
	auto iter = functions.find(_func_name);
	if (iter != functions.end())
	{
		return iter->second;
	}

	return nullptr;
}