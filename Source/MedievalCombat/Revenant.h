#pragma once

#include "BaseCharacter.h"
#include "Revenant.generated.h"

UCLASS()
class MEDIEVALCOMBAT_API ARevenant : public ABaseCharacter
{
	GENERATED_BODY()

	/* ---------------------------------- COMPONENTS ---------------------------------- */

	// Shield Mesh object
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* Shield;

	// Hurtbox for Countering Blow
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* CounteringBlowHurtbox;

	// Hurtbox for Staggering Kick
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* KickHurtbox;
	
	// Hurtbox for Powered Bash
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* PoweredBashHurtbox;

public:

	/* ------------------------------- INITIALIZATIONS -------------------------------- */

	// Sets default values for this character's properties
	ARevenant();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* ---------------------------------- VARIABLES ----------------------------------- */

	/* ********************** Animation Variables ********************** */

	// Stores Attack Animations for SBasicAttack, animations are set in the blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		TArray<UAnimMontage *> SBasicAttackAnims;

	// Stores Attack Animations for HBasicAttack, animations are set in the blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		TArray<UAnimMontage *> HBasicAttackAnims;

	/* **************************** UI Variables ***************************** */

	// Converts abilities from int value from UI to corresponding FString (Workaround for UE4 issue)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
		TArray<FString> AbilityConversionArray;

	/* ********************** Attack Effect Variables ********************** */

	// Impair effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		bool ImpairActive = false;

	// Instantiated Shadow blueprint for Shadow Stance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
		TSubclassOf<ABaseCharacter> Shadow;

	/* ********************** Attack GFX Variables ********************** */

	// Afterimage effect for Agility
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		bool AgilityEffect = false;

	// Unwaver FX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackHandler)
		TSubclassOf<AActor> UnwaverFX;

	// Impair FX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackHandler)
		TSubclassOf<AActor> ImpairFX;

	// Fortify FX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackHandler)
		TSubclassOf<AActor> FortifyFX;

	/* ---------------------------------- FUNCTIONS ----------------------------------- */

	/* ************************ Animation Functions ************************* */

	// Load Basic Attack Animations into their arrays
	UFUNCTION()
		void LoadBasicAttackAnimations();

	// Gets random montage from animation array
	UFUNCTION()
		UAnimMontage* GetRandomMontage(TArray<UAnimMontage *> MontageArray);

	/* **************************** UI Functions **************************** */

	// Populate array that converts int values of attacks to FString
	UFUNCTION(BlueprintCallable)
		void SetConvertedAbilities();

	// Reset ability selected arrays each Character Select UI Launch
	UFUNCTION(BlueprintCallable)
		void ResetSelectedAbilityArrays();
	UFUNCTION(Server, Reliable, WithValidation)
		void ResetSelectedAbilityArraysServer();
	UFUNCTION()
		void ResetSelectedAbilityArraysClient();

	// Perfom final actions once "Submit" is pressed on the "Choose Attacks" UI
	UFUNCTION(BlueprintCallable)
		virtual void AddRemainingInputs() override;
	UFUNCTION(Server, Reliable, WithValidation)
		void AddRemainingInputsServer();
	UFUNCTION()
		void AddRemainingInputsClient();

	// Add Attack to character when attack is chosen in UI
	UFUNCTION(BlueprintCallable)
		void AddAttack(int Type, int AttackName, bool Toggled);
	UFUNCTION(Server, Reliable, WithValidation)
		void AddAttackServer(int Type, int AttackName, bool Toggled);
	UFUNCTION()
		void AddAttackClient(int Type, int AttackName, bool Toggled);

	// Set Cooldown amounts upon Submit press
	UFUNCTION(BlueprintCallable)
		float SetAttackCooldownAmt(FString AttackName);

	/* *********************** Damage/Death Functions *********************** */

	// Apply override so player who received damage can send events to player who dealt damage
	UFUNCTION(BlueprintCallable)
		virtual void SendEventToAttacker(ABaseCharacter* Attacker) override;

	/* ********************** Attack Effect Functions ********************** */

	// Function for detecting abilities
	UFUNCTION()
		virtual void DetectAction() override;
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		void TurnInvisibleRepAll();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		void DetectRepAll();

	// Unset SuperArmor for Unwaver
	UFUNCTION()
		void UnsetAntiFlinch();

	// Unset Impair effect
	UFUNCTION()
		void RemoveImpairActive();

	/* ********************** Attack Handler Functions ********************** */

	// Events executed for Hitbox Attacks
	UFUNCTION()
		void CounteringBlowHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void KickHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void PoweredBashHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// Function for applying attack effects to enemy
	UFUNCTION()
		virtual void AttackEffect(ABaseCharacter* Target, FString AttackName) override;

	// Master function for converting inputs into their corresponding ability events
	UFUNCTION(BlueprintCallable)
		virtual void AttackExecute(FString AttackName) override;
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void AttackExecuteClientToServer(const FString &AttackName) override;
	UFUNCTION()
		virtual void AttackExecuteServer(FString AttackName) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle InvisibilityDelayTimerHandle;
	FTimerHandle UnwaverDelayTimerHandle;
	FTimerHandle ImpairDelayTimerHandle;
};
