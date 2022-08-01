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
#include "StaticRedundantTest.h"

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

int test_func_else(int a, int b)
{
	return a + b;
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1196);


	auto new_test_object_0 = NewObject<SomeTestClass>();
	auto new_test_object_1 = NewObject<SomeTestClass>();
	new_test_object_0->SetName("first_object");
	new_test_object_1->SetName("second_object");

	std::function<int(int, int)> test_function = test_func_else;

	void* address_test = &test_func_else;

	std::unordered_map<std::string, void*> address_map =
	{
		{ "test_func_0", address_test }
	};

	auto find_iter = address_map.find("test_func_0");
	auto second = find_iter->second;
	

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