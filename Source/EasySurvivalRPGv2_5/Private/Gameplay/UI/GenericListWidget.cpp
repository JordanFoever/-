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


	if(!bHandleMouseDownSelection) return FReply::Unhandled(); // 不处理鼠标向下按的操作
	// note=> UClass 判断是否实现接口使用的是 ImplementsInterface 而不是 Implements
	auto bStyleInferfaceType = ItemWidgetClass->ImplementsInterface(UGenericListItemWidgetStyle::StaticClass());
	if (CurrentMouseOnItemIndex != CurrentSelectedItemIndex && bStyleInferfaceType) 
	{
		if (auto SelectedWidget = ContainerList->GetChildAt(CurrentMouseOnItemIndex))
			IGenericListItemWidgetStyle::Execute_SelectItemWidgetChanged(SelectedWidget, true);
		if (auto UnSelectedWidget = ContainerList->GetChildAt(CurrentSelectedItemIndex))
			IGenericListItemWidgetStyle::Execute_SelectItemWidgetChanged(UnSelectedWidget, false);
	}

	// 调用委托,在蓝图中进行绑定
	OnListItemSelected.Broadcast(CurrentMouseOnItemIndex, CurrentSelectedItemIndex);
	 
	CurrentSelectedItemIndex = CurrentMouseOnItemIndex;

	//  note => 返回 Handled 函数后 两个控件重叠以后,表示已经被处理不会向下面的控件冒泡。有一个 UnHandled 功能刚好和前者相反。 绝大部分情况下都会使用前者
	return FReply::Handled();
}

void UGenericListWidget::SetCurrentMouseOnItemIndex(int32 Index)
{
	// 鼠标还在同一个控件上什么都不做直接返回
	if (Index == CurrentMouseOnItemIndex)return;
	// note=> UClass 判断是否实现接口使用的是 ImplementsInterface 而不是 Implements
	auto bStyleInferfaceType = ItemWidgetClass->ImplementsInterface(UGenericListItemWidgetStyle::StaticClass());

	// 该控件实现了接口的话经过和离开控件的调用
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

	//创建容器
	ContainerList = WidgetTree->ConstructWidget<UPanelWidget>(ContainerClass);
	RootSlot->AddChild(ContainerList);

	ScrollBox = Cast<UScrollBox>(RootSlot);
}

int32 UGenericListWidget::UpdateMouseCursorForIndex()
{
	
	// 鼠标位置的绝对坐标
	auto MousePositionAbx = UWidgetLayoutLibrary::GetMousePositionOnPlatform();
	// 将其在视口中的绝对坐标转换成当前控件的本地坐标 
	auto MouseLocatPosition = GetTickSpaceGeometry().AbsoluteToLocal(MousePositionAbx);

	// 通过本地坐标确定鼠标单击的是哪个具体控件
	for (int i = 0; i < ContainerList->GetChildrenCount(); i++)
	{
		auto ChildWidget = ContainerList->GetChildAt(i);

		// 得到更新时的控件形状信息
		auto ChildGeometry = ChildWidget->GetTickSpaceGeometry();

		// 得到遍历的小控件在大控件的本地坐标
		auto UpperLeft = ChildGeometry.GetLocalPositionAtCoordinates(FVector2D(0.0f, 0.0f));
		auto bottomRight = ChildGeometry.GetLocalPositionAtCoordinates(FVector2D(1.0f, 1.0f));

		if (ScrollBox)
		{
			// 假设刚开始是垂直偏移的
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
	else if (bShouldUpdateExistChild) // 没有更新构建子控件,但是要更新
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
		// 创建每个小的控件
		auto ItemWidget = WidgetTree->ConstructWidget<UWidget>(ItemWidgetClass);
		auto SlotItem = ContainerList->AddChild(ItemWidget);

		OnListItemWidgetInitiated.Broadcast(i + ExistChildrenCount, ItemWidget);
	}
	
	for (int i = 0; i < ContainerList->GetChildrenCount(); i++)
		SetItemWidgetTransform(i, ContainerList->GetChildAt(i),ContainerList->GetSlots()[i]);
}
