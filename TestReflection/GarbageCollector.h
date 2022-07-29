#pragma once
#include "Reflection.h"

class GarbageCollector
{
public:
	GarbageCollector() = default;
	~GarbageCollector() = default;

	GarbageCollector* GetInstance()
	{
		if (nullptr == gc_instance)
		{
			gc_instance = new GarbageCollector;
		}

		return gc_instance;
	}

	void CollectObject() {}
	void MarkReachableObject() {}
	void SweepUnreachableObject() {}

	void Destroy() { delete gc_instance; }

private:
	static GarbageCollector* gc_instance;
};