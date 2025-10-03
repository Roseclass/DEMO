#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "GameAbilities/AbilityUIEnums.h"
#include "UITypes.generated.h"

//#include "Datas/UITypes.h"

USTRUCT(BlueprintType)
struct FDEMOSkillUIData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FGameplayTag SkillTag;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TArray<ESkillSlotLocation> AllowedSlotTypes;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true", DisplayThumbnail = "true", DisplayName = "Image", AllowedClasses = "Texture,MaterialInterface,SlateTextureAtlasInterface", DisallowedClasses = "MediaTexture"))
		TObjectPtr<UObject> ResourceObject;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FText DisplayName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (MultiLine = true))
		FText Description;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		int32 DescriptionFontSize;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		int32 DescriptionSpacing;

};

USTRUCT(BlueprintType)
struct FDEMOSkillUIDatas
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		FDEMOSkillUIData Datas[(int32)ESkillListLocation::MAX];
};

UCLASS(BlueprintType)
class DEMO_API UDEMOSkillUIDataRegistry : public UPrimaryDataAsset
{
	GENERATED_BODY()
private:
	void CreateSkillUIDataMap()
	{
		SkillUIDataMap.FindOrAdd(FGameplayTag::RequestGameplayTag("Skill.Gideon"));
		SkillUIDataMap.FindOrAdd(FGameplayTag::RequestGameplayTag("Skill.Revenant"));
		SkillUIDataMap.FindOrAdd(FGameplayTag::RequestGameplayTag("Skill.Terra"));
	}
public:
	UDEMOSkillUIDataRegistry()
	{
		CreateSkillUIDataMap();
	};
protected:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);
		CreateSkillUIDataMap();
	};
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TMap<FGameplayTag, FDEMOSkillUIDatas> SkillUIDataMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true", DisplayThumbnail = "true", DisplayName = "Image", AllowedClasses = "Texture,MaterialInterface,SlateTextureAtlasInterface", DisallowedClasses = "MediaTexture"))
		TObjectPtr<UObject> EmptyRoundIconResourceObject;
};
