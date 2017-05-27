// Fill out your copyright notice in the Description page of Project Settings.

#include "MedievalCombat.h"
#include "BaseCharacter.h"
#include "UnrealNetwork.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

// Allows Replication of variables for Client/Server Networking
void ABaseCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ABaseCharacter, IsDead);
	DOREPLIFETIME(ABaseCharacter, Health);
	DOREPLIFETIME(ABaseCharacter, CanAttack);
	DOREPLIFETIME(ABaseCharacter, CurrentAttackHit);
	DOREPLIFETIME(ABaseCharacter, IsBlocking);
	DOREPLIFETIME(ABaseCharacter, AttackWasBlocked);
	DOREPLIFETIME(ABaseCharacter, SuccessfullyDefended);
	DOREPLIFETIME(ABaseCharacter, RecoilAnim);
	DOREPLIFETIME(ABaseCharacter, BlockingAnimation);
	DOREPLIFETIME(ABaseCharacter, HoldingBlock);
	DOREPLIFETIME(ABaseCharacter, FlinchDuration);
	DOREPLIFETIME(ABaseCharacter, FlinchTrigger);
	DOREPLIFETIME(ABaseCharacter, Flinched);
	DOREPLIFETIME(ABaseCharacter, RollAnim);
	DOREPLIFETIME(ABaseCharacter, RollSpeed);
	DOREPLIFETIME(ABaseCharacter, Resilience);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Function for handling DELAY equivalent from Blueprints

void ABaseCharacter::onTimerEnd()
{
}

//Decrements cooldown by .1 every time called, if cd > 0
void ABaseCharacter::CooldownDecrement(UPARAM(ref) float cd, UPARAM(ref) FTimerHandle& Handle)
{
	if (cd > 0)
	{
		cd -= .1;
	}
	else
	{
		cd = 0;
	}
	GetWorld()->GetTimerManager().ClearTimer(Handle);
}
// Handles block events called at every tick
