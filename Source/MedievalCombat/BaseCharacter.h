#pragma once

#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectileBase.h"
#include "BaseCharacter.generated.h"

/* ------------------------------------ STRUCTS (Uncollapse on left) ----------------------------------- */

USTRUCT(BlueprintType)
struct FDamageOverTime {
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DamageAmt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int TicksLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LastTick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float IncrementAmt;

	FDamageOverTime(FString Type2 = "", float DamageAmt2 = 0.0f, int TicksLeft2 = 0, float LastTick2 = 0.0f, float IncrementAmt2 = 0.0f) {
		Type = Type2;
		DamageAmt = DamageAmt2;
		TicksLeft = TicksLeft2;
		LastTick = LastTick2;
		IncrementAmt = IncrementAmt2;
	}
};

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

/* ---------------------------------- COMPONENTS (Uncollapse on left) ---------------------------------- */

	// Camera boom positioning the camera behind the character (SpringArm)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	// Main camera that follows the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	// Capsule collision commponent that checks if player is aiming in the enemy's general direction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* PlayerAimCollision;

	// Capsule collision component that prevents players from going through each other
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* PlayerCollision;
	
	// Capsule collision commponent that checks if players should stay in place or move when attacking
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* PlayerCollision2;

	// Sword Mesh Object
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* Weapon;

	// Weapon hurtboxes to trace attacks accurately
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

/* ------------------------------- INITIALIZATIONS (Uncollapse on left) -------------------------------- */

	// Sets default values for this character's properties
	ABaseCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

/* ---------------------------------- VARIABLES (Uncollapse on left) ----------------------------------- */

	/* ***** Defaults ***** */

	// Returns CameraBoom subobject
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// Returns FollowCamera subobject
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Keeps track of the client's local game time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Vanilla)
		float CurrentGameTime = 0.0f;

	/* ***** Variables loaded from child blueprint ***** */

	// Returns the custom Character Movement component provided by Advanced Locomotion plugin
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vanilla)
		UCharacterMovementComponent* CharMovement;

	// Returns the Camera Shake blueprint for the Damage Indicator
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterInfo)
		TSubclassOf <UCameraShake> PlayerTakeDamageCameraShake;

	/* ***** Health Variables ***** */

	// Keeps track of current health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
		float Health = 100.0;

	// Keeps track of maximum health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
		float MaxHealth = 100.0;

	/* ***** Hitbox Variables ***** */

	// Array containing all of the weapon hurtbox components
	UPROPERTY(EditAnywhere, Category = Hitbox)
		UShapeComponent* HitboxComponentArray[6];

	// Array keeping track of hurtbox component's current position in world space
	UPROPERTY(EditAnywhere, Category = Hitbox)
		FVector HitboxArray[6];

	// Allows proper setting of the hitbox on the very first tick of attacking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hitbox)
		bool InitialHitbox = true;

	// Whether or not hurtbox is active when attacking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hitbox)
		bool HurtboxActive = false;

	/* ***** Damage and Death Variables ***** */

	// Raw damage of the current attack before buffs/debuffs 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
		float CurrentDamage = 0.0f;

	// Whether or not you are in an iframe
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
		bool Invincible = false;

	// Whether or not you got scattershot by a Hanzo main
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Damage)
		bool IsDead = false;

	// Array that stores damage values of attacks that have static damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Damage)
		TArray<FDamageTableStruct> DamageTable;

	// Array that stores damage over time events
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Damage)
		TArray<FDamageOverTime> DamageOverTimeArray;

	// Array that stores buff values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Damage)
		TArray<FAttackModifierStruct> AttackModifierArray;

	// Array that stores debuff values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Damage)
		TArray<FDefenseModifierStruct> DefenseModifierArray;

	/* ***** Projectile Variables ***** */

	// Vector storing the maximum range a projectile can be shot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
		FVector MaxProjectileRange;

	// Instantiation of base projectile blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
		TSubclassOf<AProjectileBase> BaseProjectile;

	/* ***** Attack Handler Variables ***** */

	// Number of hits in an ongoing combo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		int ComboAmount = 0;

	// Whether or not there is an ongoing attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		bool CanAttack = true;

	// Whether or not the current attack landed or missed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		bool CurrentAttackHit = false;

	// Storing the previous attack for determining chaining
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		FString LastAttack = "";

	// Storing the current attack to pass to targets and UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		FString CurrentAttackName = "";

	// Storing the current attack type for display onto UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		FString CurrentAttackType = "";

	// Stores attack cooldowns and attack cooldown amounts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		TArray<FAttackStruct> AttackArray;

	// Stores the combo extenders selected by the Choose Attacks UI for cross-Blueprint interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		TArray<FString> ComboExtenderArray;

	// Stores the utilites selected by the Choose Attacks UI for cross-Blueprint interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		TArray<FString> UtilityArray;

	// Stores the combo finishers selected by the Choose Attacks UI for cross-Blueprint interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		TArray<FString> ComboFinisherArray;
	
	// Whether or not an ability/attack is detected, used for abilities that deactivate once an action is performed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		bool DetectMode = false;

	/* ***** Block Handler Variables ***** */

	// Whether or not the block button was pressed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		bool BlockPressed = false;

	// Whether or not the player is allowed to block despite the block button is pressed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		bool IsBlocking = false;

	// Used to communicate block animation threshold to the Animation Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		float BlockingAnim = 0.0;

	/* ***** Flinch Variables ***** */

	// Whether or not to initiate a flinching event to player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Flinch)
		bool FlinchTrigger = false;

	// Whether or not the player is currently flinching
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Flinch)
		bool Flinched = false;

	// Prevents player from flinching
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Flinch)
		bool SuperArmor = false;

	// Give super armor to HBasicAttack, gave it its own variable because of Timer channel issues
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Flinch)
		bool HBasicAttackSuperArmor = false;

	/* ***** Roll/Sidestep Variables ***** */

	// Whether player is rolling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = RollSidestep)
		bool IsRolling = false;

	// Whether player moves while rolling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = RollSidestep)
		bool RollMovement = false;

	// Whether player is sidestepping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = RollSidestep)
		bool IsSideStepping = false;

	/* ***** Movement Variables ***** */

	// Whether or not player can move
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Movement)
		bool CanMove = true;

	// Whether or not player can move the camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Movement)
		bool CanTurn = true;

	// Keeps track of player's forward/backward motion for determining which way to roll
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Movement)
		float CurrentFBLoc = 0;

	// Keeps track of player's left/right motion for determining which way to roll
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Movement)
		float CurrentLRLoc = 0;

	// Whether or not player should attack in place or move
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Movement)
		bool Colliding = false;
	
	// Whether or not player is within certain range to check if they should attack in place or move
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Movement)
		bool Overlapping = false;

	// Array storing speed buffs/debuffs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Movement)
		TArray<FSpeedModifierStruct> SpeedEffectsArray;

	/* ***** Resilience Variables ***** */

	// Keeps track of Resilience
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Resilience)
		float Resilience = 100.0;

	// Keeps track of the drain rate of resilience per second
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Resilience)
		float ResilienceDrainAmt = 5.0;

	// Keeps track of regen rate of resilience per second
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Resilience)
		float ResilienceRegenAmt = 5.0;

	// Keeps track of how much resilience to replenish the attacker after a combo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Resilience)
		float ResilienceAttackReplenish = 0.0f;

	// Keeps track of how much resilience to replenish the defender after a combo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Resilience)
		float ResilienceDefenseReplenish = 0.0f;

	// Whether or not to subtract the initial drain amount from pressing block
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Resilience)
		bool BlockSubtractResilience = false;

	/* ***** Cooldown Variables ***** */
	
	// Keeps track of cast cooldown for non chaining attacks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Cooldowns)
		float AttackCastCooldown = 0;

	// Array storing cooldown buffs/debuffs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Cooldowns)
		TArray<FCooldownEffectsStruct> CooldownEffectsArray;

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

	/* ***** UI Variables ***** */

	// Indicates whether a UI menu is up or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = UI)
		bool MenuUp = true;

