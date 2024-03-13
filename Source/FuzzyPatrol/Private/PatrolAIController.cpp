// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolAIController.h"
#include "AgentGameplayTags.h"

#include "Term/Gaussian.h"
#include "Term/SquareRoot.h"
#include "Term/Quadratic.h"
#include "Term/Linear.h"

#include "Variable/MaxReceiver.h"
#include "Variable/AuctionReceiver.h"
#include "Accumulation/MaxAccumulation.h"
#include "Activation/ProdActivation.h"
#include "Aggregation/MinAggregation.h"
#include "Defuzzification/CentroidDefuzzification.h"
#include "Defuzzification/LargeMaxDefuzzification.h"

#include "Hedge/HedgeNot.h"
#include "Hedge/HedgeVery.h"
#include "Hedge/HedgeApproximately.h"


void APatrolAIController::BeginPlay()
{
	Super::BeginPlay();

	/** Agent Delegates Section */
	ControlledAgent = Cast<APatrolAgent>(GetCharacter());
	StateMachine = ControlledAgent->StateMachine;

	ControlledAgent->OnMoveEndDelegate.AddUniqueDynamic(
		this,
		&APatrolAIController::OnAgentMoveEnd
	);

	ControlledAgent->OnEnemyDetectedDelegate.AddUniqueDynamic(
		this,
		&APatrolAIController::OnEnemyDetected
	);


	/** Main Fuzzy Logic Section */
	Thurst = NewObject<UVariable>();
	DangerLevel = NewObject<UVariable>();
	Action = NewObject<UVariable>();

	TeaReceivingDuration = NewObject<UVariable>();
	WaitStateDuration = NewObject<UVariable>();
	PatrolStateDuration = NewObject<UVariable>();
	RushStateDuration = NewObject<UVariable>();

	Aggregation = NewObject<UMinAggregation>();
	Activation = NewObject<UProdActivation>();
	Accumulation = NewObject<UMaxAccumulation>();
	MaxReceiver = NewObject<UMaxReceiver>();
	AuctionReceiver = NewObject<UAuctionReceiver>();
	CentroidDefuzzification = NewObject<UCentroidDefuzzification>();
	LargeMaxDefuzzification = NewObject<ULargeMaxDefuzzification>();

	RuleParser = NewObject<URuleParser>();

	SetThurstVariable();
	SetDangerLevelVariable();
	SetActionVariable();

	SetTeaReceivingDuration();
	SetWaitStateDuration();
	SetPatrolStateDuration();
	SetRushStateDuration();

	SetActionTimings();

	SetActionRuleBlocks();


	/** Rection Section */
	ResetReactionTimer(DefaultReactionTime);

}

void APatrolAIController::ResetReactionTimer(const float NewReactionTime)
{
	GetWorldTimerManager().SetTimer(ReactionTimer, this, &APatrolAIController::BeginReacting, NewReactionTime, true);

}

void APatrolAIController::StopReactionTimer()
{
	GetWorldTimerManager().ClearTimer(ReactionTimer);

}

