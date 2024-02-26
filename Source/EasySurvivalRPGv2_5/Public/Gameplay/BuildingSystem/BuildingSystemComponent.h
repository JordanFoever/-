#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/BuildingSysTypes.h"

#include "BuildingSystemComponent.generated.h"

// 打开建造面板建造时展示的警告和提示信息 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnShowBuildingWarnAndTipInfo, FName, InfoDescription,bool,IsTip,bool,IsShowCanBuilding = true);
// 可以建造时,显示按哪个键进行建造的提示文字
class ABuildingPartActor;
UCLASS(Blueprintable,ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EASYSURVIVALRPGV2_5_API UBuildingSystemComponent : public UActorComponent
{
	GENERATED_BODY()
		
public:	  
	// Sets default values for this component's properties
	UBuildingSystemComponent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void UpdateBuildingTrace();
	void SavePlacementBuildingInfoToArray();
	void InitBuildingRowNameArr();
	void SetBuildingActorCollision();
	void LoadDataFromJsonFile();
	void CheckBuildingPartType(FName RowName);
	void DestoryBuildingPart(ABuildingPartActor*  DestoryBuildingPartActor);
	
	/// <summary>
	/// 判断组件该建造部件的道具是否足够。如一个地基由一个5个木头组成，此时需要判断背包中的木头的数量是否足够
	/// </summary>
	bool CheckBuildingItemIsEnough();
	void UpdateInventoryBuildingItem();

	// 未选中状态
	int32 CurrentBuildingPartIndex = -1;
	bool bBuildingStarted = false;

	// 木质的建造部件
	TArray<FName> BuildingRowNameArr;

	// 石头建造部件
	TArray<FName> StoneBuildingRowNameArr;

	// 建造中可放置的物体
	TArray<FName> PlaceBuildingRowNameArr;

	ABuildingPartActor* BuildingPartActor;

	// 当前建造部件的 RowName
	FName CurrentBuildingPartRowName;

protected:
	// 建造菜单类型
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<ECircleBuildingMenuType> CurrentOpenCircleMenuType;
		
	UPROPERTY(EditAnywhere)
		TEnumAsByte<ETraceTypeQuery> BuildingTraceChannel;
	// 阻挡玩家
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EObjectTypeQuery> BlockPlayerChannel;
	// 阻挡地面
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EObjectTypeQuery> BlockFloorChannel;
	// 可以交互
	UPROPERTY(EditAnywhere)
		TEnumAsByte<ETraceTypeQuery> InteractionChannel;
	// 建造物体的碰撞类型
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EObjectTypeQuery> BuildingPartObjectType;
	// 对静态物体检测重叠
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EObjectTypeQuery> OverlayWorldStaticChannel;
	// 对世界动态检测重叠
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EObjectTypeQuery> OverlayWorldDynamicChannel;
	// 检测对 Pawn 的重叠
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EObjectTypeQuery> OverlayWorldPawnChannel;

	UPROPERTY(Meta = (SaveToJson = True)) 
		TArray<FPlacementBuildingInfo> PlaceWoodBuildingArray;

	// 是否保存已经建造的部件到 Json 文件中
	UPROPERTY(EditAnywhere)
		bool bSavePlacementBuildingToJson = true;
	UPROPERTY(EditAnywhere)
		FString RelativeSavePath = "";
	UPROPERTY(EditAnywhere)
		FString JsonFileName = "SaveBuilding.json";

	UPROPERTY(EditAnywhere)
		float BuilingTraceRange = 500.f;

	UPROPERTY(EditAnywhere)
		bool bShouldDebug = false;

	UPROPERTY(EditAnywhere)
		TSubclassOf<ABuildingPartActor> BuildingPartActorClass;

public:
	UPROPERTY(BlueprintAssignable)
		FOnShowBuildingWarnAndTipInfo OnShowBuildingWarnAndTipInfo;

	UFUNCTION(BlueprintCallable,BlueprintPure)
		ABuildingPartActor* GetBuildingPartActor();
	// 取消建造
	UFUNCTION(BlueprintCallable)
		void CancelBuilding();

	UFUNCTION(BlueprintCallable)
		void RotateBuildingPart(float AxisValue);
	
	// 要建造什么东西
	UFUNCTION(BlueprintCallable)
		void SetBuildingPartIndex(int32 Index);
	UFUNCTION(BlueprintCallable)
		void PlaceBuildingPart();
	UFUNCTION(BlueprintCallable)
		void ToggleBuildingPart(int32 Index);
	
	// 是否可以开始建造
	UFUNCTION(BlueprintCallable,BlueprintPure)
		bool CanStartBuild();

	// 保存建造数据到 Json 文件中
	UFUNCTION(BlueprintCallable)
		void SaveDataToJsonFile();

	UFUNCTION(BlueprintImplementableEvent)
		TSubclassOf<class ABuildingPartActor> GetBuildingPartClass(EBuildingPartType Type);
};