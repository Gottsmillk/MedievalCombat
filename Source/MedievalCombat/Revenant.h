#pragma once

#include "BaseCharacter.h"
#include "Revenant.generated.h"

UCLASS()
class MEDIEVALCOMBAT_API ARevenant : public ABaseCharacter
{
	GENERATED_BODY()

	/** Shield Mesh object */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Shield;
	
public:
	// Sets default values for this character's properties
	ARevenant();

	/** Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackCooldowns)
		float SBasicAttackCD = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackCooldowns)
		float HBasicAttackCD = 0.0;

	/** Event when block is pressed */
	UFUNCTION()
	void BlockPressedEvent();

	UFUNCTION(Server, Reliable, WithValidation)
		void BlockPressedEventServer();

	/** Event when block is released */
	UFUNCTION()
		void BlockReleasedEvent();

	UFUNCTION(Server, Reliable, WithValidation)
		void BlockReleasedEventServer();

	/** Event when roll is pressed */
	UFUNCTION()
		void RollPressedEvent();

	UFUNCTION(Server, Reliable, WithValidation)
		void RollPressedEventServer();

	/** Event when sidestep is pressed */
	UFUNCTION()
		void SideStepPressedEvent();

	UFUNCTION(Server, Reliable, WithValidation)
		void SideStepPressedEventServer();

	/** Event when SBasicAttack is pressed */
	UFUNCTION()
		void SBasicAttackPressedEvent();

	UFUNCTION(Server, Reliable, WithValidation)
		void SBasicAttackPressedEventServer();

	UFUNCTION()
		void SBasicAttackPressedEventClient();

	/** Event when HBasicAttack is pressed */
	UFUNCTION()
		void HBasicAttackPressedEvent();

	UFUNCTION(Server, Reliable, WithValidation)
		void HBasicAttackPressedEventServer();

	UFUNCTION()
		void HBasicAttackPressedEventClient();

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;
};
