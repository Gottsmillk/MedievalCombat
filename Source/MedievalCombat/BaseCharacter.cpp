
#include "MedievalCombat.h"
#include "BaseCharacter.h"
#include "DamageNumberDisplay.h"
#include "ProjectileBase.h"
#include "UnrealNetwork.h"
#include "UserWidget.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "UnrealMathUtility.h"


#define COLLISION_ATTACK ECC_GameTraceChannel4
#define COLLISION_DIRECTION ECC_GameTraceChannel5
#define COLLISION_PROJECTILE ECC_GameTraceChannel7

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->SetVisibility(false);
	GetCapsuleComponent()->InitCapsuleSize(35.0f, 70.0f);

	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Character moves in the direction of input...	
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;
	// The camera follows at this distance behind the character	
	CameraBoom->TargetArmLength = 300.0f;
	// Rotate the arm based on the controller
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Camera does not rotate relative to arm
	FollowCamera->bUsePawnControlRotation = false;

	// Create a capsule component for testing if player is correctly aimed upon
	PlayerAimCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PlayerAimCollision"));
	PlayerAimCollision->SetVisibility(false);
	PlayerAimCollision->InitCapsuleSize(70.0f, 120.0f);
	PlayerAimCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PlayerAimCollision->bDynamicObstacle = true;
	PlayerAimCollision->bGenerateOverlapEvents = false;
	PlayerAimCollision->SetupAttachment(RootComponent);

	// Create a capsule component to avoid people going through eachother
	PlayerCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PlayerCollision"));
	PlayerCollision->SetVisibility(false);
	PlayerCollision->InitCapsuleSize(70.0f, 120.0f);
	PlayerCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PlayerCollision->bDynamicObstacle = true;
	PlayerCollision->bGenerateOverlapEvents = false;
	PlayerCollision->SetupAttachment(RootComponent);

	// Create a capsule component to avoid people going through eachother
	PlayerCollision2 = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PlayerCollision2"));
	PlayerCollision2->SetVisibility(false);
	PlayerCollision2->InitCapsuleSize(90.0f, 120.0f);
	PlayerCollision2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PlayerCollision2->bDynamicObstacle = true;
	PlayerCollision2->bGenerateOverlapEvents = true;
	PlayerCollision2->SetupAttachment(RootComponent);
	PlayerCollision2->OnComponentBeginOverlap.AddDynamic(this, &ABaseCharacter::PlayerCollision2Begin);
	PlayerCollision2->OnComponentEndOverlap.AddDynamic(this, &ABaseCharacter::PlayerCollision2End);

	// Create weapon
	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	Weapon->SetVisibility(false);
	Weapon->SetHiddenInGame(true);
	Weapon->bGenerateOverlapEvents = false;

	/***** Create weapon hurtboxes *****/
	// Weapon Hurtbox Base
	WeaponHurtboxBase = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponHurtboxBase"));
	WeaponHurtboxBase->SetVisibility(false);
	WeaponHurtboxBase->SetHiddenInGame(true);
	WeaponHurtboxBase->SetBoxExtent(FVector(0.6f, 0.22f, 32.0f));
	WeaponHurtboxBase->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponHurtboxBase->bDynamicObstacle = false;
	WeaponHurtboxBase->bGenerateOverlapEvents = true;
	WeaponHurtboxBase->SetupAttachment(Weapon);

	// Hurtbox1
	Hurtbox1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Hurtbox1"));
	Hurtbox1->SetVisibility(false);
	Hurtbox1->SetHiddenInGame(true);
	Hurtbox1->SetBoxExtent(FVector(0.5f, 0.5f, 0.5f));
	Hurtbox1->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Hurtbox1->bDynamicObstacle = false;
	Hurtbox1->bGenerateOverlapEvents = false;
	Hurtbox1->SetupAttachment(WeaponHurtboxBase);

	// Hurtbox2
	Hurtbox2 = CreateDefaultSubobject<UBoxComponent>(TEXT("Hurtbox2"));
	Hurtbox2->SetVisibility(false);
	Hurtbox2->SetHiddenInGame(true);
	Hurtbox2->SetBoxExtent(FVector(0.5f, 0.5f, 0.5f));
	Hurtbox2->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Hurtbox2->bDynamicObstacle = false;
	Hurtbox2->bGenerateOverlapEvents = false;
	Hurtbox2->SetupAttachment(WeaponHurtboxBase);

	// Hurtbox3
	Hurtbox3 = CreateDefaultSubobject<UBoxComponent>(TEXT("Hurtbox3"));
	Hurtbox3->SetVisibility(false);
	Hurtbox3->SetHiddenInGame(true);
	Hurtbox3->SetBoxExtent(FVector(0.5f, 0.5f, 0.5f));
	Hurtbox3->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Hurtbox3->bDynamicObstacle = false;
	Hurtbox3->bGenerateOverlapEvents = false;
	Hurtbox3->SetupAttachment(WeaponHurtboxBase);

	// Hurtbox4
	Hurtbox4 = CreateDefaultSubobject<UBoxComponent>(TEXT("Hurtbox4"));
	Hurtbox4->SetVisibility(false);
	Hurtbox4->SetHiddenInGame(true);
	Hurtbox4->SetBoxExtent(FVector(0.5f, 0.5f, 0.5f));
	Hurtbox4->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Hurtbox4->bDynamicObstacle = false;
	Hurtbox4->bGenerateOverlapEvents = false;
	Hurtbox4->SetupAttachment(WeaponHurtboxBase);

	// Hurtbox5
	Hurtbox5 = CreateDefaultSubobject<UBoxComponent>(TEXT("Hurtbox5"));
	Hurtbox5->SetVisibility(false);
	Hurtbox5->SetHiddenInGame(true);
	Hurtbox5->SetBoxExtent(FVector(0.5f, 0.5f, 0.5f));
	Hurtbox5->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Hurtbox5->bDynamicObstacle = false;
	Hurtbox5->bGenerateOverlapEvents = false;
	Hurtbox5->SetupAttachment(WeaponHurtboxBase);

	// Store the hurtboxes inside the hitbox component array
	HitboxComponentArray[0] = WeaponHurtboxBase;
	HitboxComponentArray[1] = Hurtbox1;
	HitboxComponentArray[2] = Hurtbox2;
	HitboxComponentArray[3] = Hurtbox3;
	HitboxComponentArray[4] = Hurtbox4;
	HitboxComponentArray[5] = Hurtbox5;

	// Set Projectile to be spawned
	static ConstructorHelpers::FObjectFinder<UClass> ProjectileBPtoClass(TEXT("Class'/Game/Classes/Revenant/Blueprints/ProjectileBase.ProjectileBase_C'"));
	if (ProjectileBPtoClass.Object) {
		BaseProjectile = ProjectileBPtoClass.Object;
	}
}

