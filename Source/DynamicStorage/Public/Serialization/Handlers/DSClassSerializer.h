// Copyright (c) Sushant 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <type_traits>

/**
 * Handles class references.
 *
 * Supported:
 * - UClass*
 * - TSubclassOf<T>
 * - TSoftClassPtr<T>
 */
class DYNAMICSTORAGE_API FDSClassSerializer
{
public:

    template<typename T>
    struct TDSIsTSubclassOf
    {
        static constexpr bool Value = false;
    };

    template<typename T>
    struct TDSIsTSubclassOf<TSubclassOf<T>>
    {
        static constexpr bool Value = true;
        using ClassType = T;
    };

    template<typename T>
    struct TDSIsSoftClassPtr
    {
        static constexpr bool Value = false;
    };

    template<typename T>
    struct TDSIsSoftClassPtr<TSoftClassPtr<T>>
    {
        static constexpr bool Value = true;
        using ClassType = T;
    };

    template<typename T>
    static constexpr bool IsSupported()
    {
        return
            std::is_same_v<T, UClass*> ||
            TDSIsTSubclassOf<T>::Value ||
            TDSIsSoftClassPtr<T>::Value;
    }

    template<typename T>
    static bool Serialize(
        const T& Value,
        TArray<uint8>& OutBytes
    )
    {
        static_assert(
            IsSupported<T>(),
            "FDSClassSerializer only supports UClass*, TSubclassOf<T>, and TSoftClassPtr<T>."
        );

        FString ClassPath;

        if constexpr (std::is_same_v<T, UClass*>)
        {
            if (Value)
            {
                ClassPath = Value->GetPathName();
            }
        }
        else if constexpr (TDSIsTSubclassOf<T>::Value)
        {
            UClass* Class = Value.Get();

            if (Class)
            {
                ClassPath = Class->GetPathName();
            }
        }
        else if constexpr (TDSIsSoftClassPtr<T>::Value)
        {
            if (!Value.IsNull())
            {
                ClassPath = Value.ToSoftObjectPath().ToString();
            }
        }

        OutBytes.Empty();

        FMemoryWriter Writer(OutBytes);
        Writer << ClassPath;

        return true;
    }

    template<typename T>
    static bool Deserialize(
        const TArray<uint8>& Bytes,
        T& OutValue
    )
    {
        static_assert(
            IsSupported<T>(),
            "FDSClassSerializer only supports UClass*, TSubclassOf<T>, and TSoftClassPtr<T>."
        );

        ResetValue(OutValue);

        if (Bytes.Num() <= 0)
        {
            return false;
        }

        FString ClassPath;

        FMemoryReader Reader(Bytes);
        Reader << ClassPath;

        if (ClassPath.IsEmpty())
        {
            return true;
        }

        if constexpr (TDSIsSoftClassPtr<T>::Value)
        {
            using ClassType = typename TDSIsSoftClassPtr<T>::ClassType;

            FSoftObjectPath SoftPath(ClassPath);
            OutValue = TSoftClassPtr<ClassType>(SoftPath);

            return true;
        }
        else
        {
            UClass* LoadedClass = FindObject<UClass>(nullptr, *ClassPath);

            if (!LoadedClass)
            {
                LoadedClass = LoadObject<UClass>(nullptr, *ClassPath);
            }

            if (!LoadedClass)
            {
                return false;
            }

            if constexpr (std::is_same_v<T, UClass*>)
            {
                OutValue = LoadedClass;
                return true;
            }
            else if constexpr (TDSIsTSubclassOf<T>::Value)
            {
                using ClassType = typename TDSIsTSubclassOf<T>::ClassType;

                if (!LoadedClass->IsChildOf(ClassType::StaticClass()))
                {
                    return false;
                }

                OutValue = LoadedClass;
                return true;
            }
        }

        return false;
    }

private:

    template<typename T>
    static void ResetValue(T& Value)
    {
        if constexpr (std::is_same_v<T, UClass*>)
        {
            Value = nullptr;
        }
        else
        {
            Value = T();
        }
    }
};