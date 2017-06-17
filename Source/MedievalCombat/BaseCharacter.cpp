
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

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	

															   // Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

												// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

												   // Create a direction camera
	DirectionCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DirectionCamera"));
	DirectionCamera->SetupAttachment(RootComponent); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	DirectionCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

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
	DOREPLIFETIME(ABaseCharacter, Health);
	DOREPLIFETIME(ABaseCharacter, CanAttack);
	DOREPLIFETIME(ABaseCharacter, CurrentAttackHit);
	DOREPLIFETIME(ABaseCharacter, LastAttack);
	DOREPLIFETIME(ABaseCharacter, IsBlocking);
	DOREPLIFETIME(ABaseCharacter, BlockPressed);
	DOREPLIFETIME(ABaseCharacter, BlockCooldown);
	DOREPLIFETIME(ABaseCharacter, Flinched);
	DOREPLIFETIME(ABaseCharacter, FlinchTrigger);
	DOREPLIFETIME(ABaseCharacter, SuperArmor);
	DOREPLIFETIME(ABaseCharacter, Resilience);
	DOREPLIFETIME(ABaseCharacter, ResilienceDrainAmt);
	DOREPLIFETIME(ABaseCharacter, ResilienceRegenAmt);
	DOREPLIFETIME(ABaseCharacter, CanMove);
	DOREPLIFETIME(ABaseCharacter, CanTurn);
	DOREPLIFETIME(ABaseCharacter, Colliding);
	DOREPLIFETIME(ABaseCharacter, Overlapping);
	DOREPLIFETIME(ABaseCharacter, IsRolling);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	//Start Regen/Drain handlers
	GetWorldTimerManager().SetTimer(ResilienceRegenTimerHandle, this, &ABaseCharacter::ResilienceRegenTimer, 1.0f, true);
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HitboxHandler();
	BlockHandler();
	BlockAnimation();
	FlinchEventTrigger();
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Handling attack hitbox
void ABaseCharacter::HitboxHandler() {
	if (this->HasAuthority()) {
		if (CanDamage == true) {
			if (InitialHitbox == true) {
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

// Handling block toggle
void ABaseCharacter::BlockHandler() {
	if (BlockPressed == true) {
		if (Resilience >= ResilienceDrainAmt && Flinched == false && IsRolling == false && CanAttack == true) {
			if (BlockCooldown == 0) {
				StopAnimations();
				IsBlocking = true;
				BlockCooldown = 1;
				if (ResilienceRegenTimerHandle.IsValid() == true) {
					GetWorld()->GetTimerManager().ClearTimer(ResilienceRegenTimerHandle);
				}
				if (ResilienceDrainTimerHandle.IsValid() == false) {
					GetWorldTimerManager().SetTimer(ResilienceDrainTimerHandle, this, &ABaseCharacter::ResilienceDrainTimer, 1.0f, true, 1.0f);
				}
			}
			else {
				if (IsBlocking == true) {
					IsBlocking = false;
				}
				if (ResilienceRegenTimerHandle.IsValid() == false) {
					GetWorldTimerManager().SetTimer(ResilienceRegenTimerHandle, this, &ABaseCharacter::ResilienceRegenTimer, 1.0f, true, 1.0f);
				}
				if (ResilienceDrainTimerHandle.IsValid() == true) {
					GetWorld()->GetTimerManager().ClearTimer(ResilienceDrainTimerHandle);
				}
			}
		}
		else {
			if (IsBlocking == true) {
				IsBlocking = false;
			}
			if (ResilienceRegenTimerHandle.IsValid() == false) {
				GetWorldTimerManager().SetTimer(ResilienceRegenTimerHandle, this, &ABaseCharacter::ResilienceRegenTimer, 1.0f, true, 1.0f);
			}
			if (ResilienceDrainTimerHandle.IsValid() == true) {
				GetWorld()->GetTimerManager().ClearTimer(ResilienceDrainTimerHandle);
			}
		}
	}
	else {
		if (ResilienceRegenTimerHandle.IsValid() == false) {
			GetWorldTimerManager().SetTimer(ResilienceRegenTimerHandle, this, &ABaseCharacter::ResilienceRegenTimer, 1.0f, true, 1.0f);
		}
		if (ResilienceDrainTimerHandle.IsValid() == true) {
			GetWorld()->GetTimerManager().ClearTimer(ResilienceDrainTimerHandle);
		}
	}
}

// Smooths transition to and fro blocking
void ABaseCharacter::BlockAnimation() {
	if (IsBlocking == true && BlockingAnim < 1) {
		BlockingAnim = FMath::FInterpTo(BlockingAnim, 1, .01, 10);
	}
	else if (IsBlocking == false && BlockingAnim > 0) {
		BlockingAnim = FMath::FInterpTo(BlockingAnim, 0, .01, 10);
	}
}

// Function for handling DELAY equivalent from Blueprints
void ABaseCharacter::onTimerEnd()
{
}

//Death Handler Multicast
void ABaseCharacter::DeathAnimation_Implementation()
{
	IsDead = true;
}

//Death Handler Client
void ABaseCharacter::DeathAnimationForPlayer_Implementation()
{
	CanMove = false;
	IsRolling = true;
	FlinchTrigger = false;
	Flinched = false;
	IsBlocking = false;
	FollowCamera->SetRelativeLocation(FVector(-117.5f, 0.0f, 72.5f)); //Unsure about this line
	GetMesh()->SetOwnerNoSee(true);
}

/* On receiving any damage, will decrement health and if below or equal to zero, dies */
void ABaseCharacter::ReceiveAnyDamage(float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!Invincible)
	{
		Health = Health - Damage;
		if (Health <= 0)
		{
			//ServerDeath(); // Seems to be an unimplemented function
		}
	}
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
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some debug message!"));
				AttackedTarget->Health -= CurrentDamage;
				if (AttackedTarget->Health <= 0) {
					//AttackedTarget->ServerDeath();
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

// Play when PlayerCollision2 is overlapped
void ABaseCharacter::PlayerCollision2Begin(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor != this) {
		Overlapping = true;
	}
}

// Play when PlayerCollision2 is ended overlapped
void ABaseCharacter::PlayerCollision2End(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor != this) {
		Overlapping = false;
		Colliding = false;
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

}

void ABaseCharacter::StopAnimations() {
	if (this->HasAuthority()) {
		StopAnimationsServer_Implementation();
	}
	else {
		this->GetMesh()->GetAnimInstance()->Montage_Stop(0.0f, NULL);
	}
}

void ABaseCharacter::StopAnimationsServer_Implementation() {
	this->GetMesh()->GetAnimInstance()->Montage_Stop(0.0f, NULL);
}

/* Please only put helper functions here (functions called by a main function) */
void ABaseCharacter::FillHitboxArray() {
	// Store the hurtboxes inside the hitbox component array
	HitboxArray[0] = WeaponHurtboxBase->GetSocketLocation("");
	HitboxArray[1] = Hurtbox1->GetSocketLocation("");
	HitboxArray[2] = Hurtbox2->GetSocketLocation("");
	HitboxArray[3] = Hurtbox3->GetSocketLocation("");
	HitboxArray[4] = Hurtbox4->GetSocketLocation("");
	HitboxArray[5] = Hurtbox5->GetSocketLocation("");
}

/* Forwards function to server */
void ABaseCharacter::PlayActionAnim(UAnimMontage* Animation, float Speed) {
	if (this->HasAuthority()) {
		PlayActionAnimServer(Animation, Speed);
	}
}

/* Plays animations from server */
void ABaseCharacter::PlayActionAnimServer_Implementation(UAnimMontage* Animation, float Speed) {
	if (this->GetMesh()->GetAnimInstance() != NULL)
	{
		this->GetMesh()->GetAnimInstance()->Montage_Play(Animation, Speed);
	}
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
	PlayActionAnimServer(Animation, Speed);
}

/* Resilience drain and regen */
void ABaseCharacter::ResilienceRegenTimer() {
	if (Resilience < 100) {
		Resilience = UKismetMathLibrary::FClamp(Resilience + ResilienceRegenAmt, 0, 100);
	}
}
void ABaseCharacter::ResilienceDrainTimer() {
	if (Resilience >= 4) {
		Resilience = UKismetMathLibrary::FClamp(Resilience - ResilienceDrainAmt, 0, 100);
	}
}

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
		CanMove = false;
		Flinched = true;
		FName FlinchAnimPath = TEXT("/Game/Classes/Revenant/Animations/Recoil/Flinch_Montage.Flinch_Montage");
		UAnimMontage *FlinchAnimMontage = Cast<UAnimMontage>(StaticLoadObject(UAnimMontage::StaticClass(), NULL, *FlinchAnimPath.ToString()));
		PlayActionAnim(FlinchAnimMontage, 1.0f);
		GetWorldTimerManager().SetTimer(delayTimerHandle, this, &ABaseCharacter::FlinchEvent2, 1.1f, false);
	}
}
void ABaseCharacter::FlinchEvent2() {
	if (IsRolling == false) {
		CanMove = true;
	}
	if (Flinched == true) {
		Flinched = false;
	}
}

//Bleed Event Handler
void ABaseCharacter::BleedEvent() {
	if (!Invincible)
	{
		Health = Health - 3;
		if ((rand() % 101) < 5)//RandRange(0, 100) OR RandomIntegerInRange(0, 100) neither work
			FlinchTrigger = true;
		//if (Health <= 0) //There's an error during compiling if an if statement doesn't have anything to execute if conditions are met
			//this.ServerDeath(); //Currently only implemented in DeathHandler branch with bugs that need fixing
	}
}

/* **************************** Button Presses **************************** */
/* Block */
void ABaseCharacter::BlockPressedEventClient() {
	CanDamage = false;
	Resilience -= 4;
	BlockPressed = true;
	if (LastAttack != "Block") {
		MakeCurrentActionLastAction("Block");
	}
}
void ABaseCharacter::BlockReleasedEventClient() {
	BlockPressed = false;
	IsBlocking = false;
}
/* Roll */
void ABaseCharacter::RollPressedEventClient() {
	Resilience -= 25;
	IsRolling = true;
	MakeCurrentActionLastAction("Roll");
	FlinchTrigger = false;
	Flinched = false;
	IsBlocking = false;
	Invincible = true;
	CanDamage = false;
	if (ResilienceRegenTimerHandle.IsValid() == true) {
		GetWorld()->GetTimerManager().ClearTimer(ResilienceRegenTimerHandle);
	}
	if (ResilienceDrainTimerHandle.IsValid() == true) {
		GetWorld()->GetTimerManager().ClearTimer(ResilienceDrainTimerHandle);
	}
	GetWorldTimerManager().SetTimer(delayTimerHandle, this, &ABaseCharacter::RollPressedEventClient2, 0.745f, false);
}
void ABaseCharacter::RollPressedEventClient2() {
	IsRolling = false;
	GetWorldTimerManager().SetTimer(delayTimerHandle, this, &ABaseCharacter::RollPressedEventClient3, 0.1f, false);
}
void ABaseCharacter::RollPressedEventClient3() {
	Invincible = false;
	if (ResilienceRegenTimerHandle.IsValid() == false) {
		GetWorldTimerManager().SetTimer(ResilienceRegenTimerHandle, this, &ABaseCharacter::ResilienceRegenTimer, 1.0f, true);
	}
	if (ResilienceDrainTimerHandle.IsValid() == false) {
		GetWorldTimerManager().SetTimer(ResilienceDrainTimerHandle, this, &ABaseCharacter::ResilienceDrainTimer, 1.0f, true);
	}
}