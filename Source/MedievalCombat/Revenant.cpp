#include "MedievalCombat.h"
#include "UnrealNetwork.h"
#include "Revenant.h"

/** Where attack effects are handled:
  * Note: All attacks use AttackExecute (Attack Handler Functions)
  *       Listed below are external dependencies
  * 
  * ***********Basic Abilities***********
  * 
  * Basic Attacks - None
  * SideStep - BaseCharacter
  * Roll - BaseCharacter
  * Block - BaseCharacter
  * Countering Blow - CounteringBlowHurtboxOverlap (Attack Handler Functions)
  *
  * ***********Combo Extender Abilities***********
  * 
  * Staggering Kick - KickHurtboxOverlap (Attack Handler Functions)
  * Impede - BaseProjectile Blueprint and C++ class
  * Life Drain - AttackEffect (Attack Handler functions)
  * Debilitating Kick - KickHurtboxOverlap (Attack Handler Functions)
  * Empowering Strike - AttackEffect (Attack Handler functions)
  * Energy Drain - AttackEffect (Attack Handler functions)
  * 
  * ***********Utility Abilities***********
  * 
  * Shadow Stance - DetectAction (Attack Effect Functions)
  * Agility - DetectAction, Revenant Blueprint (for FX) (Attack Effect Functions)
  * Unwaver - UnsetAntiFlinch (Attack Effect Functions)
  * Impair - RemoveImpairActive, SendEventToAttacker (Attack Effect, Death/Damage Functions)
  * Fortify - None
  * 
  * ***********Combo Finisher Abilities***********
  * 
  * Powered Bash - PoweredBashHurtboxOverlap (Attack Handler Functions)
  * Channeling Strike - AttackEffect (Attack Handler functions)
  * Poison Blade - AttackEffect, DoT Component C++ Class (Attack Handler functions)
  * Last Resort - None
  */

/* ------------------------------- INITIALIZATIONS -------------------------------- */

