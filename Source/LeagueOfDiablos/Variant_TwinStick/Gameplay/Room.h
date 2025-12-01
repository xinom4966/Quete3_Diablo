// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomType.h"
#include "Room.generated.h"

class UEntropyCell;

UCLASS()
class LEAGUEOFDIABLOS_API ARoom : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString RoomName;

	UPROPERTY(EditAnywhere)
	bool doorTop = false;

	UPROPERTY(EditAnywhere)
	bool doorRight = false;
	
	UPROPERTY(EditAnywhere)
	bool doorBottom = false;
	
	UPROPERTY(EditAnywhere)
	bool doorLeft = false;

	UPROPERTY(EditAnywhere, Blueprintable)
	TEnumAsByte<ERoomType> roomType;
	
	UPROPERTY()
	UEntropyCell* EntropyCell;
	
public:	
	// Sets default values for this actor's properties
	ARoom();

	BYTE GetDoorHash() const
	{
		BYTE hash = 0;
		if (doorTop) hash |= 1 << 0;
		if (doorRight) hash |= 1 << 1;
		if (doorBottom) hash |= 1 << 2;
		if (doorLeft) hash |= 1 << 3;
		return hash;
	}

	int GetDoorCount() const
	{
		int count = 0;
		if (doorTop) count++;
		if (doorRight) count++;
		if (doorBottom) count++;
		if (doorLeft) count++;
		return count;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
