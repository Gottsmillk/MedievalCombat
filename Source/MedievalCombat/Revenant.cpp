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
	InputComponent->BindAction("BasicAttack", IE_Pressed, this, &ARevenant::SBasicAttackPressedEvent);
	InputComponent->BindAction("HardAttack", IE_Pressed, this, &ARevenant::HBasicAttackPressedEvent);
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

/* SBasicAttack */
void ARevenant::SBasicAttackPressedEvent() {
	if (CanAttack == true && IsRolling == false && Flinched == false) {
		if (this->HasAuthority()) {
			SBasicAttackPressedEventServer();
		}
		else {
			SBasicAttackPressedEventServer();
			SBasicAttackPressedEventClient();
		}
	}
}
void ARevenant::SBasicAttackPressedEventServer_Implementation() {
	SBasicAttackPressedEventClient();
}
bool ARevenant::SBasicAttackPressedEventServer_Validate() {
	return true;
}
void ARevenant::SBasicAttackPressedEventClient() {
	FName SBasicAttackAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/N_Basic_Attack_Montage.N_Basic_Attack_Montage");
	UAnimMontage *SBasicAttackAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *SBasicAttackAnimPath.ToString()));
	FName SBasicAttackComboAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/N_Basic_Attack_Combo_Montage.N_Basic_Attack_Combo_Montage");
	UAnimMontage *SBasicAttackComboAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *SBasicAttackComboAnimPath.ToString()));

	if (CheckChainable("SBasicAttack") == false) { // If no combo
		AttackHandler(
			"SBasicAttack", // Name
			SBasicAttackCD, // Cooldown variable
			1.0f, // Cooldown Amount
			1.5f, // Re-Casting delay
			0.9f, // Speed of Animation
			false, // Based on previous Attack, is it Chainable
			SBasicAttackAnimMontage, // Animation to use
			0.32f, // Delay before Hitbox starts
			0.2f, // Time duration of Hitbox
			3.0f); // Amount of damage
	}
	else { // If combo
		AttackHandler(
			"SBasicAttack", // Name
			SBasicAttackCD, // Cooldown variable
			1.0f, // Cooldown Amount
			1.5f, // Re-Casting delay
			0.9f, // Speed of Animation
			true, // Based on previous Attack, is it Chainable
			SBasicAttackComboAnimMontage, // Animation to use
			0.12f, // Delay before Hitbox starts
			0.2f, // Time duration of Hitbox
			3.0f); // Amount of damage
	}
}

/* HBasicAttack */
void ARevenant::HBasicAttackPressedEvent() {
	if (CanAttack == true && IsRolling == false && Flinched == false) {
		if (this->HasAuthority()) {
			HBasicAttackPressedEventServer();
		}
		else {
			HBasicAttackPressedEventServer();
			HBasicAttackPressedEventClient();
		}
	}
}
void ARevenant::HBasicAttackPressedEventServer_Implementation() {
	HBasicAttackPressedEventClient();
}
bool ARevenant::HBasicAttackPressedEventServer_Validate() {
	return true;
}
void ARevenant::HBasicAttackPressedEventClient() {
	FName HBasicAttackAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/H_Basic_Attack_Montage.H_Basic_Attack_Montage");
	UAnimMontage *HBasicAttackAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HBasicAttackAnimPath.ToString()));
	FName HBasicAttackComboAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/H_Basic_Attack_Combo_Montage.H_Basic_Attack_Combo_Montage");
	UAnimMontage *HBasicAttackComboAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HBasicAttackComboAnimPath.ToString()));

	if (CheckChainable("HBasicAttack") == false) { // If no combo
		AttackHandler(
			"HBasicAttack", // Name
			HBasicAttackCD, // Cooldown variable
			1.0f, // Cooldown Amount
			1.5f, // Re-Casting delay
			1.1f, // Speed of Animation
			false, // Based on previous Attack, is it Chainable
			HBasicAttackAnimMontage, // Animation to use
			0.6f, // Delay before Hitbox starts
			0.3f, // Time duration of Hitbox
			6.0f); // Amount of damage
	}
	else { // If combo
		AttackHandler(
			"HBasicAttack", // Name
			HBasicAttackCD, // Cooldown variable
			1.0f, // Cooldown Amount
			1.5f, // Re-Casting delay
			1.1f, // Speed of Animation
			true, // Based on previous Attack, is it Chainable
			HBasicAttackComboAnimMontage, // Animation to use
			0.4f, // Delay before Hitbox starts
			0.3f, // Time duration of Hitbox
			6.0f); // Amount of damage
	}

}