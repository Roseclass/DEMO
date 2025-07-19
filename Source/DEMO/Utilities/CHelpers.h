#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

class DEMO_API CHelpers
{
public:
	template<typename T>
	static void GetAsset(T** OutObject, FString InPath)
	{
		ConstructorHelpers::FObjectFinder<T> asset(*InPath);
		verifyf(asset.Succeeded(), L"Asset Not Found");

		*OutObject = asset.Object;
	}

	template<typename T>
	static void GetAssetDynamic(T** OutObject, FString InPath)
	{
		T* obj = Cast<T>(StaticLoadObject(T::StaticClass(), nullptr, *InPath));
		verifyf(!!obj, L"Asset Not Found(Dynamic)");

		*OutObject = obj;
	}

	template<typename T>
	static void GetClass(TSubclassOf<T>* OutClass, FString InPath)
	{
		ConstructorHelpers::FClassFinder<T> asset(*InPath);
		verifyf(asset.Succeeded(), L"Class Not Found");

		*OutClass = asset.Class;
	}

	template<typename T>
	static void CreateComponent(AActor* InActor, T** OutComponent, FName InName, USceneComponent* InParent = nullptr)
	{
		*OutComponent = InActor->CreateDefaultSubobject<T>(InName);

		if (!!InParent)
		{
			(*OutComponent)->SetupAttachment(InParent);
			return;
		}


		InActor->SetRootComponent(*OutComponent);
	}

	template<typename T>
	static void CreateActorComponent(AActor* InActor, T** OutComponent, FName InName)
	{
		*OutComponent = InActor->CreateDefaultSubobject<T>(InName);
	}

	template<typename T>
	static T* GetComponent(AActor* InActor)
	{
		return Cast<T>(InActor->GetComponentByClass(T::StaticClass()));
	}

	template<typename T>
	static FString EnumToString(FString EnumClassName, T Enum)
	{
		FString EnumToString = TEXT("Invalid");

		const UEnum* CharStateEnum = FindObject<UEnum>(ANY_PACKAGE, *EnumClassName, true);
		if (CharStateEnum)
			return EnumToString = CharStateEnum->GetNameStringByValue((int64)Enum);

		return EnumToString;
	}

};
