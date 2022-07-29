#pragma once
#include <concurrent_queue.h>
#include <type_traits>
#include <atomic>
#include "UObject.h"

class ObjectPool
{
public:
	ObjectPool(int _size = 100)
	{
		max_size = _size;

		for (int i = 0; i < max_size; ++i)
		{
			UObject* new_object = new UObject();
			object_queue.push(new_object);
		}
	}

	~ObjectPool()
	{
		while (false == object_queue.empty())
		{
			UObject* temp_object;
			if (true == object_queue.try_pop(temp_object))
			{
				delete temp_object;
			}
		}

		max_size = 0;
	}

	UObject* PopObject()
	{
		UObject* return_value;

		if (false == object_queue.try_pop(return_value))
		{
			return_value = new UObject();
			++max_size;
		}

		return new (return_value) UObject;
	}

	void ReturnObject(UObject* _object)
	{
		_object->~UObject();
		object_queue.push(_object);
	}

private:
	concurrency::concurrent_queue<UObject*> object_queue;
	std::atomic<int> max_size;
};

static ObjectPool object_pool;