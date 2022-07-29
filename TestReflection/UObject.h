#pragma once
#include "global.h"
#include "UField.h"
#include "UProperty.h"
#include "UFunction.h"

// UField
//		 UStruct
//			*UClass*
//			*UFunction*
//		 UEnum
//		*UProperty*

class UObject
{
public:
	UObject() = default;
	virtual ~UObject();

	template <typename T>
	T GetPropertyValue(std::string _prop_name) const;
	template <typename T>
	void SetPropertyValue(std::string _prop_name, T _value);

private:
	UProperty* GetProperty(std::string _prop_name) const;
	UFunction* GetFunction(std::string _func_name) const;

protected:
	void SetProperties(std::unordered_map<std::string, UProperty*> _properties) { properties = _properties; }
	void SetFunctions(std::unordered_map<std::string, UFunction*> _functions) { functions = _functions; }

private:
	std::string object_name;

	std::unordered_map<std::string, UProperty*> properties;
	std::unordered_map<std::string, UFunction*> functions;
};

template<typename T>
inline T UObject::GetPropertyValue(std::string _prop_name) const
{
	UProperty* temp_prop = GetProperty(_prop_name);
	if (nullptr == temp_prop)
	{
		return T();
	}

	return *(reinterpret_cast<T*>(temp_prop->property_address));
}

template<typename T>
inline void UObject::SetPropertyValue(std::string _prop_name, T _value)
{
	UProperty* prop_info = GetProperty(_prop_name);
	if (nullptr == prop_info)
	{
		return;
	}

	if (prop_info->property_type_hash == typeid(T).hash_code())
	{
		*(reinterpret_cast<T*>(prop_info->property_address)) = _value;
	}
}