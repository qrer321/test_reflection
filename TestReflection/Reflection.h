#pragma once

#include "global.h"
#include "UObject.h"

class Reflection
{
	friend class GarbageCollector;

public:
	static Reflection* GetInstance()
	{
		if (nullptr == reflection_instance_)
		{
			reflection_instance_ = new Reflection;
		}

		return reflection_instance_;
	}

	Reflection(const Reflection& other) = delete;
	Reflection(Reflection&& other) = delete;
	Reflection& operator= (const Reflection& other) = delete;
	Reflection& operator= (Reflection&& other) = delete;

	void Destroy() { delete reflection_instance_; }

	void Insert(UObject* object);
	void Delete(UObject* object);

	void CollectSweepObject(UObject* have_to_be_sweep);

	UObject* FindObjectBasedOnName(const std::string& object_name) const;

	bool ObjectExistence(UObject* target_object);
	const std::set<UObject*>& GetAllObject() const { return object_bundle_; }

private:
	Reflection() = default;
	~Reflection() = default;

	static Reflection*		reflection_instance_;

	std::set<UObject*>		object_bundle_;
	std::queue<UObject*>	sweep_queue_;
};

template <typename T, 
		  typename = std::enable_if_t<std::is_base_of_v<UObject, T>>>
T* NewObject()
{
	UObject* new_object = new T();
	Reflection::GetInstance()->Insert(new_object);

	return static_cast<T*>(new_object);
}

template <typename T,
		  typename = std::enable_if_t<std::is_base_of_v<UObject, T>>>
void AddToRoot(T* object)
{
	if (true == object->CheckObjectFlag(OBJECT_FLAG::ROOT))
	{
		return;
	}

	object->SetObjectFlag(OBJECT_FLAG::ROOT);
}

template <typename T,
		  typename = std::enable_if_t<std::is_base_of_v<UObject, T>>>
void SetPendingKill(T* object, bool all_reachable_object = false)
{
	object->SetObjectFlag(OBJECT_FLAG::PENDING_KILL);

	// setting pending kill all reachable object
	if (true == all_reachable_object)
	{
		for (const auto& prop : object->GetProperties())
		{
			// identify objects is UObject
			UObject* identify_object = object->GetPropertyValue<UObject*>(prop.first);
			if (true == Reflection::GetInstance()->ObjectExistence(identify_object))
			{
				if (identify_object->CheckObjectFlag(OBJECT_FLAG::PENDING_KILL))
				{
					continue;
				}

				identify_object->SetObjectFlag(OBJECT_FLAG::PENDING_KILL);
			}
		}
	}
}