// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "ExitDirection.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ExitDirection : uint8
{
	Top UMETA(DisplayName = "Top"),
	Bottom UMETA(DisplayName = "Bottom"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};
