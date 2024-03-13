// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PatrolAgent.h"

#include "Variable/Variable.h"
#include "Accumulation/Accumulation.h"
#include "Activation/Activation.h"
#include "Aggregation/Aggregation.h"
#include "Defuzzification/Defuzzification.h"
#include "Rule/RuleBlock.h"
#include "Term/Term.h"
#include "Rule/RuleParser.h"

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

	/** Reaction Timer Section */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer")
	float DefaultReactionTime = 0.5; // in sec

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer")
	float UnstableReactionTime = 0.5; // in sec

	UPROPERTY()
	FTimerHandle ReactionTimer;

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void ResetReactionTimer(const float NewReactionTime);

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void StopReactionTimer();

	UFUNCTION(BlueprintCallable, Category = "General")
	void BeginReacting();

	UFUNCTION()
	FGameplayTag GetTagFromName(const FString TagName);


	/** Agent Section */
	UPROPERTY()
	APatrolAgent* ControlledAgent;

	UFUNCTION(BlueprintCallable, Category = "General")
	void UpdateCharacter(const FGameplayTag& NewState, float ActionTime);

	UPROPERTY()
	UStateMachineComponent* StateMachine;


	/** Input Variables Section*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General")
	UVariable* Thurst;

	void SetThurstVariable();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "General")
	UVariable* DangerLevel;

	void SetDangerLevelVariable();


	/** State Variables Section */
	UPROPERTY()
	UVariable* Action;

	void SetActionVariable();


	UPROPERTY()
	UVariable* TeaReceivingDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tea Receiving")
	int32 MinimumTeaReceivingTime = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tea Receiving")
	int32 MaximumTeaReceivingTime = 10;

	void SetTeaReceivingDuration();


	UPROPERTY()
	UVariable* WaitStateDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tea Receiving")
	int32 MinimumWaitTime = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tea Receiving")
	int32 MaximumWaitTime = 5;

	void SetWaitStateDuration();


	UPROPERTY()
	UVariable* PatrolStateDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tea Receiving")
	int32 MinimumPatrolTime = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tea Receiving")
	int32 MaximumPatrolTime = 10;

	void SetPatrolStateDuration();


	UPROPERTY()
	UVariable* RushStateDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tea Receiving")
	int32 MinimumRushTime = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tea Receiving")
	int32 MaximumRushTime = 7;

	void SetRushStateDuration();


	/** Attack Section */
	float AttackRate = 0.5;

	
	/** Fuzzy Logic Functions Section */
	UPROPERTY()
	UAggregation* Aggregation;

	UPROPERTY()
	UActivation* Activation;

	UPROPERTY()
	UAccumulation* Accumulation;

	UPROPERTY()
	UVariableReceiver* MaxReceiver;

	UPROPERTY()
	UVariableReceiver* AuctionReceiver;

	UPROPERTY()
	UDefuzzification* CentroidDefuzzification;

	UPROPERTY()
	UDefuzzification* LargeMaxDefuzzification;


	/** Rule Section */
	UPROPERTY()
	URuleParser* RuleParser;

	void SetRuleParser();

	UPROPERTY()
	TMap<FGameplayTag, URuleBlock*> ActionRuleBlocks;

	void SetActionRuleBlocks();

	UPROPERTY()
	TMap<FGameplayTag, UVariable*> ActionTimings;

	void SetActionTimings();

	/** Delegate Functions Section */
	void OnAgentMoveEnd();

	void OnEnemyDetected();

};
