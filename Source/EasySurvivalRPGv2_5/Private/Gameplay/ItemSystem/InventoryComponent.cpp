// Easy Systems


#include "Gameplay/ItemSystem/InventoryComponent.h"
#include "Utils/CommonUtilLibrary.h"
#include "Global/GlobalDelegateManager.h"
#include "JsonUtilities/Public/JsonUtilities.h"
#include "PirateGameInstance.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UInventoryComponent::GetItemData(int32 Index, FInventoryItemData& OutData)
{
	OutData = FInventoryItemData();
	if(!InventoryItems.IsValidIndex(Index)) return false;
	OutData = InventoryItems[Index];
	if (OutData.Count == 0)return false;
	return true;
}

bool UInventoryComponent::IsItemEnouph(FName ItemRowName, int32 Amount)
{
	int32 HasAmount = 0;
	
	for (auto& Item : InventoryItems)
	{
		if (Item.TableRowName == ItemRowName)
			HasAmount += Item.Count;
	}
	
	// 够用
	return HasAmount >= Amount;
}
	
int32 UInventoryComponent::AddItemToInventory(FName ItemRowName, int32 Count, int32 FromStackIndex, int32 FromEmptyIndex,bool bAddTest)
{
	auto ItemTableData = UCommonUtilLibrary::GetTableRow<FItemDataTableRow>(TEXT("ItemTable"), ItemRowName);
	if (!ItemTableData) return Count;    // 添加的道具不存在直接返回,这种情况基本上不会出现,为了严谨而写

	// 添加道具需要满足下面的条件
	FInventoryItemData OutData;
	bool IsHasItem = false;
	int IndexToPut = -1;

	// 搜索可以堆叠的格子
	for (int i = FromStackIndex; i < Capacity && FromStackIndex < Capacity; i++)
	{ 
		IsHasItem = GetItemData(i, OutData);      // 是否有道具
		FromStackIndex = i;

		// 找到相同的道具,尝试堆叠
		if (IsHasItem && OutData.TableRowName == ItemRowName) 
		{
			IndexToPut = i;
			break;
		}
	}

	// 搜索空的格子
	for (int i = FromEmptyIndex; i < Capacity && IndexToPut < 0; i++)
	{
		IsHasItem = GetItemData(i, OutData);      // 是否有道具
		FromEmptyIndex = i;
		if (!IsHasItem)
			IndexToPut = i;
	}

	if (IndexToPut < 0) return Count;              // 即没有找到的空的格子也没有找到可以堆叠的

	auto InventoryCount = OutData.Count;                               // 背包中现有的道具的数量
	auto MaxCountCanPut = ItemTableData->MaxStack - InventoryCount;    // 这个格子最多能放多少
	auto LeftCount = Count - MaxCountCanPut;                           // 还剩多少放不进去

	if (MaxCountCanPut > 0)
	{
		if (LeftCount < 0) LeftCount = 0;

		OutData.Count += Count - LeftCount;
		OutData.TableRowName = ItemRowName;
		if(!bAddTest) ConfirmInventorySlotChange(IndexToPut, OutData);
	}
	else
		FromStackIndex++;


	// 递归调用
	if (LeftCount > 0)
		return AddItemToInventory(ItemRowName, LeftCount, FromStackIndex, FromEmptyIndex);

	return 0;
}

int32 UInventoryComponent::InternalRemoveItemFromInventory(int32 Index,int32 Count)
{
	FInventoryItemData Data;

	GetItemData(Index, Data);

	if (!Data.IsEmpty())
	{
		// 计算当此移除操作不可移除的数量
		auto AmountCanRemove = Count - Data.Count; 
		// 如果不可移除数量为负,则视为没有不可移除数量
		if (AmountCanRemove < 0)AmountCanRemove = 0;
		// 实际移除数量为待移除数量减不可移除的数量
		Data.Count -= Count - AmountCanRemove;
		ConfirmInventorySlotChange(Index, Data);	
		return AmountCanRemove; 
	}

	return Count;
}

int32 UInventoryComponent::RemoveIndexFromInventory(int32 Index, int32 Count)
{
	return InternalRemoveItemFromInventory(Index, Count);
}

int32 UInventoryComponent::RemoveItemFromInventory(FName ItemRowName, int32 Count)
{
	for (int32 i = 0; i < InventoryItems.Num(); i++)
	{
		if (InventoryItems[i].TableRowName == ItemRowName && InventoryItems[i].Count > 0)
		{
			Count = InternalRemoveItemFromInventory(i, Count);
			if(Count == 0) break;
		}
	}
	
	return Count;
}

