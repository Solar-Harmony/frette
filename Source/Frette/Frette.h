#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "boost/preprocessor.hpp"
#include "Logging/MessageLog.h"

FRETTE_API DECLARE_LOG_CATEGORY_EXTERN(LogFrette, Log, All);

/**
 * Frette smart format function. Same as FString::Printf but:
 * - auto-wraps format string with TEXT()
 * - auto-converts most common types to TCHAR*, no more forgetting *
 */
#define FretteFmt(Format, ...) \
	FString::Printf(TEXT(Format), FRETTE_PRIVATE_MAP_ARGS(Frette::Private::ToTCHAR, __VA_ARGS__))

#define FRETTE_PRIVATE_MAP_ARGS(func, ...) \
	BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(FRETTE_PRIVATE_WRAP, func, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

#define FRETTE_PRIVATE_WRAP(r, data, elem) Frette::Private::PrintArg(Frette::Private::ToTCHAR(elem))

/**
 * Frette logging function.
 * 
 * Usage:
 * FLOG(Frette, Log, "Message with no args.");
 * FLOG(Frette, Warning, "Message with args: %d, %s", Arg1, Arg2);
 * 
 * Same as UE_LOG() with extra conveniences:
 * - Prepends Log to the logging category
 * - Auto-wraps the format string with TEXT()
 * - Auto-converts text and UObject* arguments to TCHAR*
 */
#define FRETTE_LOG(Verbosity, Format, ...) \
	UE_LOG(LogFrette, Verbosity, TEXT(Format) __VA_OPT__(, FRETTE_PRIVATE_MAP_ARGS(Frette::Private::ToTCHAR, __VA_ARGS__)))

#define FRETTE_LOGC(Category, Verbosity, Format, ...) \
	UE_LOG(LogFrette##Category, Verbosity, TEXT(Format) __VA_OPT__(, FRETTE_PRIVATE_MAP_ARGS(Frette::Private::ToTCHAR, __VA_ARGS__)))

#define FRETTE_GET_FIRST(First, ...) First

/**
 * Frette early return assert macro. Expands similarly to `if (!ensure(Condition)`, but with extra conveniences.
 * 
 * Usage:
 * unless(bCondition) return;
 * unless(bCondition, "Message with no args.") return;
 * unless(bCondition, "Message with args: %d, %s, %s", MyInteger, MyUObject, MyFText) 
 *     return;
 * 
 * Differences from Epic's ensure():
 * - unless() prints simplified call site information instead of a full verbose callstack
 * - unless() does not cause a hitch on first hit assert
 * - unless() also prints to Message Log and brings it in focus to prevent asserts from slipping through
 * - unless() auto-converts format arguments to TCHAR* for common types (FString, FName, FText, UObject*, FGameplayTag, and more)
 * - unless() auto-wraps the format string with TEXT()
 * 
 * Use unless() instead of ensure() for early returns (function preconditions). 
 * Your error handling will be more readable and more concise. 
 */
#define unless(...) \
	if ([&]() -> bool { \
		if (UNLIKELY(!(FRETTE_GET_FIRST(__VA_ARGS__)))) \
		{ \
			BOOST_PP_OVERLOAD(FRETTE_PRIVATE_ENSURE_, __VA_ARGS__)(__VA_ARGS__) \
			UE_DEBUG_BREAK(); \
			return true; \
		} \
			return false; \
	}())

#define FRETTE_PRIVATE_ENSURE_1(Condition) \
	Frette::Private::FretteEnsureImpl(TEXT(#Condition), FString());

#define FRETTE_PRIVATE_ENSURE_2(Condition, Msg) \
	Frette::Private::FretteEnsureImpl(TEXT(#Condition), FString(TEXT(Msg)));

#define FRETTE_PRIVATE_ENSURE_3(Condition, Format, ...) FRETTE_PRIVATE_ENSURE_VARARGS(Condition, Format, __VA_ARGS__)
#define FRETTE_PRIVATE_ENSURE_4(Condition, Format, ...) FRETTE_PRIVATE_ENSURE_VARARGS(Condition, Format, __VA_ARGS__)
#define FRETTE_PRIVATE_ENSURE_5(Condition, Format, ...) FRETTE_PRIVATE_ENSURE_VARARGS(Condition, Format, __VA_ARGS__)
#define FRETTE_PRIVATE_ENSURE_6(Condition, Format, ...) FRETTE_PRIVATE_ENSURE_VARARGS(Condition, Format, __VA_ARGS__)
#define FRETTE_PRIVATE_ENSURE_7(Condition, Format, ...) FRETTE_PRIVATE_ENSURE_VARARGS(Condition, Format, __VA_ARGS__)

#define FRETTE_PRIVATE_ENSURE_VARARGS(Condition, Format, ...) \
	Frette::Private::FretteEnsureImpl(TEXT(#Condition), FretteFmt(Format, __VA_ARGS__));

namespace Frette::Private
{
	FORCEINLINE const TCHAR* PrintArg(const FString& Str)
	{
		return *Str;
	}

	FORCEINLINE const TCHAR* PrintArg(const TCHAR* TCHARPtr)
	{
		return TCHARPtr;
	}

	template <typename T>
	FORCEINLINE const T& PrintArg(const T& Value)
	{
		return Value;
	}
	
	FORCEINLINE const FString& ToTCHAR(const FString& Str)
	{
		return Str;
	}

	FORCEINLINE FString ToTCHAR(const FName& Name)
	{
		return Name.ToString();
	}

	FORCEINLINE FString ToTCHAR(const FText& Text)
	{
		return Text.ToString();
	}

	FORCEINLINE const TCHAR* ToTCHAR(const TCHAR* TCHARPtr)
	{
		return TCHARPtr;
	}

	FORCEINLINE FString ToTCHAR(const char* CharPtr)
	{
		return FString(ANSI_TO_TCHAR(CharPtr));
	}
	
	FORCEINLINE FString ToTCHAR(FGameplayTag Tag)
	{
		return Tag.ToString();
	}

	template <typename T>
	FORCEINLINE auto ToTCHAR(const T* Object) -> std::enable_if_t<std::is_base_of_v<UObjectBaseUtility, T>, FString>
	{
		return GetNameSafe(Object);
	}
	
	template<typename T>
	FORCEINLINE auto ToTCHAR(const TObjectPtr<T>& ObjectPtr) -> std::enable_if_t<std::is_base_of_v<UObjectBaseUtility, T>, FString>
	{
		return GetNameSafe(ObjectPtr);
	}

	// fallback case for types that don't use %s
	template <typename T>
	FORCEINLINE const T& ToTCHAR(const T& Value)
	{
		return Value;
	}

	FORCEINLINE FString CaptureCaller()
	{
		// Frame 0 = CaptureStackBackTrace (OS), Frame 1 = CaptureCaller (inlined away),
		// Frame 2 = the require() expansion site (the actual caller we want).
		static constexpr int32 NumFrames = 4;
		static constexpr int32 CallerFrameIndex = 2;
		uint64 Addresses[NumFrames] = {};
		FPlatformStackWalk::CaptureStackBackTrace(Addresses, NumFrames);
		
		ANSICHAR Buffer[1024] = {};
		FPlatformStackWalk::ProgramCounterToHumanReadableString(CallerFrameIndex, Addresses[CallerFrameIndex], Buffer, sizeof(Buffer));
		const FString Raw(ANSI_TO_TCHAR(Buffer));
		return Raw;
	}

	FORCEINLINE void FretteEnsureImpl(const TCHAR* ConditionStr, const FString& Reason)
	{
		const FString Location = CaptureCaller();
		FString FullMessage;
		if (Reason.IsEmpty())
		{
			FullMessage = FString::Printf(TEXT("Precondition failed: %s\nLocation: %s"), ConditionStr, *Location);
		}
		else
		{
			FullMessage = FString::Printf(TEXT("Precondition failed: %s\nLocation: %s\nReason: %s"), ConditionStr, *Location, *Reason);
		}
		UE_LOG(LogFrette, Error, TEXT("%s"), *FullMessage);
		
#if WITH_EDITOR
		FMessageLog Log("FretteAssert");
		Log.SuppressLoggingToOutputLog(true);
		Log.Error(FText::FromString(FullMessage));
		Log.Open();
#endif
	}
}