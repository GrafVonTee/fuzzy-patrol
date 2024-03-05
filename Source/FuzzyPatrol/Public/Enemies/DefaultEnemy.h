// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
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
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
