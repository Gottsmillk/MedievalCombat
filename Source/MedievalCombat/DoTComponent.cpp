// Fill out your copyright notice in the Description page of Project Settings.

#include "MedievalCombat.h"
#include "BaseCharacter.h"
#include "DoTComponent.h"


// Sets default values for this component's properties
UDoTComponent::UDoTComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UDoTComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDoTComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UDoTComponent::InitializeDelay() {
	DelayEvent.BindDynamic(this, &UDoTComponent::StartDoT);
	GetWorld()->GetTimerManager().SetTimer(DelayTimer, DelayEvent, IncrementAmt, true, this->InitialDelay);
}

void UDoTComponent::StartDoT() {
	if (TicksLeft > 0 && InflictedOwner->IsDead == false) {
		if (Type == "FlinchOnLast") {
			if (TicksLeft > 1) {
				InflictedOwner->InflictDamage(InflictedOwner, Attacker->CalcFinalDamage(DamageAmt), false, false, true);
			}
			else if (TicksLeft == 1) { // Last tick
				InflictedOwner->InflictDamage(InflictedOwner, Attacker->CalcFinalDamage(DamageAmt), false, true, true);
			}
			TicksLeft--;
		}
	}
	else {
		K2_DestroyComponent(this);// destroy actor
	}
}
