// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PatrolAgent.generated.h"

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

	/** Danger Level */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Danger")
	int CurrentDangerLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Danger")
	int MaxDangerLevel = 150;

	/** Thurst Level */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Thurst")
	int CurrentThurstLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Thurst")
	int MaxThurstLevel = 150;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


};