// Constructor for Revenant, includes blueprint instantiation
ARevenant::ARevenant() {
	bReplicates = true;
	// Create shield
	Shield = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shield"));
	Shield->SetVisibility(true);
	Shield->SetHiddenInGame(false);
	Shield->SetGenerateOverlapEvents(false);
	FName ShieldSocket = TEXT("Shieldsocket");
	Shield->SetupAttachment(GetMesh(), ShieldSocket);

	// Countering Blow hurtbox
	CounteringBlowHurtbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Countering Blow Hurtbox"));
	CounteringBlowHurtbox->SetVisibility(true);
	CounteringBlowHurtbox->SetHiddenInGame(false);
	CounteringBlowHurtbox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	CounteringBlowHurtbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CounteringBlowHurtbox->bDynamicObstacle = false;
	CounteringBlowHurtbox->SetGenerateOverlapEvents(true);
	CounteringBlowHurtbox->SetupAttachment(GetMesh(), TEXT("Weaponsocket"));

	// Staggering Kick hurtbox
	KickHurtbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Kick Hurtbox"));
	KickHurtbox->SetVisibility(true);
	KickHurtbox->SetHiddenInGame(false);
	KickHurtbox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	KickHurtbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	KickHurtbox->bDynamicObstacle = false;
	KickHurtbox->SetGenerateOverlapEvents(true);
	KickHurtbox->SetupAttachment(GetMesh(), TEXT("Kicksocket"));

	// Powered Bash hurtbox
	PoweredBashHurtbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Powered Bash Hurtbox"));
	PoweredBashHurtbox->SetVisibility(true);
	PoweredBashHurtbox->SetHiddenInGame(false);
	PoweredBashHurtbox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	PoweredBashHurtbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PoweredBashHurtbox->bDynamicObstacle = false;
	PoweredBashHurtbox->SetGenerateOverlapEvents(true);
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

	// Instantiate BodyMeshMaterial
	static ConstructorHelpers::FObjectFinder<UMaterial> FoundBodyMeshMaterial(TEXT("/Game/Classes/Revenant/Models/Materials/M_Char_Forge.M_Char_Forge"));
	if (FoundBodyMeshMaterial.Succeeded())
	{
		BodyMeshMaterial = FoundBodyMeshMaterial.Object;
	}
	BodyMeshMaterialInst = UMaterialInstanceDynamic::Create(BodyMeshMaterial, GetMesh());

	// Instantiate WeaponMeshMaterial
	static ConstructorHelpers::FObjectFinder<UMaterial> FoundWeaponMeshMaterial(TEXT("/Game/Classes/Revenant/Models/Weapon/M_Blade_BlackKnight.M_Blade_BlackKnight"));
	if (FoundWeaponMeshMaterial.Succeeded())
	{
		WeaponMeshMaterial = FoundWeaponMeshMaterial.Object;
	}
	WeaponMeshMaterialInst = UMaterialInstanceDynamic::Create(WeaponMeshMaterial, Weapon);

	// Instantiate ShieldMeshMaterial
	static ConstructorHelpers::FObjectFinder<UMaterial> FoundShieldMeshMaterial(TEXT("/Game/Classes/Revenant/Models/Weapon/Shield_Material.Shield_Material"));
	if (FoundShieldMeshMaterial.Succeeded())
	{
		ShieldMeshMaterial = FoundShieldMeshMaterial.Object;
	}
	ShieldMeshMaterialInst = UMaterialInstanceDynamic::Create(ShieldMeshMaterial, Shield);

	// Instantiate ShadowMaterial for combo extender
	static ConstructorHelpers::FObjectFinder<UMaterial> FoundShadowMeshComboExtenderMaterial(TEXT("/Game/Blueprints/WeaponEffects/Revenant/ComboExtenderMeshMaterial.ComboExtenderMeshMaterial"));
	if (FoundShadowMeshComboExtenderMaterial.Succeeded())
	{
		ShadowMaterialComboExtender = FoundShadowMeshComboExtenderMaterial.Object;
	}
	ShadowMaterialBodyInstComboExtender = UMaterialInstanceDynamic::Create(ShadowMaterialComboExtender, GetMesh());
	ShadowMaterialWeaponInstComboExtender = UMaterialInstanceDynamic::Create(ShadowMaterialComboExtender, Weapon);
	ShadowMaterialShieldInstComboExtender = UMaterialInstanceDynamic::Create(ShadowMaterialComboExtender, Shield);

	// Instantiate ShadowMaterial for combo extender
	static ConstructorHelpers::FObjectFinder<UMaterial> FoundShadowMeshComboFinisherMaterial(TEXT("/Game/Blueprints/WeaponEffects/Revenant/ComboFinisherMeshMaterial.ComboFinisherMeshMaterial"));
	if (FoundShadowMeshComboFinisherMaterial.Succeeded())
	{
		ShadowMaterialComboFinisher = FoundShadowMeshComboFinisherMaterial.Object;
	}
	ShadowMaterialBodyInstComboFinisher = UMaterialInstanceDynamic::Create(ShadowMaterialComboFinisher, GetMesh());
	ShadowMaterialWeaponInstComboFinisher = UMaterialInstanceDynamic::Create(ShadowMaterialComboFinisher, Weapon);
	ShadowMaterialShieldInstComboFinisher = UMaterialInstanceDynamic::Create(ShadowMaterialComboFinisher, Shield);
}

// Allows Replication of variables for Client/Server Networking
void ARevenant::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ARevenant, AgilityEffect);
	DOREPLIFETIME(ARevenant, ImpairActive);
}

// Setup Inputs
void ARevenant::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// Called when the game starts or when spawned
void ARevenant::BeginPlay() {
	Super::BeginPlay();
	LoadBasicAttackAnimations();
	SetConvertedAbilities();
	CounteringBlowHurtbox->OnComponentBeginOverlap.AddDynamic(this, &ARevenant::CounteringBlowHurtboxOverlap);
	KickHurtbox->OnComponentBeginOverlap.AddDynamic(this, &ARevenant::KickHurtboxOverlap);
	PoweredBashHurtbox->OnComponentBeginOverlap.AddDynamic(this, &ARevenant::PoweredBashHurtboxOverlap);
	WeaponHurtboxBase->OnComponentBeginOverlap.AddDynamic(this, &ARevenant::StabHurtboxOverlap);
}

/* ---------------------------------- FUNCTIONS ----------------------------------- */

/* ************************ Animation Functions ************************* */

