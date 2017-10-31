#pragma once

#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectileBase.h"
#include "BaseCharacter.generated.h"

/* ***** STRUCTS ***** */
/* Example of a struct */
USTRUCT(BlueprintType)
struct FSpeedModifierStruct {
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float Modifier;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float Cooldown;

		FSpeedModifierStruct(float Modifier2 = 0.0f, float SpeedCooldown2 = 0.0f) {
			Modifier = Modifier2;
			Cooldown = SpeedCooldown2;
		}
}; 

USTRUCT(BlueprintType)
struct FAttackModifierStruct {
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float Modifier;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float Cooldown;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			int NumHits;
};

USTRUCT(BlueprintType)
struct FDefenseModifierStruct {
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float Modifier;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float Cooldown;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			int NumHits;
};

USTRUCT(BlueprintType)
struct FCooldownEffectsStruct {
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float Modifier;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			FString AbilityName;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float Duration;
};

USTRUCT(BlueprintType)
struct FDamageTableStruct {
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			FString Attack;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float Damage;

		FDamageTableStruct(FString Attack2 = "", float Damage2 = 0.0f) {
			Attack = Attack2;
			Damage = Damage2;
		}
};

USTRUCT(BlueprintType)
struct FAttackStruct {
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			FString AttackName;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float Cooldown;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float CooldownAmt;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float Damage;

		FAttackStruct(FString AttackName2 = "", float Cooldown2 = 0.0f, float CooldownAmt2 = 0.0f) {
			AttackName = AttackName2;
			Cooldown = Cooldown2;
			CooldownAmt = CooldownAmt2;
		}
};