// Allows Replication of variables for Client/Server Networking
void ABaseCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ABaseCharacter, PlayerTakeDamageCameraShake);

	DOREPLIFETIME(ABaseCharacter, CurrentDamageIndicator);
	DOREPLIFETIME(ABaseCharacter, MaxDamageIndicator);
	DOREPLIFETIME(ABaseCharacter, MinDamageIndicator);
	DOREPLIFETIME(ABaseCharacter, DesiredDamageIndicator);
	DOREPLIFETIME(ABaseCharacter, DamageIndicatorSpeed);
	DOREPLIFETIME(ABaseCharacter, LowHealthIndicatorPower);
	DOREPLIFETIME(ABaseCharacter, DamageDeltaTime);

	DOREPLIFETIME(ABaseCharacter, CurrentGameTime);

	DOREPLIFETIME(ABaseCharacter, IsDead);
	DOREPLIFETIME(ABaseCharacter, Health);
	DOREPLIFETIME(ABaseCharacter, CanAttack);
	DOREPLIFETIME(ABaseCharacter, CurrentAttackHit);
	DOREPLIFETIME(ABaseCharacter, LastAttack);
	DOREPLIFETIME(ABaseCharacter, CurrentAttackName);
	DOREPLIFETIME(ABaseCharacter, CurrentAttackType);
	DOREPLIFETIME(ABaseCharacter, DetectMode);
	DOREPLIFETIME(ABaseCharacter, ComboAmount);
	DOREPLIFETIME(ABaseCharacter, ResilienceAttackReplenish);
	DOREPLIFETIME(ABaseCharacter, ResilienceDefenseReplenish);

	DOREPLIFETIME(ABaseCharacter, BlockPressed);
	DOREPLIFETIME(ABaseCharacter, IsBlocking);
	DOREPLIFETIME(ABaseCharacter, BlockingAnim);
	DOREPLIFETIME(ABaseCharacter, BlockSubtractResilience);

	DOREPLIFETIME(ABaseCharacter, FlinchTrigger);
	DOREPLIFETIME(ABaseCharacter, Flinched);
	DOREPLIFETIME(ABaseCharacter, SuperArmor);
	DOREPLIFETIME(ABaseCharacter, HBasicAttackSuperArmor);

	DOREPLIFETIME(ABaseCharacter, IsRolling);
	DOREPLIFETIME(ABaseCharacter, RollMovement);
	DOREPLIFETIME(ABaseCharacter, IsSideStepping);

	DOREPLIFETIME(ABaseCharacter, CanMove);
	DOREPLIFETIME(ABaseCharacter, CanTurn);
	DOREPLIFETIME(ABaseCharacter, SpeedEffectsArray);

	DOREPLIFETIME(ABaseCharacter, Colliding);
	DOREPLIFETIME(ABaseCharacter, Overlapping);

	DOREPLIFETIME(ABaseCharacter, Resilience);
	DOREPLIFETIME(ABaseCharacter, ResilienceDrainAmt);
	DOREPLIFETIME(ABaseCharacter, ResilienceRegenAmt);

	DOREPLIFETIME(ABaseCharacter, AttackCastCooldown);
	DOREPLIFETIME(ABaseCharacter, DamageOverTimeArray);
	DOREPLIFETIME(ABaseCharacter, AttackArray);
	DOREPLIFETIME(ABaseCharacter, ComboExtenderArray);
	DOREPLIFETIME(ABaseCharacter, UtilityArray);
	DOREPLIFETIME(ABaseCharacter, ComboFinisherArray);
	DOREPLIFETIME(ABaseCharacter, DamageTable);

	DOREPLIFETIME(ABaseCharacter, MenuUp);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	//Start Regen/Drain handlers
	if (this->HasAuthority()) {
		GetWorldTimerManager().SetTimer(ResilienceRegenTimerHandle, this, &ABaseCharacter::ResilienceRegenTimer, 1.0f, true);
	}
}

// Tick
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DamageDeltaTime = DeltaTime;
	MovementHandler(); // Changes movement based on if player is rolling or not
	HitboxHandler(); // Activates hitbox when attacking
	BlockHandler(); // Handles whether player is blocking
	BlockAnimation(); // Changes animation based on if player is blocking
	DamageIndicatorTick(); // Handles displaying the damage indicator
	FlinchEventTrigger(); // Applies flinching when player is attacked
	TrackDamageOverTime(); // Tracks Damage over Time
	CurrentGameTime = UKismetSystemLibrary::GetGameTimeInSeconds(this->GetWorld());
	if (IsDead == true && this->bUseControllerRotationYaw == 0) {
		this->bUseControllerRotationYaw = 1;
	}
	if (AttackCastCooldown <= CurrentGameTime && ComboAmount > 0) {
		ComboAmount = 0;
		Resilience = UKismetMathLibrary::FClamp(Resilience + ResilienceAttackReplenish, 0.0f, 100.0f);
		ResilienceAttackReplenish = 0.0f;
	}
	if (Flinched == false && ResilienceDefenseReplenish > 0.0f) {
		Resilience = UKismetMathLibrary::FClamp(Resilience + ResilienceDefenseReplenish, 0.0f, 100.0f);
		ResilienceDefenseReplenish = 0.0f;
	}
}

