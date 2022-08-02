#ifdef SomeTestClass_generated_h
#error SomeTestClass.generated.h already included, missing '#pragma once' in SomeTestClass.h
#endif
#define SomeTestClass_generated_h

#ifdef UCLASS
#undef UCLASS
#define UCLASS()
#endif

#ifndef UCLASS
#define UCLASS() \
		static std::unordered_map<std::string, class UProperty*> reflection_properties; \
		static std::unordered_map<std::string, class UFunction*> reflection_functions;
#endif

#ifdef GENERATED_BODY
#undef GENERATED_BODY
#endif

#define GENERATED_BODY() \
private: \
		void AddReflectionInfo() \
		{ \
			reflection_properties = \
			{ \
				{"test_int_1", new UProperty("int", typeid(int).hash_code(), &test_int_1, sizeof(test_int_1)) }, \
				{"test_int_2", new UProperty("int", typeid(int).hash_code(), &test_int_2, sizeof(test_int_2)) }, \
				{"pointing_other_object", new UProperty("SomeTestClass*", typeid(SomeTestClass*).hash_code(), &pointing_other_object, sizeof(pointing_other_object)) }, \
			}; \
 			\
			reflection_functions = \
			{ \
				{"test_function_0", new UFunction("void", { "int", "int", })}, \
				{"test_function_1", new UFunction("void", { })}, \
			}; \
 			\
			SetProperties(reflection_properties); \
			SetFunctions(reflection_functions); \
		} \
public: \


#ifdef UPROPERTY
#undef UPROPERTY
#endif

#define UPROPERTY()


#ifdef UFUNCTION
#undef UFUNCTION
#endif

#define UFUNCTION()


