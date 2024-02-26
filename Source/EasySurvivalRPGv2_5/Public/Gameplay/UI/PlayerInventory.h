// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInventory.generated.h"

/**
 * 这个类现在没有用了,之前创建这个类就是为调用委托，现在委托已经使用委托组件的方式，不需要这个了
 */
UCLASS()
class EASYSURVIVALRPGV2_5_API UPlayerInventory : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
		void InventorySwapItemSlot(int32 FromIndex,int32 ToIndex);
	UFUNCTION(BlueprintCallable)
		void InventorySplitItems(int32 FromIndex,int32 ToIndex,int32 SplitCount);
};