// Input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
void ABaseCharacter::AddRemainingInputs(){

}
// Delay function
void ABaseCharacter::onTimerEnd()
{
}
void ABaseCharacter::WeaponVisibility(bool set) {
	Weapon->SetOnlyOwnerSee(set);
}
/*********************** DAMAGE INDICATOR ***********************/
void ABaseCharacter::InitiateDamageEffect_Implementation() {
	DamageIndicatorSpeed = 15.0f;
	DesiredDamageIndicator = UKismetMathLibrary::FClamp(CurrentDamageIndicator + 1.0f, 0.0f, MaxDamageIndicator);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->PlayCameraShake(PlayerTakeDamageCameraShake, UKismetMathLibrary::RandomFloatInRange(0.01f, 0.5f));
}
void ABaseCharacter::DamageIndicatorTick() {
	if (this->IsLocallyControlled() == true) {
		float TempVar1 = UKismetMathLibrary::MapRangeUnclamped(Health, 0.0f, 50.0f, 1.0f, 0.0f);
		float TempVar2 = UKismetMathLibrary::FClamp(TempVar1, 0.0f, 1.0f);
		float TempVar3 = UKismetMathLibrary::FClamp(CurrentDamageIndicator + TempVar2, 0.0f, 1.0f);
		FollowCamera->SetPostProcessBlendWeight(UKismetMathLibrary::MapRangeUnclamped(TempVar3, 0.0f, 1.0f, 0.0f, 1.0f));
		float TempVar4 = UKismetMathLibrary::MapRangeUnclamped(Health, 0.0f, 0.1f, 3.0f, 0.0f);
		float TempVar5 = UKismetMathLibrary::FClamp(TempVar4, 0.0f, 3.0f);
		LowHealthIndicatorPower = FMath::FInterpTo(LowHealthIndicatorPower, TempVar5, DamageDeltaTime, 2.0f);
		const FName Power("Power");
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), TempParamCollection, Power, LowHealthIndicatorPower);
		float TempVar6 = FMath::FInterpTo(CurrentDamageIndicator, DesiredDamageIndicator, DamageDeltaTime, DamageIndicatorSpeed);
		CurrentDamageIndicator = TempVar6;
		const FName Blood2Power("Blood2Power");
		UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), TempParamCollection, Blood2Power, CurrentDamageIndicator);
		if (UKismetMathLibrary::NearlyEqual_FloatFloat(CurrentDamageIndicator, FMath::FInterpTo(CurrentDamageIndicator, DesiredDamageIndicator, DamageDeltaTime, DamageIndicatorSpeed), 0.01f) == true) {
			DesiredDamageIndicator = 0.0f;
			DamageIndicatorSpeed = 3.0f;
		}
	}
}
void ABaseCharacter::InitiateDamageNumberEffect_Implementation(float Damage, ABaseCharacter * Target) {
	if (DamageDisplayConst) // Check if the Asset is assigned in the blueprint.
	{
		// Create the widget and store it.
		DamageDisplay = CreateWidget<UDamageNumberDisplay>(UGameplayStatics::GetPlayerController(GetWorld(), 0), DamageDisplayConst);
		DamageDisplay->Damage = Damage * 10;
		DamageDisplay->Target = Target;

		// Now you can use the widget directly since you have a reference for it.
		// Extra check to  make sure the pointer holds the widget.
		if (DamageDisplay)
		{
			//Add it to the view port
			DamageDisplay->AddToViewport();
		}
	}
}

/*********************** HITBOX ***********************/
void ABaseCharacter::FillHitboxArray() {
	// Store the hurtboxes inside the hitbox component array
	HitboxArray[0] = WeaponHurtboxBase->GetSocketLocation("");
	HitboxArray[1] = Hurtbox1->GetSocketLocation("");
	HitboxArray[2] = Hurtbox2->GetSocketLocation("");
	HitboxArray[3] = Hurtbox3->GetSocketLocation("");
	HitboxArray[4] = Hurtbox4->GetSocketLocation("");
	HitboxArray[5] = Hurtbox5->GetSocketLocation("");
}
void ABaseCharacter::HitboxHandler() {
	if (this->HasAuthority()) { // Only done on server
		if (HurtboxActive == true) { // Only when swinging
			if (InitialHitbox == true) { // Fills the beginning trace
				InitialHitbox = false;
				FillHitboxArray();
			}
			const FName TraceTag("MyTraceTag"); // Trace for debug
			GetWorld()->DebugDrawTraceTag = TraceTag; // Trace for debug

			FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
			RV_TraceParams.TraceTag = TraceTag; // Trace for debug
			RV_TraceParams.bTraceComplex = true;
			RV_TraceParams.bTraceAsyncScene = true;
			RV_TraceParams.bReturnPhysicalMaterial = false;
			RV_TraceParams.bIgnoreBlocks = false;

			// Trace lines for each vector stored in the vector array
			for (int i = 0; i < 6; i++) {

				//Re-initialize hit info
				FHitResult Out_Hit(ForceInit);

				//call GetWorld() from within an actor extending class
				if (GetWorld()->LineTraceSingleByChannel(Out_Hit, HitboxComponentArray[i]->GetSocketLocation(""), HitboxArray[i], COLLISION_ATTACK, RV_TraceParams) == true) {
					WeaponHitEvent(Out_Hit);
					break;
				}
			}
			FillHitboxArray();
		}
		else {
			if (InitialHitbox == false) {
				InitialHitbox = true;
			}
		}
	}
}

/*********************** RESILIENCE ***********************/
void ABaseCharacter::ResilienceRegenTimer() {
	Resilience = UKismetMathLibrary::FClamp(Resilience + ResilienceRegenAmt, 0, 100);
}
void ABaseCharacter::ResilienceDrainTimer() {
	if (Resilience >= ResilienceDrainAmt) { // Drain resilience
		Resilience = UKismetMathLibrary::FClamp(Resilience - ResilienceDrainAmt, 0, 100);
	}
}

