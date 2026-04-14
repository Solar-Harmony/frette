#pragma once

#include "CoreMinimal.h"
#include "boost/preprocessor.hpp"
#include "Logging/MessageLog.h"

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
#define precondition(...) \
	BOOST_PP_OVERLOAD(FRETTE_PRIVATE_ENSURE_, __VA_ARGS__)(__VA_ARGS__) \

#define FRETTE_PRIVATE_ENSURE_1(Condition) \
	if (UNLIKELY(!(Condition))) \
	{ \
		Frette::Private::ReportPreconditionFailure(TEXT(#Condition), FString()); \
		UE_DEBUG_BREAK(); \
		return; \
	}

#define FRETTE_PRIVATE_ENSURE_2(Condition, Msg) \
	if (UNLIKELY(!(Condition))) \
	{ \
		Frette::Private::ReportPreconditionFailure(TEXT(#Condition), FString(TEXT(Msg))); \
		UE_DEBUG_BREAK(); \
		return; \
	}

#define FRETTE_PRIVATE_ENSURE_VARARGS(Condition, Format, ...) \
	if (UNLIKELY(!(Condition))) \
	{ \
		Frette::Private::ReportPreconditionFailure(TEXT(#Condition), FString::Printf(TEXT(Format), FRETTE_PRIVATE_MAP_ARGS(Frette::Private::ToTCHAR, __VA_ARGS__))); \
		UE_DEBUG_BREAK(); \
		return; \
	}

#define FRETTE_PRIVATE_ENSURE_3(Condition, Format, ...) FRETTE_PRIVATE_ENSURE_VARARGS(Condition, Format, __VA_ARGS__)
#define FRETTE_PRIVATE_ENSURE_4(Condition, Format, ...) FRETTE_PRIVATE_ENSURE_VARARGS(Condition, Format, __VA_ARGS__)
#define FRETTE_PRIVATE_ENSURE_5(Condition, Format, ...) FRETTE_PRIVATE_ENSURE_VARARGS(Condition, Format, __VA_ARGS__)
#define FRETTE_PRIVATE_ENSURE_6(Condition, Format, ...) FRETTE_PRIVATE_ENSURE_VARARGS(Condition, Format, __VA_ARGS__)
#define FRETTE_PRIVATE_ENSURE_7(Condition, Format, ...) FRETTE_PRIVATE_ENSURE_VARARGS(Condition, Format, __VA_ARGS__)


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
		// Frame 0 = CaptureStackBackTrace (OS), Frame 1 = CaptureCaller (inlined away),
		// Frame 2 = the require() expansion site (the actual caller we want).
		static constexpr int32 NumFrames = 4;
		static constexpr int32 CallerFrameIndex = 2;
		uint64 Addresses[NumFrames] = {};
		FPlatformStackWalk::CaptureStackBackTrace(Addresses, NumFrames);

		ANSICHAR Buffer[1024] = {};
		FPlatformStackWalk::ProgramCounterToHumanReadableString(CallerFrameIndex, Addresses[CallerFrameIndex], Buffer, sizeof(Buffer));
		FString Raw(ANSI_TO_TCHAR(Buffer));
		return Raw;
	}

	FORCEINLINE void LogMessageErr(const FString& Message)
	{
#if WITH_EDITOR
		FMessageLog Log("FretteAssert");
		Log.SuppressLoggingToOutputLog(true);
		Log.Error(FText::FromString(Message));
		Log.Open();
#endif
	}

	FORCEINLINE void ReportPreconditionFailure(const TCHAR* ConditionStr, const FString& Reason)
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
		LogMessageErr(FullMessage);
	}
}