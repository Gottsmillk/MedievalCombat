#include "MedievalCombat.h"
#include "Revenant.h"

// Constructor for Revenant
ARevenant::ARevenant()
{
	bReplicates = true;
	// Create shield
	Shield = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shield"));
	Shield->SetVisibility(true);
	Shield->SetHiddenInGame(false);
	Shield->bGenerateOverlapEvents = false;
	FName ShieldSocket = TEXT("Shieldsocket");
	Shield->SetupAttachment(GetMesh(), ShieldSocket);
}

// Called when the game starts or when spawned
void ARevenant::BeginPlay()
{
	Super::BeginPlay();

}

// Called to bind functionality to input
void ARevenant::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Block", IE_Pressed, this, &ARevenant::BlockPressedEvent);
	InputComponent->BindAction("Block", IE_Released, this, &ARevenant::BlockReleasedEvent);
	InputComponent->BindAction("Roll", IE_Pressed, this, &ARevenant::RollPressedEvent);
}

/* Blocks from pressing key */
void ARevenant::BlockPressedEvent() {
	if (this->HasAuthority()) {
		BlockPressedEventServer();
	}
	else {
		BlockPressedEventServer();
		BlockPressedEventClient();
	}
}
void ARevenant::BlockPressedEventServer_Implementation() {
	BlockPressedEventClient();
}
bool ARevenant::BlockPressedEventServer_Validate() {
	return true;
}

/* Blocks from releasing key */
void ARevenant::BlockReleasedEvent() {
	if (this->HasAuthority()) {
		BlockReleasedEventServer();
	}
	else {
		BlockReleasedEventServer();
		BlockReleasedEventClient();
	}
}
void ARevenant::BlockReleasedEventServer_Implementation() {
	BlockReleasedEventClient();
}
bool ARevenant::BlockReleasedEventServer_Validate() {
	return true;
}

/* Rolls from pressing key */
void ARevenant::RollPressedEvent() {
	if (IsRolling == false && Resilience >= 25 && this->GetMovementComponent()->IsMovingOnGround() == true) {
		if (this->HasAuthority()) {
			RollPressedEventServer();
		}
		else {
			RollPressedEventServer();
			RollPressedEventClient();
		}
	}
}
void ARevenant::RollPressedEventServer_Implementation() {
	RollPressedEventClient();
}
bool ARevenant::RollPressedEventServer_Validate() {
	return true;
}