// Load Basic Attack Animations into their arrays (looks ugly but much more convenient than other means)
void ARevenant::LoadBasicAttackAnimations() {
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
	//FName HTempAttackName6 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage6.HBasicAttack_Combo_Montage6");
	FName HTempAttackName7 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage7.HBasicAttack_Combo_Montage7");
	FName HTempAttackName8 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage8.HBasicAttack_Combo_Montage8");
	FName HTempAttackName9 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage9.HBasicAttack_Combo_Montage9");
	FName HTempAttackName10 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage10.HBasicAttack_Combo_Montage10");
	FName HTempAttackName11 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage11.HBasicAttack_Combo_Montage11");
	FName HTempAttackName12 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage12.HBasicAttack_Combo_Montage12");
	//FName HTempAttackName13 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage13.HBasicAttack_Combo_Montage13");
	FName HTempAttackName14 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage14.HBasicAttack_Combo_Montage14");
	FName HTempAttackName15 = TEXT("/Game/Classes/Revenant/Animations/Attacks/HardAttacks/HBasicAttack_Combo_Montage15.HBasicAttack_Combo_Montage15");
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName2.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName3.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName4.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName5.ToString())));
	//HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName6.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName7.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName8.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName9.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName10.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName11.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName12.ToString())));
	//HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName13.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName14.ToString())));
	HBasicAttackAnims.Add(Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *HTempAttackName15.ToString())));

}

// Gets random montage from animation array
UAnimMontage* ARevenant::GetRandomMontage(TArray<UAnimMontage *> MontageArray) {
	int temp = UKismetMathLibrary::RandomIntegerInRange(0, MontageArray.Num() - 1);
	//if(GEngine)
    //  GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("World delta for current frame equals %i"), temp));
	return MontageArray[temp];
}

/* **************************** UI Functions **************************** */

// Populate array that converts int values of attacks to FString
void ARevenant::SetConvertedAbilities() {
	AbilityConversionArray.Add("StaggeringKick");
	AbilityConversionArray.Add("Impede");
	AbilityConversionArray.Add("LifeDrain");
	AbilityConversionArray.Add("DebilitatingKick");
	AbilityConversionArray.Add("EmpoweringStrike");
	AbilityConversionArray.Add("EnergyDrain");
	AbilityConversionArray.Add("ShadowStance");
	AbilityConversionArray.Add("Agility");
	AbilityConversionArray.Add("Unwaver");
	AbilityConversionArray.Add("Impair");
	AbilityConversionArray.Add("Fortify");
	AbilityConversionArray.Add("PoweredBash");
	AbilityConversionArray.Add("ChannelingStrike");
	AbilityConversionArray.Add("PoisonBlade");
	AbilityConversionArray.Add("LastResort");
}

// Reset ability selected arrays each Character Select UI Launch
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

// Perfom final actions once "Submit" is pressed on the "Choose Attacks" UI
void ARevenant::AddRemainingInputs() {
	FInputActionHandlerSignature ActionBindHandler;

	InputComponent->ClearActionBindings();

	// START OF DEFAULTS

	// Add Block Press
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), FString(TEXT("BlockPress")));
	BlockPressBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(BlockPressBind);

	// Add Block Release
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), FString(TEXT("BlockRelease")));
	BlockReleaseBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(BlockReleaseBind);

	// Add Roll
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), FString(TEXT("Roll")));
	RollBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(RollBind);

	// Add SideStep
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), FString(TEXT("SideStep")));
	SideStepBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(SideStepBind);

	// Add BasicAttack
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), FString(TEXT("SBasicAttack")));
	BasicAttackBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(BasicAttackBind);

	// Add HardAttack
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), FString(TEXT("HBasicAttack")));
	HardAttackBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(HardAttackBind);

	// Add CounteringBlow
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), FString(TEXT("CounteringBlow")));
	CounteringBlowBind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(CounteringBlowBind);

	// END OF DEFAULTS

	// Add ComboExtender1
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), ComboExtenderArray[0]);
	ComboExtender1Bind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ComboExtender1Bind);

	// Add ComboExtender2
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), ComboExtenderArray[1]);
	ComboExtender2Bind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ComboExtender2Bind);

	// Add ComboExtender3
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), ComboExtenderArray[2]);
	ComboExtender3Bind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ComboExtender3Bind);

	// Add Utility 1
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), UtilityArray[0]);
	Utility1Bind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(Utility1Bind);

	// Add Utility 2
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), UtilityArray[1]);
	Utility2Bind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(Utility2Bind);

	// Add ComboFinisher 1
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), ComboFinisherArray[0]);
	ComboFinisher1Bind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ComboFinisher1Bind);

	// Add ComboFinisher 2
	ActionBindHandler.BindUFunction(this, FName("AttackExecute"), ComboFinisherArray[1]);
	ComboFinisher2Bind.ActionDelegate = ActionBindHandler;
	InputComponent->AddActionBinding(ComboFinisher2Bind);

	if (this->HasAuthority()) {
		AddRemainingInputsClient();
	}
	else {
		AddRemainingInputsServer();
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
	DamageTable.Add(FDamageTableStruct("StaggeringKick", 4.0f));
	DamageTable.Add(FDamageTableStruct("Impede", 4.0f));
	DamageTable.Add(FDamageTableStruct("LifeDrain", 4.0f));
	DamageTable.Add(FDamageTableStruct("DebilitatingKick", 2.0f));
	DamageTable.Add(FDamageTableStruct("EmpoweringStrike", 3.0f));
	DamageTable.Add(FDamageTableStruct("EnergyDrain", 6.0f));
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
}

