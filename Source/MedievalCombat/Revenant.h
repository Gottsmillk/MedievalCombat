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

	/* ********************** Attack Input Variables ********************** */

		FInputActionBinding BlockPressBind = FInputActionBinding(FName("Block"), IE_Pressed);
		FInputActionBinding BlockReleaseBind = FInputActionBinding(FName("Block"), IE_Released);
		FInputActionBinding RollBind = FInputActionBinding(FName("Roll"), IE_Pressed);
		FInputActionBinding SideStepBind = FInputActionBinding(FName("SideStep"), IE_Pressed);
		FInputActionBinding BasicAttackBind = FInputActionBinding(FName("BasicAttack"), IE_Pressed);
		FInputActionBinding HardAttackBind = FInputActionBinding(FName("HardAttack"), IE_Pressed);
		FInputActionBinding CounteringBlowBind = FInputActionBinding(FName("CounteringBlow"), IE_Pressed);
		FInputActionBinding ComboExtender1Bind = FInputActionBinding(FName("ComboExtender1"), IE_Pressed);
		FInputActionBinding ComboExtender2Bind = FInputActionBinding(FName("ComboExtender2"), IE_Pressed);
		FInputActionBinding ComboExtender3Bind = FInputActionBinding(FName("ComboExtender3"), IE_Pressed);
		FInputActionBinding Utility1Bind = FInputActionBinding(FName("Utility1"), IE_Pressed);
		FInputActionBinding Utility2Bind = FInputActionBinding(FName("Utility2"), IE_Pressed);
		FInputActionBinding ComboFinisher1Bind = FInputActionBinding(FName("ComboFinisher1"), IE_Pressed);
		FInputActionBinding ComboFinisher2Bind = FInputActionBinding(FName("ComboFinisher2"), IE_Pressed);


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

	// Normal Body Texture
	    UMaterial* BodyMeshMaterial;
 		UMaterialInstanceDynamic* BodyMeshMaterialInst;

	// Normal Weapon Texture
	    UMaterial* WeaponMeshMaterial;
 		UMaterialInstanceDynamic* WeaponMeshMaterialInst;

	// Normal Shield Texture
	    UMaterial* ShieldMeshMaterial;
 		UMaterialInstanceDynamic* ShieldMeshMaterialInst;
	
	// Shadow Texture
	    UMaterial* ShadowMaterialComboExtender;
 		UMaterialInstanceDynamic* ShadowMaterialBodyInstComboExtender;
 		UMaterialInstanceDynamic* ShadowMaterialWeaponInstComboExtender;
 		UMaterialInstanceDynamic* ShadowMaterialShieldInstComboExtender;

	    UMaterial* ShadowMaterialComboFinisher;
 		UMaterialInstanceDynamic* ShadowMaterialBodyInstComboFinisher;
 		UMaterialInstanceDynamic* ShadowMaterialWeaponInstComboFinisher;
 		UMaterialInstanceDynamic* ShadowMaterialShieldInstComboFinisher;

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
		virtual void SendEventToAttacker(ABaseCharacter* Attacker) override;

	/* ********************** Attack Effect Functions ********************** */

	// Function for detecting abilities
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
		void RemoveImpairEffect();

	// Unset Effect
	UFUNCTION()
		void RemoveEffect();

	/* ********************** Attack Handler Functions ********************** */

	// Allowing the next Queued Attack to execute
		virtual void QueueAttackHandler() override;
		
	UFUNCTION(Server, Reliable, WithValidation)
		void QueueAttackHandlerToServer();
	UFUNCTION()
		void QueueAttackHandlerServer();


	// Allowing the next Queued Attack to execute
		virtual void QueueAttack(FString AttackName) override;

	// Events executed for Hitbox Attacks
	UFUNCTION()
		void WeaponHurtboxBaseOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void CounteringBlowHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void KickHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void PoweredBashHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// Function for applying attack effects to enemy
		virtual void AttackEffect(ABaseCharacter* Target, FString AttackName) override;

	// Master function for converting inputs into their corresponding ability events
	UFUNCTION(BlueprintCallable)
		void AttackExecute(FString AttackName);
	UFUNCTION(Server, Reliable, WithValidation)
		void AttackExecuteClientToServer(const FString &AttackName);
	UFUNCTION()
		void AttackExecuteServer(FString AttackName);

	// Attack Handler
	UFUNCTION(BlueprintCallable)
		void AttackHandler(
			FString AttackName,
			FString AttackType,
			float CastCooldownAmt,
			float CastSpeed,
			bool IsChainable,
			UAnimMontage* Animation,
			float DelayBeforeHitbox,
			float LengthOfHitbox,
			float Damage,
			bool UseHitbox,
			UBoxComponent* Hitbox,
			bool Projectile,
			int Effect
		);
	UFUNCTION()
		void AttackHandler2(FString AttackName, FString AttackType, float CastCooldownAmt, float LengthOfHitbox, float Damage, bool UseHitbox, UBoxComponent* Hitbox, bool Projectile);
		virtual void AttackHandler3(FString AttackName, FString AttackType, float CastCooldownAmt, bool UseHitbox, UBoxComponent* Hitbox) override;

	// Attack Effect Handler Start
		virtual void AttackEffectHandlerStart(int StencilValue) override;

	// Attack Effect Handler End
		virtual void AttackEffectHandlerEnd(int StencilValue) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle InvisibilityDelayTimerHandle;
	FTimerHandle UnwaverDelayTimerHandle;
	FTimerHandle ImpairDelayTimerHandle;
	FTimerHandle FortifyDelayTimerHandle;
};
