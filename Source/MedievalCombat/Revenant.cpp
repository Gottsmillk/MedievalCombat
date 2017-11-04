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

	// Countering Blow hurtbox
	CounteringBlowHurtbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Countering Blow Hurtbox"));
	CounteringBlowHurtbox->SetVisibility(true);
	CounteringBlowHurtbox->SetHiddenInGame(false);
	CounteringBlowHurtbox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	CounteringBlowHurtbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CounteringBlowHurtbox->bDynamicObstacle = false;
	CounteringBlowHurtbox->bGenerateOverlapEvents = true;
	CounteringBlowHurtbox->SetupAttachment(GetMesh(), TEXT("Weaponsocket"));

	// Staggering Kick hurtbox
	KickHurtbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Kick Hurtbox"));
	KickHurtbox->SetVisibility(true);
	KickHurtbox->SetHiddenInGame(false);
	KickHurtbox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	KickHurtbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	KickHurtbox->bDynamicObstacle = false;
	KickHurtbox->bGenerateOverlapEvents = true;
	KickHurtbox->SetupAttachment(GetMesh(), TEXT("Kicksocket"));

	// Powered Bash hurtbox
	PoweredBashHurtbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Powered Bash Hurtbox"));
	PoweredBashHurtbox->SetVisibility(true);
	PoweredBashHurtbox->SetHiddenInGame(false);
	PoweredBashHurtbox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	PoweredBashHurtbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PoweredBashHurtbox->bDynamicObstacle = false;
	PoweredBashHurtbox->bGenerateOverlapEvents = true;
	PoweredBashHurtbox->SetupAttachment(GetMesh(), TEXT("Shieldsocket"));

	// Set Shadow to be spawned
	static ConstructorHelpers::FObjectFinder<UClass> ShadowBlueprint(TEXT("Class'/Game/Classes/Revenant/Blueprints/RevenantShadowBP.RevenantShadowBP_C'"));
	if (ShadowBlueprint.Object) {
		Shadow = ShadowBlueprint.Object;
	}

	// Set UnwaverFX to be spawned
	static ConstructorHelpers::FObjectFinder<UClass> UnwaverFXBlueprint(TEXT("Class'/Game/Blueprints/Particles/Blueprints/Unwaver_FX.Unwaver_FX_C'"));
	if (UnwaverFXBlueprint.Object) {
		UnwaverFX = UnwaverFXBlueprint.Object;
	}

	// Set ImpairFX to be spawned
	static ConstructorHelpers::FObjectFinder<UClass> ImpairFXBlueprint(TEXT("Class'/Game/Blueprints/Particles/Blueprints/Impair_FX.Impair_FX_C'"));
	if (ImpairFXBlueprint.Object) {
		ImpairFX = ImpairFXBlueprint.Object;
	}

	// Set FortifyFX to be spawned
	static ConstructorHelpers::FObjectFinder<UClass> FortifyFXBlueprint(TEXT("Class'/Game/Blueprints/Particles/Blueprints/Fortify_FX.Fortify_FX_C'"));
	if (FortifyFXBlueprint.Object) {
		FortifyFX = FortifyFXBlueprint.Object;
	}
}

void ARevenant::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ARevenant, AgilityEffect);
	DOREPLIFETIME(ARevenant, ImpairActive);
}

// Called when the game starts or when spawned
void ARevenant::BeginPlay()
{
	Super::BeginPlay();
	LoadBasicAttackAnimations();
	CounteringBlowHurtbox->OnComponentBeginOverlap.AddDynamic(this, &ARevenant::CounteringBlowHurtboxOverlap);
	KickHurtbox->OnComponentBeginOverlap.AddDynamic(this, &ARevenant::KickHurtboxOverlap);
	PoweredBashHurtbox->OnComponentBeginOverlap.AddDynamic(this, &ARevenant::PoweredBashHurtboxOverlap);
}

