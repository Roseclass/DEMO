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

#include "Objects/TurnbasedPhaseCamera.h"

#include "Widgets/UW_TBRoot.h"

ATurnBasedPhaseManager::ATurnBasedPhaseManager()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::GetClass(&RootWidgetClass, "WidgetBlueprint'/Game/Widgets/TB/WBP_TBRoot.WBP_TBRoot_C'");

	ReservedActions.FindOrAdd(EReservedActionTiming::AfterCurrentAction);
	ReservedActions.FindOrAdd(EReservedActionTiming::StartOfNextTurn);
	ReservedActions.FindOrAdd(EReservedActionTiming::EndOfTurn);
}

void ATurnBasedPhaseManager::BeginPlay()
{
	Super::BeginPlay();
	SpawnCamera();
	CreateRootWidget();
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
	pc->SetInputMode(FInputModeGameAndUI());
	pc->SetViewTargetWithBlend(Camera);
}

void ATurnBasedPhaseManager::CreateRootWidget()
{
	CheckTrue_Print(!RootWidgetClass, "RootWidgetClass is nullptr!!");
	RootWidget = CreateWidget<UUW_TBRoot>(UGameplayStatics::GetPlayerController(GetWorld(), 0), RootWidgetClass);
	RootWidget->AddToViewport();
	CheckTrue_Print(!RootWidgetClass, "RootWidget is nullptr!!");

	UUW_TBSelect* select = RootWidget->GetSelectWidget();

	TFunction<void(FGameplayTag)> enter = [this](FGameplayTag InSkillTag)
	{
		CurrentSelectedSkillTag = InSkillTag;
		if(CurrentStage == EActionStage::SelectSkill)
			CurrentTurnCharacter->OnAnimTagChanged.Broadcast(InSkillTag);
	};
	select->OnSkillIconMouseEnter.BindLambda(enter);

	TFunction<void(FGameplayTag)> leave = [this](FGameplayTag InSkillTag)
	{
		CurrentSelectedSkillTag = InSkillTag;
		if (CurrentStage == EActionStage::SelectSkill)
			CurrentTurnCharacter->OnAnimTagChanged.Broadcast(InSkillTag);
	};
	select->OnSkillIconMouseLeave.BindLambda(leave);

	TFunction<void()> clicked = [this]()
	{
		// 스킬 아이콘 클릭:카메라뷰 변경 + 타겟 선택 활성화
		CheckTrue(Camera->IsRotating());

		// 다른 스킬 클릭시 selectTarget 재진입
		// 선택된 스킬 아이콘은 glow된 상태로 만들기

		TargetCharacters.Empty();
		NextStage = EActionStage::SelectTarget;
		HandleStageTransition();
	};
	select->OnSkillIconClicked.BindLambda(clicked);
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
	ch->OnBeginCursorOver.AddDynamic(this,&ATurnBasedPhaseManager::OnSpawnedCharacterBeginCursorOver);
	ch->OnEndCursorOver.AddDynamic(this, &ATurnBasedPhaseManager::OnSpawnedCharacterEndCursorOver);
	ch->OnClicked.AddDynamic(this, &ATurnBasedPhaseManager::OnSpawnedCharacterClicked);
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

void ATurnBasedPhaseManager::OnSpawnedCharacterBeginCursorOver(AActor* TouchedActor)
{
	CheckTrue(CurrentStage != EActionStage::SelectTarget);
	ATurnBasedCharacter* ch = Cast<ATurnBasedCharacter>(TouchedActor);
	CheckTrue_Print(!ch, "ch cast failed!!");

	if(CurrentTurnCharacter->GetGenericTeamId() == ch->GetGenericTeamId())
		ch->ApplyHighlight(EHighlightType::Green);
	else
		ch->ApplyHighlight(EHighlightType::Red);
}

void ATurnBasedPhaseManager::OnSpawnedCharacterEndCursorOver(AActor* TouchedActor)
{
	CheckTrue(CurrentStage != EActionStage::SelectTarget);
	ATurnBasedCharacter* ch = Cast<ATurnBasedCharacter>(TouchedActor);
	CheckTrue_Print(!ch, "ch cast failed!!");
	ch->ApplyHighlight(EHighlightType::Gray);
}

void ATurnBasedPhaseManager::OnSpawnedCharacterClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	ATurnBasedCharacter* ch = Cast<ATurnBasedCharacter>(TouchedActor);
	CheckTrue_Print(!ch, "ch cast failed!!");
	CheckTrue(ButtonPressed != EKeys::LeftMouseButton);

	TargetCharacters.AddUnique(ch);
	
	if (FMath::IsNearlyEqual(TargetCharacters.Num(), TargetGoalCount))
	{
		ClearAvailableTargets();
		NextStage = EActionStage::PrePlaySequence;
		HandleStageTransition();
	}
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

void ATurnBasedPhaseManager::HighlightAvailableTargets()
{
	for(auto ch : AvailableTargets)
		ch->ApplyHighlight(EHighlightType::Gray);
}

void ATurnBasedPhaseManager::ClearAvailableTargets()
{
	for (auto ch : AvailableTargets)
		ch->ApplyHighlight(EHighlightType::NONE);
	AvailableTargets.Empty();
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

		TArray<ATurnBasedCharacter*> targets;
		targets.Add(target);
		PlaySequence(payload->SkillTag, instigator, targets, 1);
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

		TArray<ATurnBasedCharacter*> targets;
		targets.Add(target);
		PlaySequence(payload->SkillTag, instigator, targets, 1);
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

		TArray<ATurnBasedCharacter*> targets;
		targets.Add(target);
		PlaySequence(payload->SkillTag, instigator, targets, 1);
	}

	return payload->Type;
}

