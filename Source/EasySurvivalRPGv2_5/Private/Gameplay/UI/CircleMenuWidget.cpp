// Easy Systems

#include "Gameplay/UI/CircleMenuWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Blueprint/WidgetTree.h"

void  UCircleMenuWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	CircleBgImage->SetBrushSize(FVector2D(CircleOuterRadius) * 2.0f);
	if (auto Mat = CircleBgImage->GetDynamicMaterial()) 
	{
		Mat->SetScalarParameterValue(TEXT("OuterRadius"), 0.5f); // 0.5f 是 UV 坐标
		Mat->SetScalarParameterValue(TEXT("InnerRadius"), 0.5f * (CircleInnerRadius / CircleOuterRadius));
	}
}

int32 UCircleMenuWidget::UpdateMouseCursorForIndex()
{
	// UWidgetLayoutLibrary::GetMousePositionOnPlatform() 以显示器为参照的坐标
	auto MousePosition = GetTickSpaceGeometry().AbsoluteToLocal(UWidgetLayoutLibrary::GetMousePositionOnPlatform()); // 把鼠标的屏幕坐标（绝对坐标) 转换到本地
	// 为了便于后续操作,将坐标从左上角移动到左下角，此时 Y 轴朝上
	MousePosition.Y = GetTickSpaceGeometry().GetLocalSize().Y - MousePosition.Y;	                                 // 把坐标参照系原点挪到左下
	// 得到圆心的坐标（也就是本地的中心坐标) 
	auto CenterPosition = GetTickSpaceGeometry().GetLocalSize() * 0.5f;
	// 得到中心点到当前鼠标位置的向量
	auto MouseVector = MousePosition - CenterPosition;

	// 如 bSelectionCheckRadius = true 并且不在圆圈内就直接返回 
	if (bSelectionCheckRadius &&
		!(MouseVector.SizeSquared() > FMath::Square(CircleInnerRadius) && MouseVector.SizeSquared() < FMath::Square(CircleOuterRadius)))
		return CurrentMouseOnItemIndex;

	auto MouseVectorNormal = MouseVector.GetSafeNormal();
	auto Compare = FVector2D(0.0f, 1.0f);
	// 从圆心指向当前鼠标所在的位置的向量和 Y 轴夹角的余弦值
	auto CosValue = FVector2D::DotProduct(MouseVectorNormal, Compare);

	// 计算得出叉积 叉积同时垂直两个向量即垂直两向量所在的平面,大于0时在圆的右边，小于0时在圆的左边
	auto CrossVector = FVector::CrossProduct(FVector(MouseVectorNormal.X, MouseVectorNormal.Y, 0), FVector(Compare.X, Compare.Y, 0));
	// 通过反余弦得到弧度（FMath::Acos(CosValue)）然后得到相对于圆的百分比
	auto Percent = FMath::Acos(CosValue) / (2.0f * PI);

	/*UKismetSystemLibrary::PrintString(GetWorld(), FString::SanitizeFloat(CrossVector.Z));*/
	// 让左边的负的也变成正的 左边本来是 0.5-0  用1减后变成0.5到1，而右边的是 0-0.5 最终整个就变成了 0-1。可以通过这个唯一确定鼠标的位置。
	if (CrossVector.Z < 0) Percent = 1.0f - Percent;
	//UKismetSystemLibrary::PrintString(GetWorld(), FString::SanitizeFloat(Percent)); 

	auto Index = (int32)(Percent / (1.0f / FragmentCount));

	if (auto Mat = CircleBgImage->GetDynamicMaterial())
	{
		Mat->SetScalarParameterValue(TEXT("Percent"), 1.0f / FragmentCount);
		Mat->SetScalarParameterValue(TEXT("StartAngle"), Index * (360.0f / FragmentCount)); // (int32) 向下取整
	}
	return Index;
}

void UCircleMenuWidget::ConstructContainer()
{
	if (ContainerList) return;
	ContainerList = WidgetTree->ConstructWidget<UOverlay>();
	RootSlot->AddChild(ContainerList);
}

void UCircleMenuWidget::SetItemWidgetTransform(int32 Index, UWidget* ItemWidget,UPanelSlot* ItemWigetSlot)
{
	if(auto OverlaySlot = Cast<UOverlaySlot>(ItemWigetSlot))
	{
		OverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		OverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	}

	// 内径和外径的中间  
	auto Radius = CircleInnerRadius + (CircleOuterRadius - CircleInnerRadius) * 0.5f;
	// 由于屏幕的坐标系朝下是正的,因此想要朝上就需要负的
	auto UpOffset = FVector2D(0, -Radius);
	auto FragmentAngle = 360.f / FragmentCount;
	// GetRotated 将 UpOffset 向量顺时针进行旋转对应的角度得到一个新的向量
	// Index + 0.5f 表示放在转了一半的角度的位置,比如第一个位置为 0.5 即旋转角度的一半
	auto Offset = UpOffset.GetRotated((Index + 0.5f) * FragmentAngle);
	ItemWidget->SetRenderTranslation(Offset);
}
