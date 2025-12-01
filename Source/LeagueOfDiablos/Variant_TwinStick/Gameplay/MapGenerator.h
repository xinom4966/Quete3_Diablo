// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Node.h"
#include "MapGenerator.generated.h"

UCLASS()
class LEAGUEOFDIABLOS_API AMapGenerator : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category ="GridData")
	int width;

	UPROPERTY(EditAnywhere, Category ="GridData")
	int height;

	UPROPERTY()
	TArray<ANode*> grid;
	
	UPROPERTY(EditAnywhere, Category="NodeData")
	TArray<TSubclassOf<ANode>> nodeTemplates;
	
public:	
	// Sets default values for this actor's properties
	AMapGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void WaveFunctionCollapse();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
