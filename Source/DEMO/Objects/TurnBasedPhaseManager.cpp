#include "Objects/TurnBasedPhaseManager.h"
#include "Global.h"

#include "GameFramework/PlayerController.h"

#include "DEMOGameInstance.h"
#include "SaveLoadSubsystem.h"

#include "Characters/AI/TurnBasedAIController.h"
#include "Characters/AI/TurnBasedBehaviorComponent.h"
#include "Characters/AI/TurnBasedEnemy.h"

#include "Characters/TurnBasedCharacter.h"
#include "Characters/TurnBasedCharacterData.h"

#include "Datas/TurnBasedDataTypes.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/AttributeSet_Character.h"
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
	asc->OnSkillEnd.AddUFunction(this, "EndTurn");
	asc->OnDeadSequenceEnd.AddUFunction(this, "HandleDeadCharacter");
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

void ATurnBasedPhaseManager::ReduceCooldown()
{
	//
	// TODO::현재 턴인 캐릭터만 줄이는데 혹시라도 수정이 필요할수도?
	//
		
	//공용 쿨타임 태그 중복 적용 방지
	CheckTrue(CurrentTurnCharacter->IsDead());
	UAbilityComponent* asc = Cast<UAbilityComponent>(CurrentTurnCharacter->GetAbilitySystemComponent());
	CheckTrue(!asc);

	TSet<FGameplayTag> tagSet;

	TArray<FGameplayAbilitySpecHandle> specHandles;
	asc->GetAllAbilities(specHandles);
	for (auto& specHandle : specHandles)
	{
		FGameplayAbilitySpec* spec = asc->FindAbilitySpecFromHandle(specHandle);
		if (!spec->Ability->GetCooldownTags() ||
			spec->Ability->GetCooldownTags()->IsEmpty())continue;
		for (auto tag : *spec->Ability->GetCooldownTags())
			tagSet.Add(tag);
	}

	CheckTrue(tagSet.IsEmpty());

	FGameplayTagContainer tags;
	for (auto tag : tagSet) tags.AddTag(tag);
	FGameplayEffectQuery const query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(tags);
	asc->RemoveActiveEffects(query, 1);
}

void ATurnBasedPhaseManager::HandleDoTDamage()
{
	//
	// TODO::
	//

	CheckTrue(CurrentTurnCharacter->IsDead());
	UAbilityComponent* asc = Cast<UAbilityComponent>(CurrentTurnCharacter->GetAbilitySystemComponent());
	CheckTrue(!asc);
	
	TArray<FGameplayEffectSpec> specs;
	asc->GetAllActiveGameplayEffectSpecs(specs);
	for (auto& spec : specs)
	{
		if (!spec.DynamicGrantedTags.HasTagExact(FGameplayTag::RequestGameplayTag("Effect.Damage.DoT")))continue;
		const FDamageEffectContext* dotContext = static_cast<const FDamageEffectContext*>(spec.GetEffectContext().Get());
		if (!dotContext)continue;

		UAbilityComponent* instigatorASC = Cast<UAbilityComponent>(
			Cast<ATurnBasedCharacter>(spec.GetEffectContext().GetInstigator())
			->GetAbilitySystemComponent());
		float damage = dotContext->CalculatedDamage;

		FDamageEffectContext* context = new FDamageEffectContext();
		context->CalculatedDamage = damage;
		context->Location = CurrentTurnCharacter->GetActorLocation();

		FGameplayEffectContextHandle EffectContextHandle = FGameplayEffectContextHandle(context);
		EffectContextHandle.AddInstigator(spec.GetEffectContext().GetInstigator(), this);


		FGameplayEffectSpecHandle EffectSpecHandle = instigatorASC->MakeOutgoingSpec(UGE_InstantDamage::StaticClass(), 1, EffectContextHandle);
		EffectSpecHandle.Data->SetByCallerNameMagnitudes.Add(FName("calculatedDamage"), -damage);

		instigatorASC->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data, asc);

		FGameplayTagContainer tags;
		tags.AddTag(FGameplayTag::RequestGameplayTag("Effect.Damage.DoT"));
		FGameplayEffectQuery const query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(tags);
		asc->RemoveActiveEffects(query, 1);
	}
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
	else if (payload->Type == EReservedActionType::ExtraHit)
	{
		CLog::Print("ATurnBasedPhaseManager::ConsumeAfterCurrentAction() Type is ExtraTurn", -1, 10, FColor::Red);
		return EReservedActionType::ExtraHit;
	}

	return EReservedActionType::MAX;
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
		if (payload->Target == EReservedActionTarget::EventCauser)
			CurrentTurnCharacter = Cast<ATurnBasedCharacter>(context.EventCauserActor.Get());
		else if (payload->Target == EReservedActionTarget::EventTarget)
			CurrentTurnCharacter = Cast<ATurnBasedCharacter>(context.EventTargetActor.Get());
		else if (payload->Target == EReservedActionTarget::RuleSource)
			CurrentTurnCharacter = Cast<ATurnBasedCharacter>(context.RuleSourceActor.Get());

		if (!CurrentTurnCharacter)
		{
			CLog::Print("ATurnBasedPhaseManager::ConsumeStartOfNextTurn() CurrentTurnCharacter is nullptr", -1, 10, FColor::Red);
			return EReservedActionType::MAX;
		}
	}
	else if (payload->Type == EReservedActionType::ExtraHit)
	{

	}

	return payload->Type;
}

