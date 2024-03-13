// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolAgent.h"
#include "AgentGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameplayTagContainer.h"
#include "Math/UnrealMathUtility.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APatrolAgent::APatrolAgent()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	StateMachine = CreateDefaultSubobject<UStateMachineComponent>("StateMachine");
	StateMachine->InitialStateTag = AgentGameplayTags::NonTeaState_NonBattleState_Wait;
	StateMachine->StateHistoryLength = 5;
	StateMachine->InitStateDelegate.AddUniqueDynamic(this, &APatrolAgent::OnInitState);

	ReceivingComponent = CreateDefaultSubobject<UReceivingComponent>("ReceivingComponent");
	ReceivingComponent->OnValueReceivedDelegate.AddUniqueDynamic(this, &APatrolAgent::QuenchThurst);

	ThurstAccumulativeComponent = CreateDefaultSubobject<UReceivingComponent>("ThurstAccumulativeComponent");
	ThurstAccumulativeComponent->OnValueReceivedDelegate.AddUniqueDynamic(this, &APatrolAgent::RaiseThurst);

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensing");
	PawnSensing->bOnlySensePlayers = false;
	PawnSensing->bSeePawns = true;

	//bUseControllerRotationPitch = false;
	//bUseControllerRotationYaw = false;
	//bUseControllerRotationRoll = false;

	HeadSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadSphere"));
	HeadSphere->SetupAttachment(GetMesh());

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMeshAsset.Succeeded())
	{
		HeadSphere->SetStaticMesh(SphereMeshAsset.Object);
		HeadSphere->SetRelativeLocation(FVector(0, 0, 60));
		HeadSphere->SetRelativeScale3D(FVector(0.5, 0.5, 0.5));
		HeadSphere->SetRelativeRotation(FRotator(0, 0, 0));
		HeadSphere->SetCanEverAffectNavigation(false);

	}

	BodyCube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyCube"));
	BodyCube->SetupAttachment(GetMesh());

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMeshAsset.Succeeded())
	{
		BodyCube->SetStaticMesh(CubeMeshAsset.Object);
		BodyCube->SetRelativeLocation(FVector(0, 0, -10));
		BodyCube->SetRelativeScale3D(FVector(0.4, 0.4, 1.2));
		BodyCube->SetRelativeRotation(FRotator(0, 0, 0));
		BodyCube->SetCanEverAffectNavigation(false);

	}

}

// Called when the game starts or when spawned
void APatrolAgent::BeginPlay()
{
	Super::BeginPlay();
	
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ADangerDetector::StaticClass());
	ADangerDetector* DangerDetector = Cast<ADangerDetector>(FoundActor);
	DangerDetector->OnLevelChangedDelegate.AddUniqueDynamic(this,
		&APatrolAgent::UpdateObservedDangerLevel);

	TargetLocation = GetActorLocation();

}

// Called every frame
void APatrolAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APatrolAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APatrolAgent::OnInitState(const FGameplayTag& State)
{
	ThurstAccumulativeComponent->DisableReceiving();

	if (State.MatchesTag(AgentGameplayTags::TeaState))
	{
		DisableDetecting();
		SetTeaReceiverAsTargetLocation();
	}
	else
	{
		EnableDetecting();
	}

	if (State.MatchesTag(AgentGameplayTags::NonTeaState_BattleState))
	{
		SetDetectedEnemyAsTarget();
	}
	else if (State.MatchesTag(AgentGameplayTags::NonTeaState_NonBattleState))
	{
		SetNewPatrolTargetLocation();
	}

	if (State.ToString().Contains("Wait")
		|| State.MatchesTag(AgentGameplayTags::NonTeaState_BattleState_Attack))
	{
		ThurstAccumulativeComponent->EnableReceiving(TimeForAccumulation, WaitThurstValue);
		SetSpeed(0);
		StopMovingTimer();
	}
	else
	{
		ResetMovingTimer();
	}

	if (State.MatchesTag(AgentGameplayTags::TeaState_Wait))
	{
		// nothing here

	}
	else if (State.MatchesTag(AgentGameplayTags::TeaState_Rush))
	{
		ThurstAccumulativeComponent->EnableReceiving(TimeForAccumulation, RushThurstValue);
		SetSpeed(RushSpeed);

	}
	else if (State.MatchesTag(AgentGameplayTags::TeaState_Walk))
	{
		ThurstAccumulativeComponent->EnableReceiving(TimeForAccumulation, WalkThurstValue);
		SetSpeed(QuietSpeed);

	}
	else if (State.MatchesTag(AgentGameplayTags::NonTeaState_BattleState_Chase))
	{
		ThurstAccumulativeComponent->EnableReceiving(TimeForAccumulation, RushThurstValue);
		SetSpeed(RushSpeed);

	}
	else if (State.MatchesTag(AgentGameplayTags::NonTeaState_BattleState_Attack))
	{
		AttackDetectedEnemy();

	}
	else if (State.MatchesTag(AgentGameplayTags::NonTeaState_NonBattleState_Wait))
	{
		// nothing is here

	}
	else if (State.MatchesTag(AgentGameplayTags::NonTeaState_NonBattleState_Patrol))
	{
		ThurstAccumulativeComponent->EnableReceiving(TimeForAccumulation, WalkThurstValue);
		SetSpeed(QuietSpeed);

	}
	else if (State.MatchesTag(AgentGameplayTags::NonTeaState_NonBattleState_Rush))
	{
		ThurstAccumulativeComponent->EnableReceiving(TimeForAccumulation, RushThurstValue);
		SetSpeed(RushSpeed);

	}
	else
	{
		return;
	}

}