/*********************** ROLL ***********************/
void ABaseCharacter::SideStepPressedEvent() {
	if (MenuUp == false && Flinched == false && (CanAttack == true || CurrentAttackName == "SBasicAttack") && IsDead == false && IsRolling == false && IsSideStepping == false && GetCooldown("SideStep") <= CurrentGameTime && this->GetMovementComponent()->IsMovingOnGround() == true) {
		IsSideStepping = true;
		CanAttack = false;
		MakeCurrentActionLastAction("SideStep");
		IsBlocking = false;
		Invincible = true;
		HurtboxActive = false;
		SetCooldown("SideStep", GetCooldownAmt("SideStep"));
		GetWorldTimerManager().SetTimer(delayTimerHandle, this, &ABaseCharacter::SideStepPressedEvent2, 0.3f, false);
	}
}
void ABaseCharacter::SideStepPressedEvent2() {
	IsSideStepping = false;
	if (IsRolling == false && IsSideStepping == false) {
		Invincible = false;
	}
	if (CanAttack == false) {
		CanAttack = true;
	}
	AttackCastCooldown = 0.0f;
}

void ABaseCharacter::RollPressedEvent() {
	if (MenuUp == false && IsRolling == false && IsSideStepping == false && Resilience >= 30 && GetCooldown("Roll") <= CurrentGameTime && this->GetMovementComponent()->IsMovingOnGround() == true) {
		Resilience -= 30;
		IsRolling = true;
		RollMovement = true;
		MakeCurrentActionLastAction("Roll");
		FlinchTrigger = false;
		Flinched = false;
		IsBlocking = false;
		Invincible = true;
		HurtboxActive = false;
		GetWorldTimerManager().SetTimer(delayTimerHandle, this, &ABaseCharacter::RollPressedEvent2, 0.45f, false);
	}
}
void ABaseCharacter::RollPressedEvent2() {
	RollMovement = false;
	GetWorldTimerManager().SetTimer(delayTimerHandle, this, &ABaseCharacter::RollPressedEvent3, 0.15f, false);
}
void ABaseCharacter::RollPressedEvent3() {
	IsRolling = false;
	if (IsRolling == false && IsSideStepping == false) {
		Invincible = false;
	}
	if (CanAttack == false) {
		CanAttack = true;
	}
	AttackCastCooldown = 0.0f;
}

/*********************** Movement ***********************/
void ABaseCharacter::MovementHandler() {
	if (AttackCastCooldown > CurrentGameTime && IsRolling == false && IsSideStepping == false) {
		if (this->HasAuthority()) {
			CanMove = false;
		}
	}
	else {
		if ((IsRolling == true || IsSideStepping == true) && IsDead == false) {
			StopAnimations();
			if (CanMove == true) {
				CanMove = false;
			}
			if (CurrentLRLoc == 0.0f && CurrentFBLoc == 0.0f) {
				CurrentFBLoc = 1.0f;
			}
			if (RollMovement == true || IsSideStepping == true) {
				this->AddMovementInput(GetActorForwardVector(), CurrentFBLoc, false);
				this->AddMovementInput(GetActorRightVector(), CurrentLRLoc, false);
			}
		}
		else {
			if (Flinched == true) {
				CanMove = false;
			}
			else if (CanMove == false && IsDead == false && CanAttack == true) {
				CanMove = true;
			}
		}
	}
}
void ABaseCharacter::AddSpeedModifier(float Modifier, float Duration) {
	FSpeedModifierStruct tempMod;
	tempMod.Modifier = Modifier;
	tempMod.Cooldown = CurrentGameTime + Duration;
	SpeedEffectsArray.Add(tempMod);
}

/*********************** BLOCK ***********************/
void ABaseCharacter::BlockPressedEvent() {
	if (MenuUp == false) {
		BlockPressed = true;
		if (LastAttack != "Block") {
			MakeCurrentActionLastAction("Block");
		}
	}
}
void ABaseCharacter::BlockReleasedEvent() {
	IsBlocking = false;
	BlockPressed = false;
	SetCooldown("BlockPress", GetCooldownAmt("BlockPress"));
}
void ABaseCharacter::BlockHandler() {
	if (Flinched == true || AttackCastCooldown > CurrentGameTime || IsBlocking == true || IsRolling == true || IsSideStepping == true) {
		if (this->HasAuthority()) {
			if (ResilienceRegenTimerHandle.IsValid() == true) {
				GetWorld()->GetTimerManager().ClearTimer(ResilienceRegenTimerHandle);
			}
		}
	}
	else {
		if (this->HasAuthority()) {
			if (ResilienceRegenTimerHandle.IsValid() == false) {
				GetWorldTimerManager().SetTimer(ResilienceRegenTimerHandle, this, &ABaseCharacter::ResilienceRegenTimer, 1.0f, true, 1.0f);
			}
		}
	}
	if (BlockPressed == true) {
		if (Resilience < ResilienceDrainAmt || Flinched == true || MenuUp == true || CanAttack == false || IsRolling == true || IsSideStepping == true || this->GetMovementComponent()->IsMovingOnGround() == false) {
			IsBlocking = false;
			BlockSubtractResilience = false;
			if (this->HasAuthority()) {
				if (ResilienceDrainTimerHandle.IsValid() == true) {
					GetWorld()->GetTimerManager().ClearTimer(ResilienceDrainTimerHandle);
				}
			}
		}
		else {
			if (IsBlocking == false && GetCooldown("BlockPress") <= CurrentGameTime && Resilience >= 25.0f) {
				IsBlocking = true;
				if (BlockSubtractResilience == false) {
					Resilience = UKismetMathLibrary::FClamp(Resilience - 20.0f, 0.0f, 100.0f);
					BlockSubtractResilience = true;
				}
				if (this->HasAuthority()) {
					if (ResilienceDrainTimerHandle.IsValid() == false) {
						GetWorldTimerManager().SetTimer(ResilienceDrainTimerHandle, this, &ABaseCharacter::ResilienceDrainTimer, 1.0f, true, 1.0f);
					}
				}
			}
		}
	}
	else if (BlockPressed == false) {
		if (this->HasAuthority()) {
			if (ResilienceDrainTimerHandle.IsValid() == true) {
				GetWorld()->GetTimerManager().ClearTimer(ResilienceDrainTimerHandle);
			}
		}
		BlockSubtractResilience = false;
	}
}
void ABaseCharacter::BlockAnimation() {
	if (IsBlocking == true && BlockingAnim < 1 && IsDead == false) {
		BlockingAnim = FMath::FInterpTo(BlockingAnim, 1, .01, 30);
		StopAnimations();
	}
	else if (IsBlocking == false && BlockingAnim > 0 && IsDead == false) {
		BlockingAnim = FMath::FInterpTo(BlockingAnim, 0, .01, 30);
	}
}
// Check if player is facing within 90 degrees of the front of an enemy
bool ABaseCharacter::GetPlayerDirections(AActor * Attacked) {
	FVector Rotation1 = UKismetMathLibrary::Conv_RotatorToVector(UKismetMathLibrary::FindLookAtRotation((Attacked->GetActorLocation()), this->GetActorLocation()));
	FVector Rotation2 = UKismetMathLibrary::GetForwardVector(Attacked->GetActorRotation());
	if (UKismetMathLibrary::VSize(Rotation1 - Rotation2) <= 0.9f) {
		return true;
	}
	else {
		return false;
	}
}

