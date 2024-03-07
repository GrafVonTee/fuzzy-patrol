// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DangerDetector.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelChangedSignature, int32, CurrentLevel);

UCLASS()
class FUZZYPATROL_API ADangerDetector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADangerDetector();

	UPROPERTY(BlueprintAssignable)
	FOnLevelChangedSignature OnLevelChangedDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Danger Level")
	int32 CurrentDangerLevel = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Danger Level")
	void RaiseLevel(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Danger Level")
	void LowerLevel(int32 Value);

};
