#pragma once

#include "BaseCharacter.h"
#include "Revenant.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALCOMBAT_API ARevenant : public ABaseCharacter
{
	GENERATED_BODY()

	/** Shield Mesh object */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Shield;
	
public:
	// Sets default values for this character's properties
	ARevenant();
	
};