// Add Attack to character when attack is chosen in UI
void ARevenant::AddAttack(int Type, int AttackName, bool Toggled) {
	if (this->HasAuthority()) {
		AddAttackClient(Type, AttackName, Toggled);
	}
	else {
		AddAttackServer(Type, AttackName, Toggled);
		AddAttackClient(Type, AttackName, Toggled);
	}
}
void ARevenant::AddAttackServer_Implementation(int Type, int AttackName, bool Toggled) {
	AddAttackClient(Type, AttackName, Toggled);
}
bool ARevenant::AddAttackServer_Validate(int Type, int AttackName, bool Toggled) {
	return true;
}
void ARevenant::AddAttackClient(int Type, int AttackName, bool Toggled) {
	if (Type == 0) { // ComboExtender
		if (Toggled == true) {
			ComboExtenderArray.Add(AbilityConversionArray[AttackName]);
		}
		else {
			int i;
			for (i = 0; i < ComboExtenderArray.Num(); i++) {
				if (ComboExtenderArray[i] == AbilityConversionArray[AttackName]) {
					break;
				}
			}
			if (i < ComboExtenderArray.Num()) {
				ComboExtenderArray.RemoveAt(i, 1, true);
			}
		}
	}
	else if (Type == 1) { // Utility
		if (Toggled == true) {
			UtilityArray.Add(AbilityConversionArray[AttackName]);
		}
		else {
			int i;
			for (i = 0; i < UtilityArray.Num(); i++) {
				if (UtilityArray[i] == AbilityConversionArray[AttackName]) {
					break;
				}
			}
			if (i < UtilityArray.Num()) {
				UtilityArray.RemoveAt(i, 1, true);
			}
		}
	}
	else if (Type == 2) { // Combo Finisher
		if (Toggled == true) {
			ComboFinisherArray.Add(AbilityConversionArray[AttackName]);
		}
		else {
			int i;
			for (i = 0; i < ComboFinisherArray.Num(); i++) {
				if (ComboFinisherArray[i] == AbilityConversionArray[AttackName]) {
					break;
				}
			}
			if (i < ComboFinisherArray.Num()) {
				ComboFinisherArray.RemoveAt(i, 1, true);
			}
		}
	}
}