void ATurnBasedPhaseManager::HandleStageTransition()
{
	if (NextStage == EActionStage::PrePlaySequence)
	{
		CurrentStage = EActionStage::PrePlaySequence;
		for (auto target : TargetCharacters)
		{
			FEffectEventContext* eec = new FEffectEventContext();
			eec->EventCauserActor = CurrentTurnCharacter;
			eec->EventTargetActor = target;

			UTurnBasedSubsystem* TBS = GetGameInstance()->GetSubsystem<UTurnBasedSubsystem>();
			TBS->SolvePreEvent(eec);
		}
		NextStage = EActionStage::PlaySequence;
	}

	if (ConsumeAfterCurrentAction() == EReservedActionType::ExtraSkill)return;

#define Handle(x) \
CurrentStage = EActionStage::x; \
x(); \
break;

	switch (NextStage)
	{
	case EActionStage::FindNextTurn: {Handle(FindNextTurn); }
	case EActionStage::HandleEffects: {Handle(HandleEffects); }
	case EActionStage::FindDoTDamage: {Handle(FindDoTDamage); }
	case EActionStage::HandleDoTDamage: {Handle(HandleDoTDamage); }
	case EActionStage::HandleCC: {Handle(HandleCC); }
	case EActionStage::SelectSkill: {Handle(SelectSkill); }
	case EActionStage::SelectTarget: {Handle(SelectTarget); }
	case EActionStage::PrePlaySequence:break;
	case EActionStage::PlaySequence:
	{
		CurrentStage = EActionStage::PlaySequence;
		PlaySequence(CurrentSelectedSkillTag, CurrentTurnCharacter, TargetCharacters);
		break;
	}
	case EActionStage::EndTurn: {Handle(EndTurn); }
	case EActionStage::FindDeadCharacter: {Handle(FindDeadCharacter); }
	case EActionStage::HandleDeadCharacter: {Handle(HandleDeadCharacter); }
	case EActionStage::MAX:break;
	default:break;
	}

#undef Handle
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
		NextStage = EActionStage::SelectSkill;
		HandleStageTransition();
	}
}

void ATurnBasedPhaseManager::SelectSkill()
{
	UUW_TBSelect* select = RootWidget->GetSelectWidget();
	select->Activate(CurrentTurnCharacter);

	Camera->FocusSelectSkill(CurrentTurnCharacter);

	// ai 후처리
	//if (CurrentTurnCharacter->IsA<ATurnBasedEnemy>())
	//{
	//	UTurnBasedBehaviorComponent* behavior = CHelpers::GetComponent<UTurnBasedBehaviorComponent>(CurrentTurnCharacter->GetController());
	//	behavior->SetWaitMode();
	//}
}

