// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/Interfaces/InteractiveObject.h"
#include "Gameplay/ItemSystem/LootableItem.h"

#include "LootableContainer.generated.h"

// ¿ÉÊ°È¡µÄÈÝÆ÷ 
UCLASS()
class EASYSURVIVALRPGV2_5_API ALootableContainer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALootableContainer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
};
