#pragma once
#include <utility>

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
	T GetPropertyValue(std::string prop_name) const;
	template <typename T>
	void SetPropertyValue(std::string prop_name, T value);

private:
	UProperty* GetProperty(std::string prop_name) const;
	UFunction* GetFunction(std::string func_name) const;

protected:
	void SetProperties(const std::unordered_map<std::string, UProperty*>& properties) { properties_ = properties; }
	void SetFunctions(const std::unordered_map<std::string, UFunction*>& functions) { functions_ = functions; }

private:
	std::string object_name_;

	std::unordered_map<std::string, UProperty*> properties_;
	std::unordered_map<std::string, UFunction*> functions_;
};

template<typename T>
inline T UObject::GetPropertyValue(std::string prop_name) const
{
	UProperty* temp_prop = GetProperty(prop_name);
	if (nullptr == temp_prop)
	{
		return T();
	}

	return *(static_cast<T*>(temp_prop->GetAddress()));
}

template<typename T>
inline void UObject::SetPropertyValue(std::string prop_name, T value)
{
	UProperty* prop_info = GetProperty(prop_name);
	if (nullptr == prop_info)
	{
		return;
	}

	if (prop_info->GetTypeHash() == typeid(T).hash_code())
	{
		*(static_cast<T*>(prop_info->GetAddress())) = value;
	}
}