void APatrolAgent::SetSpeed(float NewSpeed)
{
	CurrentSpeed = NewSpeed;
	GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;

}

void APatrolAgent::UpdateObservedDangerLevel(int32 CurrentLevel)
{
	ObservedDangerLevel = FMath::Clamp(CurrentLevel, 0, MaximumPermissibleDangerLevel);

	if (ObservedDangerLevel == MaximumPermissibleDangerLevel)
	{
		Die();

	}

}

void APatrolAgent::QuenchThurst(int32 Value)
{
	CurrentThurstLevel = FMath::Clamp(CurrentThurstLevel - Value, 0, MaximumThurstLevel);

	if (OnThurstLevelChangedDelegate.IsBound())
	{
		OnThurstLevelChangedDelegate.Broadcast(CurrentThurstLevel);

	}

}

void APatrolAgent::RaiseThurst(int32 Value)
{
	CurrentThurstLevel = FMath::Clamp(CurrentThurstLevel + Value, 0, MaximumThurstLevel);

	if (OnThurstLevelChangedDelegate.IsBound())
	{
		OnThurstLevelChangedDelegate.Broadcast(CurrentThurstLevel);

	}

	if (CurrentThurstLevel == MaximumThurstLevel)
	{
		Die();

	}

}

void APatrolAgent::SetDetectedEnemy(ADefaultEnemy* Enemy)
{
	if (IsValid(CurrentDetectedEnemy))
	{
		return;
	}

	CurrentDetectedEnemy = Enemy;

	if (OnEnemyDetectedDelegate.IsBound())
	{
		OnEnemyDetectedDelegate.Broadcast();

	}
}

bool APatrolAgent::HasDetectedEnemy()
{
	if ((CurrentDetectedEnemy != nullptr) && (IsValid(CurrentDetectedEnemy)))
	{
		return true;
	}
	CurrentDetectedEnemy = nullptr;

	return false;
}

void APatrolAgent::EnableDetecting()
{
	PawnSensing->SetSensingUpdatesEnabled(true);

}

void APatrolAgent::DisableDetecting()
{
	PawnSensing->SetSensingUpdatesEnabled(false);
	CurrentDetectedEnemy = nullptr;

}

void APatrolAgent::SetDetectedEnemyAsTarget()
{
	if (HasDetectedEnemy())
	{
		TargetLocation = CurrentDetectedEnemy->GetActorLocation();
	}
}

bool APatrolAgent::DetectedEnemyIsNear()
{
	if (!HasDetectedEnemy())
	{
		return false;
	}

	return GetActorLocation().Equals(CurrentDetectedEnemy->GetActorLocation(), AttackRadius);
}

void APatrolAgent::AttackDetectedEnemy()
{
	if (HasDetectedEnemy() && DetectedEnemyIsNear())
	{
		CurrentDetectedEnemy->ReceiveDamage(AttackPower);
	}
}

bool APatrolAgent::IsNearTargetLocation()
{
	const float Epsilon = 10;
	return GetActorLocation().Equals(TargetLocation, Epsilon);
}

void APatrolAgent::SetNewPatrolTargetLocation()
{
	while (PatrolLocations.Num() > 0)
	{
		int32 TargetLocationIndex = UKismetMathLibrary::RandomIntegerInRange(0, PatrolLocations.Num() - 1);
		TargetLocation = PatrolLocations[TargetLocationIndex];

		if (!IsNearTargetLocation())
		{
			break;
		}
	}
}

void APatrolAgent::ResetMovingTimer()
{
	GetWorldTimerManager().SetTimer(MovingTimer, this, &APatrolAgent::GoToTargetLocation, MovingCheckRate, true);

}

void APatrolAgent::StopMovingTimer()
{
	GetWorldTimerManager().ClearTimer(MovingTimer);

}

void APatrolAgent::MovingDone()
{
	if (StateMachine->StateTag.MatchesTag(AgentGameplayTags::NonTeaState_NonBattleState))
	{
		SetNewPatrolTargetLocation();
	}
	else if (StateMachine->StateTag.MatchesTag(AgentGameplayTags::NonTeaState_BattleState))
	{
		SetDetectedEnemyAsTarget();
	}

	if (OnMoveEndDelegate.IsBound())
	{
		OnMoveEndDelegate.Broadcast();
	}
}

void APatrolAgent::Die()
{
	if (OnDeathDelegate.IsBound())
	{
		OnDeathDelegate.Broadcast();
	}

	Destroy();

}