// Called to bind functionality to input
void ARevenant::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
void ARevenant::AddRemainingInputs() {
	FInputActionBinding ActionBind;
	FInputActionHandlerSignature ActionBindHandler;

	InputComponent->ClearActionBindings();

	// START OF DEFAULTS

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

	// END OF DEFAULTS

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

	// Add ComboFinisher 1
	ActionBind.ActionName = FName("ComboFinisher1");
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), ComboFinisherArray[0]);
	ActionBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ActionBind);

	// Add ComboFinisher 2
	ActionBind.ActionName = FName("ComboFinisher2");
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), ComboFinisherArray[1]);
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
	DamageTable.Add(FDamageTableStruct("ChannelingStrike", 15.0f));
	DamageTable.Add(FDamageTableStruct("PoisonBlade", 2.0f));
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
	AttackArray.Add(FAttackStruct(ComboFinisherArray[0], 0.0f, SetAttackCooldownAmt(ComboFinisherArray[0])));
	AttackArray.Add(FAttackStruct(ComboFinisherArray[1], 0.0f, SetAttackCooldownAmt(ComboFinisherArray[1])));

	MenuUp = false;
}

void ARevenant::ResetSelectedAbilityArrays() {
	if (this->HasAuthority()) {
		ResetSelectedAbilityArraysClient();
	}
	else {
		ResetSelectedAbilityArraysServer();
		ResetSelectedAbilityArraysClient();
	}
}
void ARevenant::ResetSelectedAbilityArraysServer_Implementation() {
	ResetSelectedAbilityArraysClient();
}
bool ARevenant::ResetSelectedAbilityArraysServer_Validate() {
	return true;
}
void ARevenant::ResetSelectedAbilityArraysClient() {
	ComboExtenderArray.Empty();
	UtilityArray.Empty();
	ComboFinisherArray.Empty();
	AttackArray.Empty();
	DamageTable.Empty();
}

void ARevenant::LoadBasicAttackAnimations() {
	// Set Animation arrays (looks ugly but much more convenient than other means)
	FName TempAttackName = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage1.SBasicAttack_Combo_Montage1");
	FName TempAttackName2 = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage2.SBasicAttack_Combo_Montage2");
	FName TempAttackName3 = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage3.SBasicAttack_Combo_Montage3");
	FName TempAttackName4 = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage4.SBasicAttack_Combo_Montage4");
	FName TempAttackName5 = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage5.SBasicAttack_Combo_Montage5");
	FName TempAttackName6 = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage6.SBasicAttack_Combo_Montage6");
	FName TempAttackName7 = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage7.SBasicAttack_Combo_Montage7");
	FName TempAttackName8 = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage8.SBasicAttack_Combo_Montage8");
	FName TempAttackName9 = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage9.SBasicAttack_Combo_Montage9");
	FName TempAttackName10 = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage10.SBasicAttack_Combo_Montage10");
	FName TempAttackName11 = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage11.SBasicAttack_Combo_Montage11");
	FName TempAttackName12 = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage12.SBasicAttack_Combo_Montage12");
	FName TempAttackName13 = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage13.SBasicAttack_Combo_Montage13");
	FName TempAttackName14 = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage14.SBasicAttack_Combo_Montage14");
	FName TempAttackName15 = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage15.SBasicAttack_Combo_Montage15");
	FName TempAttackName16 = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage16.SBasicAttack_Combo_Montage16");
	FName TempAttackName17 = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Combo_Montage17.SBasicAttack_Combo_Montage17");
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName.ToString())));
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName2.ToString())));
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName3.ToString())));
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName4.ToString())));
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName5.ToString())));
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName6.ToString())));
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName7.ToString())));
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName8.ToString())));
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName9.ToString())));
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName10.ToString())));
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName11.ToString())));
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName12.ToString())));
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName13.ToString())));
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName14.ToString())));
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName15.ToString())));
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName16.ToString())));
	SBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *TempAttackName17.ToString())));

	FName HTempAttackName = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage1.HBasicAttack_Combo_Montage1");
	FName HTempAttackName2 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage2.HBasicAttack_Combo_Montage2");
	FName HTempAttackName3 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage3.HBasicAttack_Combo_Montage3");
	FName HTempAttackName4 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage4.HBasicAttack_Combo_Montage4");
	FName HTempAttackName5 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage5.HBasicAttack_Combo_Montage5");
	FName HTempAttackName6 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage6.HBasicAttack_Combo_Montage6");
	FName HTempAttackName7 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage7.HBasicAttack_Combo_Montage7");
	FName HTempAttackName8 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage8.HBasicAttack_Combo_Montage8");
	FName HTempAttackName9 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage9.HBasicAttack_Combo_Montage9");
	FName HTempAttackName10 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage10.HBasicAttack_Combo_Montage10");
	FName HTempAttackName11 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage11.HBasicAttack_Combo_Montage11");
	FName HTempAttackName12 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage12.HBasicAttack_Combo_Montage12");
	FName HTempAttackName13 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage13.HBasicAttack_Combo_Montage13");
	FName HTempAttackName14 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage14.HBasicAttack_Combo_Montage14");
	FName HTempAttackName15 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage15.HBasicAttack_Combo_Montage15");
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName2.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName3.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName4.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName5.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName6.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName7.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName8.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName9.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName10.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName11.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName12.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName13.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName14.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName15.ToString())));

}

