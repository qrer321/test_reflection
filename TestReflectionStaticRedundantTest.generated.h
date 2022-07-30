#ifdef StaticRedundantTest_generated_h
#error StaticRedundantTest.generated.h already included, missing '#pragma once' in StaticRedundantTest.h
#endif
#define StaticRedundantTest_generated_h

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
			}; \
 			\
			reflection_functions = \
			{ \
			}; \
 			\
			SetProperties(reflection_properties); \
			SetFunctions(reflection_functions); \
		} \
		\
		StaticRedundantTest() \
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


