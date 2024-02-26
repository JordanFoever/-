 // Easy Systems
#include "Gameplay/UI/GenericListWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/PanelWidget.h"
#include "Components/ScrollBox.h"
#include "Components/PanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UGenericListWidget::NativeConstruct() 
{
	Super::NativeConstruct();
	ConstructContainer();
}

void UGenericListWidget::NativeTick(const FGeometry& MyGeogetry, float DeltaTime)
{

	if (!bUpdateWhenInGeometry)
		SetCurrentMouseOnItemIndex(UpdateMouseCursorForIndex());
}

FReply UGenericListWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (bUpdateWhenInGeometry)
		SetCurrentMouseOnItemIndex(UpdateMouseCursorForIndex());
		
	return FReply::Handled();
}

FReply UGenericListWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);


	if(!bHandleMouseDownSelection) return FReply::Unhandled(); // ������������°��Ĳ���
	// note=> UClass �ж��Ƿ�ʵ�ֽӿ�ʹ�õ��� ImplementsInterface ������ Implements
	auto bStyleInferfaceType = ItemWidgetClass->ImplementsInterface(UGenericListItemWidgetStyle::StaticClass());
	if (CurrentMouseOnItemIndex != CurrentSelectedItemIndex && bStyleInferfaceType) 
	{
		if (auto SelectedWidget = ContainerList->GetChildAt(CurrentMouseOnItemIndex))
			IGenericListItemWidgetStyle::Execute_SelectItemWidgetChanged(SelectedWidget, true);
		if (auto UnSelectedWidget = ContainerList->GetChildAt(CurrentSelectedItemIndex))
			IGenericListItemWidgetStyle::Execute_SelectItemWidgetChanged(UnSelectedWidget, false);
	}

	// ����ί��,����ͼ�н��а�
	OnListItemSelected.Broadcast(CurrentMouseOnItemIndex, CurrentSelectedItemIndex);
	 
	CurrentSelectedItemIndex = CurrentMouseOnItemIndex;

	//  note => ���� Handled ������ �����ؼ��ص��Ժ�,��ʾ�Ѿ���������������Ŀؼ�ð�ݡ���һ�� UnHandled ���ܸպú�ǰ���෴�� ���󲿷�����¶���ʹ��ǰ��
	return FReply::Handled();
}

void UGenericListWidget::SetCurrentMouseOnItemIndex(int32 Index)
{
	// ��껹��ͬһ���ؼ���ʲô������ֱ�ӷ���
	if (Index == CurrentMouseOnItemIndex)return;
	// note=> UClass �ж��Ƿ�ʵ�ֽӿ�ʹ�õ��� ImplementsInterface ������ Implements
	auto bStyleInferfaceType = ItemWidgetClass->ImplementsInterface(UGenericListItemWidgetStyle::StaticClass());

	// �ÿؼ�ʵ���˽ӿڵĻ��������뿪�ؼ��ĵ���
	if (bStyleInferfaceType) 
	{
		if(Index!=CurrentSelectedItemIndex)
			if(ContainerList->GetChildAt(Index))
				IGenericListItemWidgetStyle::Execute_MouseEnterItemWidget(ContainerList->GetChildAt(Index));
		if(CurrentMouseOnItemIndex!=CurrentSelectedItemIndex)
			if(ContainerList->GetChildAt(CurrentMouseOnItemIndex))
				IGenericListItemWidgetStyle::Execute_MouseLeaveItemWidget(ContainerList->GetChildAt(CurrentMouseOnItemIndex));
	}

	CurrentMouseOnItemIndex = Index;

	OnListWidgetEnterDelegate.Broadcast(CurrentMouseOnItemIndex);
}

