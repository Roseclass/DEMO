#include "Objects/TurnBasedPhaseManager.h"
#include "Global.h"

#include "GameFramework/PlayerController.h"

#include "SaveLoadSubsystem.h"

#include "Characters/TurnBasedCharacter.h"
#include "Characters/TurnBasedCharacterData.h"

#include "Datas/TurnBasedDataTypes.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"
#include "GameAbilities/GA_Skill.h"

#include "Objects/SelectWidgetActor.h"
#include "Objects/TurnbasedPhaseCamera.h"

#include "Widgets/UW_SelectTarget.h"
#include "Widgets/UW_SelectSkill.h"

ATurnBasedPhaseManager::ATurnBasedPhaseManager()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::GetClass<ASelectWidgetActor>(&SelectTargetClass, "Blueprint'/Game/Widgets/SelectSkill/BP_SelectTarget.BP_SelectTarget_C'");
	CHelpers::GetClass<ASelectWidgetActor>(&SelectSkillClass, "Blueprint'/Game/Widgets/SelectSkill/BP_SelectSkill.BP_SelectSkill_C'");
}

void ATurnBasedPhaseManager::BeginPlay()
{
	Super::BeginPlay();
	SpawnCamera();
	SpawnSelectTarget();
	SpawnSelectSkill();
}

void ATurnBasedPhaseManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATurnBasedPhaseManager::SpawnCamera()
{
	Camera = GetWorld()->SpawnActorDeferred<ATurnbasedPhaseCamera>(ATurnbasedPhaseCamera::StaticClass(), FTransform());
	UGameplayStatics::FinishSpawningActor(Camera, FTransform());	

	APlayerController* pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	pc->SetInputMode(FInputModeUIOnly());
	pc->SetViewTargetWithBlend(Camera);
}

void ATurnBasedPhaseManager::SpawnSelectTarget()
{
	SelectTarget = ASelectWidgetActor::CreateSelectWidgetActor(GetWorld(), SelectTargetClass, FTransform(), this);
	UUW_SelectTarget* selectTarget = Cast<UUW_SelectTarget>(SelectTarget->GetWidgetObject());
	TFunction<void(ATurnBasedCharacter*)>func = [this](ATurnBasedCharacter* InCharacter)
	{
		FRotator lookat = UKismetMathLibrary::FindLookAtRotation(Camera->GetActorLocation(), InCharacter->GetActorLocation());
		//UNDONE GCN으로 교체하기
		Camera->SetTargetRotation(lookat);
	};
	selectTarget->SetOwningActor(SelectTarget);
	selectTarget->OnMoveDown.AddLambda(func);
	selectTarget->OnConfirmDown.AddUFunction(this, "ConfirmTarget");
	TFunction<bool()>func2 = [this]()
	{
		return !Camera->IsRotating();
	};
	selectTarget->OnCanSelectTargetKeyDown.BindLambda(func2);
}

void ATurnBasedPhaseManager::SpawnSelectSkill()
{
	SelectSkill = ASelectWidgetActor::CreateSelectWidgetActor(GetWorld(), SelectSkillClass, FTransform(), this);
	UUW_SelectSkill* selectSkill = Cast<UUW_SelectSkill>(SelectSkill->GetWidgetObject());
	TFunction<void(FGameplayTag)>func = [this](FGameplayTag InSkillTag)
	{
		this->ConfirmSkill(InSkillTag);
	};
	selectSkill->OnConfirmDown.AddLambda(func);
}

void ATurnBasedPhaseManager::TrySpawnCharacter()
{
	for (auto& tuple : PendingSpawnMap)
	{
		while (!tuple.Value.IsEmpty())
		{
			UTurnBasedCharacterData* data = tuple.Value[tuple.Value.Num() - 1];
			tuple.Value.RemoveAt(tuple.Value.Num() - 1);
			SpawnCharacter(tuple.Key, data);
		}
	}

	PlaceActorsOnField();
}

