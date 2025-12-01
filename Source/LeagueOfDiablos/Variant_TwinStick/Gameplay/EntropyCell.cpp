// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/Gameplay/EntropyCell.h"

UEntropyCell::UEntropyCell()
{
	position = FVector::Zero();
	rotation = FRotator::ZeroRotator;
	finalRoom = nullptr;
	roomType = Normal;
	doorTop = doorRight = doorBottom = doorLeft = false;
	doorDatas.Empty();
}

