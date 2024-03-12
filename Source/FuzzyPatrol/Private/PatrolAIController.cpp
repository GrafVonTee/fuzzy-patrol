// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolAIController.h"
#include "PatrolAgent.h"

void APatrolAIController::BeginPlay()
{
}

void APatrolAIController::ResetReactionTimer()
{
	GetWorldTimerManager().SetTimer(ReactionTimer, this, &APatrolAIController::BeginReacting, ReactionTime, true);

}

void APatrolAIController::BeginReacting()
{
	APatrolAgent* ControlledAgent = Cast<APatrolAgent>(GetCharacter());

}

void APatrolAIController::UpdateCharacter(const FGameplayTag& NewState, float ActionDegree)
{
	APatrolAgent* ControlledAgent = Cast<APatrolAgent>(GetCharacter());

}
