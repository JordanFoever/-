// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Types/ItemsSysType.h"
#include "GameUIDataProvider.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGameUIDataProvider : public UInterface
{
	GENERATED_BODY()
};

class EASYSURVIVALRPGV2_5_API IGameUIDataProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		int32 GetPlayerInventoryCapacity();

	/*UFUNCTION(BlueprintNativeEvent,BlueprintCallable) 
		int32 GetLootableChestInventory(); */
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		FInventoryItemData GetItemData(int32 Index);    // 获取背包中某一项数据

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		FInventoryItemData GetHotBarItemData(int32 Index); // 得到快捷栏背包里面的道具信息

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		TArray<FName> GetCraftingList(EItemCategory& Category);  

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		int32 GetCraftingListQueueLength();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		bool GetProcessingCraftingData(int32 Index, FName& CraftingRowName, int32& Amount);

};