void APatrolAIController::BeginReacting()
{
	if (!IsValid(ControlledAgent))
	{
		return;
	}

	TMap<UVariable*, int32> VarInput;
	VarInput.Add(Thurst, ControlledAgent->CurrentThurstLevel);
	VarInput.Add(DangerLevel, ControlledAgent->ObservedDangerLevel);
	FGameplayTag CurrentState = StateMachine->StateTag;

	URuleBlock* TemporaryRuleBlock = NewObject<URuleBlock>();
	for (auto& Elem : ActionRuleBlocks)
	{
		if (CurrentState.MatchesTag(Elem.Key))
		{
			TemporaryRuleBlock->AppendList(Elem.Value->GetRuleList());
		}
	}

	UTerm* AccumulatedAction = TemporaryRuleBlock->Accumulate(VarInput);
	int32 DeffuzzActionValue = LargeMaxDefuzzification->Defuzzify(AccumulatedAction);
	FString ActionName = Action->GetTermWithValue(DeffuzzActionValue);
	float ActionDegree = Action->LastValue[ActionName];

	FGameplayTag NewActionState = GetTagFromName(ActionName);
	if (ActionTimings.Contains(NewActionState))
	{
		float NewTime = ActionTimings[NewActionState]->GetXFromDegree(ActionDegree)["JustTime"];
		UpdateCharacter(NewActionState, NewTime);

		if (NewActionState.MatchesTag(AgentGameplayTags::TeaState))
		{
			StopReactionTimer();
		}
	}
	else if (ActionName == "None" && CurrentState.MatchesTag(AgentGameplayTags::NonTeaState_BattleState))
	{
		if (!ControlledAgent->HasDetectedEnemy())
		{
			UpdateCharacter(AgentGameplayTags::NonTeaState_NonBattleState_Wait, DefaultReactionTime);
		}
		else
		{
			if (ControlledAgent->DetectedEnemyIsNear())
			{
				UpdateCharacter(AgentGameplayTags::NonTeaState_BattleState_Attack, AttackRate);
			}
			else
			{
				UpdateCharacter(AgentGameplayTags::NonTeaState_BattleState_Chase, DefaultReactionTime);
			}
		}
	}
	else
	{
		return;
	}

}

FGameplayTag APatrolAIController::GetTagFromName(const FString TagName)
{
	if ("TeaWait" == TagName)
	{
		return AgentGameplayTags::TeaState_Wait;
	}
	else if ("TeaWalk" == TagName)
	{
		return AgentGameplayTags::TeaState_Walk;
	}
	else if ("TeaRush" == TagName)
	{
		return AgentGameplayTags::TeaState_Rush;
	}
	else if ("Attack" == TagName)
	{
		return AgentGameplayTags::NonTeaState_BattleState_Attack;
	}
	else if ("Chase" == TagName)
	{
		return AgentGameplayTags::NonTeaState_BattleState_Chase;
	}
	else if ("Wait" == TagName)
	{
		return AgentGameplayTags::NonTeaState_NonBattleState_Wait;
	}
	else if ("Patrol" == TagName)
	{
		return AgentGameplayTags::NonTeaState_NonBattleState_Patrol;
	}
	else if ("Rush" == TagName)
	{
		return AgentGameplayTags::NonTeaState_NonBattleState_Rush;
	}
	else
	{
		return FGameplayTag::EmptyTag;
	}
}

void APatrolAIController::UpdateCharacter(const FGameplayTag& NewState, float ActionTime)
{
	StateMachine->SwitchState(NewState);
	UnstableReactionTime = ActionTime;
	ResetReactionTimer(UnstableReactionTime);
}

void APatrolAIController::SetThurstVariable()
{
	Thurst->Set("Thurst", 0, ControlledAgent->MaximumThurstLevel, MaxReceiver);

	UGaussian* ThurstLow = NewObject<UGaussian>();
	ThurstLow->Set(
		"Low",
		DangerLevel->LowerBound,
		DangerLevel->UpperBound,
		0,
		65
	);

	UGaussian* ThurstMid = NewObject<UGaussian>();
	ThurstMid->Set(
		"Mid",
		DangerLevel->LowerBound,
		DangerLevel->UpperBound,
		150,
		65
	);

	UGaussian* ThurstHigh = NewObject<UGaussian>();
	ThurstMid->Set(
		"High",
		DangerLevel->LowerBound,
		DangerLevel->UpperBound,
		300,
		65
	);

	DangerLevel->AddTerm(ThurstLow->Name, ThurstLow, 1);
	DangerLevel->AddTerm(ThurstMid->Name, ThurstMid, 1);
	DangerLevel->AddTerm(ThurstHigh->Name, ThurstHigh, 1);
}

void APatrolAIController::SetDangerLevelVariable()
{
	DangerLevel->Set("DangerLevel", 0, ControlledAgent->MaximumPermissibleDangerLevel, MaxReceiver);

	USquareRoot* DangerLevelHigh = NewObject<USquareRoot>();
	DangerLevelHigh->Set(
		"High",
		DangerLevel->LowerBound,
		DangerLevel->UpperBound,
		DangerLevel->LowerBound,
		DangerLevel->UpperBound
	);
	DangerLevel->AddTerm(DangerLevelHigh->Name, DangerLevelHigh, 1);

}

