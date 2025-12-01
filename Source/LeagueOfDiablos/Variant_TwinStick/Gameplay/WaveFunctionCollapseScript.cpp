// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/Gameplay/WaveFunctionCollapseScript.h"

/// <summary>
/// Uses wave function collapse algorithm to generate a grid of rooms
/// </summary>
void UWaveFunctionCollapseScript::WaveFunctionCollapse(TArray<TArray<UEntropyCell*>>& grid)
{
    nextCollapse.Empty();

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            UEntropyCell* cell = grid[i][j];
            if (cell->IsCollapsed())
            {
                UpdateNeighbors(grid, cell);
            }
        }
    }
    
    int maxIterations = (columns-1) * (rows-1) * 10;
    while (nextCollapse.Num() > 0 && maxIterations > 0)
    {
        maxIterations--;
        int index = FMath::RandRange(0, nextCollapse.Num()-1);
        UEntropyCell* cell = nextCollapse[index];
        nextCollapse.RemoveAt(index);

        if (cell->IsCollapsed())
        {
            continue;
        }
        
        UpdatePossibleRooms(grid, cell);

        if (cell->IsCollapsed()) continue;
        
        
        CollapseCell(grid, cell);
        
        UpdateNeighbors(grid, cell);
    }
}

/// <summary>
/// Collapse the cell to the first possible room that fits with neighbors
/// </summary>
void UWaveFunctionCollapseScript::CollapseCell(TArray<TArray<UEntropyCell*>>& grid, UEntropyCell* cell)
{
    if (!cell) return;
    
    if (cell->IsCollapsed())
    {
        return;
    }

    if (!roomsMap.Contains(cell->roomType))
    {
        UE_LOG(LogTemp, Error, TEXT("Error: No rooms available for room type %d"), cell->roomType);
        return;
    }
    if (roomsMap[cell->roomType].rooms.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Error: Room list is empty for room type %d"), cell->roomType);
        return;
    }
    TArray<TSubclassOf<ARoom>> currentPossibleRooms = roomsMap[cell->roomType].rooms;
    
    //filter rooms with less doors than neighbors
    int neighborsDoors = (cell->GetDoorHash() & 1) + ((cell->GetDoorHash() & 2) >> 1) + ((cell->GetDoorHash() & 4) >> 2) + ((cell->GetDoorHash() & 8) >> 3);
    currentPossibleRooms.RemoveAll([neighborsDoors](const TSubclassOf<ARoom>& r)
    {
        ARoom* room = r ? Cast<ARoom>(r->GetDefaultObject()) : nullptr;
        return room && room->GetDoorCount() < neighborsDoors;
    });
    
    //find the orientation that fits with neighbors
    bool found = false;

    for (int _ = 0; _ < currentPossibleRooms.Num(); _++)
    {
        int index = FMath::RandRange(0, currentPossibleRooms.Num()-1);
        
        if (TrySetRoomRotation(grid, cell, currentPossibleRooms[index]))
        {
            cell->finalRoom = currentPossibleRooms[index];
            found = true;
            break;
        }

        currentPossibleRooms.RemoveAt(index);
    }
    
    if (!found){
        return; //no room can fit here
    }
    
    nextCollapse.RemoveAll([cell](const UEntropyCell* c)
    {
        return c == cell;
    });
}

bool UWaveFunctionCollapseScript::TrySetRoomRotation(TArray<TArray<UEntropyCell*>>& grid, UEntropyCell* cell, const TSubclassOf<ARoom>& room) const
{
    if (!room || !cell)
    {
        return false;
    }

    ARoom* roomCDO = Cast<ARoom>(room->GetDefaultObject());

    if (!roomCDO)
    {
        UE_LOG(LogTemp, Log, TEXT("Error: Room CDO is null"));
        return false;
    }
    
    BYTE neighborDoorHash;
    
    if (!cell->isPreGenerated)
    {
        neighborDoorHash = GetNeighborsExists(grid, cell);
    }
    else
    {
        neighborDoorHash = GetPreGenerationNeighborsExists(grid, cell);
    }
    
    for (int i = 0; i < 360; i += 90) //check every rotation
    {
        BYTE rotatedDoorHash = roomCDO->GetDoorHash();

        //rotate door hash
        int rotations = i / 90;
        for (int r = 0; r < rotations; r++)
        {
            rotatedDoorHash =
                static_cast<BYTE>(((rotatedDoorHash & 1) << 3) | (rotatedDoorHash >> 1)); //rotate bits to the right
        }
        
        if ((rotatedDoorHash & neighborDoorHash) ==
            (cell->GetDoorHash() & neighborDoorHash)) // all existing neighbor doors are matched
        {
            //this room can fit with this rotation
            cell->rotation = FRotator(0, -i, 0);
            cell->SetDoorHash(rotatedDoorHash);
            return true;
        }
    }
    return false;
}

