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

	// Countering blow hurtbox
	CounteringBlowHurtbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Countering Blow Hurtbox"));
	CounteringBlowHurtbox->SetVisibility(true);
	CounteringBlowHurtbox->SetHiddenInGame(false);
	CounteringBlowHurtbox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	CounteringBlowHurtbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CounteringBlowHurtbox->bDynamicObstacle = false;
	CounteringBlowHurtbox->bGenerateOverlapEvents = true;
	CounteringBlowHurtbox->SetupAttachment(GetMesh(), TEXT("Weaponsocket"));

}

// Called when the game starts or when spawned
void ARevenant::BeginPlay()
{
	Super::BeginPlay();
	CounteringBlowHurtbox->OnComponentBeginOverlap.AddDynamic(this, &ARevenant::CounteringBlowHurtboxOverlap);
}

// Called to bind functionality to input
void ARevenant::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Block", IE_Pressed, this, &ARevenant::BlockPressedEvent);
	InputComponent->BindAction("Block", IE_Released, this, &ARevenant::BlockReleasedEvent);
	InputComponent->BindAction("Roll", IE_Pressed, this, &ARevenant::RollPressedEvent);
	InputComponent->BindAction("SideStep", IE_Pressed, this, &ARevenant::SideStepPressedEvent);
	InputComponent->BindAction("BasicAttack", IE_Pressed, this, &ARevenant::SBasicAttackPressedEvent);
	InputComponent->BindAction("HardAttack", IE_Pressed, this, &ARevenant::HBasicAttackPressedEvent);
	InputComponent->BindAction("CounteringBlow", IE_Pressed, this, &ARevenant::CounteringBlowPressedEvent);
}

/* Hitbox Events */
void ARevenant::CounteringBlowHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{ 
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		CurrentAttackHit = true;
		//LastAttack = "CounteringBlow";
		CounteringBlowHurtbox->bGenerateOverlapEvents = false;
		ABaseCharacter* AttackedTarget = Cast<ABaseCharacter>(OtherActor);
		if (AttackedTarget->Invincible == false) {
			if (AttackedTarget->IsBlocking != true || GetPlayerDirections(AttackedTarget) == false) { // Incorrectly blocked
				AttackedTarget->IsBlocking = false;
				AttackedTarget->FlinchTrigger = true;
				AttackedTarget->Health = (AttackedTarget->Health) - 2.0f;
				AttackedTarget->InitiateDamageEffect();
				if (AttackedTarget->Health <= 0) {
					AttackedTarget->ServerDeath();
				}
			}
			else { // Correctly blocked
				FName Path = TEXT("/Game/Classes/Revenant/Animations/Recoil/Shield_Block_Recoil_Montage.Shield_Block_Recoil_Montage");
				UAnimMontage *BlockRecoilMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *Path.ToString()));
				AttackedTarget->RelayAnimation(BlockRecoilMontage, 1.0f);
			}
		}
	}
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

/* SideSteps from pressing key */
void ARevenant::SideStepPressedEvent() {
	if (IsRolling == false && IsSideStepping == false && CanAttack == true && SideStepCooldown <= UKismetSystemLibrary::GetGameTimeInSeconds(this) && this->GetMovementComponent()->IsMovingOnGround() == true) {
		if (this->HasAuthority()) {
			SideStepPressedEventServer();
		}
		else {
			SideStepPressedEventServer();
			SideStepPressedEventClient();
		}
	}
}
void ARevenant::SideStepPressedEventServer_Implementation() {
	SideStepPressedEventClient();
}
bool ARevenant::SideStepPressedEventServer_Validate() {
	return true;
}

/* Rolls from pressing key */
void ARevenant::RollPressedEvent() {
	if (IsRolling == false && IsSideStepping == false && Resilience >= 25 && this->GetMovementComponent()->IsMovingOnGround() == true) {
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
	if (CanAttack == true && IsRolling == false && IsSideStepping == false && Flinched == false) {
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
			1.1f, // Re-Casting delay
			0.9f, // Speed of Animation
			false, // Based on previous Attack, is it Chainable
			SBasicAttackAnimMontage, // Animation to use
			0.3f, // Delay before Hitbox starts
			0.2f, // Time duration of Hitbox
			3.0f, // Amount of damage
			false, // Whether or not to use hitbox instead
			NULL); // Which hitbox to initiate
	}
	else { // If combo
		AttackHandler(
			"SBasicAttack", // Name
			SBasicAttackCD, // Cooldown variable
			1.0f, // Cooldown Amount
			1.1f, // Re-Casting delay
			0.9f, // Speed of Animation
			true, // Based on previous Attack, is it Chainable
			SBasicAttackComboAnimMontage, // Animation to use
			0.3f, // Delay before Hitbox starts
			0.2f, // Time duration of Hitbox
			3.0f, // Amount of damage
			false, // Whether or not to use hitbox instead
			NULL); // Which hitbox to initiate
	}
}

/* HBasicAttack */
void ARevenant::HBasicAttackPressedEvent() {
	if (CanAttack == true && IsRolling == false && IsSideStepping == false && Flinched == false) {
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
			1.2f, // Re-Casting delay
			1.1f, // Speed of Animation
			false, // Based on previous Attack, is it Chainable
			HBasicAttackAnimMontage, // Animation to use
			0.6f, // Delay before Hitbox starts
			0.3f, // Time duration of Hitbox
			6.0f, // Amount of damage
			false, // Whether or not to use hitbox instead
			NULL); // Which hitbox to initiate
	}
	else { // If combo
		AttackHandler(
			"HBasicAttack", // Name
			HBasicAttackCD, // Cooldown variable
			1.0f, // Cooldown Amount
			1.2f, // Re-Casting delay
			1.1f, // Speed of Animation
			true, // Based on previous Attack, is it Chainable
			HBasicAttackComboAnimMontage, // Animation to use
			0.2f, // Delay before Hitbox starts
			0.3f, // Time duration of Hitbox
			6.0f, // Amount of damage
			false, // Whether or not to use hitbox instead
			NULL); // Which hitbox to initiate
	}

}

/* CounteringBlow */
void ARevenant::CounteringBlowPressedEvent() {
	if (CanAttack == true && IsRolling == false && IsSideStepping == false && Flinched == false) {
		if (this->HasAuthority()) {
			CounteringBlowPressedEventServer();
		}
		else {
			CounteringBlowPressedEventServer();
			CounteringBlowPressedEventClient();
		}
	}
}
void ARevenant::CounteringBlowPressedEventServer_Implementation() {
	CounteringBlowPressedEventClient();
}
bool ARevenant::CounteringBlowPressedEventServer_Validate() {
	return true;
}
void ARevenant::CounteringBlowPressedEventClient() {
	FName CounteringBlowAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Countering_Blow_Montage.Countering_Blow_Montage");
	UAnimMontage *CounteringBlowAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *CounteringBlowAnimPath.ToString()));
	AttackHandler(
		"CounteringBlow", // Name
		CounteringBlowCD, // Cooldown variable
		6.0f, // Cooldown Amount
		0.8f, // Re-Casting delay
		1.0f, // Speed of Animation
		false, // Based on previous Attack, is it Chainable
		CounteringBlowAnimMontage, // Animation to use
		0.2f, // Delay before Hitbox starts
		0.2f, // Time duration of Hitbox
		2.0f, // Amount of damage
		true, // Whether or not to use hitbox instead
		CounteringBlowHurtbox); // Which hitbox to initiate
}