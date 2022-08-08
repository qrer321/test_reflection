#include "GarbageCollector.h"
#include "Reflection.h"

GarbageCollector* GarbageCollector::gc_instance = nullptr;

void GarbageCollector::ActivateGarbageCollector()
{
	MarkingObject();
	SweepObject();
}

void GarbageCollector::ClearGarbageCollector()
{
	SweepAllObject();
}

void GarbageCollector::MarkingObject()
{
	// marking object in somewhere

	// queue clear
	std::queue<UObject*> empty_queue;
	std::swap(Reflection::GetInstance()->sweep_queue_, empty_queue);

	// clear mark flag
	for (const auto& elem : Reflection::GetInstance()->object_bundle_)
	{
		elem->RemoveObjectFlag(OBJECT_FLAG::MARKED);
	}

	for (const auto& elem : Reflection::GetInstance()->object_bundle_)
	{
		if (true == elem->IsPendingKill())
		{
			// none pending kill object skipped
			continue;
		}

		elem->SetObjectFlag(OBJECT_FLAG::MARKED);

		for (const auto& prop : elem->GetProperties())
		{
			// identify objects is UObject
			UObject* identify_object = elem->GetPropertyValue<UObject*>(prop.first);
			
			if (true == Reflection::GetInstance()->ObjectExistence(identify_object))
			{
				if (false == identify_object->IsPendingKill())
				{
					continue;
				}
				                                              
				// to prevent dangling pointers
				elem->SetPropertyValue<UObject*>(prop.first, nullptr);
			}
		}
	}

	for (const auto& elem : Reflection::GetInstance()->object_bundle_)
	{
		if (true == elem->IsRootObject())
		{
			// root object skipped
			continue;
		}

		if (false == elem->IsCollectable())
		{
			// none collectable object skipped
			continue;
		}

		if (false == elem->IsMarkedObject() || true == elem->IsPendingKill())
		{
			// collect unmarked objects
			// collect pending kill objects
			Reflection::GetInstance()->CollectSweepObject(elem);
		}
	}
}

void GarbageCollector::SweepObject()
{
	while (false == Reflection::GetInstance()->sweep_queue_.empty())
	{
		UObject* sweep_object = Reflection::GetInstance()->sweep_queue_.front();
		Reflection::GetInstance()->sweep_queue_.pop();

		std::cout << sweep_object->GetName() << " is sweeped" << std::endl;
		Reflection::GetInstance()->Delete(sweep_object);
		delete sweep_object;
		sweep_object = nullptr;
	}
}

void GarbageCollector::SweepAllObject()
{
	for (auto object_elem : Reflection::GetInstance()->GetAllObject())
	{
		Reflection::GetInstance()->CollectSweepObject(object_elem);
	}

	SweepObject();
}