// Set Cooldown amounts upon Submit press, THIS IS WHERE COOLDOWNS ARE CHANGED
float ARevenant::SetAttackCooldownAmt(FString AttackName) {
	if (AttackName == "CounteringBlow") {
		return 6.0f;
	}
	else if (AttackName == "StaggeringKick") {
		return 10.0f;
	}
	else if (AttackName == "Impede") {
		return 7.0f;
	}
	else if (AttackName == "LifeDrain") {
		return 15.0f;
	}
	else if (AttackName == "DebilitatingKick") {
		return 16.0f;
	}
	else if (AttackName == "EmpoweringStrike") {
		return 15.0f;
	}
	else if (AttackName == "EnergyDrain") {
		return 12.0f;
	}
	else if (AttackName == "ShadowStance") {
		return 15.0f;
	}
	else if (AttackName == "Agility") {
		return 15.0f;
	}
	else if (AttackName == "Unwaver") {
		return 15.0f;
	}
	else if (AttackName == "Impair") {
		return 9.0f;
	}
	else if (AttackName == "Fortify") {
		return 12.0f;
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

/* *********************** Damage/Death Functions *********************** */

// Apply override so player who received damage can send events to player who dealt damage
void ARevenant::SendEventToAttacker(ABaseCharacter* Attacker) {
	if (ImpairActive == true) {
		Attacker->AddCooldownModifier(1.0f, Attacker->CurrentAttackName, 1.0f);
	}
}

/* ********************** Attack Effect Functions ********************** */

// Function for detecting abilities
void ARevenant::TurnInvisibleRepAll_Implementation() {
	GetMesh()->SetOnlyOwnerSee(true);
	Shield->SetOnlyOwnerSee(true);
	WeaponVisibility(true);
	UsernameOverhead->SetVisibility(false);
	HPOverhead->SetVisibility(false);
	GetWorldTimerManager().SetTimer(InvisibilityDelayTimerHandle, this, &ARevenant::DetectAction, 2.0f, false);
}
void ARevenant::DetectAction() {
	if (DetectMode == true) {
		if (this->HasAuthority()) {
			DetectRepAll();
		}
	}
}
void ARevenant::DetectRepAll_Implementation() {
	GetMesh()->SetOnlyOwnerSee(false);
	Shield->SetOnlyOwnerSee(false);
	WeaponVisibility(false);
	UsernameOverhead->SetVisibility(true);
	HPOverhead->SetVisibility(true);
	DetectMode = false;
	AgilityEffect = false;
}

// Unset SuperArmor for Unwaver
void ARevenant::UnsetAntiFlinch() {
	SuperArmor = false;
	GetMesh()->SetCustomDepthStencilValue(1);
}

// Unset effects
void ARevenant::RemoveImpairEffect() {
	if (ImpairActive == true) {
		ImpairActive = false;
	}
	GetMesh()->SetCustomDepthStencilValue(1);
}

// Unset effects
void ARevenant::RemoveEffect() {
	GetMesh()->SetCustomDepthStencilValue(1);
}

/* ********************** Attack Handler Functions ********************** */

// Events executed for Hitbox Attacks
void ARevenant::CounteringBlowHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) { 
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		CounteringBlowHurtbox->SetGenerateOverlapEvents(false);
		ABaseCharacter* AttackedTarget = Cast<ABaseCharacter>(OtherActor);
		InflictDamage(AttackedTarget, CalcFinalDamage(GetDamage(CurrentAttackName), AttackedTarget), true, true, false);
	}
}
void ARevenant::KickHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		KickHurtbox->SetGenerateOverlapEvents(false);
		ABaseCharacter* AttackedTarget = Cast<ABaseCharacter>(OtherActor);

		if (CurrentAttackName == "StaggeringKick") {
			InflictDamage(AttackedTarget, CalcFinalDamage(GetDamage(CurrentAttackName), AttackedTarget), false, true, false);
		}
		else if (CurrentAttackName == "DebilitatingKick") {
			LastAttack = "ComboExtender";
			if (InflictDamage(AttackedTarget, CalcFinalDamage(GetDamage(CurrentAttackName), AttackedTarget), true, true, false) == true && ComboAmount > 1) {
				AttackedTarget->SetCooldown("Roll", 1.5f);
			}
		}
	}
}
void ARevenant::PoweredBashHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		PoweredBashHurtbox->SetGenerateOverlapEvents(false);
		ABaseCharacter* AttackedTarget = Cast<ABaseCharacter>(OtherActor);
		InflictDamage(AttackedTarget, CalcFinalDamage(5 + (ComboAmount * 3.0f), AttackedTarget), true, true, false);
	}
}
void ARevenant::StabHurtboxOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		WeaponHurtboxBase->SetGenerateOverlapEvents(false);
		ABaseCharacter* AttackedTarget = Cast<ABaseCharacter>(OtherActor);

		if (CurrentAttackName == "LifeDrain") {
			InflictDamage(AttackedTarget, CalcFinalDamage(GetDamage(CurrentAttackName), AttackedTarget), true, true, false);
			Health = UKismetMathLibrary::FClamp(Health + (CurrentDamage * 2), 0.0f, 100.0f);
		}
		else if (CurrentAttackName == "EnergyDrain") {
			InflictDamage(AttackedTarget, CalcFinalDamage(GetDamage(CurrentAttackName), AttackedTarget), true, true, false);
			Resilience = UKismetMathLibrary::FClamp(Resilience + 30.0f, 0.0f, 100.0f);
			AttackedTarget->Resilience = UKismetMathLibrary::FClamp(AttackedTarget->Resilience - 30.0f, 0.0f, 100.0f);
		}
		else if (CurrentAttackName == "PoisonBlade") {
			InflictDamage(AttackedTarget, CalcFinalDamage(GetDamage(CurrentAttackName), AttackedTarget), true, true, false);
			AttackedTarget->ApplyDamageOverTime("FlinchOnLast", 1.5f, 5, 2.0f, 1.0f, this);
		}
	}
}

