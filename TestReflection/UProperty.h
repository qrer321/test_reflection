#pragma once
#include "global.h"

class UProperty
{
public:
	UProperty()
		: property_type_hash(0)
		, property_address(nullptr)
		, property_size(0)
	{}

	UProperty(size_t _type_hash, void* _address, size_t _size)
		: property_type_hash(_type_hash)
		, property_address(_address)
		, property_size(_size)
	{}

	UProperty(const UProperty& other) = delete;
	UProperty(UProperty&& other) = delete;
	UProperty operator= (const UProperty& other) = delete;
	UProperty operator= (UProperty&& other) = delete;

	~UProperty();

	size_t GetTypeHash() const { return property_type_hash; }
	void* GetAddress() const { return property_address; }
	size_t GetSize() const { return property_size; }

private:
	size_t	property_type_hash;
	void*	property_address;
	size_t	property_size;
};