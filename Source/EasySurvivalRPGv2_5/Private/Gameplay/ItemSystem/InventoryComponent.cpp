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
	
	// ����
	return HasAmount >= Amount;
}
	
int32 UInventoryComponent::AddItemToInventory(FName ItemRowName, int32 Count, int32 FromStackIndex, int32 FromEmptyIndex,bool bAddTest)
{
	auto ItemTableData = UCommonUtilLibrary::GetTableRow<FItemDataTableRow>(TEXT("ItemTable"), ItemRowName);
	if (!ItemTableData) return Count;    // ��ӵĵ��߲�����ֱ�ӷ���,������������ϲ������,Ϊ���Ͻ���д

	// ��ӵ�����Ҫ�������������
	FInventoryItemData OutData;
	bool IsHasItem = false;
	int IndexToPut = -1;

	// �������Զѵ��ĸ���
	for (int i = FromStackIndex; i < Capacity && FromStackIndex < Capacity; i++)
	{ 
		IsHasItem = GetItemData(i, OutData);      // �Ƿ��е���
		FromStackIndex = i;

		// �ҵ���ͬ�ĵ���,���Զѵ�
		if (IsHasItem && OutData.TableRowName == ItemRowName) 
		{
			IndexToPut = i;
			break;
		}
	}

	// �����յĸ���
	for (int i = FromEmptyIndex; i < Capacity && IndexToPut < 0; i++)
	{
		IsHasItem = GetItemData(i, OutData);      // �Ƿ��е���
		FromEmptyIndex = i;
		if (!IsHasItem)
			IndexToPut = i;
	}

	if (IndexToPut < 0) return Count;              // ��û���ҵ��Ŀյĸ���Ҳû���ҵ����Զѵ���

	auto InventoryCount = OutData.Count;                               // ���������еĵ��ߵ�����
	auto MaxCountCanPut = ItemTableData->MaxStack - InventoryCount;    // �����������ܷŶ���
	auto LeftCount = Count - MaxCountCanPut;                           // ��ʣ���ٷŲ���ȥ

	if (MaxCountCanPut > 0)
	{
		if (LeftCount < 0) LeftCount = 0;

		OutData.Count += Count - LeftCount;
		OutData.TableRowName = ItemRowName;
		if(!bAddTest) ConfirmInventorySlotChange(IndexToPut, OutData);
	}
	else
		FromStackIndex++;


	// �ݹ����
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
		// ���㵱���Ƴ����������Ƴ�������
		auto AmountCanRemove = Count - Data.Count; 
		// ��������Ƴ�����Ϊ��,����Ϊû�в����Ƴ�����
		if (AmountCanRemove < 0)AmountCanRemove = 0;
		// ʵ���Ƴ�����Ϊ���Ƴ������������Ƴ�������
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
	// ��һ�����ӵĵ��߷ŵ������һ����������ȥ
	// ������ 1.�ŵ�һ���յĸ�������ȥ 2.�������ӵĵ�����ͬһ�� 3.�������ӵĵ��߲�һ��
	// note=> �����ʽ:Ҫô���������,Ҫô�����еĿ����Զ�ͳһʵ��

	check(FromIndex >= 0 && FromIndex < Capacity && ToIndex >= 0 && ToIndex < Capacity);

	FInventoryItemData FromItemData, ToItemData;
	GetItemData(FromIndex, FromItemData);
	GetItemData(ToIndex, ToItemData);

	if (FromItemData.IsEmpty()) return;

	if (ToItemData.TableRowName != FromItemData.TableRowName) // ��һ���ĵ���,�����յ�Ҳ�ǲ���ͬ��
	{
		
		ConfirmInventorySlotChange(ToIndex, FromItemData);
		ConfirmInventorySlotChange(FromIndex, ToItemData);
	}
	else // һ���ĵ���
	{
		if (FromIndex == ToIndex) return;      // �����ظ��Ż��Լ���λ��
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

	//  ��ѡ�ǿյĲſ��Էָ�
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
	// �� json �ļ��ж�ȡ����
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
	if (Index < 0 && Index >= Capacity)return; // Խ����ֱ�ӷ���

	if (!InventoryItems.IsValidIndex(Index)) InventoryItems.SetNum(Index + 1);
	InventoryItems[Index] = NewData;

	OnInventoryUpdated.Broadcast(Index);

}
/*if (PirateGameInstance->GetIsSaveDataToJsonFile())
	{
		auto Path = FPaths::ProjectContentDir() + RelativeSavePath + JsonFileName;
		UCommonUtilLibrary::SaveObjectPropertiesToJsonFile(this, *Path);
	}*/
	// ֪ͨUI����
	//GlobalDelegateManager::Get()->CallGenericDelegate<void, int>(PLAYERINVENTORY_UPDATE_SLOT, Index);
	// ��֮ǰ����ɵĵ���ί�еķ�ʽ�����µ�
	// �ನί�еĵ���