// Fill out your copyright notice in the Description page of Project Settings.

#include "MedievalCombat.h"
#include "Revenant.h"

ARevenant::ARevenant()
{
	// Create shield
	Shield = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shield"));
	Shield->SetVisibility(true);
	Shield->SetHiddenInGame(false);
	Shield->bGenerateOverlapEvents = false;
	FName ShieldSocket = TEXT("Shieldsocket");
	Shield->SetupAttachment(GetMesh(), ShieldSocket);
}