void APatrolAIController::SetActionVariable()
{
	Action->Set("Action", 0, 400, AuctionReceiver);

	UGaussian* ActionNone = NewObject<UGaussian>();
	ActionNone->Set(
		"None",
		Action->LowerBound,
		Action->UpperBound,
		0,
		20
	);
	Action->AddTerm(ActionNone->Name, ActionNone, 1);

	UGaussian* ActionTeaWait = NewObject<UGaussian>();
	ActionTeaWait->Set(
		"TeaWait",
		Action->LowerBound,
		Action->UpperBound,
		50,
		20
	);
	Action->AddTerm(ActionTeaWait->Name, ActionTeaWait, 1);

	UGaussian* ActionTeaWalk = NewObject<UGaussian>();
	ActionTeaWalk->Set(
		"TeaWalk",
		Action->LowerBound,
		Action->UpperBound,
		100,
		20
	);
	Action->AddTerm(ActionTeaWalk->Name, ActionTeaWalk, 1);

	UGaussian* ActionTeaRush = NewObject<UGaussian>();
	ActionTeaRush->Set(
		"TeaRush",
		Action->LowerBound,
		Action->UpperBound,
		150,
		20
	);
	Action->AddTerm(ActionTeaRush->Name, ActionTeaRush, 1);

	UGaussian* ActionWait = NewObject<UGaussian>();
	ActionWait->Set(
		"Wait",
		Action->LowerBound,
		Action->UpperBound,
		200,
		20
	);
	Action->AddTerm(ActionWait->Name, ActionWait, 1);

	UGaussian* ActionPatrol = NewObject<UGaussian>();
	ActionPatrol->Set(
		"Patrol",
		Action->LowerBound,
		Action->UpperBound,
		250,
		20
	);
	Action->AddTerm(ActionPatrol->Name, ActionPatrol, 1);

	UGaussian* ActionRush = NewObject<UGaussian>();
	ActionRush->Set(
		"Rush",
		Action->LowerBound,
		Action->UpperBound,
		300,
		20
	);
	Action->AddTerm(ActionRush->Name, ActionRush, 1);

	UGaussian* ActionAttack = NewObject<UGaussian>();
	ActionAttack->Set(
		"Attack",
		Action->LowerBound,
		Action->UpperBound,
		350,
		20
	);
	Action->AddTerm(ActionAttack->Name, ActionAttack, 1);

	UGaussian* ActionChase = NewObject<UGaussian>();
	ActionChase->Set(
		"Chase",
		Action->LowerBound,
		Action->UpperBound,
		400,
		20
	);
	Action->AddTerm(ActionChase->Name, ActionChase, 1);
}

void APatrolAIController::SetTeaReceivingDuration()
{
	TeaReceivingDuration->Set("TeaReceivingDuration", MinimumTeaReceivingTime, MaximumTeaReceivingTime, MaxReceiver);

	ULinear* JustTime = NewObject<ULinear>();
	JustTime->Set(
		"Time",
		TeaReceivingDuration->LowerBound,
		TeaReceivingDuration->UpperBound,
		TeaReceivingDuration->LowerBound,
		TeaReceivingDuration->UpperBound
	);
	TeaReceivingDuration->AddTerm(JustTime->Name, JustTime, 1);
}

void APatrolAIController::SetWaitStateDuration()
{
	WaitStateDuration->Set("WaitStateDuration", MinimumWaitTime, MaximumWaitTime, MaxReceiver);

	ULinear* JustTime = NewObject<ULinear>();
	JustTime->Set(
		"JustTime",
		WaitStateDuration->LowerBound,
		WaitStateDuration->UpperBound,
		WaitStateDuration->LowerBound,
		WaitStateDuration->UpperBound
	);
	WaitStateDuration->AddTerm(JustTime->Name, JustTime, 1);
}

