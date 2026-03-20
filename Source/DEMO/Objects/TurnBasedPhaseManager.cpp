#include "Objects/TurnBasedPhaseManager.h"
#include "Global.h"

#include "GameFramework/PlayerController.h"

#include "DEMOGameInstance.h"
#include "SaveLoadSubsystem.h"
#include "TurnBasedSubsystem.h"

#include "Characters/AI/TurnBasedAIController.h"
#include "Characters/AI/TurnBasedBehaviorComponent.h"
#include "Characters/AI/TurnBasedEnemy.h"

#include "Characters/TurnBasedCharacter.h"
#include "Characters/TurnBasedCharacterData.h"

#include "Datas/TurnBasedDataTypes.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"
#include "GameAbilities/ExecutionContextTypes.h"
#include "GameAbilities/GA_Skill.h"
#include "GameAbilities/GE_InstantDamage.h"

#include "Objects/SelectWidgetActor.h"
#include "Objects/TurnbasedPhaseCamera.h"

#include "Widgets/UW_TurnBased_Select.h"

ATurnBasedPhaseManager::ATurnBasedPhaseManager()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::GetClass<ASelectWidgetActor>(&SelectWidgetActorClass, "Blueprint'/Game/Widgets/SelectSkill/BP_SelectSkill.BP_SelectSkill_C'");
	CHelpers::GetClass<ASelectWidgetActor>(&SelectTargetCursorActorClass, "Blueprint'/Game/Widgets/SelectSkill/BP_SelectTarget.BP_SelectTarget_C'");

	ReservedActions.FindOrAdd(EReservedActionTiming::AfterCurrentAction);
	ReservedActions.FindOrAdd(EReservedActionTiming::StartOfNextTurn);
	ReservedActions.FindOrAdd(EReservedActionTiming::EndOfTurn);
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

	return;

	if (CurrentTurnCharacter)
	{
		CLog::Print(CurrentTurnCharacter->GetName(), -1, 0, FColor::Emerald);
		CurrentTurnCharacter->PrintAbilities();
	}
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
	ATurnBasedCharacter* ch = nullptr;
	if(TeamID == TEAMID_PLAYER)ch = GetWorld()->SpawnActorDeferred<ATurnBasedCharacter>(ATurnBasedCharacter::StaticClass(), FTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	else if(TeamID == TEAMID_ENEMY)ch = GetWorld()->SpawnActorDeferred<ATurnBasedEnemy>(ATurnBasedEnemy::StaticClass(), FTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	CheckTrue_Print(!ch, "ch is nullptr");
	UGameplayStatics::FinishSpawningActor(ch, FTransform());
	ch->Init(FGuid(), InData);
	ch->SetGenericTeamId(TeamID);
	SpawnedCharacterMap.FindOrAdd(TeamID).Add(ch);
	UAbilityComponent* asc = Cast<UAbilityComponent>(ch->GetAbilitySystemComponent());
	asc->OnSkillEnd.AddUFunction(this, "HandleStageTransition");
	asc->OnDeadSequenceEnd.AddUFunction(this, "HandleDeadCharacter");
	asc->OnRegisterPendingDeadArray.AddLambda([this](AActor* InActor) 
		{
			ATurnBasedCharacter* ch = Cast<ATurnBasedCharacter>(InActor);
			CheckTrue_Print(!ch, "RegisterPendingDeadArray Cast failed!!");
			PendingDeadArray.Add(ch);
		});
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

EReservedActionType ATurnBasedPhaseManager::ConsumeAfterCurrentAction()
{
	if (ReservedActions[EReservedActionTiming::AfterCurrentAction].IsEmpty())
		return EReservedActionType::MAX;

	FPayloadContext context = ReservedActions[EReservedActionTiming::AfterCurrentAction][0];
	ReservedActions[EReservedActionTiming::AfterCurrentAction].RemoveAt(0);
	UDA_ActionReservation* payload = Cast<UDA_ActionReservation>(context.Payload);

	if (!payload)
	{
		CLog::Print("ATurnBasedPhaseManager::ConsumeAfterCurrentAction() payload is nullptr", -1, 10, FColor::Red);
		return EReservedActionType::MAX;
	}

	if (payload->Type == EReservedActionType::ExtraTurn)
	{
		CLog::Print("ATurnBasedPhaseManager::ConsumeAfterCurrentAction() Type is ExtraTurn", -1, 10, FColor::Red);
		return EReservedActionType::MAX;
	}
	else if (payload->Type == EReservedActionType::ExtraSkill)
	{
		CLog::Print("NICE", -1, 10, FColor::Purple);
		ATurnBasedCharacter* instigator = nullptr;
		if (payload->Instigator == EPayloadActorType::RuleSource)
			instigator = Cast<ATurnBasedCharacter>(context.RuleSourceActor);
		else if(payload->Instigator == EPayloadActorType::EventCauser)
			instigator = Cast<ATurnBasedCharacter>(context.EventCauserActor);
		else if(payload->Instigator == EPayloadActorType::EventTarget)
			instigator = Cast<ATurnBasedCharacter>(context.EventTargetActor);

		ATurnBasedCharacter* target = nullptr;
		if (payload->Target == EPayloadActorType::RuleSource)
			target = Cast<ATurnBasedCharacter>(context.RuleSourceActor);
		else if(payload->Target == EPayloadActorType::EventCauser)
			target = Cast<ATurnBasedCharacter>(context.EventCauserActor);
		else if(payload->Target == EPayloadActorType::EventTarget)
			target = Cast<ATurnBasedCharacter>(context.EventTargetActor);

		PlaySequence(payload->SkillTag, instigator, target, 1);
	}

	return payload->Type;
}

EReservedActionType ATurnBasedPhaseManager::ConsumeStartOfNextTurn()
{
	if (ReservedActions[EReservedActionTiming::StartOfNextTurn].IsEmpty())
		return EReservedActionType::MAX;

	FPayloadContext context = ReservedActions[EReservedActionTiming::StartOfNextTurn][0];
	ReservedActions[EReservedActionTiming::StartOfNextTurn].RemoveAt(0);
	UDA_ActionReservation* payload = Cast<UDA_ActionReservation>(context.Payload);

	if (!payload)
	{
		CLog::Print("ATurnBasedPhaseManager::ConsumeStartOfNextTurn() payload is nullptr", -1, 10, FColor::Red);
		return EReservedActionType::MAX;
	}

	if (payload->Type == EReservedActionType::ExtraTurn)
	{
		if (payload->Target == EPayloadActorType::EventCauser)
			CurrentTurnCharacter = Cast<ATurnBasedCharacter>(context.EventCauserActor.Get());
		else if (payload->Target == EPayloadActorType::EventTarget)
			CurrentTurnCharacter = Cast<ATurnBasedCharacter>(context.EventTargetActor.Get());
		else if (payload->Target == EPayloadActorType::RuleSource)
			CurrentTurnCharacter = Cast<ATurnBasedCharacter>(context.RuleSourceActor.Get());

		if (!CurrentTurnCharacter)
		{
			CLog::Print("ATurnBasedPhaseManager::ConsumeStartOfNextTurn() CurrentTurnCharacter is nullptr", -1, 10, FColor::Red);
			return EReservedActionType::MAX;
		}
	}
	else if (payload->Type == EReservedActionType::ExtraSkill)
	{
		CLog::Print("NICE", -1, 10, FColor::Purple);
		ATurnBasedCharacter* instigator = nullptr;
		if (payload->Instigator == EPayloadActorType::RuleSource)
			instigator = Cast<ATurnBasedCharacter>(context.RuleSourceActor);
		else if (payload->Instigator == EPayloadActorType::EventCauser)
			instigator = Cast<ATurnBasedCharacter>(context.EventCauserActor);
		else if (payload->Instigator == EPayloadActorType::EventTarget)
			instigator = Cast<ATurnBasedCharacter>(context.EventTargetActor);

		ATurnBasedCharacter* target = nullptr;
		if (payload->Target == EPayloadActorType::RuleSource)
			target = Cast<ATurnBasedCharacter>(context.RuleSourceActor);
		else if (payload->Target == EPayloadActorType::EventCauser)
			target = Cast<ATurnBasedCharacter>(context.EventCauserActor);
		else if (payload->Target == EPayloadActorType::EventTarget)
			target = Cast<ATurnBasedCharacter>(context.EventTargetActor);

		PlaySequence(payload->SkillTag, instigator, target, 1);
	}

	return payload->Type;
}

EReservedActionType ATurnBasedPhaseManager::ConsumeEndOfTurn()
{
	if (ReservedActions[EReservedActionTiming::EndOfTurn].IsEmpty())
		return EReservedActionType::MAX;

	FPayloadContext context = ReservedActions[EReservedActionTiming::EndOfTurn][0];
	ReservedActions[EReservedActionTiming::EndOfTurn].RemoveAt(0);
	UDA_ActionReservation* payload = Cast<UDA_ActionReservation>(context.Payload);

	if (!payload)
	{
		CLog::Print("ATurnBasedPhaseManager::ConsumeAfterCurrentAction() payload is nullptr", -1, 10, FColor::Red);
		return EReservedActionType::MAX;
	}

	if (payload->Type == EReservedActionType::ExtraTurn)
	{
		CLog::Print("ATurnBasedPhaseManager::ConsumeAfterCurrentAction() Type is ExtraTurn", -1, 10, FColor::Red);
		return EReservedActionType::MAX;
	}
	else if (payload->Type == EReservedActionType::ExtraSkill)
	{
		CLog::Print("NICE", -1, 10, FColor::Purple);
		ATurnBasedCharacter* instigator = nullptr;
		if (payload->Instigator == EPayloadActorType::RuleSource)
			instigator = Cast<ATurnBasedCharacter>(context.RuleSourceActor);
		else if (payload->Instigator == EPayloadActorType::EventCauser)
			instigator = Cast<ATurnBasedCharacter>(context.EventCauserActor);
		else if (payload->Instigator == EPayloadActorType::EventTarget)
			instigator = Cast<ATurnBasedCharacter>(context.EventTargetActor);

		ATurnBasedCharacter* target = nullptr;
		if (payload->Target == EPayloadActorType::RuleSource)
			target = Cast<ATurnBasedCharacter>(context.RuleSourceActor);
		else if (payload->Target == EPayloadActorType::EventCauser)
			target = Cast<ATurnBasedCharacter>(context.EventCauserActor);
		else if (payload->Target == EPayloadActorType::EventTarget)
			target = Cast<ATurnBasedCharacter>(context.EventTargetActor);

		PlaySequence(payload->SkillTag, instigator, target, 1);
	}

	return payload->Type;
}

void ATurnBasedPhaseManager::HandleStageTransition()
{
	if (NextStage == EActionStage::PrePlaySequence)
	{
		FEffectEventContext* eec = new FEffectEventContext();
		eec->EventCauserActor = CurrentTurnCharacter;
		eec->EventTargetActor = TargetCharacter;

		UTurnBasedSubsystem* TBS = GetGameInstance()->GetSubsystem<UTurnBasedSubsystem>();
		TBS->SolvePreEvent(eec);
		NextStage = EActionStage::PlaySequence;
	}

	if (ConsumeAfterCurrentAction() == EReservedActionType::ExtraSkill)return;

	if (NextStage == EActionStage::FindNextTurn)
		FindNextTurn();
	else if (NextStage == EActionStage::HandleEffects)
		HandleEffects();
	else if (NextStage == EActionStage::FindDoTDamage)
		FindDoTDamage();
	else if (NextStage == EActionStage::HandleDoTDamage)
		HandleDoTDamage();
	else if (NextStage == EActionStage::HandleCC)
		HandleCC();
	else if (NextStage == EActionStage::FocusSelect)
		FocusSelect();
	else if (NextStage == EActionStage::PlaySequence)
		PlaySequence(CurrentSelectedSkillTag, CurrentTurnCharacter, TargetCharacter);
	else if (NextStage == EActionStage::EndTurn)
	{
		if (ConsumeEndOfTurn() == EReservedActionType::ExtraSkill)return;
		EndTurn();
	}
	else if (NextStage == EActionStage::FindDeadCharacter)
		FindDeadCharacter();
	else if (NextStage == EActionStage::HandleDeadCharacter)
		HandleDeadCharacter();
}

void ATurnBasedPhaseManager::FindNextTurn()
{
	//
	// 사이드위젯에 뒤에 순서가 어떻게되는지 스택해두기, 스피드 변경으로 순서바뀌면 gcn으로 터트리기?
	//

	EReservedActionType reservation = ConsumeStartOfNextTurn();

	// find current turn character
	if (reservation == EReservedActionType::MAX)
	{
		TArray<TTuple<float, ATurnBasedCharacter*>>Next;
		do
		{
			for (auto tuple : SpawnedCharacterMap)
			{
				for (auto& ch : tuple.Value)
				{
					if (ch->IsDead())continue;
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

		// handle turngauge
		{
			UAbilityComponent* asc = Cast<UAbilityComponent>(CurrentTurnCharacter->GetAbilitySystemComponent());
			UGameplayEffect* GE = NewObject<UGameplayEffect>(this);
			GE->DurationPolicy = EGameplayEffectDurationType::Instant;

			FGameplayModifierInfo Mod;
			Mod.Attribute = UAttributeSet_Character::GetTurnGaugeAttribute();
			Mod.ModifierOp = EGameplayModOp::Additive;
			Mod.ModifierMagnitude = FScalableFloat(-100);
			GE->Modifiers.Add(Mod);

			FGameplayEffectContextHandle context = asc->MakeEffectContext();
			FGameplayEffectSpec Spec(GE, context);

			CurrentTurnCharacter->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(Spec);
		}
	}
	else if (reservation == EReservedActionType::ExtraTurn)
	{
		// 이미 턴캐릭터 처리는 ConsumeStartOfNextTurn에서 처리됨
	}
	else if (reservation == EReservedActionType::ExtraSkill)
	{
		// ConsumeStartOfNextTurn-extra hit 재생 - HandleStageTransition - FindNextTurn 재진입
		return;
	}

	NextStage = EActionStage::HandleEffects;
	HandleStageTransition();
}

void ATurnBasedPhaseManager::HandleEffects()
{
	CheckTrue(CurrentTurnCharacter->IsDead());
	UAbilityComponent* asc = Cast<UAbilityComponent>(CurrentTurnCharacter->GetAbilitySystemComponent());
	CheckTrue(!asc);

	asc->HandleCooldown();
	asc->HandleBuff();
	asc->HandleDebuff();

	NextStage = EActionStage::FindDoTDamage;
	HandleStageTransition();
}

void ATurnBasedPhaseManager::FindDoTDamage()
{
	CheckTrue(CurrentTurnCharacter->IsDead());
	UAbilityComponent* asc = Cast<UAbilityComponent>(CurrentTurnCharacter->GetAbilitySystemComponent());
	CheckTrue(!asc);

	DoTHandles.Empty();
	asc->GetAllDoTDamageHandles(DoTHandles);

	if (DoTHandles.IsEmpty())NextStage = EActionStage::HandleCC;
	else NextStage = EActionStage::HandleDoTDamage;
	HandleStageTransition();
}

void ATurnBasedPhaseManager::HandleDoTDamage()
{
	CheckTrue(CurrentTurnCharacter->IsDead());
	UAbilityComponent* asc = Cast<UAbilityComponent>(CurrentTurnCharacter->GetAbilitySystemComponent());
	CheckTrue(!asc);

	if (!DoTHandles.IsEmpty())
	{
		AActor* EventCauserActor = nullptr;
		AActor* EventTargetActor = nullptr;

		asc->HandleDoTDamage(DoTHandles[0], &EventCauserActor, &EventTargetActor);
		DoTHandles.RemoveAt(0);

		// 추가 이벤트 판정
		FEffectEventContext* eec = new FEffectEventContext();
		eec->EventCauserActor = EventCauserActor;
		eec->EventTargetActor = EventTargetActor;

		UTurnBasedSubsystem* TBS = GetGameInstance()->GetSubsystem<UTurnBasedSubsystem>();
		TBS->SolveHitEvent(eec);
	}

	if (DoTHandles.IsEmpty())NextStage = EActionStage::HandleCC;
	else NextStage = EActionStage::HandleDoTDamage;
	HandleStageTransition();
}

void ATurnBasedPhaseManager::HandleCC()
{
	CheckTrue(CurrentTurnCharacter->IsDead());
	UAbilityComponent* asc = Cast<UAbilityComponent>(CurrentTurnCharacter->GetAbilitySystemComponent());
	CheckTrue(!asc);

	if (asc->HasTurnBlockingCC())
	{
		FTimerDelegate func =
			FTimerDelegate::CreateLambda([this]()
				{
					UAbilityComponent* asc = Cast<UAbilityComponent>(CurrentTurnCharacter->GetAbilitySystemComponent());
					CheckTrue(!asc);
					NextStage = EActionStage::FindNextTurn;
					asc->HandleCC();
				});
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, func, 1, false);		
	}
	else
	{
		asc->HandleCC();
		NextStage = EActionStage::FocusSelect;
		HandleStageTransition();
	}
}

void ATurnBasedPhaseManager::FocusSelect()
{
	Camera->FocusSelectSkill(CurrentTurnCharacter);
	SelectWidgetActor->SetActorTransform(CurrentTurnCharacter->GetActorTransform());
	//SelectWidgetActor->SetWidgetRelativeTransform(CurrentTurnCharacter->GetSelectSkillRelativeTransform());

	UUW_TurnBased_Select* select = Cast<UUW_TurnBased_Select>(SelectWidgetActor->GetWidgetObject());
	select->Activate(CurrentTurnCharacter, LocationArray[TEAMID_PLAYER], LocationArray[TEAMID_ENEMY]);
	SelectWidgetActor->Show();
	SelectTargetCursorActor->Show();

	if (CurrentTurnCharacter->IsA<ATurnBasedEnemy>())
	{
		UTurnBasedBehaviorComponent* behavior = CHelpers::GetComponent<UTurnBasedBehaviorComponent>(CurrentTurnCharacter->GetController());
		behavior->SetWaitMode();
	}
}

void ATurnBasedPhaseManager::PlaySequence(FGameplayTag SkillTag, ATurnBasedCharacter* SkillOwner, ATurnBasedCharacter* Target, bool Extra)
{
	UAbilityComponent* asc = Cast<UAbilityComponent>(SkillOwner->GetAbilitySystemComponent());
	TArray<FGameplayAbilitySpec>& abilities = asc->GetActivatableAbilities();
	for (auto& spec : abilities)
	{
		const UGA_Skill* skill = Cast<UGA_Skill>(spec.Ability);
		if (!skill)continue;
		if (skill->GetSkillTag() != SkillTag)continue;
		asc->SetTarget(Target);
		asc->TryActivateAbility(spec.Handle);
		break;
	}
	if (!Extra)
	{
		// 추가 이벤트 판정
		{
			FEffectEventContext* eec = new FEffectEventContext();
			eec->EventCauserActor = SkillOwner;
			eec->EventTargetActor = Target;

			UTurnBasedSubsystem* TBS = GetGameInstance()->GetSubsystem<UTurnBasedSubsystem>();
			TBS->SolveHitEvent(eec);
		}

		NextStage = EActionStage::EndTurn;
	}
}

void ATurnBasedPhaseManager::EndTurn()
{
	if (CurrentTurnCharacter->IsA<ATurnBasedEnemy>())
	{
		UTurnBasedBehaviorComponent* behavior = CHelpers::GetComponent<UTurnBasedBehaviorComponent>(CurrentTurnCharacter->GetController());
		behavior->SetNotMyTurnMode();
	}

	NextStage = EActionStage::FindDeadCharacter;
	HandleStageTransition();
}

void ATurnBasedPhaseManager::ConfirmSelect(FGameplayTag InSkillTag, ATurnBasedCharacter* InTarget)
{
	SelectWidgetActor->Hide();
	SelectTargetCursorActor->Hide();
	CurrentSelectedSkillTag = InSkillTag;
	TargetCharacter = InTarget;

	NextStage = EActionStage::PrePlaySequence;
	HandleStageTransition();
}

void ATurnBasedPhaseManager::FindDeadCharacter()
{
	for (auto& i : SpawnedCharacterMap)
	{
		for (auto& ch : i.Value)
		{
			if (ch->IsDead() && !HandledDeadSet.Contains(ch))
				PendingDeadArray.Add(ch);
		}
	}
	NextStage = EActionStage::HandleDeadCharacter;
	HandleStageTransition();
}

void ATurnBasedPhaseManager::HandleDeadCharacter()
{
	/*
	* pending array에 있는 시퀀스 반복 재생
	*/

	if (PendingDeadArray.IsEmpty())
	{
		if (IsPlayerVictory())
		{
			HandlePlayerVictory();
			return;
		}
		if (IsPlayerDefeat())
		{
			HandlePlayerDefeat();
			return;
		}
		NextStage = EActionStage::FindNextTurn;
		HandleStageTransition();
	}
	else
	{
		ATurnBasedCharacter* ch = PendingDeadArray[PendingDeadArray.Num() - 1];
		PendingDeadArray.Remove(ch);
		UAbilityComponent* asc = Cast<UAbilityComponent>(ch->GetAbilitySystemComponent());
		HandledDeadSet.Add(ch);
		asc->BeginDeadAbility();
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

	//필드 정리하기
	for (auto& tuple : SpawnedCharacterMap)
	{
		for (auto ch : tuple.Value)
			ch->Destroy();
	}
	Camera->Destroy();
	SelectWidgetActor->Destroy();
	SelectTargetCursorActor->Destroy();

	// ui 팝업에 tps필드 이동을 달아놓자
	UPhaseTransitionContext* context = NewObject<UPhaseTransitionContext>(this, UPhaseTransitionContext::StaticClass());

	UDEMOGameInstance* gi = Cast<UDEMOGameInstance>(GetGameInstance());
	gi->RequestPhaseChange(EGameInstancePhase::TPS, context);

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

	//필드 정리하기
	for (auto& tuple : SpawnedCharacterMap)
	{
		for (auto ch : tuple.Value)
			ch->Destroy();
	}
	Camera->Destroy();
	SelectWidgetActor->Destroy();
	SelectTargetCursorActor->Destroy();

	// ui 팝업에 tps필드 이동을 달아놓자
	UPhaseTransitionContext* context = NewObject<UPhaseTransitionContext>(this, UPhaseTransitionContext::StaticClass());

	UDEMOGameInstance* gi = Cast<UDEMOGameInstance>(GetGameInstance());
	gi->RequestPhaseChange(EGameInstancePhase::TPS, context);
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

void ATurnBasedPhaseManager::ApplyCameraMove(const FPayloadContext* InEffectContext)
{
	Camera->ApplyCameraMove(InEffectContext);
}

void ATurnBasedPhaseManager::ReserveAction(const FPayloadContext* InEffectContext)
{
	UDA_ActionReservation* da = Cast<UDA_ActionReservation>(InEffectContext->Payload);	
	CheckTrue_Print(!da, "da is nullptr");

	ReservedActions.FindOrAdd(da->Timing).Add(*InEffectContext);
}

void ATurnBasedPhaseManager::ApplyGE(const FPayloadContext* InEffectContext)
{
	UDA_ApplyGE* da = Cast<UDA_ApplyGE>(InEffectContext->Payload);
	CheckTrue_Print(!da, "da is nullptr");

	ATurnBasedCharacter* causer = Cast<ATurnBasedCharacter>(InEffectContext->EventCauserActor.Get());;
	ATurnBasedCharacter* base = nullptr;
	TArray<ATurnBasedCharacter*> targets;

	if (da->ReferenceActor == EPayloadActorType::RuleSource)
		base = Cast<ATurnBasedCharacter>(InEffectContext->RuleSourceActor.Get());
	else if (da->ReferenceActor == EPayloadActorType::EventCauser)
		base = Cast<ATurnBasedCharacter>(InEffectContext->EventCauserActor.Get());
	else if (da->ReferenceActor == EPayloadActorType::EventTarget)
		base = Cast<ATurnBasedCharacter>(InEffectContext->EventTargetActor.Get());

	CheckTrue_Print(!causer, "causer cast fail");
	CheckTrue_Print(!base, "base cast fail");
	{
		TArray<ATurnBasedCharacter*> temp;

		if (da->TeamCondition == EPayloadReferenceTeamCondition::All)
		{
			for (auto set : SpawnedCharacterMap)
				for (auto ch : set.Value)
				{
					if (ch->IsDead())continue;
					temp.Add(ch);
				}
		}
		else if (da->TeamCondition == EPayloadReferenceTeamCondition::Ally)
		{
			for (auto ch : SpawnedCharacterMap[base->GetGenericTeamId()])
			{
				if (ch->IsDead())continue;
				temp.Add(ch);
			}
		}
		else if (da->TeamCondition == EPayloadReferenceTeamCondition::Enemy)
		{
			for (auto ch : SpawnedCharacterMap[base->GetGenericTeamId() ^ 1])
			{
				if (ch->IsDead())continue;
				temp.Add(ch);
			}
		}
		else if (da->TeamCondition == EPayloadReferenceTeamCondition::Self)
			temp.Add(base);

		if (da->SelectCondition == EPayloadReferenceSelectCondition::All)
			targets = temp;
		else if (da->SelectCondition == EPayloadReferenceSelectCondition::Random)
		{
			for (int32 i = 0; i < da->GoalCount; i++)
			{
				if (!temp.Num())break;
				int32 idx = UKismetMathLibrary::RandomIntegerInRange(0, temp.Num() - 1);
				targets.Add(temp[idx]);
				temp.RemoveAt(idx);
			}
		}
		else if (
			da->SelectCondition == EPayloadReferenceSelectCondition::Highest ||
			da->SelectCondition == EPayloadReferenceSelectCondition::Lowest)
		{
			TArray<TTuple<float, ATurnBasedCharacter*>> sort;
			for (auto ch : temp)
			{
				UAbilityComponent* asc = Cast<UAbilityComponent>(ch->GetAbilitySystemComponent());
				if (!asc)continue;
				float value = 0;
				switch (da->AttributeCondition)
				{
				case EPayloadReferenceAttributeCondition::Health:
					value = asc->GetHealth(); break;
				default:
					break;
				}
				sort.Add({ value,ch });
			}
			sort.Sort();
			int32 idx = da->SelectCondition == EPayloadReferenceSelectCondition::Highest ? sort.Num() - 1 : 0;
			int32 add = da->SelectCondition == EPayloadReferenceSelectCondition::Highest ? -1 : 1;
			int32 cnt = da->GoalCount;
			while (cnt-- && sort.IsValidIndex(idx))
			{
				targets.Add(sort[idx].Value);
				idx += add;
			}
		}
	}

	for (auto target : targets)
	{
		FExecutionContext* context = new FExecutionContext();
		context->EffectSourceActor = InEffectContext->RuleSourceActor.Get();
		context->EffectCauserActor = InEffectContext->EventCauserActor.Get();
		context->EffectTargetActor = target;
		context->SkillCauserActor = CurrentTurnCharacter;
		context->SkillTargetActor = TargetCharacter;

		FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(context);
		EffectContextHandle.AddInstigator(InEffectContext->EventCauserActor.Get(), InEffectContext->EventCauserActor.Get());

		FGameplayEffectSpecHandle handle =
			causer->GetAbilitySystemComponent()->MakeOutgoingSpec(
				da->GE,
				UGameplayEffect::INVALID_LEVEL,
				EffectContextHandle);
		handle.Data.Get()->StackCount = da->StackCount;

		causer->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*handle.Data, target->GetAbilitySystemComponent());
	}


}

void ATurnBasedPhaseManager::ChangeTarget(ATurnBasedCharacter* InTarget)
{
	TargetCharacter = InTarget;
}

const TSet<ATurnBasedCharacter*>& ATurnBasedPhaseManager::GetPlayerCharacters() const
{
	return SpawnedCharacterMap[TEAMID_PLAYER];
}

const TSet<ATurnBasedCharacter*>& ATurnBasedPhaseManager::GetEnemyCharacters() const
{
	return SpawnedCharacterMap[TEAMID_ENEMY];
}
