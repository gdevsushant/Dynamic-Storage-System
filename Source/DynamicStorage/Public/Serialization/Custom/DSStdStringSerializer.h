// Copyright (c) Sushant 2026. All Rights Reserved.

#pragma once

#include <string>
#include "Serialization/Core/DSCustomSerializer.h"
#include "Serialization/Core/DSValueSerializer.h"

template<>
struct TDSCustomSerializer<std::string>
{
	static constexpr bool bSupported = true;

	static bool Serialize(
		const std::string& Value,
		TArray<uint8>& OutBytes
	)
	{
		FString UnrealString(Value.c_str());

		OutBytes.Empty();

		FMemoryWriter Writer(OutBytes);
		Writer << UnrealString;

		return true;
	}

	static bool Deserialize(
		const TArray<uint8>& Bytes,
		std::string& OutValue
	)
	{
		if (Bytes.Num() <= 0)
		{
			OutValue.clear();
			return false;
		}

		FString UnrealString;

		FMemoryReader Reader(Bytes);
		Reader << UnrealString;

		OutValue = TCHAR_TO_UTF8(*UnrealString);
		return true;
	}
};