/*********************** ANIMATIONS ***********************/
void ABaseCharacter::PlayActionAnim(UAnimMontage* Animation, float Speed) {
	if (this->HasAuthority()) {
		PlayActionAnimServer(Animation, Speed);
	}
}
void ABaseCharacter::PlayActionAnimServer_Implementation(UAnimMontage* Animation, float Speed) {
	if (this->GetMesh()->GetAnimInstance() != NULL)
	{
		this->GetMesh()->GetAnimInstance()->Montage_Play(Animation, Speed);
	}
}
void ABaseCharacter::StopAnimations() {
	if (this->HasAuthority()) {
		StopAnimationsServer_Implementation();
	}
	else {
		StopAnimationsServer_Implementation();
		this->GetMesh()->GetAnimInstance()->Montage_Stop(0.0f, NULL);
	}
}
void ABaseCharacter::StopAnimationsServer_Implementation() {
	this->StopAnimMontage();
}
// Play when PlayerCollision2 is overlapped
void ABaseCharacter::PlayerCollision2Begin(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor != this) {
		Overlapping = true;
	}
}
void ABaseCharacter::PlayerCollision2End(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor != this) {
		Overlapping = false;
		Colliding = false;
	}
}

/*********************** PARTICLE EFFECTS ***********************/
void ABaseCharacter::InitializeParticle_Implementation(TSubclassOf<AActor> Particle) {
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor * SpawnedActor = (GetWorld()->SpawnActor<AActor>(Particle, GetMesh()->K2_GetComponentLocation(), GetMesh()->K2_GetComponentRotation(), SpawnParameters));
	SpawnedActor->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false));
}

/*********************** EVENTS UPON TAKING DAMAGE ***********************/
/* Flinch Handler */
void ABaseCharacter::FlinchEventTrigger() {
	if (FlinchTrigger == true) {
		if (IsDead == false) {
			if (this->HasAuthority()) {
				FlinchEventServer();
			}
			else {
				FlinchEventServer();
				FlinchEvent();
			}
		}
		else {
			FlinchTrigger = false;
		}
	}
}
void ABaseCharacter::FlinchEventServer_Implementation() {
	FlinchEvent();
}
bool ABaseCharacter::FlinchEventServer_Validate() {
	return true;
}
void ABaseCharacter::FlinchEvent() {
	if (Health > 0 && SuperArmor == false && HBasicAttackSuperArmor == false) {
		FlinchTrigger = false;
		StopAnimations();
		Flinched = true;
		FName FlinchAnimPath = TEXT("/Game/Classes/Revenant/Animations/Recoil/Flinch_Montage.Flinch_Montage");
		UAnimMontage *FlinchAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *FlinchAnimPath.ToString()));
		PlayActionAnim(FlinchAnimMontage, 1.1f);
		GetWorldTimerManager().SetTimer(delayTimerHandle, this, &ABaseCharacter::FlinchEvent2, .7f, false);
	}
}
void ABaseCharacter::FlinchEvent2() {
	if (Flinched == true) {
		Flinched = false;
	}
	CanAttack = true;
}
/** When a hitbox is triggered and a weapon hit */
void ABaseCharacter::WeaponHitEvent(FHitResult HitResult) {
	if (HitResult.GetActor() != this) {
		HurtboxActive = false;
		ABaseCharacter* AttackedTarget = Cast<ABaseCharacter>(HitResult.GetActor());
		InflictDamage(AttackedTarget, CurrentDamage, true, true);
	}
}
bool ABaseCharacter::InflictDamage(ABaseCharacter* Target, float Damage, bool BlockCheck, bool Flinches) {
	if (Target->Invincible == false) {
		if (Target->IsBlocking != true || GetPlayerDirections(Target) == false || BlockCheck == false) { // Incorrectly blocked
			CurrentAttackHit = true;
			ResilienceAttackReplenish += Damage;
			AttackEffect(Target, CurrentAttackName);
			Target->IsBlocking = false;
			if (Flinches == true) {
				Target->FlinchTrigger = true;
			}
			Target->ApplyDamage(Damage, this);
			return true;
		}
		else { // Correctly blocked
			FName Path = TEXT("/Game/Classes/Revenant/Animations/Recoil/Shield_Block_Recoil_Montage.Shield_Block_Recoil_Montage");
			UAnimMontage *BlockRecoilMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *Path.ToString()));
			Target->PlayActionAnim(BlockRecoilMontage, 1.0f);
			Target->Resilience = UKismetMathLibrary::FClamp(Target->Resilience - (Damage / 2), 0.0f, 100.0f);
			return false;
		}
	}
	return false;
}
/** On receiving any damage, will decrement health and if below or equal to zero, dies */
void ABaseCharacter::ReceiveAnyDamage(float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!Invincible)
	{
		Health = Health - Damage;
		InitiateDamageEffect();
		SendEventToAttacker(Cast<ABaseCharacter>(DamageCauser));
		if (DetectMode == true) {
			DetectAction();
		}
		if (Health <= 0)
		{
			ServerDeath();
		}
	}
}
void ABaseCharacter::ApplyDamage(float Damage, ABaseCharacter * Attacker) {
	Health -= Damage;
	ResilienceDefenseReplenish += Damage * 2.0f;
	InitiateDamageEffect();
	InitiateDamageNumberEffect(Damage, this);
	Attacker->ComboAmount++;
	SendEventToAttacker(Attacker);

	if (DetectMode == true) {
		DetectAction();
	}
	if (Health <= 0) {
		ServerDeath();
	}
}
void ABaseCharacter::ApplyDamageOverTime(FString Type, float DamageAmt, int Amount, float StartDelay, float IncrementAmt) {
	FDamageOverTime tempMod;
	tempMod.Type = Type;
	tempMod.DamageAmt = DamageAmt;
	tempMod.TicksLeft = Amount;
	tempMod.LastTick = CurrentGameTime + StartDelay;
	tempMod.IncrementAmt = IncrementAmt;
	DamageOverTimeArray.Add(tempMod);
}
/** Apply override so player who received damage can send events to player who dealt damage */
void ABaseCharacter::SendEventToAttacker(ABaseCharacter* Attacker) {
}
/** Apply Weapon effects when damage taken */
void ABaseCharacter::AttackEffect(ABaseCharacter* Target, FString AttackName) {

}
void ABaseCharacter::AttackExecute(FString AttackName) {

}
void ABaseCharacter::AttackExecuteClientToServer_Implementation(const FString &AttackName) {
}
bool ABaseCharacter::AttackExecuteClientToServer_Validate(const FString &AttackName) {
	return true;
}
void ABaseCharacter::AttackExecuteServer(FString AttackName) {
}

