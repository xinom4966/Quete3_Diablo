// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoomList.generated.h"

class ARoom;


USTRUCT()
struct FRoomList
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Dungeon")
	TArray<TSubclassOf<ARoom>> rooms;
};