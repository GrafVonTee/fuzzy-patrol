// Fill out your copyright notice in the Description page of Project Settings.


#include "ReceivingComponent.h"

// Sets default values for this component's properties
UReceivingComponent::UReceivingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UReceivingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UReceivingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UReceivingComponent::EnableReceiving(float NewPeriod, int32 NewReceivingValue)
{
	ResetTimer(NewPeriod);
	ReceivingValue = NewReceivingValue;

}

void UReceivingComponent::DisableReceiving()
{
	StopTimer();
	ReceivingValue = 0;

}

void UReceivingComponent::ResetTimer(float Period)
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UReceivingComponent::Receive, Period, true);

}

void UReceivingComponent::StopTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

}

void UReceivingComponent::Receive()
{
	if (OnValueReceivedDelegate.IsBound())
	{
		OnValueReceivedDelegate.Broadcast(ReceivingValue);

	}

}

