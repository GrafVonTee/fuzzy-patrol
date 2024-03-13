// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Containers/Array.h"
#include "DefaultEnemy.generated.h"

UCLASS()
class FUZZYPATROL_API ADefaultEnemy : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADefaultEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
	class UStaticMeshComponent* TopCone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Meshes")
	class UStaticMeshComponent* BottomCone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	class UCapsuleComponent* CapsuleComponent;

	/** Random Patrol Section */
	UPROPERTY()
	FTimerHandle RandomPatrolTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PatrolPeriod = 1.5f;

	/** Health Section */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	int32 CurrentHealth = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	int32 MaximumHealth = 3;

	/** Danger Management Section*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Danger Level")
	int32 DangerRaisingValue = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Danger Level")
	int32 RewardByDeath = 12;

	UPROPERTY()
	FTimerHandle DangerRaisingTimerHandle;

	UPROPERTY()
	class ADangerDetector* DangerDetector;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Random Patrol Section */
	UFUNCTION(BlueprintCallable)
	void ResetRandomPatrolTimer();

	UFUNCTION(BlueprintCallable)
	void ClearRandomPatrolTimer();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Moving")
	void MoveToRandom();

	/** Danger Management Section */
	UFUNCTION(BlueprintCallable)
	void ResetDangerRaisingTimer();

	UFUNCTION(BlueprintCallable)
	void ClearDangerRaisingTimer();

	UFUNCTION(BlueprintCallable)
	void RaiseDanger();

	UFUNCTION(BlueprintCallable)
	void LowerDangerAsReward();

	/** Enemy Life Section */
	UFUNCTION(BlueprintCallable, Category = "Life")
	void ReceiveDamage(int32 DamagePoints);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Life")
	void PlayAnimationReceivingDamage();

	UFUNCTION(BlueprintCallable, Category = "Life")
	void Die();

};
