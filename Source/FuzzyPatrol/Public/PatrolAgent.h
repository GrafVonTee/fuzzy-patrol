// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StateMachineComponent.h"
#include "DangerDetector.h"
#include "ReceivingComponent.h"
#include "DefaultEnemy.h"
#include "PatrolAgent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnThurstLevelChangedSignature, int32, CurrentThurstLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDetectedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveEndSignature);

UCLASS()
class FUZZYPATROL_API APatrolAgent : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APatrolAgent();


	/** State Machine */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State Machine")
	class UStateMachineComponent* StateMachine;


	/** Static Meshes */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
	class UStaticMeshComponent* HeadSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
	class UStaticMeshComponent* BodyCube;


	/** Speed Section */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float CurrentSpeed = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Speed")
	float RushSpeed = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Speed")
	float QuietSpeed = 200.0f;


	/** Danger Level Section */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Danger Level")
	int32 ObservedDangerLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Danger Level")
	int32 MaximumPermissibleDangerLevel = 1000;


	/** Thurst Level Section */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Thurst")
	class UReceivingComponent* ReceivingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Thurst")
	class UReceivingComponent* ThurstAccumulativeComponent;

	UPROPERTY(BlueprintAssignable)
	FOnThurstLevelChangedSignature OnThurstLevelChangedDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Thurst")
	int32 CurrentThurstLevel = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Thurst")
	int32 MaximumThurstLevel = 300;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Thurst")
	int32 WaitThurstValue = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Thurst")
	int32 RushThurstValue = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Thurst")
	int32 WalkThurstValue = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Thurst")
	int32 AttackThurstValue = 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Thurst")
	float TimeForAccumulation = 1.0f;


	/** Enemy Detecting Level Section */
	UPROPERTY(BlueprintAssignable)
	FOnEnemyDetectedSignature OnEnemyDetectedDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy Detecting")
	class ADefaultEnemy* CurrentDetectedEnemy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Detecting")
	class UPawnSensingComponent* PawnSensing;


	/** Attack Section */
	UPROPERTY()
	FTimerHandle AttackTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackRate = 0.3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	int32 AttackPower = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	float AttackRadius = 100.0f;


	/** Patrol Section */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	TArray<FVector> PatrolLocations;


	/** Moving Section */
	UPROPERTY(BlueprintAssignable)
	FOnMoveEndSignature OnMoveEndDelegate;

	UPROPERTY()
	FTimerHandle MovingTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Moving")
	float MovingCheckRate = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	FVector TargetLocation;

	
	/** Death Section */
	UPROPERTY(BlueprintAssignable)
	FOnDeathSignature OnDeathDelegate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	/** State Machine Section*/
	UFUNCTION(BlueprintCallable, Category = "State Machine")
	void OnInitState(const FGameplayTag& State);


	/** Speed Section */
	UFUNCTION(BlueprintCallable, Category = "Speed")
	void SetSpeed(float NewSpeed);


	/** Danger Level Section */
	UFUNCTION(BlueprintCallable, Category = "Danger Level")
	void UpdateObservedDangerLevel(int32 CurrentLevel);


	/** Thurst Level Section */
	UFUNCTION(BlueprintCallable, Category = "Thurst")
	void QuenchThurst(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Thurst")
	void RaiseThurst(int32 Value);


	/** Enemy Detecting Section */
	UFUNCTION(BlueprintCallable, Category = "Enemy Detecting")
	void SetDetectedEnemy(ADefaultEnemy* Enemy);

	UFUNCTION(BlueprintCallable, Category = "Enemy Detecting")
	bool HasDetectedEnemy();
	
	UFUNCTION(BlueprintCallable, Category = "Enemy Detecting")
	void EnableDetecting();
	
	UFUNCTION(BlueprintCallable, Category = "Enemy Detecting")
	void DisableDetecting();

	UFUNCTION(BlueprintCallable, Category = "Enemy Detecting")
	void SetDetectedEnemyAsTarget();


	/** Enemy Attack */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	bool DetectedEnemyIsNear();

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void AttackDetectedEnemy();


	/** Patrol Section */
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	bool IsNearTargetLocation();

	UFUNCTION(BlueprintCallable, Category = "Patrol")
	void SetNewPatrolTargetLocation();


	/** Attack Section */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void BeginAttack();

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void StopAttack();


	/** Moving Section */
	UFUNCTION(BlueprintCallable, Category = "Moving")
	void ResetMovingTimer();

	UFUNCTION(BlueprintCallable, Category = "Moving")
	void StopMovingTimer();

	UFUNCTION(BlueprintCallable, BLueprintImplementableEvent, Category = "Moving")
	void GoToTargetLocation();

	UFUNCTION(BlueprintCallable, Category = "Moving")
	void MovingDone();


	/** Tea Receiving Section */
	UFUNCTION(BlueprintCallable, BLueprintImplementableEvent, Category = "Tea")
	void SetTeaReceiverAsTargetLocation();


	/** Just die lol */
	UFUNCTION(BlueprintCallable)
	void Die();

};
