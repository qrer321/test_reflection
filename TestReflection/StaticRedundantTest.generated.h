#ifdef StaticRedundantTest_generated_h
#error StaticRedundantTest.generated.h already included, missing '#pragma once' in StaticRedundantTest.h
#endif
#define StaticRedundantTest_generated_h

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
			}; \
 			\
			reflection_functions = \
			{ \
			}; \
 			\
			SetProperties(reflection_properties); \
			SetFunctions(reflection_functions); \
		} \
public: \
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


