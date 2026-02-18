#pragma once

#include "CoreMinimal.h"
#include "boost/preprocessor.hpp"

DECLARE_LOG_CATEGORY_EXTERN(LogFrette, Log, All);

namespace Frette::Utils::Log
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

	template <typename T>
	FORCEINLINE const T& ToTCHAR(const T& Value)
	{
		return Value;
	}
}

// Helper macro to auto convert log arguments to TCHAR* because I keep forgetting the fucking * in front of FStrings lol
#define WRAP(r, data, elem) data(elem)

#define MAP_ARGS(func, ...) \
	BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(WRAP, func, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

#define LOG_FRETTE_NO_ARGS(Verbosity, Format) \
	UE_LOG(LogFrette, Verbosity, TEXT(Format))

#define LOG_FRETTE_WITH_ARGS(Verbosity, Format, ...) \
	UE_LOG(LogFrette, Verbosity, TEXT(Format), MAP_ARGS(Frette::Utils::Log::ToTCHAR, __VA_ARGS__))

#define LOG_FRETTE_IMPL(Verbosity, Format, ArgCount, ...) \
	BOOST_PP_IF(ArgCount, \
		LOG_FRETTE_WITH_ARGS(Verbosity, Format, __VA_ARGS__), \
		LOG_FRETTE_NO_ARGS(Verbosity, Format) \
	)

#define LOG_FRETTE(Verbosity, Format, ...) \
	LOG_FRETTE_IMPL(Verbosity, Format, BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), __VA_ARGS__)

// Helper for more readable precondition early returns. If the condition is FALSE, print msg then returns.
// Usage:
// require(Condition);
// require(Condition, "Message with no args.");
// require(Condition, "Message with args: %d, %s", Arg1, Arg2);
// Note: format str will be auto-wrapped with TEXT(), and args will be auto-converted to TCHAR* if possible.
#define require(...) BOOST_PP_OVERLOAD(FRETTE_PRIVATE_ENSURE_, __VA_ARGS__)(__VA_ARGS__)

#define FRETTE_PRIVATE_ENSURE_1(Condition) \
	if (!ensureAlways(Condition)) return;

#define FRETTE_PRIVATE_ENSURE_2(Condition, Msg) \
	if (!ensureAlwaysMsgf(Condition, TEXT(Msg))) return;

#define FRETTE_PRIVATE_ENSURE_3(Condition, Format, ...) \
	if (!ensureAlwaysMsgf(Condition, TEXT(Format), MAP_ARGS(Frette::Utils::Log::ToTCHAR, __VA_ARGS__))) return;