void ATurnBasedPhaseManager::SelectTarget()
{
	UAbilityComponent* asc = Cast<UAbilityComponent>(CurrentTurnCharacter->GetAbilitySystemComponent());
	CheckTrue_Print(!asc, "asc is nullptr!!");

	FGameplayAbilitySpec* spec = asc->FindAbilitySpecFromSkillTag(CurrentSelectedSkillTag);
	CheckTrue_Print(!spec, "spec is nullptr!!");

	UGA_Skill* skill = Cast<UGA_Skill>(spec->Ability);
	CheckTrue_Print(!skill, "skill cast failed!!");

	//최초 1회 캠위치변경, 테두리 glow를 위한 스텐실변경
	if (TargetCharacters.IsEmpty())
	{
		ClearAvailableTargets();
		TargetGoalCount = skill->GetGoalCount();
		SkillTargetType = skill->GetTargetType();

		uint8 ally = CurrentTurnCharacter->GetGenericTeamId();
		uint8 enemy = CurrentTurnCharacter->GetGenericTeamId() ^ 1;

		//카메라 샷 추가, ai 적용 테스트, 시퀀스 재생 테스트, 다중 타겟 적용

		if(SkillTargetType == ESkillTargetType::Enemy)
		{
			for (auto ch : SpawnedCharacterMap[enemy])
			{
				if (ch->IsDead())continue;
				AvailableTargets.Add(ch);
			}
		}
		else if(SkillTargetType == ESkillTargetType::Ally || SkillTargetType == ESkillTargetType::AllyButSelf)
		{
			for (auto ch : SpawnedCharacterMap[ally])
			{
				if (ch->IsDead())continue;
				AvailableTargets.Add(ch);
			}
			if (SkillTargetType == ESkillTargetType::AllyButSelf)
				AvailableTargets.Remove(CurrentTurnCharacter);
		}
		else if(SkillTargetType == ESkillTargetType::All || SkillTargetType == ESkillTargetType::AllButSelf)
		{
			for (auto tuple : SpawnedCharacterMap)
				for (auto ch : tuple.Value)
				{
					if (ch->IsDead())continue;
					AvailableTargets.Add(ch);
				}
			if (SkillTargetType == ESkillTargetType::AllButSelf)
				AvailableTargets.Remove(CurrentTurnCharacter);
		}
		else if(SkillTargetType == ESkillTargetType::Self)
		{
			AvailableTargets.Add(CurrentTurnCharacter);
		}

		HighlightAvailableTargets();
		Camera->FocusAvailableTargets(AvailableTargets);
	}

}

void ATurnBasedPhaseManager::PlaySequence(FGameplayTag SkillTag, ATurnBasedCharacter* SkillOwner, TArray<ATurnBasedCharacter*> Targets, bool Extra)
{
	UAbilityComponent* asc = Cast<UAbilityComponent>(SkillOwner->GetAbilitySystemComponent());
	TArray<FGameplayAbilitySpec>& abilities = asc->GetActivatableAbilities();
	for (auto& spec : abilities)
	{
		const UGA_Skill* skill = Cast<UGA_Skill>(spec.Ability);
		if (!skill)continue;
		if (skill->GetSkillTag() != SkillTag)continue;
		asc->SetTargets(Targets);
		asc->TryActivateAbility(spec.Handle);
		break;
	}
	if (!Extra)
	{
		// 추가 이벤트 판정
		for(auto target : Targets)
		{
			FEffectEventContext* eec = new FEffectEventContext();
			eec->EventCauserActor = SkillOwner;
			eec->EventTargetActor = target;
			//eec->EventTargetActors.Add(Target);

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
	///*
	//* 승리 후 tps필드로
	//*/

	////필드 정리하기
	//for (auto& tuple : SpawnedCharacterMap)
	//{
	//	for (auto ch : tuple.Value)
	//		ch->Destroy();
	//}
	//Camera->Destroy();
	//SelectWidgetActor->Destroy();
	//SelectTargetCursorActor->Destroy();

	//// ui 팝업에 tps필드 이동을 달아놓자
	//UPhaseTransitionContext* context = NewObject<UPhaseTransitionContext>(this, UPhaseTransitionContext::StaticClass());

	//UDEMOGameInstance* gi = Cast<UDEMOGameInstance>(GetGameInstance());
	//gi->RequestPhaseChange(EGameInstancePhase::TPS, context);

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
	///*
	//* 패배 후 어디로? 메인? 최근 저장된곳?
	//*/

	////필드 정리하기
	//for (auto& tuple : SpawnedCharacterMap)
	//{
	//	for (auto ch : tuple.Value)
	//		ch->Destroy();
	//}
	//Camera->Destroy();
	//SelectWidgetActor->Destroy();
	//SelectTargetCursorActor->Destroy();

	//// ui 팝업에 tps필드 이동을 달아놓자
	//UPhaseTransitionContext* context = NewObject<UPhaseTransitionContext>(this, UPhaseTransitionContext::StaticClass());

	//UDEMOGameInstance* gi = Cast<UDEMOGameInstance>(GetGameInstance());
	//gi->RequestPhaseChange(EGameInstancePhase::TPS, context);
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
		for (auto ch : TargetCharacters)
			context->SkillTargetActors.Add(ch);

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
