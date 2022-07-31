// GC collection time : 30 ~ 60 ��
// -> �������� �ʴ�, ������ �ʴ� ��ü �Ҵ� ����
// -> �ش� ����� ���Ǵ� ��ü�� ��� Reference Tree�� ����Ͽ� Root Set���� Ȯ��
// -> Root Set�� ������ ������ ��ü���� Graph ���·� ����
// --> �ϳ��� ��ü�� �� �� �̻��� ��ü�� ������ �� �ֱ� ����

// ��Ÿ ������ ������ ���� ������� �Լ����� �𸣴� ���·� ȣ���� �����ϵ��� ���� / Reflection

// UCLASS(), UPROPERTY()�� ���� ��ũ�ε��� ����ؾ� �Ѵ�.
// -> �𸮾�������� �ش� ��ũ�ε��� Ȯ���Ͽ� .generated.h, .generated.cpp�� ������ ���

// Replication�� RPC�� ���� ��Ʈ��ũ�� ��������� ��Ʈ��ũ������ ������ �ʿ�ġ �ʴ�.

// �ʿ��� ��ũ��
// UCLASS, UFUNCTION, UPROPERTY

// GC�� Reference Tree�� ��� ���ʿ� Ʈ�� ������ ���� ������
// Root Set���� �����Ǵ� ������Ʈ�� ã�ư��� Mark
// ���� �������� �ʴ� ������Ʈ�� ã�� Sweep
// -> ��� ��ü�� �ּ� ������ ����ϰ� �ִ�. -> Reflection�� ���ؼ�
// -> �׷��ٸ� ��Ÿ�� ���� ��� ������ ����ϰ� �ִ� Reflection ��ü�� �ʿ��ϴ�.

#include "global.h"
#include "delta_timer.h"
#include "Reflection.h"
#include "GarbageCollector.h"
#include "UObject.h"
#include "SomeTestClass.h"

#include "helper_method.h"

void TestFunction()
{
	auto prop_address_test = &reflection_properties;
	auto func_address_test = &reflection_functions;
	SomeTestClass specific_1;
	SomeTestClass specific_2;

	specific_1.SetPropertyValue<int>("test_int_2", 99);
	specific_1.SetPropertyValue<SomeTestClass*>("pointing_other_object", &specific_2);

	int return_int_value = specific_1.GetPropertyValue<int>("test_int_2");
	SomeTestClass* return_address_value = specific_1.GetPropertyValue<SomeTestClass*>("pointing_other_object");
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1196);

	TestFunction();

	auto new_test_object_0 = NewObject<SomeTestClass>();
	auto new_test_object_1 = NewObject<SomeTestClass>();
	new_test_object_0->SetPropertyValue("pointing_other_object", new_test_object_1);
	new_test_object_0->SetName("test");


	//GarbageCollector::GetInstance()->ActivateGarbageCollector();

	menu_output();

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
	//			// Create Object And MarkingObject
	//			SomeTestClass* temp_ = new SomeTestClass;
	//			temp_->AddReflectionInfo();
	//		}();
	//	}
	//}

	GarbageCollector::GetInstance()->Destroy();
	Reflection::GetInstance()->Destroy();

	return 0;
}