void UGenericListWidget::ConstructContainer()
{
	if (ContainerList)return;

	TSubclassOf<UPanelWidget> ContainerClass = NULL;
	if (ListOrientation == EOrientation::Orient_Vertical)   ContainerClass = UVerticalBox::StaticClass();
	if (ListOrientation == EOrientation::Orient_Horizontal)  ContainerClass = UHorizontalBox::StaticClass();

	//��������
	ContainerList = WidgetTree->ConstructWidget<UPanelWidget>(ContainerClass);
	RootSlot->AddChild(ContainerList);

	ScrollBox = Cast<UScrollBox>(RootSlot);
}

int32 UGenericListWidget::UpdateMouseCursorForIndex()
{
	
	// ���λ�õľ�������
	auto MousePositionAbx = UWidgetLayoutLibrary::GetMousePositionOnPlatform();
	// �������ӿ��еľ�������ת���ɵ�ǰ�ؼ��ı������� 
	auto MouseLocatPosition = GetTickSpaceGeometry().AbsoluteToLocal(MousePositionAbx);

	// ͨ����������ȷ����굥�������ĸ�����ؼ�
	for (int i = 0; i < ContainerList->GetChildrenCount(); i++)
	{
		auto ChildWidget = ContainerList->GetChildAt(i);

		// �õ�����ʱ�Ŀؼ���״��Ϣ
		auto ChildGeometry = ChildWidget->GetTickSpaceGeometry();

		// �õ�������С�ؼ��ڴ�ؼ��ı�������
		auto UpperLeft = ChildGeometry.GetLocalPositionAtCoordinates(FVector2D(0.0f, 0.0f));
		auto bottomRight = ChildGeometry.GetLocalPositionAtCoordinates(FVector2D(1.0f, 1.0f));

		if (ScrollBox)
		{
			// ����տ�ʼ�Ǵ�ֱƫ�Ƶ�
			auto ScrollOffset = FVector2D(0, ScrollBox->GetScrollOffset());
			if (ScrollBox->Orientation = EOrientation::Orient_Horizontal)
				ScrollOffset = FVector2D(ScrollBox->GetScrollOffset(), 0);

			UpperLeft -= ScrollOffset;
			bottomRight -= ScrollOffset;
		}

		if (MouseLocatPosition.X >= UpperLeft.X && MouseLocatPosition.X <= bottomRight.X
			&& MouseLocatPosition.Y >= UpperLeft.Y && MouseLocatPosition.Y <= bottomRight.Y)
		{
			/*SetCurrentMouseOnItemIndex(i);
			break;*/
			return i;
		}
	}

	return -1;
}

void UGenericListWidget::RebuildItemWidget(int32 ItemWidgetCount, bool bReconstrcut)
{
	//if (!ItemWidgetClass) return;

	ConstructContainer();

	if (bReconstrcut)
		ContainerList->ClearChildren();
	else if (bShouldUpdateExistChild) // û�и��¹����ӿؼ�,����Ҫ����
		for (int i = 0; i < ItemWidgetCount; i++)
		{
			if(auto Child = ContainerList->GetChildAt(i))
				OnListItemWidgetInitiated.Broadcast(i, Child);
		}
	
	auto ExistChildrenCount = ContainerList->GetChildrenCount();

	TSet<UWidget*> WidgetToDelete;
	for (int i = ItemWidgetCount; i < ExistChildrenCount; i++)
		WidgetToDelete.Add(ContainerList->GetChildAt(i));

	for (auto& Child : WidgetToDelete)
		Child->RemoveFromParent();

	ItemWidgetCount = ItemWidgetCount - ExistChildrenCount; 

	for (int32 i = 0; i < ItemWidgetCount; i++)
	{
		// ����ÿ��С�Ŀؼ�
		auto ItemWidget = WidgetTree->ConstructWidget<UWidget>(ItemWidgetClass);
		auto SlotItem = ContainerList->AddChild(ItemWidget);

		OnListItemWidgetInitiated.Broadcast(i + ExistChildrenCount, ItemWidget);
	}
	
	for (int i = 0; i < ContainerList->GetChildrenCount(); i++)
		SetItemWidgetTransform(i, ContainerList->GetChildAt(i),ContainerList->GetSlots()[i]);
}
