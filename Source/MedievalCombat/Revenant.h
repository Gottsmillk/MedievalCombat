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

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;
};
