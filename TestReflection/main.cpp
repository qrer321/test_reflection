// GC collection time : 30 ~ 60 초
// -> 참조되지 않는, 사용되지 않는 객체 할당 해제
// -> 해당 방법은 사용되는 객체의 경우 Reference Tree에 등록하여 Root Set부터 확인
// -> Root Set을 제외한 나머지 객체들은 Graph 형태로 구현
// --> 하나의 객체를 두 개 이상의 객체가 참조할 수 있기 때문

// 메타 데이터 파일을 만들어서 변수명과 함수명을 모르는 상태로 호출이 가능하도록 설정 / Reflection

// UCLASS(), UPROPERTY()와 같은 매크로들을 기억해야 한다.
// -> 언리얼헤더툴은 해당 매크로들을 확인하여 .generated.h, .generated.cpp에 정보를 기록

// Replication과 RPC의 경우는 네트워크를 사용하지만 네트워크까지의 구현은 필요치 않다.

// 필요한 매크로
// UCLASS, UFUNCTION, UPROPERTY

// GC의 Reference Tree의 경우 최초에 트리 구조를 전부 날리고
// Root Set부터 참조되는 오브젝트들 찾아가며 Mark
// 이후 참조되지 않는 오브젝트를 찾아 Sweep
// -> 모든 객체의 주소 정보를 기록하고 있다. -> Reflection을 통해서
// -> 그렇다면 런타임 동안 모든 정보를 기록하고 있는 Reflection 객체가 필요하다.

#include "global.h"
#include "delta_timer.h"
#include "Reflection.h"
#include "GarbageCollector.h"
#include "UObject.h"
#include "ObjectPool.h"
#include "SomeTestClass.h"

#include "helper_method.h"

void TestFunction()
{
	auto prop_address_test = &reflection_properties;
	auto func_address_test = &reflection_functions;
	SomeTestClass specific_1;
	SomeTestClass specific_2;

	specific_1.test_int_1 = 10;
	specific_2.test_int_1 = 2;

	specific_1.test_int_2 = 10;
	specific_2.test_int_2 = 2;

	specific_1.SetPropertyValue<int>("test_int_2", 99);
	specific_1.SetPropertyValue<SomeTestClass*>("pointing_other_object", &specific_2);

	int return_int_value = specific_1.GetPropertyValue<int>("test_int_2");
	SomeTestClass* return_address_value = specific_1.GetPropertyValue<SomeTestClass*>("pointing_other_object");
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	
	//SomeTestClass* new_object_test = NewObject<SomeTestClass>();
	//SomeTestClass* ref_other = NewObject<SomeTestClass>();
	//new_object_test->test_int_1 = 10;
	//new_object_test->test_int_2 = 20;
	//
	//new_object_test->SetPropertyValue<int>("test_int_1", 66);
	//new_object_test->SetPropertyValue<int>("test_int_2", 77);
	//new_object_test->SetPropertyValue<SomeTestClass*>("pointing_other_object", ref_other);


	GarbageCollector gc;
	gc.GetInstance()->CollectObject();
	gc.GetInstance()->MarkReachableObject();
	gc.GetInstance()->SweepUnreachableObject();



	//delta_timer dt;
	//float running_time = 0.f;
	//while (true)
	//{
	//	dt.Tick();

	//	running_time += dt.GetDelta();
	//	if (running_time > 5.f)
	//	{
	//		running_time = 0.f;
	//		[]()
	//		{
	//			// Create Object And Collect
	//			SomeTestClass* temp_ = new SomeTestClass;
	//			temp_->AddReflectionInfo();
	//		}();
	//	}
	//}

	gc.Destroy();

	return 0;
}