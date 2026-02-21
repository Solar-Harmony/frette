#pragma once

#include "CoreMinimal.h"
#include "boost/preprocessor.hpp"

DECLARE_LOG_CATEGORY_EXTERN(LogFrette, Log, All);

// Helper macro to clean up logging calls
// - Auto-wraps the format string with TEXT()
// - Auto-converts text arguments to TCHAR* (FString, FName, FText)
// Usage:
// FRETTE_LOG(Log, "Message with no args.");
// FRETTE_LOG(Warning, "Message with args: %d, %s", Arg1, Arg2);
#define FRETTE_PRIVATE_WRAP(r, data, elem) data(elem)

#define FRETTE_PRIVATE_MAP_ARGS(func, ...) \
	BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(FRETTE_PRIVATE_WRAP, func, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

#define FRETTE_LOG(Verbosity, Format, ...) \
	UE_LOG(LogFrette, Verbosity, TEXT(Format) __VA_OPT__(, FRETTE_PRIVATE_MAP_ARGS(Frette::Private::ToTCHAR, __VA_ARGS__)))

// Helper for more readable precondition early returns. If the condition is FALSE, breaks + print msg then returns.
// Note: format str will be auto-wrapped with TEXT(), and args will be auto-converted to TCHAR* if possible.
// Usage:
// require(Condition);
// require(Condition, "Message with no args.");
// require(Condition, "Message with args: %d, %s", Arg1, Arg2);
#define require(...) \
	BOOST_PP_OVERLOAD(FRETTE_PRIVATE_ENSURE_, __VA_ARGS__)(__VA_ARGS__) \

#define FRETTE_PRIVATE_ENSURE_1(Condition) \
	if (UNLIKELY(!(Condition))) \
	{ \
		const FString Caller = Frette::Private::CaptureCaller(); \
		FRETTE_LOG(Error, "Precondition failed: %s in %s.", #Condition, Caller); \
		UE_DEBUG_BREAK(); \
		return; \
	}

#define FRETTE_PRIVATE_ENSURE_2(Condition, Msg) \
	if (UNLIKELY(!(Condition))) \
	{ \
		const FString Caller = Frette::Private::CaptureCaller(); \
		FRETTE_LOG(Error, "Precondition failed: %s in %s: %s", #Condition, Caller, Msg); \
		return; \
	}

#define FRETTE_PRIVATE_ENSURE_3(Condition, Format, ...) \
	if (UNLIKELY(!(Condition))) \
	{ \
		const FString Caller = Frette::Private::CaptureCaller(); \
		FRETTE_LOG(Error, "Precondition failed: %s in %s: " Format, #Condition, Caller, __VA_ARGS__); \
		return; \
	}


namespace Frette::Private
{
	FORCEINLINE const TCHAR* ToTCHAR(const FString& Str)
	{
		return *Str;
	}

	FORCEINLINE const TCHAR* ToTCHAR(const FName& Name)
	{
		return *Name.ToString();
	}

	FORCEINLINE const TCHAR* ToTCHAR(const FText& Text)
	{
		return *Text.ToString();
	}

	FORCEINLINE const TCHAR* ToTCHAR(const TCHAR* TCHARPtr)
	{
		return TCHARPtr;
	}

	FORCEINLINE const TCHAR* ToTCHAR(const char* CharPtr)
	{
		return ANSI_TO_TCHAR(CharPtr);
	}

	// fallback case for types that don't use %s
	template <typename T>
	FORCEINLINE const T& ToTCHAR(const T& Value)
	{
		return Value;
	}
	
	FORCEINLINE FString CaptureCaller()
	{
		uint64 Address = 0;
		FPlatformStackWalk::CaptureStackBackTrace(&Address, 1);

		ANSICHAR Buffer[1024];
		FPlatformStackWalk::ProgramCounterToHumanReadableString(0, Address, Buffer, sizeof(Buffer));
		
		return FString(ANSI_TO_TCHAR(Buffer));
	}
}