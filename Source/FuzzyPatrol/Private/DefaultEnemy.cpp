// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultEnemy.h"
#include "DangerDetector.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ADefaultEnemy::ADefaultEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create and add the capsule component
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->InitCapsuleSize(40.0f, 30.0f);

	TopCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TopCone"));
	TopCone->SetupAttachment(RootComponent);

	BottomCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BottomCone"));
	BottomCone->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMeshAsset(TEXT("/Engine/BasicShapes/Cone.Cone"));

	if (ConeMeshAsset.Succeeded())
	{
		TopCone->SetStaticMesh(ConeMeshAsset.Object);
		TopCone->SetRelativeLocation(FVector(0, 0, 20));
		TopCone->SetRelativeScale3D(FVector(0.5, 0.5, 0.4));
		TopCone->SetRelativeRotation(FRotator(0, 0, 0));
		TopCone->SetCanEverAffectNavigation(false);

		BottomCone->SetStaticMesh(ConeMeshAsset.Object);
		BottomCone->SetRelativeLocation(FVector(0, 0, -20));
		BottomCone->SetRelativeScale3D(FVector(0.5, 0.5, 0.4));
		BottomCone->SetRelativeRotation(FRotator(180, 0, 0));
		BottomCone->SetCanEverAffectNavigation(false);

	}

	CurrentHealth = MaximumHealth;

}

// Called when the game starts or when spawned
void ADefaultEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ADangerDetector::StaticClass());
	DangerDetector = Cast<ADangerDetector>(FoundActor);

	ResetRandomPatrolTimer();
	ResetDangerRaisingTimer();
	MoveToRandom();

}

// Called every frame
void ADefaultEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADefaultEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ADefaultEnemy::ResetRandomPatrolTimer()
{
	GetWorldTimerManager().SetTimer(RandomPatrolTimerHandle, this, &ADefaultEnemy::MoveToRandom, PatrolPeriod, true);

}

void ADefaultEnemy::ClearRandomPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(RandomPatrolTimerHandle);

}

void ADefaultEnemy::ResetDangerRaisingTimer()
{
	GetWorldTimerManager().SetTimer(DangerRaisingTimerHandle, this, &ADefaultEnemy::RaiseDanger, 1, true);

}

void ADefaultEnemy::ClearDangerRaisingTimer()
{
	GetWorldTimerManager().ClearTimer(DangerRaisingTimerHandle);

}

void ADefaultEnemy::RaiseDanger()
{
	DangerDetector->RaiseLevel(DangerRaisingValue);

}

void ADefaultEnemy::LowerDangerAsReward()
{
	DangerDetector->LowerLevel(RewardByDeath);

}

void ADefaultEnemy::ReceiveDamage(int32 DamagePoints)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - DamagePoints, 0, MaximumHealth);
	
	if (CurrentHealth == 0)
	{
		Die();

	}

}

void ADefaultEnemy::Die()
{
	ClearRandomPatrolTimer();
	ClearDangerRaisingTimer();

	LowerDangerAsReward();

	Destroy();

}

