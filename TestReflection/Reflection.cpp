#include "Reflection.h"

Reflection* Reflection::reflection_instance_ = nullptr;

void Reflection::Insert(UObject* object)
{
	object_bundle_.insert(object);
}

void Reflection::Delete(UObject* object)
{
	object_bundle_.erase(object);
}

void Reflection::CollectSweepObject(UObject* have_to_be_sweep)
{
	sweep_queue_.push(have_to_be_sweep);
}

UObject* Reflection::FindObjectBasedOnName(const std::string& object_name) const
{
	for (const auto& elem : object_bundle_)
	{
		if (object_name == elem->GetName())
		{
			return elem;
		}
	}

	return nullptr;
}
