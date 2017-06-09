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

// Called to bind functionality to input
void ARevenant::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Block", IE_Pressed, this, &ARevenant::BlockPressedEvent);
	InputComponent->BindAction("Block", IE_Released, this, &ARevenant::BlockReleasedEvent);
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
void ARevenant::BlockPressedEventClient() {
	IsBlocking = true;
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
void ARevenant::BlockReleasedEventClient() {
	IsBlocking = false;
}