/** Function for detecting abilities */
void ABaseCharacter::DetectAction() {

}

// When a character dies
void ABaseCharacter::ServerDeath() {
	if (this->HasAuthority()) {
		ServerDeathRepAll();
	}
	else {
		ServerDeathServer();
	}
}
void ABaseCharacter::ServerDeathServer_Implementation() {
	ServerDeathRepAll();
}
bool ABaseCharacter::ServerDeathServer_Validate() {
	return true;
}
void ABaseCharacter::ServerDeathRepAll_Implementation()
{
	IsDead = true;
	CurrentFBLoc = 0;
	CurrentLRLoc = 0;
	IsRolling = false;
	IsSideStepping = false;
	PlayerCollision->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	GetCapsuleComponent()->SetCollisionProfileName(FName("PassThroughPlayer"));
	this->bUseControllerRotationYaw = 0;
	GetWorldTimerManager().SetTimer(delayTimerHandle, this, &ABaseCharacter::RespawnEvent, 5.0f, false);
}
// Respawn
void ABaseCharacter::RespawnEvent()
{
	//TODO: Implement death animation stuff
	TeleportTo(FVector(0, 0, 450), FRotator(0, 0, 0));
	PlayerCollision->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionProfileName(FName("NoPassThroughPlayer"));
	Health = 100;
	IsDead = false;
}