UAnimMontage* ARevenant::GetRandomMontage(TArray<UAnimMontage *> MontageArray) {
	return MontageArray[UKismetMathLibrary::RandomIntegerInRange(0, MontageArray.Num()-1)];
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
	else if (Type == "ComboFinisher") {
		if (Toggled == true) {
			ComboFinisherArray.Add(AttackName);
		}
		else {
			int i;
			for (i = 0; i < ComboFinisherArray.Num(); i++) {
				if (ComboFinisherArray[i] == AttackName) {
					break;
				}
			}
			if (i < ComboFinisherArray.Num()) {
				ComboFinisherArray.RemoveAt(i, 1, true);
			}
		}
	}
}
/* Hitbox Events */
void ARevenant::CounteringBlowHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{ 
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		CounteringBlowHurtbox->bGenerateOverlapEvents = false;
		ABaseCharacter* AttackedTarget = Cast<ABaseCharacter>(OtherActor);
		InflictDamage(AttackedTarget, CalcFinalDamage(GetDamage(CurrentAttackName)), true, true);
	}
}
void ARevenant::KickHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		KickHurtbox->bGenerateOverlapEvents = false;
		ABaseCharacter* AttackedTarget = Cast<ABaseCharacter>(OtherActor);

		if (CurrentAttackName == "StaggeringKick") {
			InflictDamage(AttackedTarget, CalcFinalDamage(GetDamage(CurrentAttackName)), false, true);
		}
		else if (CurrentAttackName == "DebilitatingKick") {
			if (InflictDamage(AttackedTarget, CalcFinalDamage(GetDamage(CurrentAttackName)), false, true) == true) {
				AttackedTarget->SetCooldown("Roll", 1.5f);
			}
		}
	}
}
void ARevenant::PoweredBashHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		PoweredBashHurtbox->bGenerateOverlapEvents = false;
		ABaseCharacter* AttackedTarget = Cast<ABaseCharacter>(OtherActor);
		InflictDamage(AttackedTarget, CalcFinalDamage(3 + (ComboAmount * 2.0f)), true, true);
	}
}

/** Apply override so player who received damage can send events to player who dealt damage */
void ARevenant::SendEventToAttacker(ABaseCharacter* Attacker) {
	if (ImpairActive == true) {
		Attacker->AddCooldownModifier(1.5f, Attacker->CurrentAttackName, 1.0f);
		ImpairActive = false;
	}
}

