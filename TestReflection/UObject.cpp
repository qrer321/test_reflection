#include "UObject.h"

UObject::~UObject()
{
	auto p_iter = properties_.begin();
	while (properties_.end() != p_iter)
	{
		delete p_iter++->second;
	}

	auto f_iter = functions_.begin();
	while (functions_.end() != f_iter)
	{
		delete f_iter++->second;
	}
}

UProperty* UObject::GetProperty(std::string _prop_name) const
{
	auto iter = properties_.find(_prop_name);
	if (iter != properties_.end())
	{
		return iter->second;
	}

	return nullptr;
}

UFunction* UObject::GetFunction(std::string _func_name) const
{
	auto iter = functions_.find(_func_name);
	if (iter != functions_.end())
	{
		return iter->second;
	}

	return nullptr;
}