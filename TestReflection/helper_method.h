#pragma once
#include "ObjectPool.h"

template <typename T>
T* NewObject()
{
	UObject* temp_object = object_pool.PopObject();
	return (T*)temp_object;
}