void APatrolAIController::SetPatrolStateDuration()
{
	PatrolStateDuration->Set("PatrolStateDuration", MinimumPatrolTime, MaximumPatrolTime, MaxReceiver);

	ULinear* JustTime = NewObject<ULinear>();
	JustTime->Set(
		"JustTime",
		PatrolStateDuration->LowerBound,
		PatrolStateDuration->UpperBound,
		PatrolStateDuration->LowerBound,
		PatrolStateDuration->UpperBound
	);
	PatrolStateDuration->AddTerm(JustTime->Name, JustTime, 1);
}

void APatrolAIController::SetRushStateDuration()
{
	RushStateDuration->Set("RushStateDuration", MinimumRushTime, MaximumRushTime, MaxReceiver);

	ULinear* JustTime = NewObject<ULinear>();
	JustTime->Set(
		"JustTime",
		RushStateDuration->LowerBound,
		RushStateDuration->UpperBound,
		RushStateDuration->LowerBound,
		RushStateDuration->UpperBound
	);
	RushStateDuration->AddTerm(JustTime->Name, JustTime, 1);
}

void APatrolAIController::SetRuleParser()
{
	TMap<FString, UVariable*> VarList;
	TMap<FString, UHedge*> HedgeList;

	VarList.Add(Thurst->Name, Thurst);
	VarList.Add(DangerLevel->Name, DangerLevel);
	VarList.Add(Action->Name, Action);

	HedgeList.Add("", NewObject<UHedge>());
	HedgeList.Add("Not", NewObject<UHedgeNot>());
	HedgeList.Add("Very", NewObject<UHedgeVery>());
	HedgeList.Add("Approximately", NewObject<UHedgeApproximately>());

	RuleParser->Set(VarList, HedgeList, Aggregation, Activation);
}

