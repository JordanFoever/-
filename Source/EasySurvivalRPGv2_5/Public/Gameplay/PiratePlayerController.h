// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PiratePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class EASYSURVIVALRPGV2_5_API APiratePlayerController : public APlayerController
{
	GENERATED_BODY()
private:
	void Tick(float DeltaSeconds) override;
	void SetupInputComponent() override;
	void Interact();

	// 更新交互检测 从弹簧臂开始检测
	void UpdateInteractiveTrace();

	/// <summary>
	/// 对物体进行描边
	/// </summary>
	/// <param name="Actor"></param>
	/// <param name="bShowOutLine">是否展示描边</param>
	void SetActorOutLine(AActor* Actor, bool bShowOutLine);
	
protected:
	
	UPROPERTY(EditDefaultsOnly)
		TEnumAsByte<ETraceTypeQuery> InteractiveTraceChannal; // 检测通道
	
	UPROPERTY(EditDefaultsOnly)
		float InteractiveTraceRange = 180.f;                 // 检测范围
public:
	UPROPERTY(BlueprintReadOnly)
		// 已经被交互到的 Actor
		AActor* InteractiveActor;

};
