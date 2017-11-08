// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Components/ActorComponent.h"
#include "DoTComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MEDIEVALCOMBAT_API UDoTComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDoTComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	FTimerDynamicDelegate DelayEvent;
	FTimerHandle DelayTimer;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DamageAmt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int TicksLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InitialDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float IncrementAmt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class ABaseCharacter* Attacker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class ABaseCharacter* InflictedOwner = Cast<ABaseCharacter>(GetOwner());

	UFUNCTION()
		void InitializeDelay();

	UFUNCTION()
		void StartDoT();

};
