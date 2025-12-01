#pragma once
#include "DoorData.h"
#include "Room.h"
#include "RoomType.h"
#include "EntropyCell.generated.h"

UCLASS()
class UEntropyCell : public UObject
{
	GENERATED_BODY()
	
public:
	FVector position;
	FRotator rotation;

	UPROPERTY()
	TSubclassOf<ARoom> finalRoom;

	ERoomType roomType;

	bool doorTop = false;
	bool doorRight = false;
	bool doorBottom = false;
	bool doorLeft = false;

	bool isPreGenerated = false;
	
	UPROPERTY()
	TArray<ADoorData*> doorDatas;

	BYTE GetDoorHash() const
	{
		BYTE hash = 0;
		if (doorTop) hash |= 1 << 0;
		if (doorRight) hash |= 1 << 1;
		if (doorBottom) hash |= 1 << 2;
		if (doorLeft) hash |= 1 << 3;
		return hash;
	}

	void SetDoorHash(BYTE value)
	{
		doorTop = (value & 1) != 0;
		doorRight = (value & 2) != 0;
		doorBottom = (value & 4) != 0;
		doorLeft = (value & 8) != 0;
	}

	bool IsCollapsed() const
	{
		return finalRoom != nullptr;
	}

	UEntropyCell();
	
	void Initialize(const FVector& InPosition, BYTE Hash)
	{
		position = InPosition;
		rotation = FRotator::ZeroRotator;
		SetDoorHash(Hash);
		finalRoom = nullptr;
		roomType = Normal;
		doorTop = doorRight = doorBottom = doorLeft = false;
		doorDatas.Empty();
	}
};