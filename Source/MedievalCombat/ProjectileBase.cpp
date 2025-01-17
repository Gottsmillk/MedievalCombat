// Fill out your copyright notice in the Description page of Project Settings.

#include "MedievalCombat.h"
#include "ProjectileBase.h"


// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileBase::SetFX()
{
	FOutputDeviceNull ar;
	this->CallFunctionByNameWithArguments(TEXT("CallFunctionTest"), ar, NULL, true);
}
