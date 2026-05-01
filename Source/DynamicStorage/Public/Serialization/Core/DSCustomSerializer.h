#pragma once

#include "CoreMinimal.h"

/**
 * Default custom serializer.
 *
 * Unsupported unless specialized.
 */
template<typename T>
struct TDSCustomSerializer
{
	static constexpr bool bSupported = false;

	static bool Serialize(
		const T& Value,
		TArray<uint8>& OutBytes
	)
	{
		return false;
	}

	static bool Deserialize(
		const TArray<uint8>& Bytes,
		T& OutValue
	)
	{
		return false;
	}
};