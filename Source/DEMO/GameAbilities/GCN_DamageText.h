#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GCN_DamageText.generated.h"

/**
 * 데미지 텍스트를 스폰하는 GCN
 * GE에서 GCN 태그를 추가해 자동으로 적용
 * GE적용시 파라미터 설정 필수
 */

class ADamageText;

UCLASS(Blueprintable, Category = "GameplayCueNotify")
class DEMO_API UGCN_DamageText : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
public:
	UGCN_DamageText();
protected:
public:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

	//property
private:
protected:
	UPROPERTY(EditAnywhere)
		TSubclassOf<ADamageText> DamageTextClass;
public:

	//function
private:
protected:
public:
};
