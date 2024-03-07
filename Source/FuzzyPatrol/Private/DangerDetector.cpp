// Fill out your copyright notice in the Description page of Project Settings.


#include "DangerDetector.h"

// Sets default values
ADangerDetector::ADangerDetector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADangerDetector::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADangerDetector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADangerDetector::RaiseLevel(int32 Value)
{
	CurrentDangerLevel += Value;

	if (OnLevelChangedDelegate.IsBound())
	{
		OnLevelChangedDelegate.Broadcast(CurrentDangerLevel);
	}

}

void ADangerDetector::LowerLevel(int32 Value)
{
	CurrentDangerLevel = FMath::Clamp(CurrentDangerLevel - Value, 0, CurrentDangerLevel);

	if (OnLevelChangedDelegate.IsBound())
	{
		OnLevelChangedDelegate.Broadcast(CurrentDangerLevel);
	}

}

