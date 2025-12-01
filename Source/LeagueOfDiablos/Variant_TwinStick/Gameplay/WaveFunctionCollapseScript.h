// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntropyCell.h"
#include "Room.h"
#include "RoomList.h"
#include "UObject/Object.h"
#include "WaveFunctionCollapseScript.generated.h"

/**
 * 
 */
UCLASS()
class LEAGUEOFDIABLOS_API UWaveFunctionCollapseScript : public UObject
{
	GENERATED_BODY()
	
private:
	int columns;
	int rows;

	UPROPERTY()
	TMap<TEnumAsByte<ERoomType>, FRoomList> roomsMap;

	UPROPERTY()
	TArray<UEntropyCell*> nextCollapse;

public:
	UWaveFunctionCollapseScript() = default;
	
	void Initialize(int InColumns, int InRows, const TMap<TEnumAsByte<ERoomType>, FRoomList>& InRoomsMap)
	{
		columns = InColumns;
		rows = InRows;
		roomsMap = InRoomsMap;
	}

	void WaveFunctionCollapse(TArray<TArray<UEntropyCell*>>& Grid);

	TArray<TArray<UEntropyCell*>> InitializeEmptyGrid(int nbRows, int nbColumns);

	void ForcePathsRooms(TArray<TArray<UEntropyCell*>>& grid, TArray<TArray<TTuple<int,int>>> paths);
private:
	void CollapseCell(TArray<TArray<UEntropyCell*>>& grid, UEntropyCell* cell);

	bool TrySetRoomRotation(TArray<TArray<UEntropyCell*>>& grid, UEntropyCell* cell, const TSubclassOf<ARoom>& room) const;
	
	int GetNeighborsDoors(TArray<TArray<UEntropyCell*>>& grid, const FVector& pos) const;

	BYTE GetNeighborsDoorHash(TArray<TArray<UEntropyCell*>>& grid, const UEntropyCell* cell) const;

	BYTE GetNeighborsExists(TArray<TArray<UEntropyCell*>>& grid, const UEntropyCell* cell) const;

	BYTE GetPreGenerationNeighborsExists(TArray<TArray<UEntropyCell*>>& grid, const UEntropyCell* cell) const;

	void UpdatePossibleRooms(TArray<TArray<UEntropyCell*>>& grid, UEntropyCell* cell);

	void UpdateNeighbors(TArray<TArray<UEntropyCell*>>& grid, const UEntropyCell* cell);
};