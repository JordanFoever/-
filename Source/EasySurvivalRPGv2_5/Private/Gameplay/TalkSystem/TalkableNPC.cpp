// Easy Systems


#include "Gameplay/TalkSystem/TalkableNPC.h"

ATalkableNPC::ATalkableNPC() 
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATalkableNPC::BeginPlay()
{
	Super::BeginPlay();
}

void ATalkableNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
