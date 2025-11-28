// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/Gameplay/Node.h"

// Sets default values
ANode::ANode() : ANode(0,0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

ANode::ANode(int x, int y) : coordX(x), coordY(y)
{
	bIsNodeSet = false;
}


// Called when the game starts or when spawned
void ANode::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

