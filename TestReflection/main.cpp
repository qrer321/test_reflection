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

std::map<std::string, std::function<void(msgpack::Unpacker&)>> functions;
std::any any_value;

template <typename... Args>
std::vector<uint8_t> serialize(const std::string& func_name, Args&&... arguments)
{
	using ArgsTuple = std::tuple<typename std::decay_t<std::remove_reference_t<Args>>...>;
	//using ArgsTuple = std::tuple<typename std::decay_t<std::remove_reference_v<Args>>...>;
	ArgsTuple tuple = std::make_tuple(arguments...);

	msgpack::Packer packer{};
	[&] <typename Tuple, std::size_t... I>(Tuple && tuple, std::index_sequence<I...>)
	{
		(packer(std::get<I>(tuple)), ...);
	} (std::forward<ArgsTuple>(tuple), std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<ArgsTuple>>>{});

	return packer.vector();
}

template <typename R, typename... Args>
void callProxy(std::function<R(Args...)>&& func, msgpack::Unpacker& unpacker)
{
	// �ܼ��� functions map�� registerFunction�� �̿��Ͽ� �Լ��� ����ϸ�
	// �����δ� �ٸ� ���·��� map�� �����ȴ�.
	// -> std::map<std::string, std::function<double(int, double)>> / std::map<std::string, std::function<int(int, int)>>

	// �̷� ������ ���ؼ� callProxy �Լ��� ����Ѵ�.

	using ArgsTuple = std::tuple<typename std::decay_t<std::remove_reference_t<Args>>...>;
	ArgsTuple tuple;

	R result = [&]<typename Tuple, std::size_t... I>(Tuple && tuple, std::index_sequence<I...>) -> R
	{
		(unpacker(std::get<I>(tuple)), ...);
		return func(std::get<I>(std::forward<Tuple>(tuple))...);
	} (std::forward<ArgsTuple>(tuple), std::make_index_sequence<std::tuple_size_v<ArgsTuple>>{});

	any_value = result;
}

template <typename R, typename... Args>
void registerFunction(const std::string& func_name, R(*function)(Args...))
{
	functions[func_name] = std::bind(&callProxy<R, Args...>, function, std::placeholders::_1);
}

int test_function(int a, int b) 
{
	return a + b; 
}

void test_function_2(int, int) {}


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1196);

	auto new_test_object_0 = NewObject<SomeTestClass>();
	auto new_test_object_1 = NewObject<SomeTestClass>();
	new_test_object_0->SetName("first_object");
	new_test_object_1->SetName("second_object");
	
	//menu_output();
	// 
	// func�� ���ε��� ����� �Լ��� �ִ� callProxy���� �մϴ�???

	auto ret_vec = serialize("test_function", 1, 2);
	registerFunction("test_function", test_function);

	msgpack::Unpacker unpacker(&ret_vec[0], ret_vec.size());
	auto func = functions["test_function"];

	// func() ȣ�⿡�� return ���� �����Ѵٸ� �ش� ���� �޾Ƴ� ���???
	func(unpacker);
	int ret_test = std::any_cast<int>(any_value);


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