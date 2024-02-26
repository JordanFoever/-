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
		FGameplayTag SwapItemEventTag;   // 交换道具的委托标签
	UPROPERTY(EditAnywhere)
		FGameplayTag SplitItemEventTag;  // 分堆道具的委托标签
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
	bool bFastMoving = false;             // 是否快速移动
	int32 HotBarIndex = -1;               // 快捷道具栏的索引

	//void PlayerSwapItems(int32 FromIndex,int32 ToIndex);                      // 交互道具
	//void PlayerSplitItems(int32 FromIndex, int32 ToIndex, int32 SplitCount);  // 道具分堆
	
	UFUNCTION()
		void PlayerSwapItems(UObject* Payload);
	UFUNCTION()
		void PlayerSplitItems(UObject* Payload);

	UFUNCTION(BlueprintImplementableEvent)
		void HotBarSelected(class UInventoryComponent* HotBarComp,int32 Index); // 虚幻引擎会通过调用代理通过蓝图去调用  

	void LoadSavedStyle();                      // 加载已保存的样式
	void HandleFastMovingSwitch();              // 处理快速移动的切换

	void HandleWeaponAndToolSelected(const FItemDataTableRow& ItemData, FName RowName);  // 选中哪个武器或者装备

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 控制人物的移动
	void LookUp(float Axis);
	void Turn(float Axis);
	void MoveForward(float Axis);
	void MoveRight(float Axis);
	void SpringOrFastSwim();
	void SelectHotBar(FKey Key);  // 快捷栏选择
	void MainAction();       // 当前的快捷栏里面不管选的是什么道具,按鼠标左键都时使用我选择的那个道具

	UFUNCTION(BlueprintCallable)
		void EndOperatingItem();
};
