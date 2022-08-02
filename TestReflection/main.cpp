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
	// 단순히 functions map에 registerFunction를 이용하여 함수를 등록하면
	// 실제로는 다른 형태로의 map이 생성된다.
	// -> std::map<std::string, std::function<double(int, double)>> / std::map<std::string, std::function<int(int, int)>>

	// 이런 문제로 인해서 callProxy 함수를 사용한다.

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
	// func는 바인딩된 사용자 함수가 있는 callProxy여야 합니다???

	auto ret_vec = serialize("test_function", 1, 2);
	registerFunction("test_function", test_function);

	msgpack::Unpacker unpacker(&ret_vec[0], ret_vec.size());
	auto func = functions["test_function"];

	// func() 호출에서 return 값이 존재한다면 해당 값을 받아낼 방법???
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