// Easy Systems


#include "Gameplay/UI/PlayerInventory.h"
#include "Global/GlobalDelegateManager.h"

void UPlayerInventory::InventorySwapItemSlot(int32 FromIndex, int32 ToIndex) 
{
	GlobalDelegateManager::Get()->CallGenericDelegate<void, int, int>(PLAYERINVENTORY_SWAP_SLOTS, FromIndex, ToIndex);
}

void UPlayerInventory::InventorySplitItems(int32 FromIndex, int32 ToIndex, int32 SplitCount)
{
	GlobalDelegateManager::Get()->CallGenericDelegate<void, int, int, int>(PLAYERINVENTORY_SPLIT_ITEMS, FromIndex, ToIndex, SplitCount);
}
