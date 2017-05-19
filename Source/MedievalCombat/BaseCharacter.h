// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class MEDIEVALCOMBAT_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/* **************************** All Variables, comments preceed each category **************************** */

	/* ***** Base Variables ("Vanilla" in blueprints) ***** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Default)
		bool invincible = false;

	/** Timer Function to replicate DELAY in Blueprints */
	UFUNCTION()
		void onTimerEnd();

	/** Handles block events called at every tick */
	UFUNCTION()
		void blockHandler2();

private:
	FTimerHandle delayTimerHandle;
};

