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

	/** Hurtbox for countering blow */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* CounteringBlowHurtbox;

	/** Hurtbox for Staggering Kick */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* KickHurtbox;
	
public:
	// Sets default values for this character's properties
	ARevenant();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = AttackHandler)
		bool AgilityEffect = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UWidgetComponent * HPOverhead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
		TSubclassOf<ABaseCharacter> Shadow;

	/** Called to bind functionality to input */
		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Add Remaining Inputs */
	UFUNCTION(BlueprintCallable)
		virtual void AddRemainingInputs() override;

	UFUNCTION(Server, Reliable, WithValidation)
		void AddRemainingInputsServer();

	UFUNCTION()
		void AddRemainingInputsClient();

	/** Hitbox Events */
	UFUNCTION()
		void CounteringBlowHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void KickHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** Overriding Attack Effect Handler */
	UFUNCTION()
		virtual void AttackEffect(ABaseCharacter* Target, FString AttackName) override;

	/** Overriding Attack Execute */
	UFUNCTION(BlueprintCallable)
		virtual void AttackExecute(FString AttackName) override;

	UFUNCTION(Server, Reliable, WithValidation)
		virtual void AttackExecuteServer(const FString &AttackName) override;

	UFUNCTION()
		virtual void AttackExecuteClient(FString AttackName) override;

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

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	FTimerHandle delay2TimerHandle;
};
