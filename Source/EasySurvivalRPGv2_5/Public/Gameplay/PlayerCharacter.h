// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gameplay/PirateCharacterBase.h"
#include "Gameplay/Interfaces/AssociatedComponentGetter.h"
#include "Gameplay/Interfaces/CameraDataGetter.h"
#include "Global/GlobalDelegateManager.h"
#include "Types/ItemsSysType.h"

#include "PlayerCharacter.generated.h"

UCLASS()
class EASYSURVIVALRPGV2_5_API APlayerCharacter : public APirateCharacterBase,public IAssociatedComponentGetter,public ICameraDataGetter
{
	GENERATED_BODY()
		
		
public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
		USkeletalMeshComponent* HeadMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USkeletalMeshComponent* BodyMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USkeletalMeshComponent* HandsMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USkeletalMeshComponent* LegsMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USkeletalMeshComponent* FeetMesh;

	UPROPERTY(EditAnywhere)
		FGameplayTag SwapItemEventTag;   // �������ߵ�ί�б�ǩ
	UPROPERTY(EditAnywhere)
		FGameplayTag SplitItemEventTag;  // �ֶѵ��ߵ�ί�б�ǩ
	UPROPERTY(EditAnywhere)
		float SpringSpeed = 600.f;         
	UPROPERTY(EditAnywhere)
		float RunSpeed = 450.f;
	UPROPERTY(EditAnywhere)
		FName WeaponSocketName = NAME_None;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AWeaponAndTool> WeaponToolClass;
	UPROPERTY(BlueprintReadOnly)
		UObject* HotBarItem;

	
	class AWeaponAndTool* HoldingWeapon;
	bool bFastMoving = false;             // �Ƿ�����ƶ�
	int32 HotBarIndex = -1;               // ��ݵ�����������

	//void PlayerSwapItems(int32 FromIndex,int32 ToIndex);                      // ��������
	//void PlayerSplitItems(int32 FromIndex, int32 ToIndex, int32 SplitCount);  // ���߷ֶ�
	
	UFUNCTION()
		void PlayerSwapItems(UObject* Payload);
	UFUNCTION()
		void PlayerSplitItems(UObject* Payload);

	UFUNCTION(BlueprintImplementableEvent)
		void HotBarSelected(class UInventoryComponent* HotBarComp,int32 Index); // ��������ͨ�����ô���ͨ����ͼȥ����  

	void LoadSavedStyle();                      // �����ѱ������ʽ
	void HandleFastMovingSwitch();              // ��������ƶ����л�

	void HandleWeaponAndToolSelected(const FItemDataTableRow& ItemData, FName RowName);  // ѡ���ĸ���������װ��

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ����������ƶ�
	void LookUp(float Axis);
	void Turn(float Axis);
	void MoveForward(float Axis);
	void MoveRight(float Axis);
	void SpringOrFastSwim();
	void SelectHotBar(FKey Key);  // �����ѡ��
	void MainAction();       // ��ǰ�Ŀ�������治��ѡ����ʲô����,����������ʱʹ����ѡ����Ǹ�����

	UFUNCTION(BlueprintCallable)
		void EndOperatingItem();
};
