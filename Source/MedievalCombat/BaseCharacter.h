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
	/* **************************** Variables **************************** */

	/* ***** Base Variables ("Vanilla" in blueprints) ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vanilla)
		bool Invincible = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Vanilla)
		bool IsDead = false;

	/* ***** Health Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
		float Health = 100.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
		float MaxHealth = 100.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
		float KnockbackValue = 0;

	/* ***** Attack Handler Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackHandler)
		bool CanDamage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		bool CanAttack = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		bool CurrentAttackHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackHandler)
		float CurrentDamage = 0;

	/* ***** Block Handler Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		bool IsBlocking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		bool AttackWasBlocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		bool SuccessfullyDefended = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		bool RecoilAnim = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		bool BlockingAnimation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		bool HoldingBlock = false;

	/* ***** Flinch Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Flinch)
		float FlinchDuration = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Flinch)
		bool FlinchTrigger = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Flinch)
		bool Flinched = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flinch)
		bool CantFlinch = false;

	/* ***** Roll Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Roll)
		bool RollAnim = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Roll)
		bool IsRolling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Roll)
		float RollSpeed = 1.0;

	/* ***** Resilience Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Resilience)
		float Resilience = 100.0;

	/* ***** Other Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class HPOverhead* HPOverhead;

	/* **************************** Functions **************************** */

	//Decrements cooldown by .1 every time called, if cd > 0
	UFUNCTION(BlueprintCallable)
		void CooldownDecrement(UPARAM(ref) float cd, UPARAM(ref) FTimerHandle& Handle);

	/* **************************** Other Functions **************************** */

	/** Timer Function to replicate DELAY in Blueprints */
	UFUNCTION()
		void onTimerEnd();

private:
	FTimerHandle delayTimerHandle;
};

