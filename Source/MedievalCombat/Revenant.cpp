#include "MedievalCombat.h"
#include "UnrealNetwork.h"
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

	// Staggering kick hurtbox
	KickHurtbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Kick Hurtbox"));
	KickHurtbox->SetVisibility(true);
	KickHurtbox->SetHiddenInGame(false);
	KickHurtbox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	KickHurtbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	KickHurtbox->bDynamicObstacle = false;
	KickHurtbox->bGenerateOverlapEvents = true;
	KickHurtbox->SetupAttachment(GetMesh(), TEXT("Kicksocket"));

	// Set Shadow to be spawned
	static ConstructorHelpers::FObjectFinder<UBlueprint> ShadowBlueprint(TEXT("Blueprint'/Game/Classes/Revenant/Blueprints/RevenantShadowBP.RevenantShadowBP'"));
	if (ShadowBlueprint.Object) {
		Shadow = (UClass*)ShadowBlueprint.Object->GeneratedClass;
	}
}

void ARevenant::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ARevenant, AgilityEffect);
}

// Called when the game starts or when spawned
void ARevenant::BeginPlay()
{
	Super::BeginPlay();
	CounteringBlowHurtbox->OnComponentBeginOverlap.AddDynamic(this, &ARevenant::CounteringBlowHurtboxOverlap);
	KickHurtbox->OnComponentBeginOverlap.AddDynamic(this, &ARevenant::KickHurtboxOverlap);
}

// Called to bind functionality to input
void ARevenant::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Add Variables
	FInputActionBinding ActionBind;
	FInputActionHandlerSignature ActionBindHandler;

	// Add Block Press
	ActionBind.ActionName = FName("Block");
	ActionBind.KeyEvent = IE_Pressed;
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), FString(TEXT("BlockPress")));
	ActionBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ActionBind);

	// Add Block Release
	ActionBind.ActionName = FName("Block");
	ActionBind.KeyEvent = IE_Released;
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), FString(TEXT("BlockRelease")));
	ActionBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ActionBind);

	// Add Roll
	ActionBind.ActionName = FName("Roll");
	ActionBind.KeyEvent = IE_Pressed;
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), FString(TEXT("Roll")));
	ActionBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ActionBind);

	// Add SideStep
	ActionBind.ActionName = FName("SideStep");
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), FString(TEXT("SideStep")));
	ActionBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ActionBind);

	// Add BasicAttack
	ActionBind.ActionName = FName("BasicAttack");
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), FString(TEXT("SBasicAttack")));
	ActionBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ActionBind);

	// Add HardAttack
	ActionBind.ActionName = FName("HardAttack");
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), FString(TEXT("HBasicAttack")));
	ActionBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ActionBind);

	// Add CounteringBlow
	ActionBind.ActionName = FName("CounteringBlow");
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), FString(TEXT("CounteringBlow")));
	ActionBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ActionBind);

}
void ARevenant::AddRemainingInputs() {
	FInputActionBinding ActionBind;
	FInputActionHandlerSignature ActionBindHandler;
	// Add ComboExtender1
	ActionBind.ActionName = FName("ComboExtender1");
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), ComboExtenderArray[0]);
	ActionBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ActionBind);

	// Add ComboExtender2
	ActionBind.ActionName = FName("ComboExtender2");
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), ComboExtenderArray[1]);
	ActionBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ActionBind);

	// Add ComboExtender3
	ActionBind.ActionName = FName("ComboExtender3");
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), ComboExtenderArray[2]);
	ActionBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ActionBind);

	// Add Utility 1
	ActionBind.ActionName = FName("Utility1");
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), UtilityArray[0]);
	ActionBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ActionBind);

	// Add Utility 2
	ActionBind.ActionName = FName("Utility2");
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), UtilityArray[1]);
	ActionBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ActionBind);

	if (this->HasAuthority()) {
		AddRemainingInputsClient();
	}
	else {
		AddRemainingInputsServer();
		AddRemainingInputsClient();
	}
}
void ARevenant::AddRemainingInputsServer_Implementation() {
	AddRemainingInputsClient();
}
bool ARevenant::AddRemainingInputsServer_Validate() {
	return true;
}
void ARevenant::AddRemainingInputsClient() {
	//Construct Damage Tables for all Attacks
	DamageTable.Add(FDamageTableStruct("SBasicAttack", 3.0f));
	DamageTable.Add(FDamageTableStruct("HBasicAttack", 6.0f));
	DamageTable.Add(FDamageTableStruct("CounteringBlow", 2.0f));
	DamageTable.Add(FDamageTableStruct("StaggeringKick", 1.0f));
	DamageTable.Add(FDamageTableStruct("Impede", 1.0f));
	DamageTable.Add(FDamageTableStruct("LifeDrain", 6.0f));
	DamageTable.Add(FDamageTableStruct("DebilitatingKick", 1.0f));
	DamageTable.Add(FDamageTableStruct("EmpoweringStrike", 3.0f));
	DamageTable.Add(FDamageTableStruct("EnergyDrain", 4.0f));

	//Add to Attack Array
	AttackArray.Add(FAttackStruct("BlockPress", 0.0f, 1.0f));
	AttackArray.Add(FAttackStruct("BlockRelease", 0.0f, 0.0f));
	AttackArray.Add(FAttackStruct("Roll", 0.0f, 0.0f));
	AttackArray.Add(FAttackStruct("SideStep", 0.0f, 5.0f));
	AttackArray.Add(FAttackStruct("SBasicAttack", 0.0f, 0.0f));
	AttackArray.Add(FAttackStruct("HBasicAttack", 0.0f, 0.0f));
	AttackArray.Add(FAttackStruct("CounteringBlow", 0.0f, 6.0f));
	AttackArray.Add(FAttackStruct(ComboExtenderArray[0], 0.0f, SetAttackCooldownAmt(ComboExtenderArray[0])));
	AttackArray.Add(FAttackStruct(ComboExtenderArray[1], 0.0f, SetAttackCooldownAmt(ComboExtenderArray[1])));
	AttackArray.Add(FAttackStruct(ComboExtenderArray[2], 0.0f, SetAttackCooldownAmt(ComboExtenderArray[2])));
	AttackArray.Add(FAttackStruct(UtilityArray[0], 0.0f, SetAttackCooldownAmt(UtilityArray[0])));
	AttackArray.Add(FAttackStruct(UtilityArray[1], 0.0f, SetAttackCooldownAmt(UtilityArray[1])));
}

