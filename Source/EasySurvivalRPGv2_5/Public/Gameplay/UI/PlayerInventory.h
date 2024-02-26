// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInventory.generated.h"

/**
 * ���������û������,֮ǰ������������Ϊ����ί�У�����ί���Ѿ�ʹ��ί������ķ�ʽ������Ҫ�����
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