EReservedActionType ATurnBasedPhaseManager::ConsumeEndOfTurn()
{
	return EReservedActionType::MAX;
}

void ATurnBasedPhaseManager::HandleStageTransition()
{
	EReservedActionType reservation = ConsumeAfterCurrentAction();

	// 예약된거 없음
	if (reservation == EReservedActionType::MAX)
	{

	}
	else if (reservation == EReservedActionType::ExtraHit)
	{
		// TODO::
	}



	if (NextStage == EActionStage::FindNextTurn)
	{

	}
	else if (NextStage == EActionStage::FocusSelect)
	{

	}
	else if (NextStage == EActionStage::PlaySequence)
	{

	}
	else if (NextStage == EActionStage::EndTurn)
	{
		reservation = ConsumeEndOfTurn();
		// 예약된거 없음
		if (reservation == EReservedActionType::MAX)
		{

		}
		else if (reservation == EReservedActionType::ExtraHit)
		{
			// TODO::
		}
	}
	else if (NextStage == EActionStage::FindDeadCharacter)
	{

	}
	else if (NextStage == EActionStage::HandleDeadCharacter)
	{

	}
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
			UAbilitySystemComponent* asc = CurrentTurnCharacter->GetAbilitySystemComponent();
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
		// 뭐 딱히 할일은 없는거 같습니다만..
	}
	else if (reservation == EReservedActionType::ExtraHit)
	{
		// TODO::
	}

	{
		ReduceCooldown();
		HandleDoTDamage();
	}

	//FocusSelect();
	HandleStageTransition();
}

void ATurnBasedPhaseManager::FocusSelect()
{
	Camera->FocusSelectSkill(CurrentTurnCharacter);
	SelectWidgetActor->SetActorTransform(CurrentTurnCharacter->GetActorTransform());
	//SelectWidgetActor->SetWidgetRelativeTransform(CurrentTurnCharacter->GetSelectSkillRelativeTransform());

	UUW_TurnBased_Select* select = Cast<UUW_TurnBased_Select>(SelectWidgetActor->GetWidgetObject());
	int32 targetID = CurrentTurnCharacter->GetGenericTeamId() == TEAMID_PLAYER ? TEAMID_ENEMY : TEAMID_PLAYER;

	select->Activate(CurrentTurnCharacter, LocationArray[targetID]);
	SelectWidgetActor->Show();
	SelectTargetCursorActor->Show();

	if (CurrentTurnCharacter->IsA<ATurnBasedEnemy>())
	{
		UTurnBasedBehaviorComponent* behavior = CHelpers::GetComponent<UTurnBasedBehaviorComponent>(CurrentTurnCharacter->GetController());
		behavior->SetWaitMode();
	}
}

void ATurnBasedPhaseManager::PlaySequence()
{
	UAbilityComponent* asc = Cast<UAbilityComponent>(CurrentTurnCharacter->GetAbilitySystemComponent());
	TArray<FGameplayAbilitySpec>& abilities = asc->GetActivatableAbilities();
	for (auto& spec : abilities)
	{
		if (!spec.Ability->AbilityTags.HasTagExact(CurrentSelectedSkillTag))
			continue;
		asc->SetTarget(TargetCharacter);
		asc->TryActivateAbility(spec.Handle);
		break;
	}
}

void ATurnBasedPhaseManager::EndTurn()
{
	if (CurrentTurnCharacter->IsA<ATurnBasedEnemy>())
	{
		UTurnBasedBehaviorComponent* behavior = CHelpers::GetComponent<UTurnBasedBehaviorComponent>(CurrentTurnCharacter->GetController());
		behavior->SetNotMyTurnMode();
	}

	//FindDeadCharacter();
	HandleStageTransition();
}

void ATurnBasedPhaseManager::ConfirmSelect(FGameplayTag InSkillTag, ATurnBasedCharacter* InTarget)
{
	SelectWidgetActor->Hide();
	SelectTargetCursorActor->Hide();
	CurrentSelectedSkillTag = InSkillTag;
	TargetCharacter = InTarget;

	//PlaySequence();
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
	//HandleDeadCharacter();
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
		//FindNextTurn();
		HandleStageTransition();
	}
	else
	{
		ATurnBasedCharacter* ch = PendingDeadArray[PendingDeadArray.Num() - 1];
		PendingDeadArray.Remove(ch);
		UAbilityComponent* asc = Cast<UAbilityComponent>(ch->GetAbilitySystemComponent());
		HandledDeadSet.Add(ch);
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

void ATurnBasedPhaseManager::ApplyCameraMove(const FCameraMoveEffectContext* InEffectContext)
{
	Camera->ApplyCameraMove(InEffectContext);
}

void ATurnBasedPhaseManager::ReserveAction(const FPayloadContext* InEffectContext)
{
	UDA_ActionReservation* da = Cast<UDA_ActionReservation>(InEffectContext->Payload);	
	CheckTrue_Print(!da, "da is nullptr");

	ReservedActions.FindOrAdd(da->Timing).Add(*InEffectContext);
}

const TSet<ATurnBasedCharacter*>& ATurnBasedPhaseManager::GetPlayerCharacters() const
{
	return SpawnedCharacterMap[TEAMID_PLAYER];
}

const TSet<ATurnBasedCharacter*>& ATurnBasedPhaseManager::GetEnemyCharacters() const
{
	return SpawnedCharacterMap[TEAMID_ENEMY];
}
