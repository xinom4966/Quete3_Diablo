// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExitDirection.h"
#include "Node.generated.h"

UCLASS(Blueprintable)
class LEAGUEOFDIABLOS_API ANode : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="NodeData")
	TArray<TSubclassOf<ANode>> CompatibleNodesUp;

	UPROPERTY(EditAnywhere, Category="NodeData")
	TArray<TSubclassOf<ANode>> CompatibleNodesDown;

	UPROPERTY(EditAnywhere, Category="NodeData")
	TArray<TSubclassOf<ANode>> CompatibleNodesLeft;

	UPROPERTY(EditAnywhere, Category="NodeData")
	TArray<TSubclassOf<ANode>> CompatibleNodesRight;

	//Used to store the different states a node can be during wave function collapse.
	TArray<ANode*> PossibleStates;

	bool bIsNodeSet;

	int coordX;
	int coordY;
	
public:	
	// Sets default values for this actor's properties
	ANode();
	ANode(int x, int y);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