int UWaveFunctionCollapseScript::GetNeighborsDoors(TArray<TArray<UEntropyCell*>>& grid, const FVector& pos) const
{
    int doors = 0;
    //top
    if (pos.Y < rows - 2)
    {
        if (grid[pos.Y + 1][pos.X]->IsCollapsed())
        {
            doors += grid[pos.Y + 1][pos.X]->doorBottom ? 1 : 0;
        }
    }
    else
    {
        doors += 1;
    }
    //bottom
    if (pos.Y > 0)
    {
        if (grid[pos.Y - 1][pos.X]->IsCollapsed())
        {
            doors += grid[pos.Y - 1][pos.X]->doorTop ? 1 : 0;
        }
    }
    else
    {
        doors += 1;
    }
    //left
    if (pos.X > 0)
    {
        if (grid[pos.Y][pos.X - 1]->IsCollapsed())
        {
            doors += grid[pos.Y][pos.X - 1]->doorRight ? 1 : 0;
        }
    }
    else
    {
        doors += 1;
    }
    //right
    if (pos.X < columns - 2)
    {
        if (grid[pos.Y][pos.X + 1]->IsCollapsed())
        {
            doors += grid[pos.Y][pos.X + 1]->doorLeft ? 1 : 0;
        }
    }
    else
    {
        doors += 1;
    }

    return doors;
}

BYTE UWaveFunctionCollapseScript::GetNeighborsDoorHash(TArray<TArray<UEntropyCell*>>& grid, const UEntropyCell* cell) const
{
    if (!cell) return 0;
    
    BYTE doors = 0;
    FVector pos = cell->position;
    //top
    if (pos.Y < rows - 1)
    {
        if (grid[pos.Y + 1][pos.X]->IsCollapsed())
        {
            doors |= static_cast<BYTE>(grid[pos.Y + 1][pos.X]->doorBottom ? 1 : 0);
        }
    }
    //right
    if (pos.X < columns - 1)
    {
        if (grid[pos.Y][pos.X + 1]->IsCollapsed())
        {
            doors |= static_cast<BYTE>(grid[pos.Y][pos.X + 1]->doorLeft ? 2 : 0);
        }
    }
    //bottom
    if (pos.Y > 0)
    {
        if (grid[pos.Y - 1][pos.X]->IsCollapsed())
        {
            doors |= static_cast<BYTE>(grid[pos.Y - 1][pos.X]->doorTop ? 4 : 0);
        }
    }
    //left
    if (pos.X > 0)
    {
        if (grid[pos.Y][pos.X - 1]->IsCollapsed())
        {
            doors |= static_cast<BYTE>(grid[pos.Y][pos.X - 1]->doorRight ? 8 : 0);
        }
    }

    return doors;
}

BYTE UWaveFunctionCollapseScript::GetNeighborsExists(TArray<TArray<UEntropyCell*>>& grid, const UEntropyCell* cell) const
{
    if (!cell) return 0;
    
    BYTE neighbors = 0;
    FVector pos = cell->position;
    
    //top
    if (pos.Y < rows - 1)
    {
        if (grid[pos.Y + 1][pos.X]->IsCollapsed())
        {
            neighbors |= 1;
        }
    }
    else
    {
        neighbors |= 1;
    }
    //right
    if (pos.X < columns - 1)
    {
        if (grid[pos.Y][pos.X + 1]->IsCollapsed())
        {
            neighbors |= 2;
        }
    }
    else
    {
        neighbors |= 2;
    }
    //bottom
    if (pos.Y > 0)
    {
        if (grid[pos.Y - 1][pos.X]->IsCollapsed())
        {
            neighbors |= 4;
        }
    }
    else
    {
        neighbors |= 4;
    }
    //left
    if (pos.X > 0)
    {
        if (grid[pos.Y][pos.X - 1]->IsCollapsed())
        {
            neighbors |= 8;
        }
    }
    else
    {
        neighbors |= 8;
    }

    return neighbors;
}

BYTE UWaveFunctionCollapseScript::GetPreGenerationNeighborsExists(TArray<TArray<UEntropyCell*>>& grid, const UEntropyCell* cell) const
{
    if (!cell) return 0;
    
    BYTE neighbors = 0;
    FVector pos = cell->position;
    
    //top
    if (pos.Y < rows - 1)
    {
        if (grid[pos.Y + 1][pos.X]->isPreGenerated)
        {
            neighbors |= 1;
        }
    }
    else
    {
        neighbors |= 1;
    }
    //right
    if (pos.X < columns - 1)
    {
        if (grid[pos.Y][pos.X + 1]->isPreGenerated)
        {
            neighbors |= 2;
        }
    }
    else
    {
        neighbors |= 2;
    }
    //bottom
    if (pos.Y > 0)
    {
        if (grid[pos.Y - 1][pos.X]->isPreGenerated)
        {
            neighbors |= 4;
        }
    }
    else
    {
        neighbors |= 4;
    }
    //left
    if (pos.X > 0)
    {
        if (grid[pos.Y][pos.X - 1]->isPreGenerated)
        {
            neighbors |= 8;
        }
    }
    else
    {
        neighbors |= 8;
    }

    return neighbors;
}