// Function for applying attack effects to enemy
void ARevenant::AttackEffect(ABaseCharacter* Target, FString AttackName) {
	if (AttackName == "EmpoweringStrike") {
		AddDamageModifier(0.3f, 2.0f, 1);
	}
	else if (AttackName == "ChannelingStrike") {
		float PushValue = 500 + (20 * ComboAmount);
		FVector TempVector = (this->GetActorForwardVector() * PushValue) + (this->GetActorUpVector() * PushValue);
		Target->LaunchCharacter(TempVector, true, true);
	}
}

// Master function for converting inputs into their corresponding ability events
void ARevenant::AttackExecute(FString AttackName) {
	if (this->HasAuthority()) {
		AttackExecuteServer(AttackName);
	}
	else {
		AttackExecuteClientToServer(AttackName);
	}
}
void ARevenant::AttackExecuteClientToServer_Implementation(const FString &AttackName) {
	AttackExecuteServer(AttackName);
}
bool ARevenant::AttackExecuteClientToServer_Validate(const FString &AttackName) {
	return true;
}
void ARevenant::AttackExecuteServer(FString AttackName) {
	if (IsDead == false && MenuUp == false) {
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
			if (Flinched == false && CanAttack == true && IsRolling == false && IsSideStepping == false && GetCooldown(AttackName) <= CurrentGameTime && this->GetMovementComponent()->IsMovingOnGround() == true) {
				DetectMode = true;
				if (this->HasAuthority()) {
					FActorSpawnParameters SpawnParameters;
					SpawnParameters.Instigator = this;
					SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					FRotator TempRot = GetMesh()->K2_GetComponentRotation();
					TempRot.Yaw += 90;
					ABaseCharacter * SpawnedShadow = (GetWorld()->SpawnActor<ABaseCharacter>(Shadow, RootComponent->K2_GetComponentLocation(), TempRot, SpawnParameters));
					SpawnedShadow->Username = Username;
					SpawnedShadow->Health = Health;
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
			if (Flinched == false && CanAttack == true && GetCooldown(AttackName) <= CurrentGameTime) {
				DetectMode = true;
				AgilityEffect = true;
				AddSpeedModifier(2.0f, 2.0f);
				TurnInvisibleRepAll();
				SetCooldown(AttackName, GetFinalCooldownAmt(AttackName, GetCooldownAmt(AttackName)));
			}
		}
		else if (AttackName == "Unwaver") {
			if (Flinched == false && GetCooldown(AttackName) <= CurrentGameTime) {
				SuperArmor = true;
				if (DetectMode == false) {
					InitializeParticle(UnwaverFX, GetMesh()->K2_GetComponentLocation(), true);
				}
				GetMesh()->SetCustomDepthStencilValue(2);
				SetCooldown(AttackName, GetFinalCooldownAmt(AttackName, GetCooldownAmt(AttackName)));
				GetWorldTimerManager().SetTimer(UnwaverDelayTimerHandle, this, &ARevenant::UnsetAntiFlinch, 1.5f, false);
			}
		}
		else if (AttackName == "Impair") {
			if (GetCooldown(AttackName) <= CurrentGameTime) {
				if (DetectMode == false) {
					InitializeParticle(ImpairFX, GetMesh()->K2_GetComponentLocation(), true);
				}
				SetCooldown(AttackName, GetFinalCooldownAmt(AttackName, GetCooldownAmt(AttackName)));
				ImpairActive = true;
				GetMesh()->SetCustomDepthStencilValue(3);
				GetWorldTimerManager().SetTimer(ImpairDelayTimerHandle, this, &ARevenant::RemoveImpairEffect, 2.0f, false);
			}
		}
		else if (AttackName == "Fortify") {
			if (GetCooldown(AttackName) <= CurrentGameTime) {
				if (DetectMode == false) {
					InitializeParticle(FortifyFX, GetMesh()->K2_GetComponentLocation(), true);
				}
				AddDefenseModifier(3.0f, 1.5f, 99);
				GetMesh()->SetCustomDepthStencilValue(4);
				SetCooldown(AttackName, GetFinalCooldownAmt(AttackName, GetCooldownAmt(AttackName)));
				GetWorldTimerManager().SetTimer(FortifyDelayTimerHandle, this, &ARevenant::RemoveEffect, 1.5f, false);
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
							false, // Use Projectile?
							1); // Aura effect number arond player
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
							false, // Use Projectile?
							1); // Aura effect number arond player
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
							false, // Use Projectile?
							1); // Aura effect number arond player
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
							false, // Use Projectile?
							1); // Aura effect number arond player
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
						false, // Use Projectile?
						1); // Aura effect number arond player
				}
				// Combo Extenders
				else if (AttackName == "StaggeringKick") {
					FName KickAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Kick_Montage.Kick_Montage");
					UAnimMontage *KickAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *KickAnimPath.ToString()));
					AttackHandler(
						AttackName, // Name
						"HBasicAttack", // Type
						0.6f, // Re-Casting delay
						1.2f, // Speed of Animation
						CheckChainable("ComboExtender"), // Based on previous Attack, is it Chainable
						KickAnimMontage, // Animation to use
						0.2f, // Delay before Hitbox starts
						0.2f, // Time duration of Hitbox
						0.0f, // Amount of damage
						true, // Whether or not to use hitbox instead
						KickHurtbox, // Which hitbox to initiate
						false, // Use Projectile?
						1); // Aura effect number arond player
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
						true, // Use Projectile?
						1); // Aura effect number arond player
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
						true, // Whether or not to use hitbox instead
						WeaponHurtboxBase, // Which hitbox to initiate
						false, // Use Projectile?
						1); // Aura effect number arond player
				}
				else if (AttackName == "DebilitatingKick") {
					FName KickAnimPath = TEXT("/Game/Classes/Revenant/Animations/Attacks/Kick_Montage.Kick_Montage");
					UAnimMontage *KickAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *KickAnimPath.ToString()));
					AttackHandler(
						AttackName, // Name
						"ComboExtender", // Type
						0.6f, // Re-Casting delay
						1.2f, // Speed of Animation
						CheckChainable("ComboExtender"), // Based on previous Attack, is it Chainable
						KickAnimMontage, // Animation to use
						0.2f, // Delay before Hitbox starts
						0.2f, // Time duration of Hitbox
						0.0f, // Amount of damage
						true, // Whether or not to use hitbox instead
						KickHurtbox, // Which hitbox to initiate
						false, // Use Projectile?
						1); // Aura effect number arond player
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
						false, // Use Projectile?
						1); // Aura effect number arond player
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
						true, // Whether or not to use hitbox instead
						WeaponHurtboxBase, // Which hitbox to initiate
						false, // Use Projectile?
						1); // Aura effect number arond player
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
							false, // Use Projectile?
							5); // Aura effect number arond player
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
							PoweredBashHurtbox, // Which hitbox to initiate
							false, // Use Projectile?
							5); // Aura effect number arond player
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
							0.65f, // Re-Casting delay
							1.0f, // Speed of Animation
							false, // Based on previous Attack, is it Chainable
							ChannelingStrikeAnimMontage, // Animation to use
							0.3f, // Delay before Hitbox starts
							0.4f, // Time duration of Hitbox
							GetDamage(AttackName), // Amount of damage
							false, // Whether or not to use hitbox instead
							NULL, // Which hitbox to initiate
							false, // Use Projectile?
							6); // Aura effect number arond player
					}
					else { // If combo
						AttackHandler(
							AttackName, // Name
							"ComboFinisher", // Type
							0.8f, // Re-Casting delay
							1.0f, // Speed of Animation
							true, // Based on previous Attack, is it Chainable
							ChannelingStrikeComboAnimMontage, // Animation to use
							0.18f, // Delay before Hitbox starts
							0.25f, // Time duration of Hitbox
							GetDamage(AttackName), // Amount of damage
							false, // Whether or not to use hitbox instead
							NULL, // Which hitbox to initiate
							false, // Use Projectile?
							6); // Aura effect number arond player
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
							0.65f, // Re-Casting delay
							1.0f, // Speed of Animation
							false, // Based on previous Attack, is it Chainable
							ChannelingStrikeAnimMontage, // Animation to use
							0.3f, // Delay before Hitbox starts
							0.35f, // Time duration of Hitbox
							GetDamage(AttackName), // Amount of damage
							true, // Whether or not to use hitbox instead
							WeaponHurtboxBase, // Which hitbox to initiate
							false, // Use Projectile?
							8); // Aura effect number arond player
					}
					else { // If combo
						AttackHandler(
							AttackName, // Name
							"ComboFinisher", // Type
							0.8f, // Re-Casting delay
							1.0f, // Speed of Animation
							true, // Based on previous Attack, is it Chainable
							PoisonBladeComboAnimMontage, // Animation to use
							0.12f, // Delay before Hitbox starts
							0.3f, // Time duration of Hitbox
							GetDamage(AttackName), // Amount of damage
							true, // Whether or not to use hitbox instead
							WeaponHurtboxBase, // Which hitbox to initiate
							false, // Use Projectile?
							8); // Aura effect number arond player
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
							0.65f, // Re-Casting delay
							1.0f, // Speed of Animation
							false, // Based on previous Attack, is it Chainable
							LastResortAnimMontage, // Animation to use
							0.5f, // Delay before Hitbox starts
							0.3f, // Time duration of Hitbox
							3 + UKismetMathLibrary::MultiplyMultiply_FloatFloat(3.2f, (3.0f - (Resilience / 50.0f))), // Amount of damage
							false, // Whether or not to use hitbox instead
							NULL, // Which hitbox to initiate
							false, // Use Projectile?
							7); // Aura effect number arond player
					}
					else { // If combo
						AttackHandler(
							AttackName, // Name
							"ComboFinisher", // Type
							0.8f, // Re-Casting delay
							1.0f, // Speed of Animation
							true, // Based on previous Attack, is it Chainable
							LastResortComboAnimMontage, // Animation to use
							0.25f, // Delay before Hitbox starts
							0.2f, // Time duration of Hitbox
							3 + UKismetMathLibrary::MultiplyMultiply_FloatFloat(3.2f, (3.0f - (Resilience / 50.0f))), // Amount of damage
							false, // Whether or not to use hitbox instead
							NULL, // Which hitbox to initiate
							false, // Use Projectile?
							7); // Aura effect number arond player
					}
				}
			}
			if (DetectMode == true) {
				DetectAction();
			}
		}
	}
}

