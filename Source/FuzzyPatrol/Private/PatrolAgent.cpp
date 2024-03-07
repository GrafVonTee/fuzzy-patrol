// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolAgent.h"
#include "StateMachineComponent.h"
#include "AgentGameplayTags.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
APatrolAgent::APatrolAgent()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StateMachine = CreateDefaultSubobject<UStateMachineComponent>("StateMachine");
	StateMachine->InitialStateTag = AgentGameplayTags::NonTeaState_NonBattleState_Wait;
	StateMachine->StateTag = StateMachine->InitialStateTag;
	StateMachine->StateHistoryLength = 5;

	ReceivingComponent = CreateDefaultSubobject<UReceivingComponent>("ReceivingComponent");
	ReceivingComponent->OnValueReceivedDelegate.AddUniqueDynamic(this, &APatrolAgent::QuenchThurst);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

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

void APatrolAgent::RaiseDetectingLevel(int32 Value)
{
	CurrentDetectingLevel = FMath::Clamp(CurrentDetectingLevel + Value, 0, MaximumDetectingLevel);

	if (OnDetectingLevelChangedDelegate.IsBound())
	{
		OnDetectingLevelChangedDelegate.Broadcast(CurrentDetectingLevel);

	}

}

void APatrolAgent::ResetDetectingLevel()
{
	CurrentDetectingLevel = 0;

	if (OnDetectingLevelChangedDelegate.IsBound())
	{
		OnDetectingLevelChangedDelegate.Broadcast(CurrentDetectingLevel);

	}

}

void APatrolAgent::Die()
{
	Destroy();

}

