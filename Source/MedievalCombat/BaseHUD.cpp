// Fill out your copyright notice in the Description page of Project Settings.

#include "MedievalCombat.h"
#include "BaseHUD.h"

ABaseHUD::ABaseHUD(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Every time a new object appears it now runs this
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "debug msg");
}