// Attack Effect Handler Start
void ARevenant::AttackEffectHandlerStart(int StencilValue){
	AttackEffectTrigger = true;
	if (CurrentAttackType == "ComboExtender") {
		//Weapon->SetMaterial(0, ShadowMaterialWeaponInstComboExtender);
		//Shield->SetMaterial(0, ShadowMaterialShieldInstComboExtender);
		//GetMesh()->SetMaterial(0, ShadowMaterialBodyInstComboExtender);
	}
	else if (CurrentAttackType == "ComboFinisher") {
		//Weapon->SetMaterial(0, ShadowMaterialWeaponInstComboFinisher);
		//Shield->SetMaterial(0, ShadowMaterialShieldInstComboFinisher);
		//GetMesh()->SetMaterial(0, ShadowMaterialBodyInstComboFinisher);
	}
	GetMesh()->SetCustomDepthStencilValue(StencilValue);
}

// Attack Effect Handler End
void ARevenant::AttackEffectHandlerEnd(int StencilValue){
	//Weapon->SetMaterial(0, WeaponMeshMaterialInst);
	//Shield->SetMaterial(0, ShieldMeshMaterialInst);
	//GetMesh()->SetMaterial(0, BodyMeshMaterialInst);
	GetMesh()->SetCustomDepthStencilValue(1);
}