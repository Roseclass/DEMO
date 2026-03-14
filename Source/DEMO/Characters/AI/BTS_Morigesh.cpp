#include "Characters/AI/BTS_Morigesh.h"
#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "TurnBasedSubsystem.h"

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

	SelectedSkillTag.AddStringFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_Morigesh, SelectedSkillTag));
	TargetSkillTag.AddStringFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_Morigesh, TargetSkillTag));

	SelectedCharacter.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_Morigesh, SelectedCharacter), ATurnBasedCharacter::StaticClass());
	TargetCharacter.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_Morigesh, TargetCharacter), ATurnBasedCharacter::StaticClass());
}

void UBTS_Morigesh::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	//Init
}

void UBTS_Morigesh::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
	CheckTrue(!blackboardComp);

	AAIController* controller = Cast<AAIController>(OwnerComp.GetOwner());
	CheckTrue(!controller);

	UTurnBasedBehaviorComponent* behavior = CHelpers::GetComponent<UTurnBasedBehaviorComponent>(controller);
	CheckTrue(!behavior);
	CheckTrue(behavior->IsNotMyTurnMode());
	CheckTrue(behavior->IsSkillSequenceMode());

	ATurnBasedEnemy* aiPawn = Cast<ATurnBasedEnemy>(controller->GetPawn());
	CheckTrue(!aiPawn);
	UAbilityComponent* asc = CHelpers::GetComponent<UAbilityComponent>(aiPawn);
	CheckTrue(!asc);

	CheckTrue(asc->GetHealth() < 1e-9);

	if (behavior->IsWaitMode())
	{
		EvaluateTurnAction(OwnerComp);
		behavior->SetSelectSkillMode();
	}
	else if (behavior->IsSelectSkillMode())
	{
		FString currentTag = blackboardComp->GetValueAsString(SelectedSkillTag.SelectedKeyName);
		FString targetTag = blackboardComp->GetValueAsString(TargetSkillTag.SelectedKeyName);

		if (currentTag == targetTag)
		{
			behavior->SetSelectTargetMode();
			FTimerDelegate func =
				FTimerDelegate::CreateLambda([&]()
					{
					});
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, func, 0.5, false);
		}
	}
	else if (behavior->IsSelectTargetMode())
	{
		ATurnBasedCharacter* currentTarget = Cast<ATurnBasedCharacter>(blackboardComp->GetValueAsObject(SelectedCharacter.SelectedKeyName));
		ATurnBasedCharacter* targetTarget = Cast<ATurnBasedCharacter>(blackboardComp->GetValueAsObject(TargetCharacter.SelectedKeyName));

		if (currentTarget == targetTarget)
		{
			behavior->SetSkillSequenceMode();
			aiPawn->Confirm();
			FTimerDelegate func =
				FTimerDelegate::CreateLambda([&]()
					{
					});
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, func, 0.5, false);
		}
	}

	/*
	* selectskill만듦
	* selecttarget위해서 widget 통합이 필요함
	* 위젯 통합하고 시퀀스 재생되는거 확인하고 ai 돌아가는거 확인하고 preturn postturn 체크 넣고 전투 종료 체크하고
	* tps필드 ai만들고 시스템관련은 그만해도될거같음
	* 
	* 별개로 셀렉트 스킬, 타겟을 wasd로 컨트롤할수있게변경하자
	* ws는 스킬변경 ad는 타겟변경
	* 시점은 오른쪽어깨 tps시점으로
	* 
	* 비헤이비어 컴포넌트에서 턴, 턴아님, 사망, SelectSkill, SelectTarget, PlaySequence
	* 상태를 만들어두고
	* 하나씩 돌아가게 만들자
	* 타겟을 옮기거나 스킬을 변경할때 0.2초의 텀을 두고 플레이어가 컨트롤하는듯한 느낌주자
	* 
	* type이 바뀌면  하위 트리로 이동
	* wait - 대기( 사망인경우도있음 )
	* selectskill - 현재 select된 skilltag == 목표 skilltag 될때까지 ui 이동
	* 일치하면 그때 selecttarget으로
	* selecttarget - 마찬가지로 현재 target == 목표 target 될때까지 ui 이동
	* 일치하면 playsequence로
	* playsequence - 시퀀스 플레이 후 턴 종료 및 wait 모드로 전환
	* 
	* task 목록
	* changeskill - ui 이동 (가까운 방향으로 or 왼쪽 고정)
	* changetarget - ui 이동 (가까운 방향으로 or 아래 고정)
	* playskill - 블랙보드에 설정된 스킬 태그 가져와서 재생
	*/
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

	blackboardComp->SetValueAsString(SelectedSkillTag.SelectedKeyName, aiPawn->GetCurrentSkillTag().ToString());
	blackboardComp->SetValueAsObject(SelectedCharacter.SelectedKeyName, aiPawn->GetCurrentTarget());

	// 표식이 찍힌 적이 없다면 표식 먼저 찍기
	{
		blackboardComp->SetValueAsString(TargetSkillTag.SelectedKeyName, FGameplayTag::RequestGameplayTag("Skill.Morigesh.Attack").ToString());
		//blackboardComp->SetValueAsString(TargetSkillTag.SelectedKeyName, FGameplayTag::RequestGameplayTag("Skill.Morigesh.Skill1").ToString());
		//for (int32 i = 0; i < sort.Num(); i++)
		//{
		//	FGameplayTag markTag = FGameplayTag::RequestGameplayTag("Effect.Mark.Morigesh");
		//	FGameplayTagContainer tags;
		//	sort[i].Value->GetAbilitySystemComponent()->GetOwnedGameplayTags(tags);
		//	if (tags.HasAnyExact(FGameplayTagContainer(markTag)))
		//	{
		//		targetIdx = i;
		//		blackboardComp->SetValueAsString(TargetSkillTag.SelectedKeyName, FGameplayTag::RequestGameplayTag("Skill.Morigesh.Skill0").ToString());
		//	}
		//}
	}
	blackboardComp->SetValueAsObject(TargetCharacter.SelectedKeyName, sort[targetIdx].Value);

	CLog::Print(targetTag.ToString() + " " + sort[targetIdx].Value->GetName());
}