/** Overriding Attack Effect Handler */
void ARevenant::AttackEffect(ABaseCharacter* Target, FString AttackName) {
	if (AttackName == "LifeDrain") {
		Health = UKismetMathLibrary::FClamp(Health + ((CurrentDamage * 2)/3), 0.0f, 100.0f);
	}
	else if (AttackName == "EmpoweringStrike") {
		AddDamageModifier(0.2f, 2.0f, 1);
	}
	else if (AttackName == "EnergyDrain") {
		Resilience = UKismetMathLibrary::FClamp(Resilience + 20.0f, 0.0f, 100.0f);
		Target->Resilience = UKismetMathLibrary::FClamp(Target->Resilience - 20.0f, 0.0f, 100.0f);
	}
	else if (AttackName == "ChannelingStrike") {
		float PushValue = 500 + (20 * ComboAmount);
		FVector TempVector = (this->GetActorForwardVector() * PushValue) + (this->GetActorUpVector() * PushValue);
		Target->LaunchCharacter(TempVector, true, true);
	}
	else if (AttackName == "PoisonBlade") {
		Target->ApplyDamageOverTime("FlinchOnLast", CalcFinalDamage(2.5f), 5, 2.0f, 1.0f);
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
		if (Flinched == false && CanAttack == true && MenuUp == false && IsRolling == false && IsSideStepping == false && GetCooldown(AttackName) <= CurrentGameTime&& this->GetMovementComponent()->IsMovingOnGround() == true) {
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
	else if (AttackName == "Unwaver") {
		if (Flinched == false && MenuUp == false && GetCooldown(AttackName) <= CurrentGameTime) {
			SuperArmor = true;
			if (DetectMode == false) {
				InitializeParticle(UnwaverFX);
			}
			SetCooldown(AttackName, GetFinalCooldownAmt(AttackName, GetCooldownAmt(AttackName)));
			GetWorldTimerManager().SetTimer(delay3TimerHandle, this, &ARevenant::UnsetAntiFlinch, 1.0f, false);
		}
	}
	else if (AttackName == "Impair") {
		if (MenuUp == false && GetCooldown(AttackName) <= CurrentGameTime) {
			if (DetectMode == false) {
				InitializeParticle(ImpairFX);
			}
			SetCooldown(AttackName, GetFinalCooldownAmt(AttackName, GetCooldownAmt(AttackName)));
			ImpairActive = true;
			GetWorldTimerManager().SetTimer(delay3TimerHandle, this, &ARevenant::RemoveImpairActive, 1.0f, false);
		}
	}
	else if (AttackName == "Fortify") {
		if (MenuUp == false && GetCooldown(AttackName) <= CurrentGameTime) {
			if (DetectMode == false) {
				InitializeParticle(FortifyFX);
			}
			AddDefenseModifier(1.0f, 1.0f, 99);
			SetCooldown(AttackName, GetFinalCooldownAmt(AttackName, GetCooldownAmt(AttackName)));
		}
	}
	else {
		if (AttackName == "BlockPress") {
			BlockPressedEvent();
		}
		else if (this->GetMovementComponent()->IsMovingOnGround() == true) {
			// Basic Attacks
			if (AttackName == "SBasicAttack") {
				FName SBasicAttackAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/BasicAttacks/SBasicAttack_Montage.SBasicAttack_Montage");
				UAnimMontage *SBasicAttackAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *SBasicAttackAnimPath.ToString()));

				if (CheckChainable(AttackName) == false) { // If no combo
					AttackHandler(
						AttackName, // Name
						AttackName, // Type
						0.5f, // Re-Casting delay
						1.0f, // Speed of Animation
						false, // Based on previous Attack, is it Chainable
						SBasicAttackAnimMontage, // Animation to use
						0.3f, // Delay before Hitbox starts
						0.2f, // Time duration of Hitbox
						GetDamage(AttackName), // Amount of damage
						false, // Whether or not to use hitbox instead
						NULL, // Which hitbox to initiate
						false); // Use Projectile?
				}
				else { // If combo
					AttackHandler(
						AttackName, // Name
						AttackName, // Type
						0.5f, // Re-Casting delay
						0.9f, // Speed of Animation
						true, // Based on previous Attack, is it Chainable
						GetRandomMontage(SBasicAttackAnims), // Animation to use
						0.2f, // Delay before Hitbox starts
						0.2f, // Time duration of Hitbox
						GetDamage(AttackName), // Amount of damage
						false, // Whether or not to use hitbox instead
						NULL, // Which hitbox to initiate
						false); // Use Projectile?
				}
			}
			else if (AttackName == "HBasicAttack") {
				FName HBasicAttackAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Montage.HBasicAttack_Montage");
				UAnimMontage *HBasicAttackAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HBasicAttackAnimPath.ToString()));

				if (CheckChainable(AttackName) == false) { // If no combo
					AttackHandler(
						AttackName, // Name
						AttackName, // Type
						0.5f, // Re-Casting delay
						1.0f, // Speed of Animation
						false, // Based on previous Attack, is it Chainable
						HBasicAttackAnimMontage, // Animation to use
						0.6f, // Delay before Hitbox starts
						0.3f, // Time duration of Hitbox
						GetDamage(AttackName), // Amount of damage
						false, // Whether or not to use hitbox instead
						NULL, // Which hitbox to initiate
						false); // Use Projectile?
				}
				else { // If combo
					AttackHandler(
						AttackName, // Name
						AttackName, // Type
						0.5f, // Re-Casting delay
						1.0f, // Speed of Animation
						true, // Based on previous Attack, is it Chainable
						GetRandomMontage(HBasicAttackAnims), // Animation to use
						0.2f, // Delay before Hitbox starts
						0.3f, // Time duration of Hitbox
						GetDamage(AttackName), // Amount of damage
						false, // Whether or not to use hitbox instead
						NULL, // Which hitbox to initiate
						false); // Use Projectile?
				}
			}
			else if (AttackName == "CounteringBlow") {
				FName CounteringBlowAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Countering_Blow_Montage.Countering_Blow_Montage");
				UAnimMontage *CounteringBlowAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *CounteringBlowAnimPath.ToString()));
				AttackHandler(
					AttackName, // Name
					AttackName, // Type
					0.5f, // Re-Casting delay
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
			// Combo Extenders
			else if (AttackName == "StaggeringKick") {
				FName KickAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Kick_Montage.Kick_Montage");
				UAnimMontage *KickAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *KickAnimPath.ToString()));
				AttackHandler(
					AttackName, // Name
					"HBasicAttack", // Type
					0.5f, // Re-Casting delay
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
					AttackName, // Name
					"ComboExtender", // Type
					0.5f, // Re-Casting delay
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
					AttackName, // Name
					"ComboExtender", // Type
					0.5f, // Re-Casting delay
					1.0f, // Speed of Animation
					CheckChainable("ComboExtender"), // Based on previous Attack, is it Chainable
					DrainAnimMontage, // Animation to use
					0.05f, // Delay before Hitbox starts
					0.35f, // Time duration of Hitbox
					GetDamage(AttackName), // Amount of damage
					false, // Whether or not to use hitbox instead
					NULL, // Which hitbox to initiate
					false); // Use Projectile?
			}
			else if (AttackName == "DebilitatingKick") {
				FName KickAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Kick_Montage.Kick_Montage");
				UAnimMontage *KickAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *KickAnimPath.ToString()));
				AttackHandler(
					AttackName, // Name
					"ComboExtender", // Type
					0.5f, // Re-Casting delay
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
					AttackName, // Name
					"ComboExtender", // Type
					0.5f, // Re-Casting delay
					1.0f, // Speed of Animation
					CheckChainable("ComboExtender"), // Based on previous Attack, is it Chainable
					EmpoweringStrikeAnimMontage, // Animation to use
					0.15f, // Delay before Hitbox starts
					0.2f, // Time duration of Hitbox
					GetDamage(AttackName), // Amount of damage
					false, // Whether or not to use hitbox instead
					NULL, // Which hitbox to initiate
					false); // Use Projectile?
			}
			else if (AttackName == "EnergyDrain") {
				FName DrainAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Drain_Montage.Drain_Montage");
				UAnimMontage *DrainAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *DrainAnimPath.ToString()));
				AttackHandler(
					AttackName, // Name
					"ComboExtender", // Type
					0.5f, // Re-Casting delay
					1.0f, // Speed of Animation
					CheckChainable("ComboExtender"), // Based on previous Attack, is it Chainable
					DrainAnimMontage, // Animation to use
					0.05f, // Delay before Hitbox starts
					0.35f, // Time duration of Hitbox
					GetDamage(AttackName), // Amount of damage
					false, // Whether or not to use hitbox instead
					NULL, // Which hitbox to initiate
					false); // Use Projectile?
			}
			// Combo Finishers
			else if (AttackName == "PoweredBash") {
				FName PoweredBashAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Powered_Bash_Montage.Powered_Bash_Montage");
				UAnimMontage *PoweredBashAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *PoweredBashAnimPath.ToString()));
				FName PoweredBashComboAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Powered_Bash_Combo_Montage.Powered_Bash_Combo_Montage");
				UAnimMontage *PoweredBashComboAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *PoweredBashComboAnimPath.ToString()));
				if (CheckChainable("ComboFinisher") == false) {
					AttackHandler(
						AttackName, // Name
						"ComboFinisher", // Type
						0.5f, // Re-Casting delay
						1.0f, // Speed of Animation
						false, // Based on previous Attack, is it Chainable
						PoweredBashAnimMontage, // Animation to use
						0.6f, // Delay before Hitbox starts
						0.1f, // Time duration of Hitbox
						0.0f, // Amount of damage
						true, // Whether or not to use hitbox instead
						PoweredBashHurtbox, // Which hitbox to initiate
						false); // Use Projectile?
				}
				else { // If combo
					AttackHandler(
						AttackName, // Name
						"ComboFinisher", // Type
						0.7f, // Re-Casting delay
						1.0f, // Speed of Animation
						true, // Based on previous Attack, is it Chainable
						PoweredBashComboAnimMontage, // Animation to use
						0.3f, // Delay before Hitbox starts
						0.2f, // Time duration of Hitbox
						0.0f, // Amount of damage
						true, // Whether or not to use hitbox instead
						PoweredBashHurtbox, // Which h
						false); // Use Projectile?
				}
			}
			else if (AttackName == "ChannelingStrike") {
				FName ChannelingStrikeAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Channeling_Strike_Montage.Channeling_Strike_Montage");
				UAnimMontage *ChannelingStrikeAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *ChannelingStrikeAnimPath.ToString()));
				FName ChannelingStrikeComboAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Channeling_Strike_Combo_Montage.Channeling_Strike_Combo_Montage");
				UAnimMontage *ChannelingStrikeComboAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *ChannelingStrikeComboAnimPath.ToString()));

				if (CheckChainable("ComboFinisher") == false) { // If no combo
					AttackHandler(
						AttackName, // Name
						"ComboFinisher", // Type
						0.5f, // Re-Casting delay
						1.0f, // Speed of Animation
						false, // Based on previous Attack, is it Chainable
						ChannelingStrikeAnimMontage, // Animation to use
						0.35f, // Delay before Hitbox starts
						0.5f, // Time duration of Hitbox
						GetDamage(AttackName), // Amount of damage
						false, // Whether or not to use hitbox instead
						NULL, // Which hitbox to initiate
						false); // Use Projectile?
				}
				else { // If combo
					AttackHandler(
						AttackName, // Name
						"ComboFinisher", // Type
						0.65f, // Re-Casting delay
						1.0f, // Speed of Animation
						true, // Based on previous Attack, is it Chainable
						ChannelingStrikeComboAnimMontage, // Animation to use
						0.25f, // Delay before Hitbox starts
						0.35f, // Time duration of Hitbox
						GetDamage(AttackName), // Amount of damage
						false, // Whether or not to use hitbox instead
						NULL, // Which hitbox to initiate
						false); // Use Projectile?
				}
			}
			else if (AttackName == "PoisonBlade") {
				FName PoisonBladeAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Poison_Blade_Montage.Poison_Blade_Montage");
				UAnimMontage *ChannelingStrikeAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *PoisonBladeAnimPath.ToString()));
				FName PoisonBladeComboAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Poison_Blade_Combo_Montage.Poison_Blade_Combo_Montage");
				UAnimMontage *PoisonBladeComboAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *PoisonBladeComboAnimPath.ToString()));

				if (CheckChainable("ComboFinisher") == false) { // If no combo
					AttackHandler(
						AttackName, // Name
						"ComboFinisher", // Type
						0.5f, // Re-Casting delay
						1.0f, // Speed of Animation
						false, // Based on previous Attack, is it Chainable
						ChannelingStrikeAnimMontage, // Animation to use
						0.35f, // Delay before Hitbox starts
						0.4f, // Time duration of Hitbox
						GetDamage(AttackName), // Amount of damage
						false, // Whether or not to use hitbox instead
						NULL, // Which hitbox to initiate
						false); // Use Projectile?
				}
				else { // If combo
					AttackHandler(
						AttackName, // Name
						"ComboFinisher", // Type
						0.7f, // Re-Casting delay
						1.0f, // Speed of Animation
						true, // Based on previous Attack, is it Chainable
						PoisonBladeComboAnimMontage, // Animation to use
						0.25f, // Delay before Hitbox starts
						0.3f, // Time duration of Hitbox
						GetDamage(AttackName), // Amount of damage
						false, // Whether or not to use hitbox instead
						NULL, // Which hitbox to initiate
						false); // Use Projectile?
				}
			}
			else if (AttackName == "LastResort") {
				FName LastResortAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Last_Resort_Montage.Last_Resort_Montage");
				UAnimMontage *LastResortAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *LastResortAnimPath.ToString()));
				FName LastResortComboAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Last_Resort_Combo_Montage.Last_Resort_Combo_Montage");
				UAnimMontage *LastResortComboAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *LastResortComboAnimPath.ToString()));

				if (CheckChainable("ComboFinisher") == false) { // If no combo
					AttackHandler(
						AttackName, // Name
						"ComboFinisher", // Type
						0.5f, // Re-Casting delay
						1.0f, // Speed of Animation
						false, // Based on previous Attack, is it Chainable
						LastResortAnimMontage, // Animation to use
						0.6f, // Delay before Hitbox starts
						0.2f, // Time duration of Hitbox
						UKismetMathLibrary::MultiplyMultiply_FloatFloat(3.0f, (3.0f - (Resilience/50.0f))), // Amount of damage
						false, // Whether or not to use hitbox instead
						NULL, // Which hitbox to initiate
						false); // Use Projectile?
				}
				else { // If combo
					AttackHandler(
						AttackName, // Name
						"ComboFinisher", // Type
						0.8f, // Re-Casting delay
						1.0f, // Speed of Animation
						true, // Based on previous Attack, is it Chainable
						LastResortComboAnimMontage, // Animation to use
						0.4f, // Delay before Hitbox starts
						0.1f, // Time duration of Hitbox
						UKismetMathLibrary::MultiplyMultiply_FloatFloat(3.0f, (3.0f - (Resilience / 50.0f))), // Amount of damage
						false, // Whether or not to use hitbox instead
						NULL, // Which hitbox to initiate
						false); // Use Projectile?
				}
			}
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
		return 11.0f;
	}
	else if (AttackName == "ShadowStance") {
		return 14.0f;
	}
	else if (AttackName == "Agility") {
		return 14.0f;
	}
	else if (AttackName == "Unwaver") {
		return 13.0f;
	}
	else if (AttackName == "Impair") {
		return 10.0f;
	}
	else if (AttackName == "Fortify") {
		return 10.0f;
	}
	else if (AttackName == "PoweredBash") {
		return 20.0f;
	}
	else if (AttackName == "ChannelingStrike") {
		return 16.0f;
	}
	else if (AttackName == "PoisonBlade") {
		return 20.0f;
	}
	else if (AttackName == "LastResort") {
		return 25.0f;
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

void ARevenant::UnsetAntiFlinch() {
	SuperArmor = false;
}
void ARevenant::RemoveImpairActive() {
	ImpairActive = false;
}