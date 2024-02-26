// Easy Systems


#include "Gameplay/ItemSystem/CraftingComponent.h"
#include "Gameplay/Interfaces/AssociatedComponentGetter.h"
#include "Gameplay/ItemSystem/InventoryComponent.h"

#include "Utils/CommonUtilLibrary.h"

// Sets default values for this component's properties
UCraftingComponent::UCraftingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCraftingComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...	
}

void UCraftingComponent::SetCraftingCategory(EItemCategory Category)
{
	if (CurrentCategory == Category) return;
	CurrentCategory = Category;
	if (Category == EItemCategory::All) 
		CurrentCraftingList = CraftingInfoList;
	else
	{
		TArray<FName> NewList;
		for (auto& CraftingRowName : CraftingInfoList)
		{
			if (auto CraftingInfoPtr = UCommonUtilLibrary::GetTableRow<FCraftingItemInfo>(TEXT("CraftingTable"), CraftingRowName)) 
			{
				if (auto CraftingItemInfo = UCommonUtilLibrary::GetTableRow<FItemDataTableRow>(TEXT("ItemTable"), CraftingInfoPtr->CraftingItemRowName))
					if (Category == CraftingItemInfo->Category)
						NewList.Add(CraftingRowName);
			}
		}

		CurrentCraftingList = NewList;
	}

	OnCraftingListUpdated.Broadcast();
}

bool UCraftingComponent::GetProcessingData(int32 Index, FName& CraftingRowName, int32& Amount)
{
	if (ProcessingCraftingItems.IsValidIndex(Index)) 
	{
		auto Data = ProcessingCraftingItems[Index];
		CraftingRowName = Data.CraftingRowName;
		Amount = Data.Amount;
		return true;
	}
	else   
		return false;
}

void UCraftingComponent::PostInitiateList()
{
	SetCraftingCategory(EItemCategory::All);
}

void UCraftingComponent::SelectCraftingItem(int32 Index)
{
	CurrentCraftingIndex = Index;
	OnCraftingItemSelected.Broadcast(CurrentCraftingIndex);
}

void UCraftingComponent::StartCrafting(int Count)
{
	if (!CurrentCraftingList.IsValidIndex(CurrentCraftingIndex))return;

	UInventoryComponent* Inventory = 0; 
	if (GetOwner()->Implements<UAssociatedComponentGetter>())
		Inventory = IAssociatedComponentGetter::Execute_GetInventoryComponent(GetOwner());

	if (!IsValid(Inventory)) return;

	auto CraftingRowName = CurrentCraftingList[CurrentCraftingIndex];

	// Ŀǰ�Ȳ����ǵ������еĵ��߽���
	if (auto CraftingInfo = UCommonUtilLibrary::GetTableRow<FCraftingItemInfo>(TEXT("CraftingTable"), CraftingRowName)) 
	{
		bool bItemEnouph = true;

		for (auto& Request : CraftingInfo->RequestItemMap)
		{
			// ֻҪһ�����߲������ǲ���
			if (!Inventory->IsItemEnouph(Request.Key, Request.Value * Count))
			{
				bItemEnouph = false;
				return;
			}
		}
		
		for (auto& Request : CraftingInfo->RequestItemMap)
			Inventory->RemoveItemFromInventory(Request.Key, Request.Value * Count);

		// ������˲��������
		if (Inventory->AddItemToInventory(CraftingInfo->CraftingItemRowName, Count, 0, 0,true) > 0) return;


		// ��һ��ʱ�佨��
		// ����ʹ����������Ϊ�������캯����˽�л���
		auto& Manager = GetWorld()->GetTimerManager();
		FTimerHandle NewCraftingHandle;
		
		Manager.SetTimer(NewCraftingHandle, [=]()
			{
				Inventory->AddItemToInventory(CraftingInfo->CraftingItemRowName, Count);
				ProcessingCraftingItems.Remove(FCraftingItemData(NewCraftingHandle));
					
			}, CraftingInfo->CraftingCostTime * Count, false);
		
		ProcessingCraftingItems.Add(FCraftingItemData(CraftingRowName, Count, NewCraftingHandle));
		OnstartCrafting.Broadcast(); 
	}

}

// Called every frame
void UCraftingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}