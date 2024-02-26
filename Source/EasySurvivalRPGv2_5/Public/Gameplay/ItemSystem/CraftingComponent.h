// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/ItemsSysType.h"
#include "CraftingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCraftingListUpdateDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCraftingSelectionDelegate, int32, index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCraftingStartDelegate);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EASYSURVIVALRPGV2_5_API UCraftingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCraftingComponent();

private:

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/*
	* building craft list  
	*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		TArray<FName> CraftingInfoList;
		
	// show list for mouse button current
	TArray<FName> CurrentCraftingList;

	EItemCategory CurrentCategory = EItemCategory::None;

	// ???????????
	int32 CurrentCraftingIndex = -1;
	
	struct FCraftingItemData
	{
		FCraftingItemData() : CraftingRowName(NAME_None), Amount(0), Handle(FTimerHandle()) {}
		FCraftingItemData(const FTimerHandle& NewHandle) : CraftingRowName(NAME_None), Amount(0), Handle(NewHandle) {}
		FCraftingItemData(FName NewRowName,int NewAmount,const FTimerHandle& NewHandle) : 
			CraftingRowName(NewRowName), Amount(NewAmount), Handle(NewHandle) {}

		bool operator==(const FCraftingItemData& RValue) { return this->Handle == RValue.Handle; }

		FName CraftingRowName;
		int32 Amount;
		FTimerHandle Handle;
	};

	// ????????§Ö???????
	TArray<FCraftingItemData> ProcessingCraftingItems;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UPROPERTY(BlueprintAssignable)
		FCraftingListUpdateDelegate OnCraftingListUpdated;

	UPROPERTY(BlueprintAssignable)
		FCraftingSelectionDelegate OnCraftingItemSelected;

	UPROPERTY(BlueprintAssignable)
		FCraftingStartDelegate OnstartCrafting;
	/*
	* set the crafting list
	*/
	UFUNCTION(BlueprintCallable)
		void SetCraftingCategory(EItemCategory Category);

	UFUNCTION(BlueprintPure)
		FORCEINLINE TArray<FName> GetCurrentCraftingList() {return CurrentCraftingList; }

	UFUNCTION(BlueprintPure)
		bool GetProcessingData(int32 Index,FName& CraftingRowName,int32& Amount);

	UFUNCTION(BlueprintPure)
		FORCEINLINE int32 GetCraftingListQueueLength() {return ProcessingCraftingItems.Num();}

	UFUNCTION(BlueprintPure)
		FORCEINLINE EItemCategory GetCraftingCategory() {return CurrentCategory;}

	UFUNCTION(BlueprintCallable)
		void PostInitiateList();

	UFUNCTION(BlueprintCallable)
		void SelectCraftingItem(int32 Index);

	UFUNCTION(BlueprintCallable)
		void StartCrafting(int Count);
};