UCLASS(config = Game)
class MEDIEVALCOMBAT_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character (SpringArm) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera (FirstPersonCamera) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** Direction camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* DirectionCamera;

	/** Capsule component for player collision (Player) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* PlayerAimCollision;

	/** Capsule component for player collision (Player) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* PlayerCollision;
	
	/** Capsule component for player collision (Player) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* PlayerCollision2;

	/** Weapon Mesh object (BasicSword) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* Weapon;

	/** Weapon hurtbox base (BasicSwordHurtbox followed by BasicSword1-BasicSword5) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* WeaponHurtboxBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* Hurtbox1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* Hurtbox2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* Hurtbox3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* Hurtbox4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* Hurtbox5;

public:
	/** Sets default values for this character's properties */
	ABaseCharacter();

	/* **************************** Variables **************************** */
	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	/** Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	/** Add Remaining Inputs */
	UFUNCTION(BlueprintCallable)
		virtual void AddRemainingInputs();

	/* ***** Component Variables ***** */

	/** Returns CameraBoom subobject */
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject */
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vanilla)
		UCharacterMovementComponent* CharMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterInfo)
		TSubclassOf <UCameraShake> PlayerTakeDamageCameraShake;

	/* ***** Base Variables ("Vanilla" in blueprints) ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vanilla)
		bool Invincible = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Vanilla)
		bool IsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vanilla)
		float CurrentGameTime = 0.0f;

	/* ***** Hitbox Variables ***** */

	UPROPERTY(EditAnywhere, Category = Hitbox)
		UShapeComponent* HitboxComponentArray[6];

	UPROPERTY(EditAnywhere, Category = Hitbox)
		FVector HitboxArray[6];

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hitbox)
		bool InitialHitbox = true;

	/* ***** Health Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
		float Health = 100.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
		float MaxHealth = 100.0;

	/* ***** Attack Handler Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackHandler)
		bool CanDamage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		bool CanAttack = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		bool CurrentAttackHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackHandler)
		float CurrentDamage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		FString LastAttack = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackHandler)
		FVector MaxProjectileRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackHandler)
		TSubclassOf<AProjectileBase> BaseProjectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		FString CurrentAttackName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Cooldowns)
		TArray<FAttackModifierStruct> AttackModifierArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Cooldowns)
		TArray<FDefenseModifierStruct> DefenseModifierArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		TArray<FAttackStruct> AttackArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		TArray<FString> ComboExtenderArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		TArray<FString> UtilityArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		TArray<FString> ComboFinisherArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		TArray<FDamageTableStruct> DamageTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		bool DetectMode = false;

	/* ***** Block Handler Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		bool BlockPressed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		bool IsBlocking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		float BlockingAnim = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		bool SubResilience = false;

	/* ***** Flinch Variables ***** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Flinch)
		bool FlinchTrigger = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Flinch)
		bool Flinched = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Flinch)
		bool SuperArmor = false;

	/* ***** Roll/Sidestep Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = RollSidestep)
		bool IsRolling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = RollSidestep)
		bool RollMovement = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = RollSidestep)
		bool IsSideStepping = false;

	/* ***** Movement Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Movement)
		bool CanMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Movement)
		bool CanTurn = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Movement)
		float CurrentFBLoc = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Movement)
		float CurrentLRLoc = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Movement)
		bool Colliding = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Movement)
		bool Overlapping = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Movement)
		TArray<FSpeedModifierStruct> SpeedEffectsArray;

	/* ***** Damage Indicator Variables ***** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = DamageIndicator)
		float CurrentDamageIndicator = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = DamageIndicator)
		float MaxDamageIndicator = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = DamageIndicator)
		float MinDamageIndicator = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = DamageIndicator)
		float DesiredDamageIndicator = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = DamageIndicator)
		float DamageIndicatorSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = DamageIndicator)
		float LowHealthIndicatorPower = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = DamageIndicator)
		float DamageDeltaTime = 0;

	/* ***** Resilience Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Resilience)
		float Resilience = 100.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Resilience)
		float ResilienceDrainAmt = 4.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Resilience)
		float ResilienceRegenAmt = 4.0;

	/* ***** Cooldown Variables ***** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Cooldowns)
		float AttackCastCooldown = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Cooldowns)
		TArray<FCooldownEffectsStruct> CooldownEffectsArray;

	/* ***** Other Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Resilience)
		bool MenuUp = true;

	/* **************************** Functions **************************** */
	/** Make weapon visible */
	UFUNCTION()
		void WeaponVisibility(bool set);

	/** Damage Indicator per Tick Function */
	UFUNCTION()
		void DamageIndicatorTick();

	/** Adding Damage Effect */
	UFUNCTION()
		void InitiateDamageEffect();
	UFUNCTION(Server, Reliable, WithValidation)
		void InitiateDamageEffectServer();
	UFUNCTION(Client, Reliable)
		void InitiateDamageEffectClient();

	/** Hitbox Handler */
	UFUNCTION(BlueprintCallable)
		void HitboxHandler();

	/* When a character dies, now also including SwordCharacter's overwritten Server Death */
	UFUNCTION(BlueprintCallable)
		void ServerDeath();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerDeathServer();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		void ServerDeathRepAll();

	UFUNCTION()
		void RespawnEvent();

	/* On receiving any damage, will decrement health and if below or equal to zero, dies. Overridden function.*/
	UFUNCTION(BlueprintCallable)
		void ReceiveAnyDamage(float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
		void ApplyDamage(float Damage);

	/** Function for when an attack hits (SwordContactEvent) */
	UFUNCTION()
		void WeaponHitEvent(FHitResult HitResult);

	/** Function for applying weapon effects */
	UFUNCTION()
		virtual void AttackEffect(ABaseCharacter* Target, FString AttackName);

	/** Function for choosing attack */
	UFUNCTION(BlueprintCallable)
		virtual void AttackExecute(FString AttackName);

	UFUNCTION(Server, Reliable, WithValidation)
		virtual void AttackExecuteServer(const FString &AttackName);

	UFUNCTION()
		virtual void AttackExecuteClient(FString AttackName);

	/** Function for detecting abilities */
	UFUNCTION()
		virtual void DetectAction();

	/** Timer Function to replicate DELAY in Blueprints */
	UFUNCTION()
		void onTimerEnd();

	/** PlayerCollision2 begin overlap */
	UFUNCTION()
		void PlayerCollision2Begin(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** PlayerCollision2 end overlap */
	UFUNCTION()
		void PlayerCollision2End(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/* Example of multicasting */
	/** Function for playing an animation montage */
	UFUNCTION()
		void PlayActionAnim(UAnimMontage* Animation, float Speed, bool IsAttack);

	/** Function for server playing an animation montage */
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		void PlayActionAnimServer(UAnimMontage* Animation, float Speed, bool IsAttack); // Ignore green squiggle

	/** Function for finding if two players are within 90 degrees of facing eachother */
	UFUNCTION(BlueprintCallable)
		bool GetPlayerDirections(AActor * Attacked); 

	/** Function for checking if player should move during attack or stay still */
	UFUNCTION(BlueprintCallable)
		void CheckMoveDuringAttack();

	/** Helper function for Hitbox Handler */
	UFUNCTION()
		void FillHitboxArray();

	/** Helper function for Block animation */
	UFUNCTION(BlueprintCallable)
		void BlockAnimation();

	/** Movement Handler */
	UFUNCTION()
		void MovementHandler();

	UFUNCTION(Server, Reliable, WithValidation)
		void MovementHandlerServer();

	UFUNCTION()
		void MovementHandlerClient();

	UFUNCTION(BlueprintCallable)
		void AddSpeedModifier(float Modifier, float Duration);

	UFUNCTION(BlueprintCallable)
		void AddDamageModifier(float Modifier, float Duration, int NumHits);

	UFUNCTION(BlueprintCallable)
		void AddDefenseModifier(float Modifier, float Duration, int NumHits);

	UFUNCTION(BlueprintCallable)
		float CalcFinalDamage(float Damage);

	UFUNCTION(BlueprintCallable)
		float GetSpeedAmt();

	/** Block Handler */
	UFUNCTION(BlueprintCallable)
		void BlockHandler();

	/** Function for shooting projectiles */
	UFUNCTION(BlueprintCallable)
		void ProjectileHandler(FString AttackName);

	/** Attack Handler */
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
			bool Projectile
		);
	UFUNCTION()
		void AttackHandler2(FString AttackName, FString AttackType, float CastCooldownAmt, float LengthOfHitbox, float Damage, bool UseHitbox, UBoxComponent* Hitbox, bool Projectile);
	UFUNCTION()
		void AttackHandler3(FString AttackName, FString AttackType, float CastCooldownAmt, bool UseHitbox, UBoxComponent* Hitbox);

	/** Checks if the current attack should be chainable */
	UFUNCTION()
		bool CheckChainable(FString CurrentAttack);

	/** Calculate Cooldown Amount after cooldown buffs and debuffs */
	UFUNCTION()
		float GetFinalCooldownAmt(FString AttackName, float CooldownAmt);

	/** Get Cooldown for attack */
	UFUNCTION(BlueprintCallable)
		float GetCooldown(FString AttackName);

	/** Set Cooldown for attack */
	UFUNCTION(BlueprintCallable)
		void SetCooldown(FString AttackName, float CooldownAmt);

	/** Get Cooldown Amount for attack */
	UFUNCTION(BlueprintCallable)
		float GetCooldownAmt(FString AttackName);

	/** Get Damage for attack */
	UFUNCTION(BlueprintCallable)
		float GetDamage(FString AttackName);

	/** Checks if the current attack should be carried out */
	UFUNCTION()
		bool IsValidAttack(bool IsChainable, float CastCooldownAmt, FString CurrentAttack, float CooldownAmt);

	/** Moves current action to last action for attack handler */
	UFUNCTION(BlueprintCallable)
		void MakeCurrentActionLastAction(FString CurrentAttack);

	/** Forces all ongoing montages to stop */
	UFUNCTION()
		void StopAnimations();

	/** Forces all ongoing montages to stop */
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		void StopAnimationsServer();
	
	/** Wrapper for relaying animations to clients */
	UFUNCTION()
		void RelayAnimation(UAnimMontage* Animation, float Speed);

	UFUNCTION(Server, Reliable, WithValidation)
		void RelayAnimationServer(UAnimMontage* Animation, float Speed);

	UFUNCTION()
		void RelayAnimationClient(UAnimMontage* Animation, float Speed);

	/** Event for resilience regen */
	UFUNCTION()
		void ResilienceRegenTimer();

	/** Event for resilience drain */
	UFUNCTION()
		void ResilienceDrainTimer();

	/** Flinch Event */
	UFUNCTION()
		void FlinchEventTrigger();

	UFUNCTION(Server, Reliable, WithValidation)
		void FlinchEventServer();

	UFUNCTION()
		void FlinchEvent();

	UFUNCTION()
		void FlinchEvent2();

	/* Bleed Event Handler */
	UFUNCTION()
		void BleedEvent();

	/* **************************** Button Presses **************************** */
	/* Block */
	UFUNCTION()
		void BlockPressedEvent();
	UFUNCTION()
		void BlockReleasedEvent();
	/* SideStep */
	UFUNCTION()
		void SideStepPressedEvent();
	UFUNCTION()
		void SideStepPressedEvent2();
	UFUNCTION()
		void SideStepPressedEvent3();
	/* Roll */
	UFUNCTION()
		void RollPressedEvent();
	UFUNCTION()
		void RollPressedEvent2();
	UFUNCTION()
		void RollPressedEvent3();

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Timers */
	FTimerHandle ResilienceRegenTimerHandle;
	FTimerHandle ResilienceDrainTimerHandle;
	FTimerHandle delayTimerHandle;
	
private:
	
};