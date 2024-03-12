// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "PatrolAIController.generated.h"

/**
 * 
 */
UCLASS()
class FUZZYPATROL_API APatrolAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer")
	float ReactionTime = 1; // in sec

	UPROPERTY()
	FTimerHandle ReactionTimer;

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void ResetReactionTimer();

	UFUNCTION(BlueprintCallable, Category = "General")
	void BeginReacting();

	UFUNCTION(BlueprintCallable, Category = "General")
	void UpdateCharacter(const FGameplayTag& NewState, float ActionDegree);

};
