#ifdef SomeTestClass_generated_h
#error SomeTestClass.generated.h already included, missing '#pragma once' in SomeTestClass.h
#endif
#define SomeTestClass_generated_h

#ifdef UCLASS
#undef UCLASS
#endif

#define UCLASS() \
		static std::unordered_map<std::string, UProperty*> reflection_properties; \
		static std::unordered_map<std::string, UFunction*> reflection_functions;


#ifdef GENERATED_BODY
#undef GENERATED_BODY
#endif

#define GENERATED_BODY() \
public: \
		void AddReflectionInfo() \
		{ \
			reflection_properties = \
			{ \
				{"test_int_1", new UProperty(typeid(int).hash_code(), &test_int_1, sizeof(test_int_1))}, \
				{"test_int_2", new UProperty(typeid(int).hash_code(), &test_int_2, sizeof(test_int_2))}, \
				{"pointing_other_object", new UProperty(typeid(SomeTestClass*).hash_code(), &pointing_other_object, sizeof(pointing_other_object))}, \
			}; \
 			\
			reflection_functions = \
			{ \
				{"test_function_0", new UFunction("void", { "int", "int", })}, \
				{"test_function_1", new UFunction("void", { })}, \
			}; \
 			\
			SetProperties(std::move(reflection_properties)); \
			SetFunctions(std::move(reflection_functions)); \
		} \
		\
		SomeTestClass() \
		{ \
			AddReflectionInfo(); \
		}


#ifdef UPROPERTY
#undef UPROPERTY
#endif

#define UPROPERTY()


#ifdef UFUNCTION
#undef UFUNCTION
#endif

#define UFUNCTION()