/*********************** ATTACKING ***********************/
/** Attack Handler */
void ABaseCharacter::AttackHandler(FString AttackName, FString AttackType, float CastCooldownAmt, float CastSpeed, bool IsChainable, UAnimMontage* Animation, float DelayBeforeHitbox, float LengthOfHitbox, float Damage, bool UseHitbox, UBoxComponent* Hitbox, bool Projectile) {
	if (IsValidAttack(IsChainable, CastCooldownAmt, AttackName, GetCooldownAmt(AttackName)) == true && MenuUp == false && CanAttack == true && IsRolling == false && IsSideStepping == false && Flinched == false) {
		CheckMoveDuringAttack();
		CanAttack = false;
		CurrentAttackName = AttackName;
		CurrentAttackType = AttackType;
		PlayActionAnim(Animation, CastSpeed);
		AttackCastCooldown = CurrentGameTime + DelayBeforeHitbox + LengthOfHitbox + CastCooldownAmt;
		if (AttackName == "HBasicAttack" && ComboAmount == 0) {
			HBasicAttackSuperArmor = true;
		}
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("AttackHandler2"), AttackName, AttackType, CastCooldownAmt, LengthOfHitbox, Damage, UseHitbox, Hitbox, Projectile);
		GetWorldTimerManager().SetTimer(delayTimerHandle, TimerDel, DelayBeforeHitbox, false);
	}
	else {
		LastAttack = "Missed";
		CurrentAttackHit = false;
	}
}
void ABaseCharacter::AttackHandler2(FString AttackName, FString AttackType, float CastCooldownAmt, float LengthOfHitbox, float Damage, bool UseHitbox, UBoxComponent* Hitbox, bool Projectile) {
	if (IsRolling == false && IsSideStepping == false && Flinched == false) {
		if (UseHitbox == false && Projectile == false) {
			HurtboxActive = true;
		}
		else if (UseHitbox == true) {
			Hitbox->bGenerateOverlapEvents = true;
		}
		else if (Projectile == true) {
			ProjectileHandler(AttackName);
		}
	}
	HBasicAttackSuperArmor = false;
	CurrentDamage = CalcFinalDamage(Damage);
	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, FName("AttackHandler3"), AttackName, AttackType, CastCooldownAmt, UseHitbox, Hitbox);
	GetWorldTimerManager().SetTimer(delayTimerHandle, TimerDel, LengthOfHitbox, false);
}
void ABaseCharacter::AttackHandler3(FString AttackName, FString AttackType, float CastCooldownAmt, bool UseHitbox, UBoxComponent* Hitbox) {
	MakeCurrentActionLastAction(AttackType);
	if (UseHitbox == true) {
		Hitbox->bGenerateOverlapEvents = false;
	}
	SetCooldown("BlockPress", .2);
	HurtboxActive = false;
	CurrentDamage = 0.0f;
	CurrentAttackHit = false;
	if (Flinched == false) {
		CanAttack = true;
	}
	SetCooldown(AttackName, GetFinalCooldownAmt(AttackName, GetCooldownAmt(AttackName)));
}
/** Function for shooting projectiles */
void ABaseCharacter::ProjectileHandler(FString AttackName) {
	if (this->HasAuthority()) {
		MaxProjectileRange = FollowCamera->K2_GetComponentLocation() + (FollowCamera->GetForwardVector() * 10000.0f);
		FVector TempProjectileStartLoc = FollowCamera->K2_GetComponentLocation();
		FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
		RV_TraceParams.bTraceComplex = true;
		RV_TraceParams.bTraceAsyncScene = true;
		RV_TraceParams.bReturnPhysicalMaterial = false;
		RV_TraceParams.bIgnoreBlocks = false;
		//Re-initialize hit info
		FHitResult Out_Hit(ForceInit);
		//call GetWorld() from within an actor extending class
		if (GetWorld()->LineTraceSingleByChannel(
			Out_Hit,
			FollowCamera->K2_GetComponentLocation() + (FollowCamera->GetForwardVector() * 250.0f),
			FollowCamera->K2_GetComponentLocation() + (FollowCamera->GetForwardVector() * 10000.0f),
			COLLISION_PROJECTILE,
			RV_TraceParams) == true
			) {
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Instigator = this;
			AProjectileBase * ProjectileLaunched = (GetWorld()->SpawnActor<AProjectileBase>(BaseProjectile, GetMesh()->GetSocketLocation(FName(TEXT("Shieldsocket"))), UKismetMathLibrary::MakeRotFromX(Out_Hit.ImpactPoint - GetMesh()->GetSocketLocation(FName(TEXT("Shieldsocket")))), SpawnParameters));
			ProjectileLaunched->ProjectileName = AttackName;
			ProjectileLaunched->ProjectileStartLoc = TempProjectileStartLoc;
		}
		else {
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Instigator = this;
			AProjectileBase * ProjectileLaunched = (GetWorld()->SpawnActor<AProjectileBase>(BaseProjectile, GetMesh()->GetSocketLocation(FName(TEXT("Shieldsocket"))), UKismetMathLibrary::MakeRotFromX(MaxProjectileRange - GetMesh()->GetSocketLocation(FName(TEXT("Shieldsocket")))), SpawnParameters));
			ProjectileLaunched->ProjectileName = AttackName;
			ProjectileLaunched->ProjectileStartLoc = TempProjectileStartLoc;
		}
	}
}
// Do not move player if within proximity and facing them, move if not
void ABaseCharacter::CheckMoveDuringAttack() {
	if (Overlapping == true) {

		FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
		RV_TraceParams.bTraceComplex = true;
		RV_TraceParams.bTraceAsyncScene = true;
		RV_TraceParams.bReturnPhysicalMaterial = false;
		RV_TraceParams.bIgnoreBlocks = false;

		//Re-initialize hit info
		FHitResult Out_Hit(ForceInit);

		//call GetWorld() from within an actor extending class
		if (GetWorld()->LineTraceSingleByChannel(
			Out_Hit,
			(FollowCamera->GetForwardVector() * 250) + FollowCamera->GetComponentLocation(),
			(FollowCamera->GetForwardVector() * 1000) + FollowCamera->GetComponentLocation(),
			COLLISION_DIRECTION,
			RV_TraceParams) == true
			) {
			Colliding = true;
		}
		else {
			Colliding = false;
		}
	}
}
/* Moves CurrentAction to LastAction */
void ABaseCharacter::MakeCurrentActionLastAction(FString CurrentAttack) {
	if (CurrentAttack == "SideStep" || CurrentAttack == "Roll" || CurrentAttack == "Block" || CurrentAttackHit == true) {
		LastAttack = CurrentAttack;
	}
	else {
		LastAttack = "Missed";
	}
}
/** Checks if the current attack should be carried out */
bool ABaseCharacter::IsValidAttack(bool IsChainable, float CastCooldownAmt, FString CurrentAttack, float CooldownAmt) {
	if (GetCooldown(CurrentAttack) >= CurrentGameTime || (CurrentAttack == "CounteringBlow" && AttackCastCooldown + 0.2f >= CurrentGameTime)) {
		return false;
	}
	// If you are not in cast cooldown or attack cooldown
	else if (AttackCastCooldown < CurrentGameTime && GetCooldown(CurrentAttack) < CurrentGameTime) {
		return true;
	}
	// If you are still in cast cooldown
	else if (AttackCastCooldown >= CurrentGameTime && IsChainable == true) {
		return true;
	}
	else {
		return false;
	}
}
/** Checks if the current attack should be chainable */
bool ABaseCharacter::CheckChainable(FString CurrentAttack) {
	if (AttackCastCooldown < CurrentGameTime) {
		return false;
	}
	if (CurrentAttack == "SBasicAttack") {
		if (LastAttack == "CounteringBlow" || LastAttack == "ComboExtender") {
			return true;
		}
		else {
			return false;
		}
	}
	else if (CurrentAttack == "HBasicAttack") {
		if (LastAttack == "SBasicAttack" || LastAttack == "CounteringBlow" || LastAttack == "ComboExtender") {
			return true;
		}
		else {
			return false;
		}
	}
	else if (CurrentAttack == "ComboExtender") {
		if (LastAttack == "SBasicAttack" || LastAttack == "HBasicAttack" || LastAttack == "CounteringBlow" || LastAttack == "ComboExtender") {
			return true;
		}
		else {
			return false;
		}
	}
	else if (CurrentAttack == "ComboFinisher") {
		if (LastAttack == "SBasicAttack" || LastAttack == "HBasicAttack" || LastAttack == "CounteringBlow" || LastAttack == "ComboExtender") {
			return true;
		}
		else {
			return false;
		}

	}
	else {
		return false;
	}
}

