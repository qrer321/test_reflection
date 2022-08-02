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

enum OBJECT_FLAG
{
	ROOT = 0x01 << 0,
	PENDING_KILL = 0x01 << 1,
	COLLECTABLE = 0x01 << 2,
	MARKED = 0x01 << 3,
};

class UObject
{
public:
	UObject()
		: flag_()
	{
		SetObjectFlag(OBJECT_FLAG::COLLECTABLE);
	}
	virtual ~UObject();

	UObject(const UObject& other) = default;
	UObject(UObject&& other) = delete;
	UObject& operator= (const UObject& other) = default;
	UObject& operator= (UObject&& other) = delete;

	// get, set value method
	std::unordered_map<std::string, UProperty*>& GetProperties() { return properties_; }
	std::unordered_map<std::string, UFunction*>& GetFunctions() { return functions_; }
	template <typename T> T GetPropertyValue(const std::string& prop_name) const;
	template <typename T> void SetPropertyValue(std::string prop_name, T value);

	// helper method
	void SetObjectFlag(OBJECT_FLAG mark_enum) { flag_ = mark_enum | flag_; }
	void RemoveObjectFlag(OBJECT_FLAG mark_enum) { flag_ = ~mark_enum & flag_; }
	bool CheckObjectFlag(OBJECT_FLAG mark_enum) { return flag_ & mark_enum ? true : false; }

	bool IsCollectable() { return CheckObjectFlag(OBJECT_FLAG::COLLECTABLE); }
	bool IsRootObject() { return CheckObjectFlag(OBJECT_FLAG::ROOT); }
	bool IsMarkedObject() { return CheckObjectFlag(OBJECT_FLAG::MARKED); }
	bool IsPendingKill() { return CheckObjectFlag(OBJECT_FLAG::PENDING_KILL); }

	void SetName(const std::string& name) { name_ = name; }
	std::string GetName() const { return name_; }

	UProperty* GetProperty(const std::string& prop_name) const;
	UFunction* GetFunction(const std::string& func_name) const;

protected:
	void SetProperties(const std::unordered_map<std::string, UProperty*>& properties) { properties_ = properties; }
	void SetFunctions(const std::unordered_map<std::string, UFunction*>& functions) { functions_ = functions; }

private:
	std::string name_;
	int flag_;

	std::unordered_map<std::string, UProperty*> properties_;
	std::unordered_map<std::string, UFunction*> functions_;
};

template<typename T>
inline T UObject::GetPropertyValue(const std::string& prop_name) const
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
	
	if (prop_info->GetTypeHash() == typeid(T).hash_code() ||
		typeid(UObject*).hash_code() == typeid(T).hash_code())
	{
		*(static_cast<T*>(prop_info->GetAddress())) = value;
	}
}