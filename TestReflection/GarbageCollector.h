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

	void Destroy() 
	{ 
		ClearGarbageCollector();
		delete gc_instance;
	}

	GarbageCollector(const GarbageCollector& other) = delete;
	GarbageCollector(GarbageCollector&& other) = delete;
	GarbageCollector& operator= (const GarbageCollector& other) = delete;
	GarbageCollector& operator= (GarbageCollector&& other) = delete;


	void ActivateGarbageCollector();
	void ClearGarbageCollector();


private:
	GarbageCollector() = default;
	~GarbageCollector() = default;

	void MarkingObject();
	void SweepObject();
	void SweepAllObject();

	static GarbageCollector* gc_instance;
};