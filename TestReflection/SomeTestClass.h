#pragma once
#include "UObject.h"
#include "SomeTestClass.generated.h"

UCLASS()
class SomeTestClass : public UObject
{
	GENERATED_BODY()

public:
	~SomeTestClass() override = default;

	UFUNCTION()
	int test_function_0(int variable_left, int variable_right)
	{
		std::cout << "two param / int return member function call" << std::endl; 
		std::cout << "return value : " << variable_left + variable_right << std::endl;
		return variable_left + variable_right; 
	}
	UFUNCTION()
	int test_function_1() 
	{
		std::cout << "zero param / int return member function call" << std::endl;
		std::cout << "test_int_1 + test_int_2 : " << test_int_1 + test_int_2 << std::endl;
		return test_int_1 + test_int_2;
	}

private:
	UPROPERTY()
	int test_int_1;
	UPROPERTY()
	int test_int_2;

	UPROPERTY()
	SomeTestClass* pointing_other_object;
};