// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/Gameplay/DungeonGenerator.h"
#include "Room.h"
#include "DoorData.h"

// Sets default values
ADungeonGenerator::ADungeonGenerator()
    : wfcScript(nullptr)
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

void ADungeonGenerator::PrintGrid(TArray<TArray<UEntropyCell*>> Array)
{
    for (int i = 0; i < Array.Num(); ++i)
    {
        int cols = Array[i].Num();
        for (int j = 0; j < cols; ++j)
        {
            UEntropyCell* cell = Array[i][j];
            if (!cell)
            {
                UE_LOG(LogTemp, Log, TEXT("Grid[%d][%d]: <null>"), i, j);
                continue;
            }

            FString roomName = cell->finalRoom ? cell->finalRoom->GetName() : FString(TEXT("None"));
            const TCHAR* collapsedStr = cell->IsCollapsed() ? TEXT("true") : TEXT("false");
            FString posStr = cell->position.ToString();

            UE_LOG(LogTemp, Log, TEXT("Grid[%d][%d]: room=%s collapsed=%s rotY=%.1f pos=%s"),
                   i, j, *roomName, collapsedStr, cell->rotation.Yaw, *posStr);
        }
    }
}

// Called when the game starts or when spawned
void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();
    
    wfcScript = NewObject<UWaveFunctionCollapseScript>(this);

    if (!wfcScript) return;
    
    wfcScript->Initialize(columns, rows, roomsMap);
    dungeonGrid = wfcScript->InitializeEmptyGrid(rows, columns);

    GenerateSpecialRooms(dungeonGrid);
    
    wfcScript->WaveFunctionCollapse(dungeonGrid);

    GenerateRooms(dungeonGrid);
}

// Called every frame
void ADungeonGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ADungeonGenerator::GenerateSpecialRooms(TArray<TArray<UEntropyCell*>>& grid) const
{
    grid[(rows-1) / 2][(columns-1) / 2]->roomType = Entrance;
    
    TArray<TArray<int>> weightedPositions = FloodFillWeight((rows-1) / 2,(columns-1) / 2);
    
    TArray<TArray<TTuple<int,int>>> paths{};
    
    for (int _ = 0; _ < treasureNumber; _++) // treasure rooms
    {
        GenerateSpecialPaths(grid, weightedPositions, paths, Treasure);
    }

    for (int _ = 0; _ < exitNumber; _++) // exit room
    {
        GenerateSpecialPaths(grid, weightedPositions, paths, Exit);
    }

    if (wfcScript) wfcScript->ForcePathsRooms(grid, paths);
}

void ADungeonGenerator::GenerateSpecialPaths(
    TArray<TArray<UEntropyCell*>>& grid,
    const TArray<TArray<int>>& weightedPositions,
    TArray<TArray<TTuple<int,int>>>& paths,
    ERoomType type) const
{
    TArray<TTuple<int,int>> queue{};
    queue.Add(TTuple<int, int>((rows-1) / 2, (columns-1) / 2));
    int maxWeight = 0;
        
    int distanceFromBegin = FMath::RandRange(FMath::Max(rows, columns)/2 , (rows + columns)/2 - 2);

    for (int i = 0; i < distanceFromBegin; i++)
    {
        TArray<TTuple<int,int>> possibleNextSteps{};
        int currentRow = queue.Last().Key;;
        int currentColumn = queue.Last().Value;
        int dRows[] = { -1, 1, 0, 0 };
        int dCols[] = { 0, 0, -1, 1 };
            
        for (int j = 0; j < 4; j++)
        {
            int newRow = currentRow + dRows[j];
            int newColumn = currentColumn + dCols[j];
                
            if (newRow >= 0 && newRow < rows && newColumn >= 0 && newColumn < columns)
            {
                if (weightedPositions[newRow][newColumn] >= maxWeight)
                {
                    possibleNextSteps.Add(TTuple<int, int>(newRow, newColumn));
                }
            }
        }
            
        if (possibleNextSteps.Num() == 0)
        {
            break; // No more possible steps
        }
            
        TTuple<int, int> next = possibleNextSteps[FMath::RandRange(0, possibleNextSteps.Num()-1)];
        queue.Add(next);
        maxWeight++;
    }

    bool cellIsFree = false;
    while (!cellIsFree && queue.Num() > 1)
    {
        cellIsFree = true;
        for (TArray q : paths)
        {
            if (q.Last() == queue.Last())
            {
                cellIsFree = false;
                break;
            }
        }
        if (!cellIsFree && queue.Num() > 1)
        {
            queue.RemoveAt(queue.Num() - 1);
        }
    }

    switch (type)
    {
        case Normal:
            break;
        case Entrance:
            grid[queue.Last().Key][queue.Last().Value]->roomType = Entrance;
            break;
        case Exit:
            grid[queue.Last().Key][queue.Last().Value]->roomType = Exit;
            break;
        case Treasure:
            grid[queue.Last().Key][queue.Last().Value]->roomType = Treasure;
            break;
        default:
            break;
    }
    paths.Add(queue);
}

