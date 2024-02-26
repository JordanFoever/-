// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/ItemsSysType.h"
#include "PirateGameInstance.h"
#include "InventoryComponent.generated.h"


// note=> 只有动态委托才可以使用 UPROPERTY 的标记，只有动态的多波委托才可以在蓝图里面进行绑定
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryUpdateDelegate, int32, Index);  // 动态的委托虽然只有一个参数但是类型和标识符都要声明出来

UCLASS( Blueprintable,ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EASYSURVIVALRPGV2_5_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
		
public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override; 

	UPROPERTY(EditAnywhere,meta= (ClampMin=1) )
		int32 Capacity = 18;                     // 容量

	UPROPERTY(EditAnywhere)
		bool bSaveInventoryToJson = true;       // 是否将背包中的道具保存到 Json 文件中
	
	UPROPERTY(EditAnywhere)
		FString RelativeSavePath = "";

	UPROPERTY(EditAnywhere)
		FString JsonFileName = "Save.json";
	
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure)
		FORCEINLINE int32 GetCapacity() { return Capacity; }
	UFUNCTION(BlueprintPure)
		bool GetItemData(int32 Index, FInventoryItemData& OutData);    //  返回值用于判断是否超出范围  
	/*
	* 确定数量的道具是否足够
	* @ItemRowName 道具的名字
	* @Amount 看道具够不够
	*/
	UFUNCTION(BlueprintPure)
		bool IsItemEnouph(FName ItemRowName,int32 Amount);
	UFUNCTION(BlueprintCallable)
		/// <summary>
		/// 向背包中添加道具
		/// </summary>
		/// <param name="ItemRowName">道具行的名字</param>
		/// <param name="Count">捡起的道具的数量</param>
		/// <param name="FromStackIndex">堆叠的索引</param>
		/// <param name="FromEmptyIndex">空的索引</param>
		/// <param name="">是否真的往里面添加道具</param>
		/// <returns>将捡起来的道具添加后剩余具后剩余的数量</returns>
		int32 AddItemToInventory(FName ItemRowName, int32 Count, int32 FromStackIndex = 0, int32 FromEmptyIndex = 0,bool bAddTest = false);
	
	/*
	* @Return int32 从背包中移除后还剩多少道具 
	*/
	UFUNCTION(BlueprintCallable)
		int32 RemoveIndexFromInventory(int32 Index,int32 Count);		// 移除某个特定索引处的道具  移除多少个道具

	UFUNCTION(BlueprintCallable)
		int32 RemoveItemFromInventory(FName ItemRowName,int32 Count);
	UFUNCTION(BlueprintCallable)
		void SwapItems(int32 FromIndex,int32 ToIndex);					// 交换道具
	UFUNCTION(BlueprintCallable)
		void SplitItems(int32 FromIndex,int32 ToIndex,int SplitCount);   // 从哪个格子分割到哪个格子以及分割的数量
	UFUNCTION(BlueprintCallable)
		void PostInventoryInitiate();                    
	UFUNCTION(BlueprintCallable)
		void ClearAll();
	UFUNCTION(BlueprintCallable)
		void SaveDataToJsonFile();

	UPROPERTY(BlueprintAssignable)										// BlueprintAssignable  这个标识符表示蓝图可以绑定
		FInventoryUpdateDelegate OnInventoryUpdated; 
	
private:
	// meta = (SavaToJson) 此时的名字是自定义的
	UPROPERTY(Meta = (SaveToJson = True))
		TArray<FInventoryItemData> InventoryItems;					// 背包,保存背包中的每一项数据
	
	void ConfirmInventorySlotChange(int Index, FInventoryItemData NewData);
	int32 InternalRemoveItemFromInventory(int32 Index, int32 Count); 
};
