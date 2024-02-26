// Easy Systems


#include "Gameplay/ItemSystem/LootableContainer.h"

// Sets default values
ALootableContainer::ALootableContainer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALootableContainer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALootableContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}