/** Calculate Speed Amount */
float ABaseCharacter::GetSpeedAmt() {
	float TotalAmount = 230.0f;
	for (int i = 0; i < SpeedEffectsArray.Num(); i++) {
		if (SpeedEffectsArray[i].Cooldown < CurrentGameTime) {
			SpeedEffectsArray.RemoveAt(i, 1, true);
			i--;
		}
		else  {
			TotalAmount += (SpeedEffectsArray[i].Modifier * 230.0f);
		}
	}
	return TotalAmount;
}
/** Calculate Cooldown Amount after cooldown buffs and debuffs */
float ABaseCharacter::GetFinalCooldownAmt(FString AttackName, float CooldownAmt) {
	float TotalAmount = 0.0f;
	TotalAmount += CooldownAmt;
	for (int i = 0; i < CooldownEffectsArray.Num(); i++) {
		if (CooldownEffectsArray[i].Duration < CurrentGameTime) {
			CooldownEffectsArray.RemoveAt(i, 1, true);
			i--;
		}
		else if (CooldownEffectsArray[i].AbilityName == AttackName) {
			TotalAmount += (CooldownEffectsArray[i].Modifier * CooldownAmt);
			CooldownEffectsArray.RemoveAt(i, 1, true);
			i--;
		}
	}
	return TotalAmount;
}
/** Get Cooldown for attack */
float ABaseCharacter::GetCooldown(FString AttackName) {
	for (int i = 0; i < AttackArray.Num(); i++) {
		if (AttackArray[i].AttackName.Equals(AttackName)) {
			return AttackArray[i].Cooldown;
		}
	}
	return 0.0f;
}
/** Set Cooldown for attack */
void ABaseCharacter::SetCooldown(FString AttackName, float CooldownAmt) {
	for (int i = 0; i < AttackArray.Num(); i++) {
		if (AttackArray[i].AttackName.Equals(AttackName)) {
			AttackArray[i].Cooldown = CurrentGameTime + CooldownAmt;
		}
	}
}
/** Get Damage for attack */
float ABaseCharacter::GetDamage(FString AttackName) {
	for (int i = 0; i < DamageTable.Num(); i++) {
		if (DamageTable[i].Attack.Equals(AttackName)) {
			return DamageTable[i].Damage;
		}
	}
	return 0.0f;
}
/** Get Cooldown Amount for attack */
float ABaseCharacter::GetCooldownAmt(FString AttackName) {
	for (int i = 0; i < AttackArray.Num(); i++) {
		if (AttackArray[i].AttackName.Equals(AttackName)) {
			return AttackArray[i].CooldownAmt;
		}
	}
	return 0.0f;
}
/** Add Cooldown Buff/Debuff */
void ABaseCharacter::AddCooldownModifier(float Modifier, FString AbilityName, float Duration) {
	FCooldownEffectsStruct tempMod;
	tempMod.Modifier = Modifier;
	tempMod.AbilityName = AbilityName;
	tempMod.Duration = CurrentGameTime + Duration;
	CooldownEffectsArray.Add(tempMod);
}
/** Add Damage Buff/Debuff */
void ABaseCharacter::AddDamageModifier(float Modifier, float Duration, int NumHits) {
	FAttackModifierStruct tempMod;
	tempMod.Modifier = Modifier;
	tempMod.Cooldown = CurrentGameTime + Duration;
	tempMod.NumHits = NumHits;
	AttackModifierArray.Add(tempMod);
}
/** Add Defense Buff/Debuff */
void ABaseCharacter::AddDefenseModifier(float Modifier, float Duration, int NumHits) {
	FDefenseModifierStruct tempMod;
	tempMod.Modifier = Modifier;
	tempMod.Cooldown = CurrentGameTime + Duration;
	tempMod.NumHits = NumHits;
	DefenseModifierArray.Add(tempMod);
}
float ABaseCharacter::CalcFinalDamage(float Damage) {
	float TotalAttackMods = 1.0f;
	float TotalDefenseMods = 1.0f;
	for (int i = 0; i < AttackModifierArray.Num(); i++) {
		if (AttackModifierArray[i].Cooldown > CurrentGameTime) {
			if (AttackModifierArray[i].NumHits > 0) {
				TotalAttackMods += (AttackModifierArray[i].Modifier);
				AttackModifierArray[i].NumHits--;
			}
			else {
				AttackModifierArray.RemoveAt(i, 1, true);
				i--;
			}
		}
		else {
			AttackModifierArray.RemoveAt(i, 1, true);
			i--;
		}
	}
	for (int i = 0; i < DefenseModifierArray.Num(); i++) {
		if (DefenseModifierArray[i].Cooldown > CurrentGameTime) {
			if (DefenseModifierArray[i].NumHits > 0) {
				TotalDefenseMods += (DefenseModifierArray[i].Modifier);
				DefenseModifierArray[i].NumHits--;
			}
			else {
				DefenseModifierArray.RemoveAt(i, 1, true);
				i--;
			}
		}
		else {
			DefenseModifierArray.RemoveAt(i, 1, true);
			i--;
		}
	}
	return Damage * (TotalAttackMods/TotalDefenseMods);
}
/*********************** DAMAGE EFFECTS ***********************/
/* Track Damage over Time */
void ABaseCharacter::TrackDamageOverTime() {
	for (int i = 0; i < DamageOverTimeArray.Num(); i++) {
		if (DamageOverTimeArray[i].TicksLeft <= 0) {
			DamageOverTimeArray.RemoveAt(i, 1, true);
			i--;
			continue;
		}
		if (DamageOverTimeArray[i].LastTick <= CurrentGameTime) {
			if (DamageOverTimeArray[i].Type == "FlinchOnLast") {

				if (DamageOverTimeArray[i].TicksLeft == 1) { // Last tick
					InflictDamage(this, DamageOverTimeArray[i].DamageAmt, false, true);
				}
				else {
					InflictDamage(this, DamageOverTimeArray[i].DamageAmt, false, false);
				}
			}
			DamageOverTimeArray[i].TicksLeft--;
			DamageOverTimeArray[i].LastTick = CurrentGameTime + DamageOverTimeArray[i].IncrementAmt;
		}
	}
}