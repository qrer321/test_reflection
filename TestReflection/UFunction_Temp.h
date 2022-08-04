#pragma once
#include "global.h"

template <typename Signature>
class UFunction_Temp;

template <typename R, typename... Args>
class UFunction_Temp<R(Args...)>
{
public:
	UFunction_Temp() = default;
	UFunction_Temp(const UFunction_Temp& other) = default;
	UFunction_Temp& operator=(const UFunction_Temp& other) = default;

	~UFunction_Temp() = default;

	template <R(*Function)(Args...)>
	auto bind() -> void
	{
		func_instance_ = nullptr;
		function_ = static_cast<function_form>([](const void*, msgpack::Unpacker& unpacker, Args... args) -> R
			{
				return (*Function)(args...);
			});
	}

	template <typename Class, R(Class::*MemberFunction)(Args...) const>
	auto bind(const Class* c) -> void
	{
		func_instance_ = c;
		function_ = static_cast<function_form>([](const void* pointer, msgpack::Unpacker&, Args... args) -> R
			{
				const auto* class_pointer = static_cast<const Class*>(pointer);
				return (class_pointer->*MemberFunction)(args...);
			});
	}

	auto operator()(msgpack::Unpacker& unpacker, Args... args) const -> R
	{
		if (nullptr == function_)
		{
			return;
		}

		return (*function_)(func_instance_, unpacker, args...);
	}



	template <typename... Args_Inner>
	void SerializeFunction(Args_Inner&&... arguments);

private:
	using function_form = R(*)(const void*, msgpack::Unpacker&, Args...);

	const void* func_instance_;
	function_form function_;

	std::vector<uint8_t> serialize_;
};

template<typename R, typename ...Args>
template<typename ...Args_Inner>
inline void UFunction_Temp<R(Args...)>::SerializeFunction(Args_Inner && ...arguments)
{
	using ArgsTuple = std::tuple<typename std::decay_t<std::remove_reference_t<Args_Inner>>...>;
	ArgsTuple tuple = std::make_tuple(arguments...);

	msgpack::Packer packer = {};
	[&packer] <typename Tuple, std::size_t... Index>(Tuple && tuple, std::index_sequence<Index...>)
	{
		(packer(std::get<Index>(tuple)), ...);
	} (std::forward<ArgsTuple>(tuple), std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<ArgsTuple>>>{});

	serialize_ = std::move(packer.vector());
}
