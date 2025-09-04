#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameAbilities/AbilityTypes.h"
#include "AbilityComponent.generated.h"

/**
 * DEMO 프로젝트에서 사용할 ASC
 */

class UGA_BaseAbility;

USTRUCT(BlueprintType)
struct FSkillData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Class")
		TSubclassOf<UGA_BaseAbility> SkillClass;

	//UPROPERTY(EditAnywhere, Category = "Class")
	//	ESkillTreeSkillState BaseState;

	//UPROPERTY(EditAnywhere, Category = "Widget", meta = (DisplayThumbnail = "true", DisplayName = "Image", AllowedClasses = "Texture,MaterialInterface,SlateTextureAtlasInterface", DisallowedClasses = "MediaTexture"))
	//	TObjectPtr<UObject> SkillImage;

	//UPROPERTY(EditAnywhere, Category = "Widget")
	//	FVector2D PannelPosition;

	//UPROPERTY(EditAnywhere, Category = "Widget")
	//	FVector2D ParentPosition = FVector2D(-1, -1);

	//UPROPERTY(EditAnywhere, Category = "Widget")
	//	UMediaSource* PreviewSource;

	//UPROPERTY(EditAnywhere, Category = "Widget")
	//	FText Name;

	//UPROPERTY(EditAnywhere, Category = "Widget")
	//	FText Description;
};

UCLASS()
class DEMO_API UAbilityComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UAbilityComponent();
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:

	//property
private:
protected:
public:

	//function
private:
protected:
public:
	void Init(const TArray<FAbilitySpecInfo>& NewGAs);

	float GetDefense() const;
	float GetPower() const;
	float GetSpeed() const;
};


//
// 태그 설정
// 캐릭터 리타게팅 및 쓸만한 스킬시퀀스 찾기
//