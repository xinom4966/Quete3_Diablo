// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorData.generated.h"

UCLASS()
class LEAGUEOFDIABLOS_API ADoorData : public AActor
{
	GENERATED_BODY()
	
public:	
	FVector position;
    FVector rotation;
	
	ADoorData() = default;
	ADoorData(FVector position, FVector rotation) : position(position), rotation(rotation){}
};
