
#include "MedievalCombat.h"
#include "BaseCharacter.h"
#include "UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnrealMathUtility.h"


#define COLLISION_ATTACK ECC_GameTraceChannel4
#define COLLISION_DIRECTION ECC_GameTraceChannel5

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

	// Create a direction camera
	DirectionCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DirectionCamera"));
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	DirectionCamera->SetupAttachment(RootComponent);
	// Camera does not rotate relative to arm
	DirectionCamera->bUsePawnControlRotation = false;

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
	FName WepSocket = TEXT("Weaponsocket");
	Weapon->SetupAttachment(GetMesh(), WepSocket);

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
}

// Allows Replication of variables for Client/Server Networking
void ABaseCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ABaseCharacter, IsDead);
	DOREPLIFETIME(ABaseCharacter, DamageEffect);
	DOREPLIFETIME(ABaseCharacter, Health);
	DOREPLIFETIME(ABaseCharacter, CanAttack);
	DOREPLIFETIME(ABaseCharacter, CurrentAttackHit);
	DOREPLIFETIME(ABaseCharacter, LastAttack);

	DOREPLIFETIME(ABaseCharacter, BlockPressed);
	DOREPLIFETIME(ABaseCharacter, IsBlocking);
	DOREPLIFETIME(ABaseCharacter, BlockingAnim);
	DOREPLIFETIME(ABaseCharacter, BlockCooldown);
	DOREPLIFETIME(ABaseCharacter, SubResilience);

	DOREPLIFETIME(ABaseCharacter, FlinchTrigger);
	DOREPLIFETIME(ABaseCharacter, Flinched);
	DOREPLIFETIME(ABaseCharacter, SuperArmor);

	DOREPLIFETIME(ABaseCharacter, IsRolling);
	DOREPLIFETIME(ABaseCharacter, IsSideStepping);

	DOREPLIFETIME(ABaseCharacter, CanMove);
	DOREPLIFETIME(ABaseCharacter, CanTurn);

	DOREPLIFETIME(ABaseCharacter, Colliding);
	DOREPLIFETIME(ABaseCharacter, Overlapping);

	//DOREPLIFETIME(ABaseCharacter, Resilience);
	//DOREPLIFETIME(ABaseCharacter, ResilienceDrainAmt);
	//DOREPLIFETIME(ABaseCharacter, ResilienceRegenAmt);

	DOREPLIFETIME(ABaseCharacter, AttackCastCooldown);

	DOREPLIFETIME(ABaseCharacter, MenuUp);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	//Start Regen/Drain handlers
	GetWorldTimerManager().SetTimer(ResilienceRegenTimerHandle, this, &ABaseCharacter::ResilienceRegenTimer, 1.0f, true);
}

// Tick
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MovementHandler(); // Changes movement based on if player is rolling or not
	HitboxHandler(); // Activates hitbox when attacking
	BlockHandler(); // Handles whether player is blocking
	BlockAnimation(); // Changes animation based on if player is blocking
	FlinchEventTrigger(); // Applies flinching when player is attacked
	if (IsDead == true && this->bUseControllerRotationYaw == 0) {
		this->bUseControllerRotationYaw = 1;
	}
}

