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

	// ���½������ �ӵ��ɱۿ�ʼ���
	void UpdateInteractiveTrace();

	/// <summary>
	/// ������������
	/// </summary>
	/// <param name="Actor"></param>
	/// <param name="bShowOutLine">�Ƿ�չʾ���</param>
	void SetActorOutLine(AActor* Actor, bool bShowOutLine);
	
protected:
	
	UPROPERTY(EditDefaultsOnly)
		TEnumAsByte<ETraceTypeQuery> InteractiveTraceChannal; // ���ͨ��
	
	UPROPERTY(EditDefaultsOnly)
		float InteractiveTraceRange = 180.f;                 // ��ⷶΧ
public:
	UPROPERTY(BlueprintReadOnly)
		// �Ѿ����������� Actor
		AActor* InteractiveActor;

};
