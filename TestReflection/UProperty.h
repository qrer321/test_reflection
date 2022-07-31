#pragma once
#include "global.h"

class UProperty
{
public:
	UProperty()
		: property_type_hash_(0)
		, property_address_(nullptr)
		, property_size_(0)
	{}

	UProperty(std::string type_name, size_t type_hash, void* address, size_t size)
		: property_type_(std::move(type_name))
		, property_type_hash_(type_hash)
		, property_address_(address)
		, property_size_(size)
	{}

	UProperty(const UProperty& other) = delete;
	UProperty(UProperty&& other) = delete;
	UProperty operator= (const UProperty& other) = delete;
	UProperty operator= (UProperty&& other) = delete;

	~UProperty() = default;

	std::string GetType() const { return property_type_; }
	size_t GetTypeHash() const { return property_type_hash_; }
	void* GetAddress() const { return property_address_; }
	size_t GetSize() const { return property_size_; }

private:
	std::string property_type_;
	size_t		property_type_hash_;
	void*		property_address_;
	size_t		property_size_;
};