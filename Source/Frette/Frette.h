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

#define CALISS LOG_FRETTE
#define CALISSE LOG_FRETTE
#define CALVAIRE LOG_FRETTE
#define CIBOIRE LOG_FRETTE
#define CIBOLE LOG_FRETTE
#define CRISS LOG_FRETTE
#define CRISSE LOG_FRETTE
#define ESTI LOG_FRETTE
#define MAUDIT LOG_FRETTE
#define MAUTADIT LOG_FRETTE
#define OSTI LOG_FRETTE
#define OSTIE LOG_FRETTE
#define HOSTIE LOG_FRETTE
#define SACRAMENT LOG_FRETTE
#define SACREMENT LOG_FRETTE
#define TABARNAC LOG_FRETTE
#define TABARNAK LOG_FRETTE
#define TABARNOUCHE LOG_FRETTE
#define BOUT_DE_SAINT_SACERDOCE LOG_FRETTE