#pragma once

#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BaseCharacter.generated.h"

/* ***** STRUCTS ***** */
/* Example of a struct
USTRUCT(BlueprintType)
struct FDirResult {
	GENERATED_USTRUCT_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool CorrectOrientation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector Direction;
}; */

//This is for UI Health update event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUIDelegate, FName, name, float, NewPercentage);

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
	
	/* ***** Component Variables ***** */

	/** Returns CameraBoom subobject */
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject */
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** UI Delegate */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "UI")
		FUIDelegate UIDelegate;

	void UIFunction(FName name, float NewPercentage);

	/* ***** Base Variables ("Vanilla" in blueprints) ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vanilla)
		bool Invincible = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Vanilla)
		bool IsDead = false;

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

	/* ***** Block Handler Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		bool BlockPressed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		bool IsBlocking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		float BlockingAnim = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = BlockHandler)
		float BlockCooldown = 0.0;

	/* ***** Flinch Variables ***** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Flinch)
		bool FlinchTrigger = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Flinch)
		bool Flinched = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Flinch)
		bool SuperArmor = false;

	/* ***** Roll Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Roll)
		bool IsRolling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Roll)
		float RollSpeed = 2.0f;

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

	/* ***** Resilience Variables ***** */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Resilience)
		float Resilience = 100.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Resilience)
		float ResilienceDrainAmt = 4.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Resilience)
		float ResilienceRegenAmt = 4.0;

	/* ***** Cooldown Variables ***** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Resilience)
		float AttackCastCooldown = 0;

	/* ***** Other Variables ***** */

	/* **************************** Functions **************************** */

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

	/** Function for when an attack hits (SwordContactEvent) */
	UFUNCTION()
		void WeaponHitEvent(FHitResult HitResult);

	/** Cooldown Decrement */
	UFUNCTION()
		void CooldownDecrement(UPARAM(ref) float cd, UPARAM(ref) FTimerHandle& Handle);

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
	UFUNCTION()
		void BlockAnimation();

	/** Roll Direction Handler */
	UFUNCTION()
	void RollDirectionHandler();

	/** Block Handler */
	UFUNCTION()
		void RollHandler();

	UFUNCTION(Server, Reliable, WithValidation)
		void RollHandlerServer();

	UFUNCTION()
		void RollHandlerClient();

	/** Block Handler */
	UFUNCTION(BlueprintCallable)
		void BlockHandler();

	/** Attack Handler */
	UFUNCTION(BlueprintCallable)
		void AttackHandler(
			FString AttackName,
			UPARAM(ref) float &Cooldown,
			float CooldownAmt,
			float CastCooldownAmt,
			float CastSpeed, 
			bool IsChainable,
			UAnimMontage* Animation,
			float DelayBeforeHitbox,
			float LengthOfHitbox,
			float Damage
		);
	UFUNCTION()
		void AttackHandler2(FString AttackName, float LengthOfHitbox, float Damage);
	UFUNCTION()
		void AttackHandler3(FString AttackName);

	/** Checks if the current attack should be chainable */
	UFUNCTION()
		bool CheckChainable(FString CurrentAttack);

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
		void BlockPressedEventClient();
	UFUNCTION()
		void BlockReleasedEventClient();
	/* Roll */
	UFUNCTION()
		void RollPressedEventClient();
	UFUNCTION()
		void RollPressedEventClient2();
	UFUNCTION()
		void RollPressedEventClient3();

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Timers */
	FTimerHandle ResilienceRegenTimerHandle;
	FTimerHandle ResilienceDrainTimerHandle;
	FTimerHandle delayTimerHandle;

private:
	
};