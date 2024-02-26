 // Easy Systems


#include "Gameplay/UI/ItemSlotBox.h"
#include "Components/UniformGridPanel.h"

void UItemSlotBox::InitiateSlots(int SlotCount)
{
	if (!IsValid(ItemSlotWidgetClass))return;

	Grid_ItemSlotsContainer->ClearChildren();
	for (int i = 0; i < SlotCount; i++)
	{
		int32 Row = i / MaxColumn;
		int32 Column = i % MaxColumn;
		auto ItemSlotWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), ItemSlotWidgetClass);
		Grid_ItemSlotsContainer->AddChildToUniformGrid(ItemSlotWidget, Row, Column);
		InitiateSlotWidget(ItemSlotWidget,i); 
	}
}

void UItemSlotBox::UpdateItemSlot(int32 Index, UTexture2D* Icon, int32 Count, float Duration)
{
	if (auto Widget = GetChildSlotWidget(Index))
	{
		SetUpItemSlotWidget(Widget, Icon, Count, Duration);
	}
}

UWidget* UItemSlotBox::GetChildSlotWidget(int32 Index)
{
	return Grid_ItemSlotsContainer->GetChildAt(Index);
}