/* ---------------------------------- FUNCTIONS (Uncollapse on left) ----------------------------------- */

	// Perfom final actions once "Submit" is pressed on the "Choose Attacks" UI
	UFUNCTION(BlueprintCallable)
		virtual void AddRemainingInputs();

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

	/** Deal Damage */
	UFUNCTION(BlueprintCallable)
		bool InflictDamage(ABaseCharacter* Target, float Damage, bool BlockCheck, bool Flinches);

	/* On receiving any damage, will decrement health and if below or equal to zero, dies. Overridden function.*/
	UFUNCTION(BlueprintCallable)
		void ReceiveAnyDamage(float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
		void ApplyDamage(float Damage, ABaseCharacter * Attacker);

	UFUNCTION(BlueprintCallable)
		void ApplyDamageOverTime(FString Type, float DamageAmt, int Amount, float StartDelay, float IncrementAmt);

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

	/** Apply override so player who received damage can send events to player who dealt damage */
	UFUNCTION(BlueprintCallable)
	virtual void SendEventToAttacker(ABaseCharacter* Attacker);

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

	/** Add Cooldown Buff/Debuff */
	UFUNCTION(BlueprintCallable)
		void AddCooldownModifier(float Modifier, FString AbilityName, float Duration);

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
	UFUNCTION(BlueprintCallable)
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
	UFUNCTION(BlueprintCallable)
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

	/** Create Particle Effects */
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
		void InitializeParticle(TSubclassOf<AActor> Particle);

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

	/* Track Damage over Time */
	UFUNCTION()
		void TrackDamageOverTime();

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