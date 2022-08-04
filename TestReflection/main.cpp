#pragma once
#include "global.h"
#include "delta_timer.h"
#include "Reflection.h"
#include "GarbageCollector.h"
#include "UObject.h"
#include "helper_method.h"
#include "SomeTestClass.h"
#include "StaticRedundantTest.h"

#include "UFunction_Temp.h"

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1196);

	auto new_test_object_0 = NewObject<SomeTestClass>();
	auto new_test_object_1 = NewObject<SomeTestClass>();
	new_test_object_0->SetName("first_object");
	new_test_object_1->SetName("second_object");

	menu_output();

	//delta_timer dt;
	//float running_time = 0.f;
	//float end_time = 0.f;
	//while (true)
	//{
	//	dt.Tick();

	//	running_time += dt.GetDelta();
	//	end_time += dt.GetDelta();
	//	if (running_time > 3.f)
	//	{
	//		running_time = 0.f;
	//		[&end_time]()
	//		{
	//			// Create Object And MarkingObject
	//			auto run_object = NewObject<SomeTestClass>();
	//			run_object->SetName(std::to_string(end_time));
	//			std::cout << run_object->GetName() << " object created" << std::endl;
	//		}();
	//	}

	//	if (end_time > 10.f)
	//	{
	//		break;
	//	}
	//}

	GarbageCollector::GetInstance()->Destroy();
	Reflection::GetInstance()->Destroy();

	return 0;
};