// Input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Delay function
void ABaseCharacter::onTimerEnd()
{
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
		if (CanDamage == true) { // Only when swinging
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
	if (Resilience < 100) { // Regen resilience
		Resilience = UKismetMathLibrary::FClamp(Resilience + ResilienceRegenAmt, 0, 100);
	}
}
void ABaseCharacter::ResilienceDrainTimer() {
	if (Resilience >= ResilienceDrainAmt) { // Drain resilience
		Resilience = UKismetMathLibrary::FClamp(Resilience - ResilienceDrainAmt, 0, 100);
	}
}

/*********************** ROLL ***********************/
void ABaseCharacter::SideStepPressedEventClient() {
	if (MenuUp == false && Flinched == false && CanAttack == true && IsDead == false) {
		IsSideStepping = true;
		CanAttack = false;
		MakeCurrentActionLastAction("SideStep");
		IsBlocking = false;
		Invincible = true;
		CharMovement->MaxWalkSpeed = 3000;
		CharMovement->MaxAcceleration = 3500;
		SideStepCooldown = UKismetSystemLibrary::GetGameTimeInSeconds(this) + SideStepCooldownAmt;
		GetWorldTimerManager().SetTimer(delayTimerHandle, this, &ABaseCharacter::SideStepPressedEventClient2, 0.25f, false);
	}
}
void ABaseCharacter::SideStepPressedEventClient2() {
	IsSideStepping = false;
	CharMovement->MaxWalkSpeed = 230;
	CharMovement->MaxAcceleration = 1500;
	GetWorldTimerManager().SetTimer(delayTimerHandle, this, &ABaseCharacter::SideStepPressedEventClient3, 0.1f, false);
}
void ABaseCharacter::SideStepPressedEventClient3() {
	if (IsRolling == false && IsSideStepping == false) {
		Invincible = false;
	}
	if (CanAttack == false) {
		CanAttack = true;
	}
}

void ABaseCharacter::RollPressedEventClient() {
	if (MenuUp == false) {
		Resilience -= 25;
		IsRolling = true;
		MakeCurrentActionLastAction("Roll");
		FlinchTrigger = false;
		Flinched = false;
		IsBlocking = false;
		Invincible = true;
		CanDamage = false;
		CharMovement->MaxWalkSpeed = 600;
		CharMovement->MaxAcceleration = 3500;
		if (ResilienceRegenTimerHandle.IsValid() == true) {
			GetWorld()->GetTimerManager().ClearTimer(ResilienceRegenTimerHandle);
		}
		if (ResilienceDrainTimerHandle.IsValid() == true) {
			GetWorld()->GetTimerManager().ClearTimer(ResilienceDrainTimerHandle);
		}
		GetWorldTimerManager().SetTimer(delayTimerHandle, this, &ABaseCharacter::RollPressedEventClient2, 0.5f, false);
	}
}
void ABaseCharacter::RollPressedEventClient2() {
	IsRolling = false;
	CharMovement->MaxWalkSpeed = 230;
	CharMovement->MaxAcceleration = 1500;
	GetWorldTimerManager().SetTimer(delayTimerHandle, this, &ABaseCharacter::RollPressedEventClient3, 0.1f, false);
}
void ABaseCharacter::RollPressedEventClient3() {
	if (IsRolling == false && IsSideStepping == false) {
		Invincible = false;
	}
	if (ResilienceRegenTimerHandle.IsValid() == false) {
		GetWorldTimerManager().SetTimer(ResilienceRegenTimerHandle, this, &ABaseCharacter::ResilienceRegenTimer, 1.0f, true);
	}
	if (ResilienceDrainTimerHandle.IsValid() == false) {
		GetWorldTimerManager().SetTimer(ResilienceDrainTimerHandle, this, &ABaseCharacter::ResilienceDrainTimer, 1.0f, true);
	}
	if (CanAttack == false) {
		CanAttack = true;
	}
}
void ABaseCharacter::MovementHandler() {
	if (this->HasAuthority()) {
		MovementHandlerServer();
	}
	else {
		MovementHandlerServer();
		MovementHandlerClient();
	}
}
void ABaseCharacter::MovementHandlerServer_Implementation() {
	MovementHandlerClient();
}
bool ABaseCharacter::MovementHandlerServer_Validate() {
	return true;
}
void ABaseCharacter::MovementHandlerClient() {
	if ((IsRolling == true || IsSideStepping == true) && IsDead == false) {
		StopAnimations();
		if (CanMove == true) {
			CanMove = false;
		}
		if (CurrentLRLoc == 0.0f && CurrentFBLoc == 0.0f) {
			CurrentFBLoc = 1.0f;
		}
		this->AddMovementInput(GetActorForwardVector(), CurrentFBLoc, false);
		this->AddMovementInput(GetActorRightVector(), CurrentLRLoc, false);
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

/*********************** BLOCK ***********************/
void ABaseCharacter::BlockPressedEventClient() {
	if (MenuUp == false) {
		BlockPressed = true;
		if (LastAttack != "Block") {
			MakeCurrentActionLastAction("Block");
		}
	}
}
void ABaseCharacter::BlockReleasedEventClient() {
	IsBlocking = false;
	BlockPressed = false;
	BlockCooldown = UKismetSystemLibrary::GetGameTimeInSeconds(this) + 1;
}
void ABaseCharacter::BlockHandler() {
	if (BlockPressed == true) {
		if (Resilience < ResilienceDrainAmt || Flinched == true || MenuUp == true || CanAttack == false || IsRolling == true || IsSideStepping == true) {
			IsBlocking = false;
			SubResilience = false;
			if (ResilienceRegenTimerHandle.IsValid() == false) {
				GetWorldTimerManager().SetTimer(ResilienceRegenTimerHandle, this, &ABaseCharacter::ResilienceRegenTimer, 1.0f, true, 1.0f);
			}
			if (ResilienceDrainTimerHandle.IsValid() == true) {
				GetWorld()->GetTimerManager().ClearTimer(ResilienceDrainTimerHandle);
			}
		}
		else {
			if (IsBlocking == false && BlockCooldown <= UKismetSystemLibrary::GetGameTimeInSeconds(this)) {
				IsBlocking = true;
				if (SubResilience == false) {
					Resilience -= ResilienceDrainAmt;
					SubResilience = true;
				}
				if (ResilienceRegenTimerHandle.IsValid() == true) {
					GetWorld()->GetTimerManager().ClearTimer(ResilienceRegenTimerHandle);
				}
				if (ResilienceDrainTimerHandle.IsValid() == false) {
					GetWorldTimerManager().SetTimer(ResilienceDrainTimerHandle, this, &ABaseCharacter::ResilienceDrainTimer, 1.0f, true, 1.0f);
				}
			}
		}
	}
	else if (BlockPressed == false) {
		if (ResilienceRegenTimerHandle.IsValid() == false) {
			GetWorldTimerManager().SetTimer(ResilienceRegenTimerHandle, this, &ABaseCharacter::ResilienceRegenTimer, 1.0f, true, 1.0f);
		}
		if (ResilienceDrainTimerHandle.IsValid() == true) {
			GetWorld()->GetTimerManager().ClearTimer(ResilienceDrainTimerHandle);
		}
		SubResilience = false;
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
void ABaseCharacter::PlayActionAnim(UAnimMontage* Animation, float Speed, bool IsAttack) {
	if (this->HasAuthority()) {
		PlayActionAnimServer(Animation, Speed, IsAttack);
	}
}
void ABaseCharacter::PlayActionAnimServer_Implementation(UAnimMontage* Animation, float Speed, bool IsAttack) {
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
/* Wrapper for playing animations and displaying for client too */
void ABaseCharacter::RelayAnimation(UAnimMontage* Animation, float Speed) {
	if (this->HasAuthority()) {
		RelayAnimationServer(Animation, Speed);
	}
	else {
		RelayAnimationServer(Animation, Speed);
		RelayAnimationClient(Animation, Speed);
	}
}
void ABaseCharacter::RelayAnimationServer_Implementation(UAnimMontage* Animation, float Speed) {
	RelayAnimationClient(Animation, Speed);
}
bool ABaseCharacter::RelayAnimationServer_Validate(UAnimMontage* Animation, float Speed) {
	return true;
}
void ABaseCharacter::RelayAnimationClient(UAnimMontage* Animation, float Speed) {
	PlayActionAnimServer(Animation, Speed, false);
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

/*********************** EVENTS UPON TAKING DAMAGE ***********************/
/* Flinch Handler */
void ABaseCharacter::FlinchEventTrigger() {
	if (FlinchTrigger == true) {
		if (this->HasAuthority()) {
			FlinchEventServer();
		}
		else {
			FlinchEventServer();
			FlinchEvent();
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
	if (Health > 0 && SuperArmor == false) {
		FlinchTrigger = false;
		StopAnimations();
		Flinched = true;
		FName FlinchAnimPath = TEXT("/Game/Classes/Revenant/Animations/Recoil/Flinch_Montage.Flinch_Montage");
		UAnimMontage *FlinchAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *FlinchAnimPath.ToString()));
		PlayActionAnim(FlinchAnimMontage, 1.1f, false);
		GetWorldTimerManager().SetTimer(delayTimerHandle, this, &ABaseCharacter::FlinchEvent2, .8f, false);
	}
}
void ABaseCharacter::FlinchEvent2() {
	if (Flinched == true) {
		Flinched = false;
	}
	CanAttack = true;
}
//When a hitbox is triggered and a weapon hit
void ABaseCharacter::WeaponHitEvent(FHitResult HitResult) {
	if (HitResult.GetActor() != this) {
		CurrentAttackHit = true;
		CanDamage = false;
		ABaseCharacter* AttackedTarget = Cast<ABaseCharacter>(HitResult.GetActor());
		if (AttackedTarget->Invincible == false) {
			if (AttackedTarget->IsBlocking != true || GetPlayerDirections(AttackedTarget) == false) { // Incorrectly blocked
				AttackedTarget->IsBlocking = false;
				AttackedTarget->FlinchTrigger = true;
				AttackedTarget->Health = (AttackedTarget->Health) - CurrentDamage;
				AttackedTarget->DamageEffect = true;
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
/* On receiving any damage, will decrement health and if below or equal to zero, dies */
void ABaseCharacter::ReceiveAnyDamage(float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!Invincible)
	{
		Health = Health - Damage;
		if (Health <= 0)
		{
			ServerDeath();
		}
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Damage Taken")));
	}
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
void ABaseCharacter::AttackHandler(FString AttackName, UPARAM(ref) float &Cooldown, float CooldownAmt, float CastCooldownAmt, float CastSpeed, bool IsChainable, UAnimMontage* Animation, float DelayBeforeHitbox, float LengthOfHitbox, float Damage) {
	if (IsValidAttack(IsChainable, CastCooldownAmt, AttackName, Cooldown) == true && MenuUp == false) {
		CheckMoveDuringAttack();
		CanAttack = false;
		//Change sensitivity
		Cooldown = UKismetSystemLibrary::GetGameTimeInSeconds(this) + CooldownAmt;
		AttackCastCooldown = UKismetSystemLibrary::GetGameTimeInSeconds(this) + CastCooldownAmt;
		PlayActionAnim(Animation, CastSpeed, true);
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("AttackHandler2"), AttackName, LengthOfHitbox, Damage);
		GetWorldTimerManager().SetTimer(delayTimerHandle, TimerDel, DelayBeforeHitbox, false);
	}
}
void ABaseCharacter::AttackHandler2(FString AttackName, float LengthOfHitbox, float Damage) {
	if (IsRolling == false && IsSideStepping == false) {
		CanDamage = true;
	}
	CurrentDamage = Damage;
	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, FName("AttackHandler3"), AttackName);
	GetWorldTimerManager().SetTimer(delayTimerHandle, TimerDel, LengthOfHitbox, false);
}
void ABaseCharacter::AttackHandler3(FString AttackName) {
	MakeCurrentActionLastAction(AttackName);
	CanDamage = false;
	CurrentDamage = 0.0f;
	CurrentAttackHit = false;
	if (Flinched == false) {
		CanAttack = true;
	}
	//Reset sensitivity
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
			DirectionCamera->GetComponentLocation(),
			(DirectionCamera->GetForwardVector() * 1000) + DirectionCamera->GetComponentLocation(),
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
	// If you are not in cast cooldown or attack cooldown
	if (AttackCastCooldown < UKismetSystemLibrary::GetGameTimeInSeconds(this) && CooldownAmt < UKismetSystemLibrary::GetGameTimeInSeconds(this)) {
		return true;
	}
	// If you are still in cast cooldown
	else if (AttackCastCooldown >= UKismetSystemLibrary::GetGameTimeInSeconds(this) && IsChainable == true) {
		return true;
	}
	else {
		return false;
	}
}
/** Checks if the current attack should be chainable */
bool ABaseCharacter::CheckChainable(FString CurrentAttack) {
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
		return true;
	}
	else {
		return false;
	}
}

/*********************** DAMAGE EFFECTS ***********************/
/* Bleed Event Handler */
void ABaseCharacter::BleedEvent() {
	if (!Invincible)
	{
		Health = Health - 3;
		if (Health <= 0) 
			ServerDeath(); 
	}
}