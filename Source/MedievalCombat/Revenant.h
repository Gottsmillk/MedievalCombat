#pragma once

#include "BaseCharacter.h"
#include "WidgetComponent.h"
#include "Revenant.generated.h"

UCLASS()
class MEDIEVALCOMBAT_API ARevenant : public ABaseCharacter
{
	GENERATED_BODY()

	/** Shield Mesh object */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* Shield;

	/** Hurtbox for Countering Blow */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* CounteringBlowHurtbox;

	/** Hurtbox for Staggering Kick */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* KickHurtbox;
	
	/** Hurtbox for Powered Bash */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* PoweredBashHurtbox;

public:
	// Sets default values for this character's properties
	ARevenant();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		bool AgilityEffect = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		bool ImpairActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UWidgetComponent * HPOverhead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
		TSubclassOf<ABaseCharacter> Shadow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackHandler)
		TSubclassOf<AActor> UnwaverFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackHandler)
		TSubclassOf<AActor> ImpairFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackHandler)
		TSubclassOf<AActor> FortifyFX;

	// Stores Attack Animations for SBasicAttack, animations are set in the blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		TArray<UAnimMontage *> SBasicAttackAnims;

	// Stores Attack Animations for HBasicAttack, animations are set in the blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		TArray<UAnimMontage *> HBasicAttackAnims;

	/** Called to bind functionality to input */
		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Reset ability selected arrays */
	UFUNCTION(BlueprintCallable)
		void ResetSelectedAbilityArrays();
	UFUNCTION(Server, Reliable, WithValidation)
		void ResetSelectedAbilityArraysServer();
	UFUNCTION()
		void ResetSelectedAbilityArraysClient();


	/** Add Remaining Inputs */
	UFUNCTION(BlueprintCallable)
		virtual void AddRemainingInputs() override;

	UFUNCTION(Server, Reliable, WithValidation)
		void AddRemainingInputsServer();

	UFUNCTION()
		void AddRemainingInputsClient();

	/** Load Basic Attack Animations into their arrays */
	UFUNCTION()
		void LoadBasicAttackAnimations();

	/** Gets random montage */
	UFUNCTION()
		UAnimMontage* GetRandomMontage(TArray<UAnimMontage *> MontageArray);

	/** Hitbox Events */
	UFUNCTION()
		void CounteringBlowHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void KickHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void PoweredBashHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** Overriding Attack Effect Handler */
	UFUNCTION()
		virtual void AttackEffect(ABaseCharacter* Target, FString AttackName) override;

	/** Overriding Attack Execute */
	UFUNCTION(BlueprintCallable)
		virtual void AttackExecute(FString AttackName) override;

	UFUNCTION(Server, Reliable, WithValidation)
		virtual void AttackExecuteClientToServer(const FString &AttackName) override;

	UFUNCTION()
		virtual void AttackExecuteServer(FString AttackName) override;

	/** Function for detecting abilities */
	UFUNCTION()
		virtual void DetectAction() override;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		void TurnInvisibleRepAll();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		void DetectRepAll();

	/** Add Attacks */

	UFUNCTION(BlueprintCallable)
		void AddAttack(FString Type, FString AttackName, bool Toggled);

	UFUNCTION(Server, Reliable, WithValidation)
		void AddAttackServer(const FString &Type, const FString &AttackName, bool Toggled);

	UFUNCTION()
		void AddAttackClient(FString Type, FString AttackName, bool Toggled);

	UFUNCTION(BlueprintCallable)
		float SetAttackCooldownAmt(FString AttackName);

	/** Unset SuperArmor for Unwaver */
	UFUNCTION()
		void UnsetAntiFlinch();

	/** Apply override so player who received damage can send events to player who dealt damage */
	UFUNCTION(BlueprintCallable)
		virtual void SendEventToAttacker(ABaseCharacter* Attacker) override;

	UFUNCTION()
		void RemoveImpairActive();

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	FTimerHandle InvisibilityDelayTimerHandle;
	FTimerHandle UnwaverDelayTimerHandle;
	FTimerHandle ImpairDelayTimerHandle;
};
