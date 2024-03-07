// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DangerDetector.h"
#include "ReceivingComponent.h"
#include "PatrolAgent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnThurstLevelChangedSignature, int32, CurrentThurstLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDetectingLevelChangedSignature, int32, CurrentDetectingLevel);

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
	float CurrentSpeed = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Speed")
	float RushSpeed = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Speed")
	float QuietSpeed = 150.0f;

	/** Danger Level Section */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Danger Level")
	int32 ObservedDangerLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Danger Level")
	int32 MaximumPermissibleDangerLevel = 1000;

	/** Thurst Level Section */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Thurst")
	class UReceivingComponent* ReceivingComponent;

	UPROPERTY(BlueprintAssignable)
	FOnThurstLevelChangedSignature OnThurstLevelChangedDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Thurst")
	int32 CurrentThurstLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Thurst")
	int32 MaximumThurstLevel = 150;

	/** Enemy Detecting Level Section */
	UPROPERTY(BlueprintAssignable)
	FOnDetectingLevelChangedSignature OnDetectingLevelChangedDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy Detecting")
	int32 CurrentDetectingLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enemy Detecting")
	int32 MaximumDetectingLevel = 150;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Danger Level")
	void UpdateObservedDangerLevel(int32 CurrentLevel);

	UFUNCTION(BlueprintCallable, Category = "Thurst")
	void QuenchThurst(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Thurst")
	void RaiseThurst(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Detecting")
	void RaiseDetectingLevel(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Detecting")
	void ResetDetectingLevel();
	
	UFUNCTION(BlueprintCallable)
	void Die();

};