void ATurnBasedPhaseManager::SpawnCharacter(uint8 TeamID, UTurnBasedCharacterData* InData)
{
	CheckNull(InData);
	ATurnBasedCharacter* ch = GetWorld()->SpawnActorDeferred<ATurnBasedCharacter>(ATurnBasedCharacter::StaticClass(), FTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	UGameplayStatics::FinishSpawningActor(ch, FTransform());
	ch->Init(FGuid(), InData);
	ch->SetGenericTeamId(TeamID);
	SpawnedCharacterMap.FindOrAdd(TeamID).Add(ch);
}

void ATurnBasedPhaseManager::PlaceActorsOnField()
{
	CheckTrue_Print(!LevelData, "LevelData is nullptr");

	Camera->Init(LevelData->CameraInitalTranform);

	if(!SpawnedCharacterMap.FindOrAdd(TEAMID_PLAYER).IsEmpty())
	{
		FVector gap;
		gap = (LevelData->PlayerEnd - LevelData->PlayerStart)
			/ (SpawnedCharacterMap[TEAMID_PLAYER].Num() + 1);

		FVector offset = gap;
		for (auto i : SpawnedCharacterMap[TEAMID_PLAYER])
		{
			i->SetActorLocation(LevelData->PlayerStart + offset);
			i->SetActorRotation(LevelData->PlayerRotation);
			LocationArray[TEAMID_PLAYER].Add(i);
			offset += gap;
		}
	}

	if(!SpawnedCharacterMap.FindOrAdd(TEAMID_ENEMY).IsEmpty())
	{
		FVector gap;
		gap = (LevelData->EnemyEnd - LevelData->EnemyStart)
			/ (SpawnedCharacterMap[TEAMID_ENEMY].Num() + 1);

		FVector offset = gap;
		for (auto i : SpawnedCharacterMap[TEAMID_ENEMY])
		{
			i->SetActorLocation(LevelData->EnemyStart + offset);
			i->SetActorRotation(LevelData->EnemyRotation);
			LocationArray[TEAMID_ENEMY].Add(i);
			offset += gap;
		}
	}

	FindNextTurn();
}

void ATurnBasedPhaseManager::FindNextTurn()
{
	//
	// 사이드위젯에 뒤에 순서가 어떻게되는지 스택해두기, 스피드 변경으로 순서바뀌면 gcn으로 터트리기?
	//

	TArray<TTuple<float, ATurnBasedCharacter*>>Next;
	do
	{
		for (auto tuple : SpawnedCharacterMap)
		{
			for (auto& ch : tuple.Value)
			{
				UAbilitySystemComponent* asc = ch->GetAbilitySystemComponent();
				UGameplayEffect* GE = NewObject<UGameplayEffect>(this);
				GE->DurationPolicy = EGameplayEffectDurationType::Instant;

				FGameplayModifierInfo Mod;
				Mod.Attribute = UAttributeSet_Character::GetTurnGaugeAttribute();
				Mod.ModifierOp = EGameplayModOp::Additive;
				Mod.ModifierMagnitude = FScalableFloat(ch->GetSpeed());
				GE->Modifiers.Add(Mod);

				FGameplayEffectContextHandle context = asc->MakeEffectContext();
				FGameplayEffectSpec Spec(GE, context);

				ch->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(Spec);
				float gauge = ch->GetTurnGauge();
				if (100 <= gauge)Next.Add({ gauge,ch });
			}
		}
	} while (Next.IsEmpty());

	Next.Sort();
	CurrentTurnCharacter = Next[Next.Num() - 1].Value;

	FocusSelectSkill();
}

void ATurnBasedPhaseManager::FocusSelectSkill()
{
	Camera->FocusSelectSkill(CurrentTurnCharacter);
	SelectSkill->SetActorTransform(CurrentTurnCharacter->GetSelectSkillTransform());
	SelectSkill->SetWidgetRelativeTransform(CurrentTurnCharacter->GetSelectSkillRelativeTransform());

	UUW_SelectSkill* selectSkill = Cast<UUW_SelectSkill>(SelectSkill->GetWidgetObject());

	selectSkill->Activate(CurrentTurnCharacter);

	SelectSkill->Show();
}

void ATurnBasedPhaseManager::FocusSelectTarget()
{
	int32 targetID = CurrentTurnCharacter->GetGenericTeamId() == TEAMID_PLAYER ? TEAMID_ENEMY : TEAMID_PLAYER;

	Camera->FocusSelectTarget();

	UUW_SelectTarget* selectTarget = Cast<UUW_SelectTarget>(SelectTarget->GetWidgetObject());

	selectTarget->Activate(LocationArray[targetID]);

	SelectTarget->Show();
}

void ATurnBasedPhaseManager::PlaySequence()
{
	UAbilityComponent* asc = Cast<UAbilityComponent>(CurrentTurnCharacter->GetAbilitySystemComponent());
	TArray<FGameplayAbilitySpec>& abilities = asc->GetActivatableAbilities();
	for (auto& ability : abilities)
	{
		if (!ability.Ability->AbilityTags.HasTagExact(CurrentSelectedSkillTag))
			continue;
		asc->SetTarget(TargetCharacter);
		asc->TryActivateAbility(ability.Handle);
		break;
	}
}

void ATurnBasedPhaseManager::EndTurn()
{
	//
	// 혹시 누구 죽음? - 죽음 재생
	// 혹시 플레이어 이김? - 승리 후 tps필드로
	// 혹시 플레이어 짐? - 패배 후 어디로? 메인? 최근 저장된곳?
	// 엥 안끝났어? - findnextturn
	//
}

void ATurnBasedPhaseManager::ConfirmSkill(FGameplayTag InSkillTag)
{
	//UNDONE
	SelectSkill->Hide();
	FocusSelectTarget();
	CurrentSelectedSkillTag = InSkillTag;
}

void ATurnBasedPhaseManager::ConfirmTarget(ATurnBasedCharacter* InTarget)
{
	SelectTarget->Hide();
	TargetCharacter = InTarget;
	PlaySequence();
}

void ATurnBasedPhaseManager::SetLevelData(FTurnBasedFieldLayoutRow* NewLevelData)
{
	LevelData = NewLevelData;
}

void ATurnBasedPhaseManager::AddToSpawnMap(uint8 TeamID, TArray<FGameplayTag> InGameplayTags)
{
	for (auto tag : InGameplayTags)
		SpawnRequestCountMap.FindOrAdd(tag)++;
}

void ATurnBasedPhaseManager::RequestSpawnCharacter(uint8 TeamID, UTurnBasedCharacterData* InData)
{
	TArray<UTurnBasedCharacterData*>& datas = PendingSpawnMap.FindOrAdd(TeamID);
	datas.Add(InData);
	SpawnRequestCountMap.FindOrAdd(InData->RuntimeData.DataTag)--;

	for (const auto& i : SpawnRequestCountMap)
		if (i.Value > 0)return;
	TrySpawnCharacter();
}