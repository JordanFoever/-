// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/UI/GenericListWidget.h"
#include "CircleMenuWidget.generated.h"

UCLASS()
class EASYSURVIVALRPGV2_5_API UCircleMenuWidget : public UGenericListWidget
{
	GENERATED_BODY()
	
private:
	void NativePreConstruct() override;
	// 先前的索引
	int32 SelectedIndex = -1;

protected:
	/*
	* 将菜单分成多少段
	*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Meta = (ClampMin = 2))
		int32 FragmentCount = 12;
	/*
	* 图片的外径
	*/
	UPROPERTY(EditAnywhere)
		float CircleOuterRadius = 500.f;  
	/*
	* 图片内径
	*/
	UPROPERTY(EditAnywhere)
		float CircleInnerRadius = 300.f;
	/*
	* 是否鼠标必须要在圆环内才生效
	*/
	UPROPERTY(EditAnywhere)
		bool bSelectionCheckRadius = false;
	UPROPERTY(Meta = (BindWidget)) 
		class UImage* CircleBgImage;

	int32 UpdateMouseCursorForIndex() override;
	void ConstructContainer() override;
	void SetItemWidgetTransform(int32 Index, UWidget* ItemWidget, class UPanelSlot* ItemWigetSlot);

	
};
