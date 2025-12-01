// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoomList.h"
#include "GameFramework/Actor.h"
#include "WaveFunctionCollapseScript.h"
#include "RoomType.h"
#include "DungeonGenerator.generated.h"

UCLASS()
class LEAGUEOFDIABLOS_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	int rows = 7;

	UPROPERTY(EditAnywhere)
	int columns = 7;

	UPROPERTY(EditAnywhere)
	int roomSize = 1000;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ADoorData> doorPrefab;

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<ERoomType>, FRoomList> roomsMap;
	
	UPROPERTY(EditDefaultsOnly)
	int exitNumber = 1;
	
	UPROPERTY(EditDefaultsOnly)
	int treasureNumber = 3;

private:
	bool isInitialized = false;

	UPROPERTY()
	UWaveFunctionCollapseScript* wfcScript;

	//[SerializeField] private GameObject doorPrefab;
	
	TArray<TArray<UEntropyCell*>> dungeonGrid;
	
public:	
	// Sets default values for this actor's properties
	ADungeonGenerator();

protected:
	static void PrintGrid(TArray<TArray<UEntropyCell*>> Array);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GenerateSpecialRooms(TArray<TArray<UEntropyCell*>>& grid) const;

	TArray<TArray<int>> FloodFillWeight(int rowBegin, int columnBegin) const;

	void GenerateDoors(TArray<TArray<UEntropyCell*>>& grid);

	void GenerateRooms(TArray<TArray<UEntropyCell*>>& grid);

	void GenerateSpecialPaths(TArray<TArray<UEntropyCell*>>& grid, const TArray<TArray<int>>& weightedPositions, TArray<TArray<TTuple<int,int>>>& paths, ERoomType type) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
