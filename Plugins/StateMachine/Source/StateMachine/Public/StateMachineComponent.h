// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"
#include "StateMachineComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStateChangedSignature, const FGameplayTag&, NewStateTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInitStateSignature, const FGameplayTag&, StateTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEndStateSignature, const FGameplayTag&, StateTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTickStateSignature, float, DeltaTime, const FGameplayTag&, StateTag);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType )
class STATEMACHINE_API UStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStateMachineComponent();


	/* Delegates */
	UPROPERTY(BlueprintAssignable)
	FInitStateSignature InitStateDelegate;

	UPROPERTY(BlueprintAssignable)
	FEndStateSignature EndStateDelegate;

	UPROPERTY(BlueprintAssignable)
	FTickStateSignature TickStateDelegate;

	UPROPERTY(BlueprintAssignable)
	FStateChangedSignature StateChangedDelegate;


	/* State Properties */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag StateTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag InitialStateTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bDebug = false;


	/* State History */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<FGameplayTag> StateHistory;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 StateHistoryLength = -1;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool SwitchState(FGameplayTag NewStateTag);

private:
	bool bCanTickState = false;
	void InitState();
	void TickState(float DeltaTime);
	void EndState();
		
};