void UWaveFunctionCollapseScript::UpdatePossibleRooms(TArray<TArray<UEntropyCell*>>& grid, UEntropyCell* cell)
{
    if (!cell) return;
    
    //Update cell doors based on neighbors
    cell->SetDoorHash(GetNeighborsDoorHash(grid, cell));
    
    nextCollapse.AddUnique(cell);
    
    if (GetNeighborsExists(grid, cell) == 0b1111) // all neighbors exist
    {
        CollapseCell(grid, cell); //only one possibility left
    }
    
    if (cell->IsCollapsed())
    {
        UpdateNeighbors(grid, cell);
    }
}

void UWaveFunctionCollapseScript::UpdateNeighbors(TArray<TArray<UEntropyCell*>>& grid, const UEntropyCell* cell)
{
    if (!cell) return;
    
    UEntropyCell* neighbor;

    if (cell->position.Y < rows - 1) // Top neighbor
    {
        neighbor = grid[cell->position.Y + 1][cell->position.X];
        if (!neighbor->IsCollapsed())
        {
            neighbor->doorBottom = true;
            UpdatePossibleRooms(grid, neighbor);
        }
    }
    if (cell->position.Y > 0) // Bottom neighbor
    {
        neighbor = grid[cell->position.Y - 1][cell->position.X];
        if (!neighbor->IsCollapsed())
        {
            neighbor->doorTop = true;
            UpdatePossibleRooms(grid, neighbor);
        }
    }
    if (cell->position.X < columns - 1) // Right neighbor
    {
        neighbor = grid[cell->position.Y][cell->position.X + 1];
        if (!neighbor->IsCollapsed())
        {
            neighbor->doorLeft = true;
            UpdatePossibleRooms(grid, neighbor);
        }
    }
    if (cell->position.X > 0) // Left neighbor
    {                
        neighbor = grid[cell->position.Y][cell->position.X - 1];
        if (!neighbor->IsCollapsed())
        {
            neighbor->doorRight = true;
            UpdatePossibleRooms(grid, neighbor);
        }
    }
}

TArray<TArray<UEntropyCell*>> UWaveFunctionCollapseScript::InitializeEmptyGrid(int nbRows, int nbColumns)
{
    TArray<TArray<UEntropyCell*>> grid;
    grid.Reserve(nbRows);
    for (int i = 0; i < nbRows; i++)
    {
        grid.AddDefaulted();
        grid[i].Reserve(nbColumns);
        for (int j = 0; j < nbColumns; j++)
        {
            UEntropyCell* cell = NewObject<UEntropyCell>(this);
            if (cell)
            {
                cell->Initialize(FVector(j, i, 0), 0);
            }
            grid[i].Add(cell);
        }
    }
    return grid;
}

void UWaveFunctionCollapseScript::ForcePathsRooms(TArray<TArray<UEntropyCell*>>& grid, TArray<TArray<TTuple<int,int>>> paths)
{
    for (auto& path : paths)// first pass to add doors to tiles in paths
    {
        TArray tempPath = path;

        TTuple<int,int> previous = TTuple<int,int>(-1,-1);
        TTuple<int,int> current = tempPath.Pop();
        TTuple<int,int> next = tempPath.Pop();
        
        for (int i = 0; i < path.Num(); i++)
        {
            UEntropyCell* cell = grid[current.Key][current.Value];
            cell->isPreGenerated = true;

            if (i != 0)
            {
                //determine direction from previous to current
                if (previous.Key > current.Key) // came from top
                {
                    cell->doorTop = true; // add top door
                }
                else if (previous.Value > current.Value) // came from right
                {
                    cell->doorRight = true; // add right door
                }
                else if (previous.Key < current.Key) // came from bottom
                {
                    cell->doorBottom = true; // add bottom door
                }
                else if (previous.Value < current.Value) // came from left
                {
                    cell->doorLeft = true; // add left door
                }
            }

            if (i == path.Num() - 1) break; // Skips the check for next because it's the end
            
            //determine direction from current to next
            if (next.Key > current.Key) // going to top
            {
                cell->doorTop = true; // add top door
            }
            else if (next.Value > current.Value) // going to right
            {
                cell->doorRight = true; // add right door
            }
            else if (next.Key < current.Key) // going to bottom
            {
                cell->doorBottom = true; // add bottom door
            }
            else if (next.Value < current.Value) // going to left
            {
                cell->doorLeft = true; // add left door
            }
            
            previous = current;
            current = next;
            
            if (i < path.Num() - 2) //skips the second to last
            {
                next = tempPath.Pop();
            }
        }
    }
    
    for (auto& path : paths)// second pass to collapse cells in paths
    {
        for (int i = 0; i < path.Num(); ++i)
        {
            if (path[i].Key < 0 || path[i].Value < 0) continue;
            if (UEntropyCell* cell = grid[path[i].Key][path[i].Value])
            {
                CollapseCell(grid, cell);
            }
        }
    }
}