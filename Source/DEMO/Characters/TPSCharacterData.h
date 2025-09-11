#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameAbilities/AbilityTypes.h"
#include "TPSCharacterData.generated.h"

/**
 *
 */

//#include "Characters/TPSCharacterData.h"

class UAnimInstance;
class USkeletalMesh;
class UGameplayEffect;
class UGA_BaseAbility;
class AEventTrigger;

USTRUCT(BlueprintType)
struct FTPSCharacterRuntimeData
{
	GENERATED_BODY()
public:
	bool bInitComplete = 0;
	// 서브 시스템 (레지스트리->맵)에서의 키값
	UPROPERTY(Transient)
		FGameplayTag DataTag;

	// Action 입력시 사용할 어빌리티 태그
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Action")
		FGameplayTag ActionTag;

	// Action 상태 종료시 사용할 tag leaf name
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Action")
		FName ActionEndLeaf = "End";

	// Action trigger oncollsion시 사용할 tag leaf name
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Action")
		FName ActionOnCollisionLeaf = "OnCollision";

	// Action trigger offcollsion시 사용할 tag leaf name
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Action")
		FName ActionOffCollisionLeaf = "OffCollision";

	// Action trigger beginoverlap시 사용할 tag leaf name
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Action")
		FName ActionOnBeginOverlapLeaf = "OnBeginOverlap";

	// Action trigger endoverlap시 사용할 tag leaf name
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Action")
		FName ActionOnEndOverlapLeaf = "OnEndOverlap";

	// 턴제전투에서 스폰될 캐릭터 데이터 태그
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "TurnBased")
		TArray<FGameplayTag> TurnBasedDataTags;
};

UCLASS(BlueprintType)
class DEMO_API UTPSCharacterData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// 캐릭터에 Init 때 복사할 데이터
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Runtime|Datas")
		FTPSCharacterRuntimeData RuntimeData;

	// 시작 시 부여할 태그(예: State.Combat.Ready 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Init|GAS")
		FGameplayTagContainer DefaultOwnedTags;

	// 시작 시 부여할 어빌리티 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Init|GAS")
		TArray<FAbilitySpecInfo> GrantedAbilities;

	// 시작 시 적용할 GE(버프/기본 스탯)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Init|GAS")
		TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	// Action시 충돌을 감지할 Trigger
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Init|Action")
		TSubclassOf<AEventTrigger> ActionTrigger;

	// Trigger를 Attach할 SocketName
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Init|Action")
		FName ActionSocketName = "Action";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Init|Mesh")
		TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Init|Mesh")
		TSoftClassPtr<UAnimInstance> AnimBlueprint;
};
