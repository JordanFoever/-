// Easy Systems


#include "Gameplay/PirateCharacterBase.h"

// Sets default values
APirateCharacterBase::APirateCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APirateCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APirateCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APirateCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
