#include "GarbageCollector.h"
#include "Reflection.h"

GarbageCollector* GarbageCollector::gc_instance = nullptr;

void GarbageCollector::ActivateGarbageCollector()
{
	MarkingObject();
	SweepObject();
}

void GarbageCollector::MarkingObject()
{
	// queue clear
	std::queue<UObject*> empty_queue;
	std::swap(Reflection::GetInstance()->sweep_queue_, empty_queue);

	for (const auto& elem : Reflection::GetInstance()->object_bundle_)
	{
		// ref_count üũ?
		// 

		// collectable üũ
		if (false == elem->IsCollectable())
		{
			continue;
		}

		Reflection::GetInstance()->CollectSweepObject(elem);
	}
}

void GarbageCollector::SweepObject()
{
	while (false == Reflection::GetInstance()->sweep_queue_.empty())
	{
		UObject* sweep_object = Reflection::GetInstance()->sweep_queue_.front();
		Reflection::GetInstance()->sweep_queue_.pop();

		Reflection::GetInstance()->Delete(sweep_object);
		delete sweep_object;
	}
}