void ARevenant::AddAttack(FString Type, FString AttackName, bool Toggled) {
	if (this->HasAuthority()) {
		AddAttackClient(Type, AttackName, Toggled);
	}
	else {
		AddAttackServer(Type, AttackName, Toggled);
		AddAttackClient(Type, AttackName, Toggled);
	}
}
void ARevenant::AddAttackServer_Implementation(const FString &Type, const FString &AttackName, bool Toggled) {
	AddAttackClient(Type, AttackName, Toggled);
}
bool ARevenant::AddAttackServer_Validate(const FString &Type, const FString &AttackName, bool Toggled) {
	return true;
}
void ARevenant::AddAttackClient(FString Type, FString AttackName, bool Toggled) {
	if (Type == "ComboExtender") {
		if (Toggled == true) {
			ComboExtenderArray.Add(AttackName);
		}
		else {
			int i;
			for (i = 0; i < ComboExtenderArray.Num(); i++) {
				if (ComboExtenderArray[i] == AttackName) {
					break;
				}
			}
			if (i < ComboExtenderArray.Num()) {
				ComboExtenderArray.RemoveAt(i, 1, true);
			}
		}
	}
	else if (Type == "Utility") {
		if (Toggled == true) {
			UtilityArray.Add(AttackName);
		}
		else {
			int i;
			for (i = 0; i < UtilityArray.Num(); i++) {
				if (UtilityArray[i] == AttackName) {
					break;
				}
			}
			if (i < UtilityArray.Num()) {
				UtilityArray.RemoveAt(i, 1, true);
			}
		}
	}
}
/* Hitbox Events */
void ARevenant::CounteringBlowHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{ 
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		CurrentAttackHit = true;
		CounteringBlowHurtbox->bGenerateOverlapEvents = false;
		ABaseCharacter* AttackedTarget = Cast<ABaseCharacter>(OtherActor);
		if (AttackedTarget->Invincible == false) {
			if (AttackedTarget->IsBlocking != true || GetPlayerDirections(AttackedTarget) == false) { // Incorrectly blocked
				AttackedTarget->IsBlocking = false;
				AttackedTarget->FlinchTrigger = true;
				AttackedTarget->ApplyDamage(CalcFinalDamage(GetDamage("CounteringBlow")));
			}
			else { // Correctly blocked
				FName Path = TEXT("/Game/Classes/Revenant/Animations/Recoil/Shield_Block_Recoil_Montage.Shield_Block_Recoil_Montage");
				UAnimMontage *BlockRecoilMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *Path.ToString()));
				AttackedTarget->RelayAnimation(BlockRecoilMontage, 1.0f);
			}
		}
	}
}
void ARevenant::KickHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if (CurrentAttackName == "StaggeringKick") {
			CurrentAttackHit = true;
			KickHurtbox->bGenerateOverlapEvents = false;
			ABaseCharacter* AttackedTarget = Cast<ABaseCharacter>(OtherActor);
			if (AttackedTarget->Invincible == false) {
				AttackedTarget->IsBlocking = false;
				AttackedTarget->FlinchTrigger = true;
				AttackedTarget->ApplyDamage(CalcFinalDamage(GetDamage("StaggeringKick")));
			}
		}
		else if (CurrentAttackName == "DebilitatingKick") {
			CurrentAttackHit = true;
			KickHurtbox->bGenerateOverlapEvents = false;
			ABaseCharacter* AttackedTarget = Cast<ABaseCharacter>(OtherActor);
			if (AttackedTarget->Invincible == false) {
				if (AttackedTarget->IsBlocking != true || GetPlayerDirections(AttackedTarget) == false) { // Incorrectly blocked
					AttackedTarget->IsBlocking = false;
					AttackedTarget->FlinchTrigger = true;
					AttackedTarget->ApplyDamage(CalcFinalDamage(GetDamage("DebilitatingKick")));
					AttackedTarget->SetCooldown("Roll", 1.5f);
				}
				else { // Correctly blocked
					FName Path = TEXT("/Game/Classes/Revenant/Animations/Recoil/Shield_Block_Recoil_Montage.Shield_Block_Recoil_Montage");
					UAnimMontage *BlockRecoilMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *Path.ToString()));
					AttackedTarget->RelayAnimation(BlockRecoilMontage, 1.0f);
				}
			}
		}
	}
}

