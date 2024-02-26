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
		Mat->SetScalarParameterValue(TEXT("OuterRadius"), 0.5f); // 0.5f �� UV ����
		Mat->SetScalarParameterValue(TEXT("InnerRadius"), 0.5f * (CircleInnerRadius / CircleOuterRadius));
	}
}

int32 UCircleMenuWidget::UpdateMouseCursorForIndex()
{
	// UWidgetLayoutLibrary::GetMousePositionOnPlatform() ����ʾ��Ϊ���յ�����
	auto MousePosition = GetTickSpaceGeometry().AbsoluteToLocal(UWidgetLayoutLibrary::GetMousePositionOnPlatform()); // ��������Ļ���꣨��������) ת��������
	// Ϊ�˱��ں�������,����������Ͻ��ƶ������½ǣ���ʱ Y �ᳯ��
	MousePosition.Y = GetTickSpaceGeometry().GetLocalSize().Y - MousePosition.Y;	                                 // ���������ϵԭ��Ų������
	// �õ�Բ�ĵ����꣨Ҳ���Ǳ��ص���������) 
	auto CenterPosition = GetTickSpaceGeometry().GetLocalSize() * 0.5f;
	// �õ����ĵ㵽��ǰ���λ�õ�����
	auto MouseVector = MousePosition - CenterPosition;

	// �� bSelectionCheckRadius = true ���Ҳ���ԲȦ�ھ�ֱ�ӷ��� 
	if (bSelectionCheckRadius &&
		!(MouseVector.SizeSquared() > FMath::Square(CircleInnerRadius) && MouseVector.SizeSquared() < FMath::Square(CircleOuterRadius)))
		return CurrentMouseOnItemIndex;

	auto MouseVectorNormal = MouseVector.GetSafeNormal();
	auto Compare = FVector2D(0.0f, 1.0f);
	// ��Բ��ָ��ǰ������ڵ�λ�õ������� Y ��нǵ�����ֵ
	auto CosValue = FVector2D::DotProduct(MouseVectorNormal, Compare);

	// ����ó���� ���ͬʱ��ֱ������������ֱ���������ڵ�ƽ��,����0ʱ��Բ���ұߣ�С��0ʱ��Բ�����
	auto CrossVector = FVector::CrossProduct(FVector(MouseVectorNormal.X, MouseVectorNormal.Y, 0), FVector(Compare.X, Compare.Y, 0));
	// ͨ�������ҵõ����ȣ�FMath::Acos(CosValue)��Ȼ��õ������Բ�İٷֱ�
	auto Percent = FMath::Acos(CosValue) / (2.0f * PI);

	/*UKismetSystemLibrary::PrintString(GetWorld(), FString::SanitizeFloat(CrossVector.Z));*/
	// ����ߵĸ���Ҳ������� ��߱����� 0.5-0  ��1������0.5��1�����ұߵ��� 0-0.5 ���������ͱ���� 0-1������ͨ�����Ψһȷ������λ�á�
	if (CrossVector.Z < 0) Percent = 1.0f - Percent;
	//UKismetSystemLibrary::PrintString(GetWorld(), FString::SanitizeFloat(Percent)); 

	auto Index = (int32)(Percent / (1.0f / FragmentCount));

	if (auto Mat = CircleBgImage->GetDynamicMaterial())
	{
		Mat->SetScalarParameterValue(TEXT("Percent"), 1.0f / FragmentCount);
		Mat->SetScalarParameterValue(TEXT("StartAngle"), Index * (360.0f / FragmentCount)); // (int32) ����ȡ��
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

	// �ھ����⾶���м�  
	auto Radius = CircleInnerRadius + (CircleOuterRadius - CircleInnerRadius) * 0.5f;
	// ������Ļ������ϵ����������,�����Ҫ���Ͼ���Ҫ����
	auto UpOffset = FVector2D(0, -Radius);
	auto FragmentAngle = 360.f / FragmentCount;
	// GetRotated �� UpOffset ����˳ʱ�������ת��Ӧ�ĽǶȵõ�һ���µ�����
	// Index + 0.5f ��ʾ����ת��һ��ĽǶȵ�λ��,�����һ��λ��Ϊ 0.5 ����ת�Ƕȵ�һ��
	auto Offset = UpOffset.GetRotated((Index + 0.5f) * FragmentAngle);
	ItemWidget->SetRenderTranslation(Offset);
}
