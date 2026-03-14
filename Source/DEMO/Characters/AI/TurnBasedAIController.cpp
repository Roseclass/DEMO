#include "Characters/AI/TurnBasedAIController.h"
#include "Global.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Characters/TurnBasedCharacter.h"
#include "Characters/AI/TurnBasedEnemy.h"
#include "Characters/AI/TurnBasedBehaviorComponent.h"

ATurnBasedAIController::ATurnBasedAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateActorComponent(this, &Behavior, "Behavior");
}

void ATurnBasedAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ATurnBasedAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATurnBasedAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	bPossess = 1;
	OwnerEnemy = Cast<ATurnBasedEnemy>(InPawn);

	if (bInit && OwnerEnemy)InitBT();
	CheckTrue_Print(!OwnerEnemy, "owner enemy cast failed");
}

void ATurnBasedAIController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ATurnBasedAIController::InitBT()
{
	CheckTrue(bInitBT);
	bInitBT = 1;

	UBlackboardComponent* bb = Blackboard.Get();
	UseBlackboard(BehaviorTree->BlackboardAsset, bb);

	SetGenericTeamId(OwnerEnemy->GetGenericTeamId());
	RunBehaviorTree(BehaviorTree);

	Behavior->SetBlackboard(Blackboard);
}

void ATurnBasedAIController::Init(UBehaviorTree* InBehaviorTree)
{
	CheckTrue(bInit);
	bInit = 1;
	BehaviorTree = InBehaviorTree;

	if (bPossess && BehaviorTree)InitBT();
	CheckTrue_Print(!BehaviorTree, "BehaviorTree is nullptr");
}

void ATurnBasedAIController::StopLogic(FString Reason)
{
	GetBrainComponent()->StopLogic(Reason);
}

void ATurnBasedAIController::RestartLogic()
{
	GetBrainComponent()->RestartLogic();
}
