// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/ItemsSysType.h"
#include "PirateGameInstance.h"
#include "InventoryComponent.generated.h"


// note=> ֻ�ж�̬ί�вſ���ʹ�� UPROPERTY �ı�ǣ�ֻ�ж�̬�Ķನί�вſ�������ͼ������а�
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryUpdateDelegate, int32, Index);  // ��̬��ί����Ȼֻ��һ�������������ͺͱ�ʶ����Ҫ��������

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
		int32 Capacity = 18;                     // ����

	UPROPERTY(EditAnywhere)
		bool bSaveInventoryToJson = true;       // �Ƿ񽫱����еĵ��߱��浽 Json �ļ���
	
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
		bool GetItemData(int32 Index, FInventoryItemData& OutData);    //  ����ֵ�����ж��Ƿ񳬳���Χ  
	/*
	* ȷ�������ĵ����Ƿ��㹻
	* @ItemRowName ���ߵ�����
	* @Amount �����߹�����
	*/
	UFUNCTION(BlueprintPure)
		bool IsItemEnouph(FName ItemRowName,int32 Amount);
	UFUNCTION(BlueprintCallable)
		/// <summary>
		/// �򱳰�����ӵ���
		/// </summary>
		/// <param name="ItemRowName">�����е�����</param>
		/// <param name="Count">����ĵ��ߵ�����</param>
		/// <param name="FromStackIndex">�ѵ�������</param>
		/// <param name="FromEmptyIndex">�յ�����</param>
		/// <param name="">�Ƿ������������ӵ���</param>
		/// <returns>���������ĵ�����Ӻ�ʣ��ߺ�ʣ�������</returns>
		int32 AddItemToInventory(FName ItemRowName, int32 Count, int32 FromStackIndex = 0, int32 FromEmptyIndex = 0,bool bAddTest = false);
	
	/*
	* @Return int32 �ӱ������Ƴ���ʣ���ٵ��� 
	*/
	UFUNCTION(BlueprintCallable)
		int32 RemoveIndexFromInventory(int32 Index,int32 Count);		// �Ƴ�ĳ���ض��������ĵ���  �Ƴ����ٸ�����

	UFUNCTION(BlueprintCallable)
		int32 RemoveItemFromInventory(FName ItemRowName,int32 Count);
	UFUNCTION(BlueprintCallable)
		void SwapItems(int32 FromIndex,int32 ToIndex);					// ��������
	UFUNCTION(BlueprintCallable)
		void SplitItems(int32 FromIndex,int32 ToIndex,int SplitCount);   // ���ĸ����ӷָ�ĸ������Լ��ָ������
	UFUNCTION(BlueprintCallable)
		void PostInventoryInitiate();                    
	UFUNCTION(BlueprintCallable)
		void ClearAll();
	UFUNCTION(BlueprintCallable)
		void SaveDataToJsonFile();

	UPROPERTY(BlueprintAssignable)										// BlueprintAssignable  �����ʶ����ʾ��ͼ���԰�
		FInventoryUpdateDelegate OnInventoryUpdated; 
	
private:
	// meta = (SavaToJson) ��ʱ���������Զ����
	UPROPERTY(Meta = (SaveToJson = True))
		TArray<FInventoryItemData> InventoryItems;					// ����,���汳���е�ÿһ������
	
	void ConfirmInventorySlotChange(int Index, FInventoryItemData NewData);
	int32 InternalRemoveItemFromInventory(int32 Index, int32 Count); 
};
