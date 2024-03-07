// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReceivingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValueReceivedSignature, int32, Value);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FUZZYPATROL_API UReceivingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UReceivingComponent();

	UPROPERTY(BlueprintAssignable)
	FOnValueReceivedSignature OnValueReceivedDelegate;

	UPROPERTY(BlueprintReadOnly)
	int32 ReceivingValue = 0;

	UPROPERTY()
	FTimerHandle TimerHandle;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void EnableReceiving(float NewPeriod, int32 NewReceivingValue);

	UFUNCTION(BlueprintCallable)
	void DisableReceiving();

	UFUNCTION()
	void ResetTimer(float Period);

	UFUNCTION()
	void StopTimer();

	UFUNCTION()
	void Receive();
};
