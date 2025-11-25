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

#include "Widgets/UW_TurnBased_Select.h"

ATurnBasedPhaseManager::ATurnBasedPhaseManager()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::GetClass<ASelectWidgetActor>(&SelectWidgetActorClass, "Blueprint'/Game/Widgets/SelectSkill/BP_SelectSkill.BP_SelectSkill_C'");
	CHelpers::GetClass<ASelectWidgetActor>(&SelectTargetCursorActorClass, "Blueprint'/Game/Widgets/SelectSkill/BP_SelectTarget.BP_SelectTarget_C'");
}

void ATurnBasedPhaseManager::BeginPlay()
{
	Super::BeginPlay();
	SpawnCamera();
	SpawnSelectWidget();
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

void ATurnBasedPhaseManager::SpawnSelectWidget()
{
	SelectWidgetActor = ASelectWidgetActor::CreateSelectWidgetActor(GetWorld(), SelectWidgetActorClass, FTransform(), this);
	UUW_TurnBased_Select* select = Cast<UUW_TurnBased_Select>(SelectWidgetActor->GetWidgetObject());
	TFunction<void(FGameplayTag, ATurnBasedCharacter*)>func = [this](FGameplayTag InSkillTag, ATurnBasedCharacter* InTarget)
	{
		this->ConfirmSelect(InSkillTag, InTarget);
	};
	select->OnConfirm.AddLambda(func);

	SelectTargetCursorActor = ASelectWidgetActor::CreateSelectWidgetActor(GetWorld(), SelectTargetCursorActorClass, FTransform(), this);
	
	TFunction<void(ATurnBasedCharacter*)>func1 = [this](ATurnBasedCharacter* InCharacter)
	{
		FRotator lookat = UKismetMathLibrary::FindLookAtRotation(Camera->GetActorLocation(), InCharacter->GetActorLocation());
		Camera->SetTargetRotation(lookat);
	};

	select->SetTargetCursorActor(SelectTargetCursorActor);
	select->OnHorizontalMove.AddLambda(func1);

	TFunction<bool()>func2 = [this]()
	{
		return !Camera->IsRotating();
	};
	select->CanAcceptKey.BindLambda(func2);
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
	UAbilityComponent* asc = Cast<UAbilityComponent>(ch->GetAbilitySystemComponent());
	asc->OnSkillEnd.AddUFunction(this, "EndTurn");
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

	FocusSelect();
}

void ATurnBasedPhaseManager::FocusSelect()
{
	Camera->FocusSelectSkill(CurrentTurnCharacter);
	SelectWidgetActor->SetActorTransform(CurrentTurnCharacter->GetSelectSkillTransform());
	SelectWidgetActor->SetWidgetRelativeTransform(CurrentTurnCharacter->GetSelectSkillRelativeTransform());

	UUW_TurnBased_Select* select = Cast<UUW_TurnBased_Select>(SelectWidgetActor->GetWidgetObject());
	int32 targetID = CurrentTurnCharacter->GetGenericTeamId() == TEAMID_PLAYER ? TEAMID_ENEMY : TEAMID_PLAYER;


	select->Activate(CurrentTurnCharacter, LocationArray[targetID]);
	SelectWidgetActor->Show();
	SelectTargetCursorActor->Show();
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
	CLog::Print("ATurnBasedPhaseManager::EndTurn()");
	FindDeadCharacter();
}

void ATurnBasedPhaseManager::ConfirmSelect(FGameplayTag InSkillTag, ATurnBasedCharacter* InTarget)
{
	SelectWidgetActor->Hide();
	SelectTargetCursorActor->Hide();
	CurrentSelectedSkillTag = InSkillTag;
	TargetCharacter = InTarget;
	PlaySequence();
}

void ATurnBasedPhaseManager::FindDeadCharacter()
{
	for (auto& i : SpawnedCharacterMap)
	{
		for (auto& ch : i.Value)
		{
			UAbilityComponent* asc = Cast<UAbilityComponent>(ch->GetAbilitySystemComponent());
			float hp = asc->GetHealth();
			if (hp <= 1e-9 && !HandledDeadSet.Contains(ch))
				PendingDeadArray.Add(ch);
		}
	}
	HandleDeadCharacter();
}

void ATurnBasedPhaseManager::HandleDeadCharacter()
{
	/*
	* pending array에 있는 시퀀스 반복 재생
	*/
	if (PendingDeadArray.IsEmpty())
	{
		if (IsPlayerVictory())HandlePlayerVictory();
		if (IsPlayerDefeat())HandlePlayerDefeat();
		FindNextTurn();
	}
	else
	{
		UAbilityComponent* asc = Cast<UAbilityComponent>(PendingDeadArray[PendingDeadArray.Num() - 1]->GetAbilitySystemComponent());
		asc->OnDeadSequenceEnd.AddUFunction(this, "HandleDeadCharacter");
		asc->PlayDeadSequence();
	}
}

bool ATurnBasedPhaseManager::IsPlayerVictory()
{
	const TSet<ATurnBasedCharacter*>& set = SpawnedCharacterMap[TEAMID_ENEMY];

	for (auto ch : set)
		if (!HandledDeadSet.Contains(ch))return 0;
	return 1;
}

void ATurnBasedPhaseManager::HandlePlayerVictory()
{
	/*
	* 승리 후 tps필드로
	*/

	// ui 팝업에 tps필드 이동을 달아놓자
}

bool ATurnBasedPhaseManager::IsPlayerDefeat()
{
	const TSet<ATurnBasedCharacter*>& set = SpawnedCharacterMap[TEAMID_PLAYER];

	for (auto ch : set)
		if (!HandledDeadSet.Contains(ch))return 0;
	return 1;
}

void ATurnBasedPhaseManager::HandlePlayerDefeat()
{
	/*
	* 패배 후 어디로? 메인? 최근 저장된곳?
	*/

	// ui 팝업에 다음 페이즈 이동을 달아놓자
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