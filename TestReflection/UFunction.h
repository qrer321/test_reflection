#pragma once
#include "global.h"

class UFunction
{
public:
	UFunction()
		: function_return_type_()
		, function_params_()
	{
	}
	UFunction(std::string return_type, std::vector<std::string> params)
		: function_return_type_(std::move(return_type))
		, function_params_(std::move(params))
	{
	}

	UFunction(const UFunction& other) = delete;
	UFunction(UFunction&& other) = delete;
	UFunction& operator= (const UFunction& other) = delete;
	UFunction& operator= (UFunction&& other) = delete;

	~UFunction() = default;

	const std::string& GetReturnType() const { return function_return_type_; }
	const std::vector<std::string>& GetFunctionParams() const { return function_params_; }
	
	template <typename ReturnType, typename... Args>
	void RegisterFunction(ReturnType(*function)(Args...));
	template <typename ReturnType, typename ClassType, typename... Args>
	void RegisterFunction(ReturnType(ClassType::*function)(Args...));
	template <typename... Args>
	void CallFunction(Args... arguments);

	template <typename ReturnType>
	typename std::enable_if_t<!std::is_void_v<ReturnType>>
	GetReturnValue() const; // must know the type of function return.

private:
	template <typename... Args>
	bool ArgumentsVerifyCorrect(Args... arguments);
	template <typename... Args>
	void SerializeFunction(Args&&... arguments);

	template <typename ReturnType, typename... Args>
	typename std::enable_if_t<!std::is_void_v<ReturnType>>
	BindFunction(std::function<ReturnType(Args...)>&& function, msgpack::Unpacker& unpacker);
	template <typename ReturnType, typename... Args>
	typename std::enable_if_t<std::is_void_v<ReturnType>>
	BindFunction(std::function<ReturnType(Args...)>&& function, msgpack::Unpacker& unpacker);


	std::string function_return_type_;
	std::vector<std::string> function_params_;

	std::function<void(msgpack::Unpacker&)> function_;
	std::map<std::string, std::function<void(msgpack::Unpacker&)>> functions_;
	std::any function_return_;
	std::vector<uint8_t> serialize_vector_;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Generate Function Base /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
template<typename ReturnType, typename ...Args>
inline void UFunction::RegisterFunction(ReturnType(*function)(Args...))
{
	// for non-member function
	function_ = std::bind(&UFunction::BindFunction<ReturnType, Args...>, this, function, std::placeholders::_1);
}

template<typename ReturnType, typename ClassType, typename ...Args>
inline void UFunction::RegisterFunction(ReturnType(ClassType::*function)(Args...))
{
	// for member function
	auto test = std::bind(&UFunction::BindFunction<ReturnType, Args...>, this, function, std::placeholders::_1);
	function_ = std::bind(&UFunction::BindFunction<ReturnType, Args...>, this, function, std::placeholders::_1);
	int a = 0;
}

template<typename ReturnType, typename ...Args>
typename std::enable_if_t<!std::is_void_v<ReturnType>>
inline UFunction::BindFunction(std::function<ReturnType(Args...)>&& function, msgpack::Unpacker& unpacker)
{
	// for non-void function
	using ArgsTuple = std::tuple<typename std::decay_t<std::remove_reference_t<Args>>...>;
	ArgsTuple tuple;

	ReturnType result = [&] <typename Tuple, std::size_t... Index>(Tuple && tuple, std::index_sequence<Index...>) -> ReturnType
	{
		(unpacker(std::get<Index>(tuple)), ...);
		return function(std::get<Index>(std::forward<Tuple>(tuple))...);
	} (std::forward<ArgsTuple>(tuple), std::make_index_sequence<std::tuple_size_v<ArgsTuple>>{});

	function_return_ = result;
}

template<typename ReturnType, typename ...Args>
typename std::enable_if_t<std::is_void_v<ReturnType>>
inline UFunction::BindFunction(std::function<ReturnType(Args...)>&& function, msgpack::Unpacker& unpacker)
{
	// for void function
	using ArgsTuple = std::tuple<typename std::decay_t<std::remove_reference_t<Args>>...>;
	ArgsTuple tuple;

	[&] <typename Tuple, std::size_t... Index>(Tuple && tuple, std::index_sequence<Index...>)
	{
		(unpacker(std::get<Index>(tuple)), ...);
		function(std::get<Index>(std::forward<Tuple>(tuple))...);
	} (std::forward<ArgsTuple>(tuple), std::make_index_sequence<std::tuple_size_v<ArgsTuple>>{});
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Function Call //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
template<typename ...Args>
inline void UFunction::CallFunction(Args ...arguments)
{
	// 매개변수가 원본 함수와 동일한지 체크해야한다
	// 다를 경우 원본 함수가 정상적으로 동작하지 못할 것.
	if (false == ArgumentsVerifyCorrect(arguments...))
	{
		// 입력한 arguments와 원본 param type이 다름.
		assert(false);
		return;
	}

	SerializeFunction(arguments);
	if (true == serialize_vector_.empty())
	{
		// something wrong...
		assert(false);
		return;
	}

	msgpack::Unpacker unpacker(&serialize_vector_[0], serialize_vector_.size());
	function_(unpacker);
}

template <typename... Args>
inline void UFunction::SerializeFunction(Args&&... arguments)
{
	using ArgsTuple = std::tuple<typename std::decay_t<std::remove_reference_t<Args>>...>;
	ArgsTuple tuple = std::make_tuple(arguments...);

	msgpack::Packer packer = {};
	[&packer] <typename Tuple, std::size_t... Index>(Tuple && tuple, std::index_sequence<Index...>)
	{
		(packer(std::get<Index>(tuple)), ...);
	} (std::forward<ArgsTuple>(tuple), std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<ArgsTuple>>>{});

	serialize_vector_ = std::move(packer.vector());
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ...Args>
inline bool UFunction::ArgumentsVerifyCorrect(Args ...arguments)
{
	using ArgsTuple = std::tuple<typename std::decay_t<std::remove_reference_t<Args>>...>;
	ArgsTuple tuple = std::make_tuple(arguments...);

	size_t tuple_size = std::tuple_size_v<ArgsTuple>;
	if (function_params_.size() != tuple_size)
	{
		return false;
	}

	for (size_t i = 0; i < tuple_size; ++i)
	{
		// typeid().name과 param string이 다를 가능성이 존재
		if (typeid(std::get<i>(tuple)).name() != function_params_[i])
		{
			return false;
		}
	}

	return true;
}

template<typename ReturnType>
inline typename std::enable_if_t<!std::is_void_v<ReturnType>>
UFunction::GetReturnValue() const
{
	// return type check?
	if (typeid(ReturnType).name() != function_return_type_)
	{
		// something wrong...
		return ReturnType();
	}

	return std::any_cast<ReturnType>(function_return_);
}