// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/PirateCharacterBase.h"
#include "Gameplay/Interfaces/InteractiveObject.h"
#include "TalkableNPC.generated.h"

// 可谈话的 NPC
UCLASS()
class EASYSURVIVALRPGV2_5_API ATalkableNPC : public ACharacter,public IInteractiveObject
{
	GENERATED_BODY()
	
public:
	ATalkableNPC(); 

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	/*void BeginInteractive_Implementation(APlayerController* Controller) override;
	void EndInteractive_Implementation(APlayerController* Controller) override;
	void InteractiveResponse_Implementation(APlayerController* Controller) override;
	bool GetInteractiveInformation_Implementation(FString& Title, FString& Summary, FString& Tip) override;*/
	
};