TArray<TArray<int>> ADungeonGenerator::FloodFillWeight(int rowBegin, int columnBegin) const
{
    TArray<TArray<int>> weightedPositions{};
    
    for (int i = 0; i < rows; i++)
    {
        weightedPositions.Add(TArray<int>());
        for (int j = 0; j < columns; j++)
        {
            weightedPositions[i].Add(-1);
        }
    }
    
    TQueue<TTuple<int, int>> queue;
    queue.Enqueue(TTuple<int, int>(rowBegin, columnBegin));
    weightedPositions[rowBegin][columnBegin] = 0;
    
    int dRows[] = { -1, 1, 0, 0 };
    int dCols[] = { 0, 0, -1, 1 };

    int count = 1;
    while (count > 0)
    {
        TTuple<int, int> current;
        queue.Dequeue(current);
        count--;
        int currentWeight = weightedPositions[current.Key][current.Value];
        
        for (int i = 0; i < 4; i++)
        {
            int newRow = current.Key + dRows[i];
            int newColumn = current.Value + dCols[i];
            
            if (newRow >= 0 && newRow < rows && newColumn >= 0 && newColumn < columns)
            {
                if (weightedPositions[newRow][newColumn] == -1)
                {
                    weightedPositions[newRow][newColumn] = currentWeight + 1;
                    queue.Enqueue(TTuple<int, int>(newRow, newColumn));
                    count++;
                }
            }
        }
    }
    
    return weightedPositions;
}

void ADungeonGenerator::GenerateDoors(TArray<TArray<UEntropyCell*>>& grid)
{
    TArray<FVector> doorLocations{};

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            UEntropyCell* cell = grid[i][j];
            if (!cell) continue;

            FVector roomPosition = cell->position; // assuming each room is 10x10 units

            ARoom* room = Cast<ARoom>(cell->finalRoom);

            if (!room) continue;
            
            TArray<ADoorData*> doorDatas;

            for (int d = 0; d < 4; d++)
            {
                
                
                if (((Cast<ARoom>(cell->finalRoom ? cell->finalRoom->GetDefaultObject() : nullptr) ? Cast<ARoom>(cell->finalRoom->GetDefaultObject())->GetDoorHash() : 0 >> d) & 1) == 1)
                {
                    FVector doorPos = FVector::Zero();

                    switch (d)
                    {
                    case 0: // Forward / top
                        doorPos = FVector::ForwardVector / 2.0f;
                        break;
                    case 1: // Right
                        doorPos = FVector::RightVector / 2.0f;
                        break;
                    case 2: // Back / bottom
                        doorPos = FVector::BackwardVector / 2.0f;
                        break;
                    case 3: // Left
                        doorPos = FVector::LeftVector / 2.0f;
                        break;
                    default:
                        break;
                    }

                    FVector worldDoorPos = doorPos * roomSize + roomPosition;
                    worldDoorPos = FVector(worldDoorPos.Y, worldDoorPos.X, worldDoorPos.Z); // Swap X and Y to match Unreal's coordinate system
                    if (doorLocations.Contains(worldDoorPos))
                    {
                        continue;
                    }

                    doorLocations.Add(worldDoorPos);
                    
                    if (!doorPrefab) continue;
                    ADoorData* doorActor = GetWorld()->SpawnActor<ADoorData>(doorPrefab.Get(), worldDoorPos, FRotator::ZeroRotator);
                    if (!doorActor) continue;

                    doorActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
                    doorActor->SetActorRotation(cell->rotation + FRotator(0, 0, d * 90.0f));
                    doorActor->SetActorScale3D(FVector(roomSize / 100.0f, roomSize / 100.0f, roomSize / 100.0f));
                    doorDatas.Add(doorActor);
                }
            }
            cell->doorDatas = doorDatas;
        }
    }
}

void ADungeonGenerator::GenerateRooms(TArray<TArray<UEntropyCell*>>& grid)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            UEntropyCell* cell = grid[i][j];
            if (!cell) continue;

            if (cell->finalRoom == nullptr)
            {
                UE_LOG(LogTemp, Error, TEXT("Cell [%d,%d] has no finalRoom"), i, j);
                continue;
            }

            UClass* roomClass = cell->finalRoom.Get();
            if (!roomClass)
            {
                UE_LOG(LogTemp, Error, TEXT("Invalid room class at [%d,%d]"), i, j);
                continue;
            }

            FVector position = cell->position * roomSize; // assuming each room is roomSize x roomSize units
            position = FVector(position.Y, position.X, position.Z); // Swap X and Y to match Unreal's coordinate system

            FActorSpawnParameters params;
            params.Owner = this;

            // Spawn the actual ARoom instance from the TSubclassOf
            ARoom* spawnedRoom = GetWorld()->SpawnActor<ARoom>(roomClass, position, cell->rotation, params);
            
            if (!spawnedRoom)
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to spawn room at [%d,%d]"), i, j);
                continue;
            }

            spawnedRoom->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
            spawnedRoom->SetActorScale3D(FVector(roomSize / 1000.0f, roomSize / 1000.0f, roomSize / 1000.0f));
            spawnedRoom->EntropyCell = cell;
        }
    }
}
