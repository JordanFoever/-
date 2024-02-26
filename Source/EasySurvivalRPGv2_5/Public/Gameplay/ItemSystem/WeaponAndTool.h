// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/Interfaces/OperativeItem.h"
#include "Types/ItemsSysType.h"

#include "WeaponAndTool.generated.h"

UCLASS()
class EASYSURVIVALRPGV2_5_API AWeaponAndTool : public AActor,public IOperativeItem
{
	GENERATED_BODY()
public:	
	
	// Sets default values for this actor's properties
	AWeaponAndTool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MainActionOperative_Implementation(USkeletalMeshComponent* OperatingActorMesh) override;  
	bool CanBeOperated_Implementation() override { return !bIsOperating; }                  
	void EndOperating_Implementation() override;

	void UpdateDamageTracing();           // 更新检测
	TArray<FVector> GetTracingLocations(); // 获取坐标

	UStaticMeshComponent* Mesh;
	USkeletalMeshComponent* CharacterMesh;
	FWeaponAndToolTableRow* WeaponDataPtr; // 保存查表的得出的数据，避免重复查表
	bool bDamageTracing = false;
	TArray<FVector> TracingLocations;       // 保存用来当上一次的位置
	TSet<AActor*> TraceActors;

	UPROPERTY(BlueprintReadWrite)
		bool bIsOperating = false;
	UPROPERTY(EditAnywhere)
		FName MontageSlotName;                        // 蒙太奇槽的名字
	UPROPERTY(EditAnywhere)
		FName CharacterWeaponSocketName;              // 角色身上的武器的插槽的名字
	UPROPERTY(EditAnywhere)
		TEnumAsByte<ETraceTypeQuery> TraceChannel;                 // 挥武器检测通道    
	UPROPERTY(EditAnywhere,meta=(ClampMin=2))
		int32 TraceStepNumber = 5;                    // 武器检测分成多少段
	UPROPERTY(EditAnywhere)
		bool bDebugTrace = false;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Initiate(UStaticMesh* MeshAsset, FName WeaponToolRowName);
	FORCEINLINE FWeaponAndToolTableRow GetWeaponData() { return WeaponDataPtr ? *WeaponDataPtr : FWeaponAndToolTableRow(); }

	UFUNCTION(BlueprintCallable)
		void StartDamageTracing(); // 由动画事件触发
	UFUNCTION(BlueprintCallable)
		void EndDamageTracing();   // 由动画事件触发
};
