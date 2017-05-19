// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "ClassSelection.generated.h"

UCLASS()
class MEDIEVALCOMBAT_API AClassSelection : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AClassSelection();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Bool to check if Sword Class is picked */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Default)
		bool SwordClassPicked = false;

	/** Bool to check if Sword and Shield Class is picked */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Default)
		bool SwordAndShieldClassPicked = false;

	/** Timer Function to replicate DELAY in Blueprints */
	UFUNCTION()
		void onTimerEnd();

private:
	FTimerHandle delayTimerHandle;
};
