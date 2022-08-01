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
	void test_function_0(int variable_left, int variable_right) {}
	UFUNCTION()
	void test_function_1() {}

private:
	UPROPERTY()
	int test_int_1;
	UPROPERTY()
	int test_int_2;

	UPROPERTY()
	SomeTestClass* pointing_other_object;

public:
	void Test()
	{
	}
};