void APatrolAIController::SetActionRuleBlocks()
{
	/* Non Tea State Block */
	URuleBlock* NonTeaBlock = NewObject<URuleBlock>();
	NonTeaBlock->SetAccumulation(Accumulation);

	URule* TeaWalkRule = RuleParser->ParseRule(
		"IF Thurst IS High AND DangerLevel IS Not High THEN Action IS TeaWalk"
	);
	URule* TeaRushRule = RuleParser->ParseRule(
		"IF Thurst IS High AND DangerLevel IS High THEN Action IS TeaRush"
	);
	NonTeaBlock->AddRuleToList(TeaWalkRule);
	NonTeaBlock->AddRuleToList(TeaRushRule);

	ActionRuleBlocks.Add(AgentGameplayTags::NonTeaState, NonTeaBlock);


	/* Tea State Block*/
	URuleBlock* TeaBlock = NewObject<URuleBlock>();
	TeaBlock->SetAccumulation(Accumulation);

	URule* FromTeaPatrolRule = RuleParser->ParseRule(
		"IF Thurst IS Low AND DangerLevel IS Not High THEN Action IS Patrol"
	);
	URule* FromTeaRushRule = RuleParser->ParseRule(
		"IF Thurst IS Not High AND DangerLevel IS High THEN Action IS Rush"
	);
	TeaBlock->AddRuleToList(FromTeaPatrolRule);
	TeaBlock->AddRuleToList(FromTeaRushRule);

	ActionRuleBlocks.Add(AgentGameplayTags::TeaState, TeaBlock);


	/* Non Battle State */
	URuleBlock* NonBattleBlock = NewObject<URuleBlock>();
	NonBattleBlock->SetAccumulation(Accumulation);

	URule* ToRushRule = RuleParser->ParseRule(
		"IF Thurst IS Low AND DangerLevel IS High THEN Action IS Rush"
	);
	URule* ToPatrolRule = RuleParser->ParseRule(
		"IF Thurst IS Mid AND DangerLevel IS High THEN Action IS Patrol"
	);
	NonBattleBlock->AddRuleToList(ToRushRule);
	NonBattleBlock->AddRuleToList(ToPatrolRule);

	ActionRuleBlocks.Add(AgentGameplayTags::NonTeaState_BattleState, NonBattleBlock);

	
	/* Wait Block */
	URuleBlock* WaitBlock = NewObject<URuleBlock>();
	WaitBlock->SetAccumulation(Accumulation);

	URule* FromWaitToPatrol = RuleParser->ParseRule(
		"IF Thurst IS Low AND DangerLevel IS Not High THEN Action IS Patrol"
	);
	URule* FromWaitToTeaRush = RuleParser->ParseRule(
		"IF Thurst IS Mid AND DangerLevel IS Not High THEN Action IS TeaRush"
	);
	WaitBlock->AddRuleToList(FromWaitToPatrol);
	WaitBlock->AddRuleToList(FromWaitToTeaRush);

	ActionRuleBlocks.Add(AgentGameplayTags::NonTeaState_NonBattleState_Wait, WaitBlock);


	/* Patrol Block */
	URuleBlock* PatrolBlock = NewObject<URuleBlock>();
	PatrolBlock->SetAccumulation(Accumulation);

	URule* FromPatrolToWait = RuleParser->ParseRule(
		"IF Thurst IS Not High AND DangerLevel IS Not High THEN Action IS Wait"
	);
	PatrolBlock->AddRuleToList(FromPatrolToWait);

	ActionRuleBlocks.Add(AgentGameplayTags::NonTeaState_NonBattleState_Patrol, PatrolBlock);


	/* RUsh Block */
	URuleBlock* RushBlock = NewObject<URuleBlock>();
	RushBlock->SetAccumulation(Accumulation);

	URule* FromRushToPatrol = RuleParser->ParseRule(
		"IF Thurst IS Low AND DangerLevel IS Not High THEN Action IS Patrol"
	);
	URule* FromRushToWait = RuleParser->ParseRule(
		"IF Thurst IS Mid AND DangerLevel IS Not High THEN Action IS Wait"
	);
	RushBlock->AddRuleToList(FromRushToPatrol);
	RushBlock->AddRuleToList(FromRushToWait);

	ActionRuleBlocks.Add(AgentGameplayTags::NonTeaState_NonBattleState_Rush, RushBlock);


	/* Battle Block */
	URuleBlock* BattleBlock = NewObject<URuleBlock>();
	BattleBlock->SetAccumulation(Accumulation);

	URule* DoNotChangeState = RuleParser->ParseRule(
		"IF Thurst IS Not High THEN Action IS None"
	);
	BattleBlock->AddRuleToList(DoNotChangeState);

	ActionRuleBlocks.Add(AgentGameplayTags::NonTeaState_BattleState, BattleBlock);

}

void APatrolAIController::SetActionTimings()
{
	ActionTimings.Add(AgentGameplayTags::TeaState_Walk, TeaReceivingDuration);
	ActionTimings.Add(AgentGameplayTags::TeaState_Rush, TeaReceivingDuration);
	ActionTimings.Add(AgentGameplayTags::NonTeaState_NonBattleState_Wait, WaitStateDuration);
	ActionTimings.Add(AgentGameplayTags::NonTeaState_NonBattleState_Patrol, PatrolStateDuration);
	ActionTimings.Add(AgentGameplayTags::NonTeaState_NonBattleState_Rush, RushStateDuration);
}

void APatrolAIController::OnAgentMoveEnd()
{
	if (StateMachine->StateTag.MatchesTag(AgentGameplayTags::TeaState))
	{
		UpdateCharacter(AgentGameplayTags::TeaState_Wait, UnstableReactionTime);
	}
	else if (StateMachine->StateTag.MatchesTag(AgentGameplayTags::NonTeaState_BattleState))
	{
		BeginReacting();
	}
}

void APatrolAIController::OnEnemyDetected()
{
	if (!StateMachine->StateTag.MatchesTag(AgentGameplayTags::TeaState))
	{
		UpdateCharacter(AgentGameplayTags::NonTeaState_BattleState_Chase, DefaultReactionTime);
	}
}