void UInventoryComponent::SwapItems(int32 FromIndex, int32 ToIndex)
{
	// 把一个格子的道具放到另外的一个格子里面去
	// 可能性 1.放到一个空的格子里面去 2.两个格子的道具是同一种 3.两个格子的道具不一样
	// note=> 解决方式:要么分情况讨论,要么将所有的可能性都统一实现

	check(FromIndex >= 0 && FromIndex < Capacity && ToIndex >= 0 && ToIndex < Capacity);

	FInventoryItemData FromItemData, ToItemData;
	GetItemData(FromIndex, FromItemData);
	GetItemData(ToIndex, ToItemData);

	if (FromItemData.IsEmpty()) return;

	if (ToItemData.TableRowName != FromItemData.TableRowName) // 不一样的道具,两个空的也是不相同的
	{
		
		ConfirmInventorySlotChange(ToIndex, FromItemData);
		ConfirmInventorySlotChange(FromIndex, ToItemData);
	}
	else // 一样的道具
	{
		if (FromIndex == ToIndex) return;      // 避免重复放回自己的位置
		auto ItemTableData = UCommonUtilLibrary::GetTableRow<FItemDataTableRow>(TEXT("ItemTable"), FromItemData.TableRowName);
		if (!ItemTableData) return;

		auto FromLeft = FromItemData.Count + ToItemData.Count - ItemTableData->MaxStack;
		if (FromLeft < 0) FromLeft = 0;
		ToItemData.Count += FromItemData.Count - FromLeft;
		FromItemData.Count = FromLeft;

		ConfirmInventorySlotChange(FromIndex, FromItemData);
		ConfirmInventorySlotChange(ToIndex, ToItemData);
	}
}

void UInventoryComponent::SplitItems(int32 FromIndex, int32 ToIndex, int SplitCount)
{
	check(FromIndex >= 0 && FromIndex < Capacity&& ToIndex >= 0 && ToIndex < Capacity);

	FInventoryItemData FromItemData, ToItemData;
	GetItemData(FromIndex, FromItemData);
	GetItemData(ToIndex, ToItemData);

	//  必选是空的才可以分割
	if (!ToItemData.IsEmpty() || FromItemData.Count < SplitCount) return;

	FromItemData.Count -= SplitCount;
	ToItemData.TableRowName = FromItemData.TableRowName;
	ToItemData.Count += SplitCount;

	ConfirmInventorySlotChange(FromIndex, FromItemData);
	ConfirmInventorySlotChange(ToIndex, ToItemData);

}

void UInventoryComponent::PostInventoryInitiate()
{
	UPirateGameInstance* PirateGameInstance = Cast<UPirateGameInstance>(GetWorld()->GetGameInstance());
	// 从 json 文件中读取数据
	if (bSaveInventoryToJson && PirateGameInstance->GetClickContinueGame())
	{
		/*auto Path = FPaths::ProjectContentDir() + RelativeSavePath + JsonFileName;

		FString JsonStr;
		if (FFileHelper::LoadFileToString(JsonStr, *Path)) 
		{
			auto Reader = TJsonReaderFactory<>::Create(JsonStr);
			TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject);

			if (FJsonSerializer::Deserialize(Reader, JsonObj)) 
			{
				auto ItemJsonValueArr = JsonObj->GetArrayField(TEXT("Items"));

				for (auto& ItemJsonValue : ItemJsonValueArr)
				{
					if (auto ItemJsonObjValue = ItemJsonValue->AsObject()) 
					{
						FName ItemRowName = FName(ItemJsonObjValue->GetStringField(TEXT("ItemRowName")));
						int32 Count = FMath::RoundToInt(ItemJsonObjValue->GetNumberField(TEXT("Count")));

						InventoryItems.Add(FInventoryItemData(ItemRowName, Count));
					}
				}
			}*/

		/*auto Reader = TJsonReaderFactory<>::Create(JsonStr);
			EJsonNotation Notation;
			Reader->ReadNext(Notation);
			Reader->ReadNext(Notation);
			while (true)
			{
				Reader->ReadNext(Notation);
				if(Notation == EJsonNotation::ArrayEnd) break;
				Reader->ReadNext(Notation);
				auto RowName = Reader->GetValueAsString();
				Reader->ReadNext(Notation);
				auto Count = Reader->GetValueAsNumber();
				Reader->ReadNext(Notation);

				InventoryItems.Add(FInventoryItemData(FName(RowName), Count));
			}
			}*/
		auto Path = FPaths::ProjectContentDir() + RelativeSavePath + JsonFileName;
		UCommonUtilLibrary::LoadObjectPropertiesFromJsonFile(this, *Path);
	}


	for (int i = 0; i < Capacity; i++)
	{
		FInventoryItemData Data;
		GetItemData(i, Data);
		ConfirmInventorySlotChange(i, Data);
	}
}

void UInventoryComponent::ClearAll()
{
	auto Path = FPaths::ProjectContentDir() + RelativeSavePath + JsonFileName;
	UCommonUtilLibrary::ClearFile(*Path);
}

void UInventoryComponent::SaveDataToJsonFile()
{
	auto Path = FPaths::ProjectContentDir() + RelativeSavePath + JsonFileName;
	UCommonUtilLibrary::SaveObjectPropertiesToJsonFile(this, *Path);	
}

void UInventoryComponent::ConfirmInventorySlotChange(int Index, FInventoryItemData NewData)
{
	if (Index < 0 && Index >= Capacity)return; // 越界了直接返回

	if (!InventoryItems.IsValidIndex(Index)) InventoryItems.SetNum(Index + 1);
	InventoryItems[Index] = NewData;

	OnInventoryUpdated.Broadcast(Index);

}
/*if (PirateGameInstance->GetIsSaveDataToJsonFile())
	{
		auto Path = FPaths::ProjectContentDir() + RelativeSavePath + JsonFileName;
		UCommonUtilLibrary::SaveObjectPropertiesToJsonFile(this, *Path);
	}*/
	// 通知UI更新
	//GlobalDelegateManager::Get()->CallGenericDelegate<void, int>(PLAYERINVENTORY_UPDATE_SLOT, Index);
	// 将之前这个旧的调用委托的方式换成新的
	// 多波委托的调用