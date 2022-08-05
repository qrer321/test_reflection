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

	//menu_output();

	GarbageCollector::GetInstance()->Destroy();
	Reflection::GetInstance()->Destroy();

	return 0;
};