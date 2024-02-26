// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"

#include "ItemSlotBox.generated.h"


UCLASS()
class EASYSURVIVALRPGV2_5_API UItemSlotBox : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
		void InitiateSlots(int SlotCount);
	UFUNCTION(BlueprintCallable)
		void UpdateItemSlot(int32 Index, UTexture2D* Icon,int32 Count,float Duration);
	UFUNCTION(BlueprintPure)
		UWidget* GetChildSlotWidget(int32 Index);

	UFUNCTION(BlueprintImplementableEvent) 
		void SetUpItemSlotWidget(UWidget* SlotWidget,UTexture2D* Icon, int32 Count, float Duration);
	UFUNCTION(BlueprintImplementableEvent)
		void InitiateSlotWidget(UWidget* SlotWidget,int32 Index);                 // ��������


	UPROPERTY(meta = (BindWidget))
		class UUniformGridPanel* Grid_ItemSlotsContainer;              // �󶨱����ĺ�������
	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> ItemSlotWidgetClass = UImage::StaticClass();                   // ���߸��ӵĿؼ�����

	/*UPROPERTY(EditAnywhere,meta = (ClampMin = 0))
		int32 PreviewSlotCount = 12; */
	UPROPERTY(EditAnywhere,meta = (ClampMin = 1))
		int32 MaxColumn = 6;                                          
};
