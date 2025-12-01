// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/Gameplay/MapGenerator.h"

#include "GameFramework/DefaultPawn.h"

// Sets default values
AMapGenerator::AMapGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	grid = TArray<ANode*>();

}

// Called when the game starts or when spawned
void AMapGenerator::BeginPlay()
{
	Super::BeginPlay();
	if (nodeTemplates.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("No nodeTemplates in array."));
		return;
	}
	WaveFunctionCollapse();
}

// Called every frame
void AMapGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMapGenerator::WaveFunctionCollapse()
{
	//Initializing the grid
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			ANode* node = new ANode(i,j);
			grid.Add(node);
		}
	}
	for (ANode* node : grid)
	{
		for (TSubclassOf<ANode> temp : nodeTemplates)
		{
			node->PossibleStates.Add(Cast<ANode>(temp));
		}
	}
	
	int currentIndex = 0;
	bool mapComplete = false;
	ANode* spawnedNode;

	while (!mapComplete)
	{
		//All possible states of the current node crash into one definitive stat
		if (grid.Num() == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("The grid is empty, the garbage collector needs to collect me."));
			return;
		}
		int randomIndex = FMath::FRandRange(0.0, grid[currentIndex]->PossibleStates.Num()-1);
		ANode* tempNode = grid[currentIndex]->PossibleStates[randomIndex];
		grid[currentIndex] = tempNode;
		grid[currentIndex]->bIsNodeSet = true;
		grid[currentIndex]->PossibleStates.Empty();

		//Spawn the node
		FVector worldPos = FVector(grid[currentIndex]->coordX*10, grid[currentIndex]->coordY*10, 0);
		spawnedNode = GetWorld()->SpawnActor<ANode>(worldPos, FRotator(0, 0, 0));
		spawnedNode = grid[currentIndex];

		//Getting the neighbours of the current node.
		TArray<ANode*> neighbours = TArray<ANode*>();
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				//We remove the diagonals
				if (i == 0 && j == 0 || i == 1 && j == 1 || i == -1 && j == -1 || i == 1 && j == -1
					|| i == -1 && j == 1)
				{
					continue;
				}
				int neighbourInd = (grid[currentIndex]->coordX + i) * height + (grid[currentIndex]->coordY + j);
				if (neighbourInd >= 0 && neighbourInd < width * height)
				{
					neighbours.Add(grid[neighbourInd]);
				}
			}
		}

		//For each neighbour we remove the states that are not compatible with the current node.
		for (ANode* neighbour : neighbours)
		{
			if (neighbour->bIsNodeSet)
			{
				continue;
			}
			FVector2D neighbourVector = FVector2D(neighbour->coordX, neighbour->coordY);
			FVector2D currentVector = FVector2D(grid[currentIndex]->coordX, grid[currentIndex]->coordY);
			FVector2D vectorDiff = neighbourVector - currentVector;

			//I couldn't find any premade static variables for these
			FVector2D upVector = FVector2D(0,1);
			FVector2D downVector = FVector2D(0,-1);
			FVector2D leftVector = FVector2D(-1,0);
			FVector2D rightVector = FVector2D(1,0);
			
			for (ANode* possibleState : neighbour->PossibleStates)
			{
				bool bIsCompatible = false;
				//This series of condition would have been a switch if switch could handle a simple FVector2D
				if (vectorDiff == upVector)
				{
					for (TSubclassOf<ANode> compatibleNode : grid[currentIndex]->CompatibleNodesUp)
					{
						if (Cast<ANode>(compatibleNode) == possibleState)
						{
							bIsCompatible = true;
						}
					}
					if (!bIsCompatible)
					{
						neighbour->PossibleStates.Remove(possibleState);
					}
				}
				else if (vectorDiff == downVector)
				{
					for (TSubclassOf<ANode> compatibleNode : grid[currentIndex]->CompatibleNodesDown)
					{
						if (Cast<ANode>(compatibleNode) == possibleState)
						{
							bIsCompatible = true;
						}
					}
					if (!bIsCompatible)
					{
						neighbour->PossibleStates.Remove(possibleState);
					}
				}
				else if (vectorDiff == leftVector)
				{
					for (TSubclassOf<ANode> compatibleNode : grid[currentIndex]->CompatibleNodesLeft)
					{
						if (Cast<ANode>(compatibleNode) == possibleState)
						{
							bIsCompatible = true;
						}
					}
					if (!bIsCompatible)
					{
						neighbour->PossibleStates.Remove(possibleState);
					}
				}
				else if (vectorDiff == rightVector)
				{
					for (TSubclassOf<ANode> compatibleNode : grid[currentIndex]->CompatibleNodesRight)
					{
						if (Cast<ANode>(compatibleNode) == possibleState)
						{
							bIsCompatible = true;
						}
					}
					if (!bIsCompatible)
					{
						neighbour->PossibleStates.Remove(possibleState);
					}
				}
			}
		}

		//Calculating the next index (we choose the one with the least amount of possibilities)
		int minEntropy = INT_MAX;
		for (ANode* node : grid)
		{
			if (node->PossibleStates.Num() < minEntropy)
			{
				minEntropy = node->PossibleStates.Num();
				currentIndex = node->coordX * height + node->coordY;
			}
		}

		//If all nodes are set the function is finished
		mapComplete = true;
		for (ANode* node : grid)
		{
			if (!node->bIsNodeSet)
			{
				mapComplete = false;
			}
		}
	}
}
