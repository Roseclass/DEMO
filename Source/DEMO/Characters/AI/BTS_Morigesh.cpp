#include "Characters/AI/BTS_Morigesh.h"
#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "TurnBasedSubsystem.h"

#include "Characters/AI/BlackboardTypes.h"
#include "Characters/TurnBasedCharacter.h"
#include "Characters/AI/TurnBasedEnemy.h"
#include "Characters/AI/TurnBasedAIController.h"
#include "Characters/AI/TurnBasedBehaviorComponent.h"

#include "GameAbilities/AbilityComponent.h"
#include "GameAbilities/GA_Skill.h"


UBTS_Morigesh::UBTS_Morigesh()
{
	bNotifyBecomeRelevant = 1;

	NodeName = "Morigesh";

	BehaviorType.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_Morigesh, BehaviorType), StaticEnum<EBehaviorType>());
	Data.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_Morigesh, Data), UTurnBasedBlackboardContainer::StaticClass());
}

void UBTS_Morigesh::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	//Init
}

void UBTS_Morigesh::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	CheckTrue(!controller);

	ATurnBasedEnemy* aiPawn = Cast<ATurnBasedEnemy>(controller->GetPawn());
	CheckTrue(!aiPawn);
	CheckTrue(aiPawn->IsDead())

	UTurnBasedBehaviorComponent* behavior = CHelpers::GetComponent<UTurnBasedBehaviorComponent>(controller);
	CheckTrue(!behavior);

	if (behavior->IsWaitMode())
	{
		EvaluateTurnAction(OwnerComp);
		behavior->SetSelectSkillMode();
	}
}

void UBTS_Morigesh::EvaluateTurnAction(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
	if (!blackboardComp)return;

	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	UTurnBasedBehaviorComponent* behavior = CHelpers::GetComponent<UTurnBasedBehaviorComponent>(controller);

	ATurnBasedEnemy* aiPawn = Cast<ATurnBasedEnemy>(controller->GetPawn());
	UAbilityComponent* asc = CHelpers::GetComponent<UAbilityComponent>(aiPawn);
	CheckTrue_Print(!asc, "asc is nullptr");

	// 남은 체력을 기준으로 정리
	UTurnBasedSubsystem* TBS = GetWorld()->GetGameInstance()->GetSubsystem<UTurnBasedSubsystem>();
	TSet<ATurnBasedCharacter*> playerCharacterSet = TBS->GetPlayerCharacters();
	TArray<TTuple<float, ATurnBasedCharacter*>> sort;
	for (auto playerCharacter : playerCharacterSet)
	{
		if (playerCharacter->IsDead())continue;
		UAbilityComponent* pcASC = CHelpers::GetComponent<UAbilityComponent>(playerCharacter);
		sort.Add({ pcASC->GetHealth(), playerCharacter });
	}
	sort.Sort();

	// 사용 가능한 스킬들의 데미지 정리
	FGameplayTag targetTag;
	int32 targetIdx = 0;
	float overDamage = -1e9;
	float cd = 1e9;
	for (const FGameplayTag& tag : aiPawn->GetEquippedSkillTags())
	{
		for (const FGameplayAbilitySpec& spec : asc->GetActivatableAbilities())
		{
			const UGA_Skill* skill = Cast<UGA_Skill>(spec.Ability);
			if (!skill)continue;
			if (skill->GetSkillTag() != tag)continue;

			TArray<float> damages = skill->GetCalculatedDamages(asc->GetPower(), spec.Level);
			float total = 0;
			for (auto i : damages) total += i;

			for (int32 i = 0; i < sort.Num(); i++)
			{
				float cod = total - sort[i].Key;
				if (cod < 0 && cod < overDamage) break;
				if (0 <= cod && 0 <= overDamage && overDamage < cod) continue;
				targetTag = tag;
				targetIdx = i;
				overDamage = cod;
				cd = 1e9; // TODO::쿨타임 계산해서 넣어주기
			}

			break;
		}
	}

	UTurnBasedBlackboardContainer* data = Cast<UTurnBasedBlackboardContainer>(blackboardComp->GetValueAsObject(Data.SelectedKeyName));	

	// 표식이 찍힌 적이 없다면 표식 먼저 찍기
	{
		data->TargetSkillTag = FGameplayTag::RequestGameplayTag("Skill.Morigesh.Mark");
		for (int32 i = 0; i < sort.Num(); i++)
		{
			FGameplayTag markTag = FGameplayTag::RequestGameplayTag("Effect.Debuff.Mark.Morigesh");
			FGameplayTagContainer tags; 
			sort[i].Value->GetAbilitySystemComponent()->GetOwnedGameplayTags(tags);

			// 표식이 찍힌 적이 있다면 스킬사용
			if (tags.HasAnyExact(FGameplayTagContainer(markTag)))
			{
				targetIdx = i;
				data->TargetSkillTag = FGameplayTag::RequestGameplayTag("Skill.Morigesh.DoT");
			}
		}
	}
	data->SkillTargets.Add(sort[targetIdx].Value);

	CLog::Print(targetTag.ToString() + " " + sort[targetIdx].Value->GetName());
}
