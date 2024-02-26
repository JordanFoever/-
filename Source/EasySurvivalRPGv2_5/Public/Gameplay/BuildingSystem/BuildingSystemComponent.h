#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/BuildingSysTypes.h"

#include "BuildingSystemComponent.generated.h"

// �򿪽�����彨��ʱչʾ�ľ������ʾ��Ϣ 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnShowBuildingWarnAndTipInfo, FName, InfoDescription,bool,IsTip,bool,IsShowCanBuilding = true);
// ���Խ���ʱ,��ʾ���ĸ������н������ʾ����
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
	/// �ж�����ý��첿���ĵ����Ƿ��㹻����һ���ػ���һ��5��ľͷ��ɣ���ʱ��Ҫ�жϱ����е�ľͷ�������Ƿ��㹻
	/// </summary>
	bool CheckBuildingItemIsEnough();
	void UpdateInventoryBuildingItem();

	// δѡ��״̬
	int32 CurrentBuildingPartIndex = -1;
	bool bBuildingStarted = false;

	// ľ�ʵĽ��첿��
	TArray<FName> BuildingRowNameArr;

	// ʯͷ���첿��
	TArray<FName> StoneBuildingRowNameArr;

	// �����пɷ��õ�����
	TArray<FName> PlaceBuildingRowNameArr;

	ABuildingPartActor* BuildingPartActor;

	// ��ǰ���첿���� RowName
	FName CurrentBuildingPartRowName;

protected:
	// ����˵�����
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<ECircleBuildingMenuType> CurrentOpenCircleMenuType;
		
	UPROPERTY(EditAnywhere)
		TEnumAsByte<ETraceTypeQuery> BuildingTraceChannel;
	// �赲���
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EObjectTypeQuery> BlockPlayerChannel;
	// �赲����
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EObjectTypeQuery> BlockFloorChannel;
	// ���Խ���
	UPROPERTY(EditAnywhere)
		TEnumAsByte<ETraceTypeQuery> InteractionChannel;
	// �����������ײ����
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EObjectTypeQuery> BuildingPartObjectType;
	// �Ծ�̬�������ص�
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EObjectTypeQuery> OverlayWorldStaticChannel;
	// �����綯̬����ص�
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EObjectTypeQuery> OverlayWorldDynamicChannel;
	// ���� Pawn ���ص�
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EObjectTypeQuery> OverlayWorldPawnChannel;

	UPROPERTY(Meta = (SaveToJson = True)) 
		TArray<FPlacementBuildingInfo> PlaceWoodBuildingArray;

	// �Ƿ񱣴��Ѿ�����Ĳ����� Json �ļ���
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
	// ȡ������
	UFUNCTION(BlueprintCallable)
		void CancelBuilding();

	UFUNCTION(BlueprintCallable)
		void RotateBuildingPart(float AxisValue);
	
	// Ҫ����ʲô����
	UFUNCTION(BlueprintCallable)
		void SetBuildingPartIndex(int32 Index);
	UFUNCTION(BlueprintCallable)
		void PlaceBuildingPart();
	UFUNCTION(BlueprintCallable)
		void ToggleBuildingPart(int32 Index);
	
	// �Ƿ���Կ�ʼ����
	UFUNCTION(BlueprintCallable,BlueprintPure)
		bool CanStartBuild();

	// ���潨�����ݵ� Json �ļ���
	UFUNCTION(BlueprintCallable)
		void SaveDataToJsonFile();

	UFUNCTION(BlueprintImplementableEvent)
		TSubclassOf<class ABuildingPartActor> GetBuildingPartClass(EBuildingPartType Type);
};