/** Overriding Attack Effect Handler */
void ARevenant::AttackEffect(ABaseCharacter* Target, FString AttackName) {
	if (AttackName == "LifeDrain") {
		Health = UKismetMathLibrary::FClamp(Health + ((CurrentDamage * 2)/3), 0.0f, 100.0f);
	}
	else if (AttackName == "EmpoweringStrike") {
		AddDamageModifier(0.3f, 2.0f, 1);
	}
	else if (AttackName == "EnergyDrain") {
		Resilience = UKismetMathLibrary::FClamp(Resilience + 20.0f, 0.0f, 100.0f);
		Target->Resilience = UKismetMathLibrary::FClamp(Target->Resilience - 20.0f, 0.0f, 100.0f);
	}
}
void ARevenant::AttackExecute(FString AttackName) {
	if (this->HasAuthority()) {
		AttackExecuteClient(AttackName);
	}
	else {
		AttackExecuteServer(AttackName);
		AttackExecuteClient(AttackName);
	}
}
void ARevenant::AttackExecuteServer_Implementation(const FString &AttackName) {
	AttackExecuteClient(AttackName);
}
bool ARevenant::AttackExecuteServer_Validate(const FString &AttackName) {
	return true;
}
void ARevenant::AttackExecuteClient(FString AttackName) {
	// Standard Actions
	if (AttackName == "BlockRelease") {
		BlockReleasedEvent();
	}
	else if (AttackName == "Roll") {
		RollPressedEvent();
	}
	else if (AttackName == "SideStep") {
		SideStepPressedEvent();
	}
	// Utilities
	else if (AttackName == "ShadowStance") {
		if (Flinched == false && CanAttack == true && MenuUp == false && IsRolling == false && IsSideStepping == false && GetCooldown(AttackName) <= CurrentGameTime) {
			DetectMode = true;
			if (this->HasAuthority()) {
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Instigator = this;
				SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				FRotator TempRot = GetMesh()->K2_GetComponentRotation();
				TempRot.Yaw += 90;
				ABaseCharacter * SpawnedShadow = (GetWorld()->SpawnActor<ABaseCharacter>(Shadow, GetMesh()->K2_GetComponentLocation(), TempRot, SpawnParameters));

				if (IsBlocking == true) {
					SpawnedShadow->IsBlocking = true;
					SpawnedShadow->BlockingAnim = 1.0f;
				}
				TurnInvisibleRepAll();
			}
			SetCooldown(AttackName, GetFinalCooldownAmt(AttackName, GetCooldownAmt(AttackName)));
		}
	}
	else if (AttackName == "Agility") {
		if (Flinched == false && CanAttack == true && MenuUp == false && GetCooldown(AttackName) <= CurrentGameTime) {
			DetectMode = true;
			AgilityEffect = true;
			AddSpeedModifier(2.0f, 2.0f);
			TurnInvisibleRepAll();
			SetCooldown(AttackName, GetFinalCooldownAmt(AttackName, GetCooldownAmt(AttackName)));
		}
	}
	else {
		if (AttackName == "BlockPress") {
			BlockPressedEvent();
		}
		// Basic Attacks
		else if (AttackName == "SBasicAttack") {
			FName SBasicAttackAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/N_Basic_Attack_Montage.N_Basic_Attack_Montage");
			UAnimMontage *SBasicAttackAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *SBasicAttackAnimPath.ToString()));
			FName SBasicAttackComboAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/N_Basic_Attack_Combo_Montage.N_Basic_Attack_Combo_Montage");
			UAnimMontage *SBasicAttackComboAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *SBasicAttackComboAnimPath.ToString()));

			if (CheckChainable("SBasicAttack") == false) { // If no combo
				AttackHandler(
					"SBasicAttack", // Name
					"SBasicAttack", // Type
					0.45f, // Re-Casting delay
					1.0f, // Speed of Animation
					false, // Based on previous Attack, is it Chainable
					SBasicAttackAnimMontage, // Animation to use
					0.35f, // Delay before Hitbox starts
					0.2f, // Time duration of Hitbox
					GetDamage("SBasicAttack"), // Amount of damage
					false, // Whether or not to use hitbox instead
					NULL, // Which hitbox to initiate
					false); // Use Projectile?
			}
			else { // If combo
				AttackHandler(
					"SBasicAttack", // Name
					"SBasicAttack", // Type
					0.4f, // Re-Casting delay
					0.9f, // Speed of Animation
					true, // Based on previous Attack, is it Chainable
					SBasicAttackComboAnimMontage, // Animation to use
					0.2f, // Delay before Hitbox starts
					0.2f, // Time duration of Hitbox
					GetDamage("SBasicAttack"), // Amount of damage
					false, // Whether or not to use hitbox instead
					NULL, // Which hitbox to initiate
					false); // Use Projectile?
			}
		}
		else if (AttackName == "HBasicAttack") {
			FName HBasicAttackAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/H_Basic_Attack_Montage.H_Basic_Attack_Montage");
			UAnimMontage *HBasicAttackAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HBasicAttackAnimPath.ToString()));
			FName HBasicAttackComboAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/H_Basic_Attack_Combo_Montage.H_Basic_Attack_Combo_Montage");
			UAnimMontage *HBasicAttackComboAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HBasicAttackComboAnimPath.ToString()));

			if (CheckChainable("HBasicAttack") == false) { // If no combo
				AttackHandler(
					"HBasicAttack", // Name
					"HBasicAttack", // Type
					0.4f, // Re-Casting delay
					1.0f, // Speed of Animation
					false, // Based on previous Attack, is it Chainable
					HBasicAttackAnimMontage, // Animation to use
					0.6f, // Delay before Hitbox starts
					0.3f, // Time duration of Hitbox
					GetDamage("HBasicAttack"), // Amount of damage
					false, // Whether or not to use hitbox instead
					NULL, // Which hitbox to initiate
					false); // Use Projectile?
			}
			else { // If combo
				AttackHandler(
					"HBasicAttack", // Name
					"HBasicAttack", // Type
					0.4f, // Re-Casting delay
					1.0f, // Speed of Animation
					true, // Based on previous Attack, is it Chainable
					HBasicAttackComboAnimMontage, // Animation to use
					0.2f, // Delay before Hitbox starts
					0.3f, // Time duration of Hitbox
					GetDamage("HBasicAttack"), // Amount of damage
					false, // Whether or not to use hitbox instead
					NULL, // Which hitbox to initiate
					false); // Use Projectile?
			}
		}
		// Combo Extenders
		else if (AttackName == "CounteringBlow") {
			FName CounteringBlowAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Countering_Blow_Montage.Countering_Blow_Montage");
			UAnimMontage *CounteringBlowAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *CounteringBlowAnimPath.ToString()));
			AttackHandler(
				"CounteringBlow", // Name
				"CounteringBlow", // Type
				0.4f, // Re-Casting delay
				1.0f, // Speed of Animation
				false, // Based on previous Attack, is it Chainable
				CounteringBlowAnimMontage, // Animation to use
				0.2f, // Delay before Hitbox starts
				0.2f, // Time duration of Hitbox
				0.0f, // Amount of damage
				true, // Whether or not to use hitbox instead
				CounteringBlowHurtbox, // Which hitbox to initiate
				false); // Use Projectile?
		}
		else if (AttackName == "StaggeringKick") {
			FName KickAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Kick_Montage.Kick_Montage");
			UAnimMontage *KickAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *KickAnimPath.ToString()));
			AttackHandler(
				FString(TEXT("StaggeringKick")), // Name
				"HBasicAttack", // Type
				0.4f, // Re-Casting delay
				1.0f, // Speed of Animation
				CheckChainable("ComboExtender"), // Based on previous Attack, is it Chainable
				KickAnimMontage, // Animation to use
				0.3f, // Delay before Hitbox starts
				0.2f, // Time duration of Hitbox
				0.0f, // Amount of damage
				true, // Whether or not to use hitbox instead
				KickHurtbox, // Which hitbox to initiate
				false); // Use Projectile?
		}
		else if (AttackName == "Impede") {
			FName ImpedeAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Impede_Montage.Impede_Montage");
			UAnimMontage *ImpedeAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *ImpedeAnimPath.ToString()));
			AttackHandler(
				FString(TEXT("Impede")), // Name
				"ComboExtender", // Type
				0.4f, // Re-Casting delay
				1.0f, // Speed of Animation
				CheckChainable("ComboExtender"), // Based on previous Attack, is it Chainable
				ImpedeAnimMontage, // Animation to use
				0.2f, // Delay before Hitbox starts
				0.1f, // Time duration of Hitbox
				0.0f, // Amount of damage
				false, // Whether or not to use hitbox instead
				NULL, // Which hitbox to initiate
				true); // Use Projectile?
		}
		else if (AttackName == "LifeDrain") {
			FName DrainAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Drain_Montage.Drain_Montage");
			UAnimMontage *DrainAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *DrainAnimPath.ToString()));
			AttackHandler(
				FString(TEXT("LifeDrain")), // Name
				"ComboExtender", // Type
				0.4f, // Re-Casting delay
				1.0f, // Speed of Animation
				CheckChainable("ComboExtender"), // Based on previous Attack, is it Chainable
				DrainAnimMontage, // Animation to use
				0.05f, // Delay before Hitbox starts
				0.35f, // Time duration of Hitbox
				GetDamage("LifeDrain"), // Amount of damage
				false, // Whether or not to use hitbox instead
				NULL, // Which hitbox to initiate
				false); // Use Projectile?
		}
		else if (AttackName == "DebilitatingKick") {
			FName KickAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Kick_Montage.Kick_Montage");
			UAnimMontage *KickAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *KickAnimPath.ToString()));
			AttackHandler(
				FString(TEXT("DebilitatingKick")), // Name
				"ComboExtender", // Type
				0.4f, // Re-Casting delay
				1.0f, // Speed of Animation
				CheckChainable("ComboExtender"), // Based on previous Attack, is it Chainable
				KickAnimMontage, // Animation to use
				0.3f, // Delay before Hitbox starts
				0.2f, // Time duration of Hitbox
				0.0f, // Amount of damage
				true, // Whether or not to use hitbox instead
				KickHurtbox, // Which hitbox to initiate
				false); // Use Projectile?
		}
		else if (AttackName == "EmpoweringStrike") {
			FName EmpoweringStrikeAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Empowering_Strike_Montage.Empowering_Strike_Montage");
			UAnimMontage *EmpoweringStrikeAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *EmpoweringStrikeAnimPath.ToString()));
			AttackHandler(
				FString(TEXT("EmpoweringStrike")), // Name
				"ComboExtender", // Type
				0.4f, // Re-Casting delay
				1.0f, // Speed of Animation
				CheckChainable("ComboExtender"), // Based on previous Attack, is it Chainable
				EmpoweringStrikeAnimMontage, // Animation to use
				0.15f, // Delay before Hitbox starts
				0.2f, // Time duration of Hitbox
				GetDamage("EmpoweringStrike"), // Amount of damage
				false, // Whether or not to use hitbox instead
				NULL, // Which hitbox to initiate
				false); // Use Projectile?
		}
		else if (AttackName == "EnergyDrain") {
			FName DrainAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Drain_Montage.Drain_Montage");
			UAnimMontage *DrainAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *DrainAnimPath.ToString()));
			AttackHandler(
				FString(TEXT("EnergyDrain")), // Name
				"ComboExtender", // Type
				0.4f, // Re-Casting delay
				1.0f, // Speed of Animation
				CheckChainable("ComboExtender"), // Based on previous Attack, is it Chainable
				DrainAnimMontage, // Animation to use
				0.05f, // Delay before Hitbox starts
				0.35f, // Time duration of Hitbox
				GetDamage("EnergyDrain"), // Amount of damage
				false, // Whether or not to use hitbox instead
				NULL, // Which hitbox to initiate
				false); // Use Projectile?
		}
		if (DetectMode == true) {
			DetectAction();
		}
	}
}
/** Set Cooldowns */
float ARevenant::SetAttackCooldownAmt(FString AttackName) {
	if (AttackName == "CounteringBlow") {
		return 6.0f;
	}
	else if (AttackName == "StaggeringKick") {
		return 12.0f;
	}
	else if (AttackName == "Impede") {
		return 7.0f;
	}
	else if (AttackName == "LifeDrain") {
		return 12.0f;
	}
	else if (AttackName == "DebilitatingKick") {
		return 12.0f;
	}
	else if (AttackName == "EmpoweringStrike") {
		return 10.0f;
	}
	else if (AttackName == "EnergyDrain") {
		return 1.0f;
	}
	else if (AttackName == "ShadowStance") {
		return 14.0f;
	}
	else if (AttackName == "Agility") {
		return 14.0f;
	}
	else {
		return 0.0f;
	}
}

/** Function that will call detect action, which will call function declaring to all */
void ARevenant::TurnInvisibleRepAll_Implementation()
{
	GetMesh()->SetOnlyOwnerSee(true);
	Shield->SetOnlyOwnerSee(true);
	WeaponVisibility(true);
	HPOverhead->SetVisibility(false);
	GetWorldTimerManager().SetTimer(delay2TimerHandle, this, &ARevenant::DetectAction, 2.0f, false);
}
/** Function for detecting abilities */
void ARevenant::DetectAction() {
	if (DetectMode == true) {
		if (this->HasAuthority()) {
			DetectRepAll();
		}
	}
}
void ARevenant::DetectRepAll_Implementation()
{
	GetMesh()->SetOnlyOwnerSee(false);
	Shield->SetOnlyOwnerSee(false);
	WeaponVisibility(false);
	HPOverhead->SetVisibility(true);
	DetectMode = false;
	AgilityEffect = false;
}