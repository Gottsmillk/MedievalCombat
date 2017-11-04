// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageNumberDisplay.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALCOMBAT_API UDamageNumberDisplay : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D InitialScreenLoc;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* Target;
};
