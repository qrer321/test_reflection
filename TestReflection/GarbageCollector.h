#pragma once

class GarbageCollector
{
public:
	static GarbageCollector* GetInstance()
	{
		if (nullptr == gc_instance)
		{
			gc_instance = new GarbageCollector;
		}

		return gc_instance;
	}

	void Destroy() { delete gc_instance;}

	GarbageCollector(const GarbageCollector& other) = delete;
	GarbageCollector(GarbageCollector&& other) = delete;
	GarbageCollector& operator= (const GarbageCollector& other) = delete;
	GarbageCollector& operator= (GarbageCollector&& other) = delete;

	void ActivateGarbageCollector();


private:
	GarbageCollector() = default;
	~GarbageCollector() = default;

	void MarkingObject();
	void SweepObject();

	static GarbageCollector* gc_instance;
};