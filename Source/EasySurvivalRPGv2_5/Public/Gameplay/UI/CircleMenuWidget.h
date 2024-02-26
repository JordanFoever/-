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
	// ��ǰ������
	int32 SelectedIndex = -1;

protected:
	/*
	* ���˵��ֳɶ��ٶ�
	*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Meta = (ClampMin = 2))
		int32 FragmentCount = 12;
	/*
	* ͼƬ���⾶
	*/
	UPROPERTY(EditAnywhere)
		float CircleOuterRadius = 500.f;  
	/*
	* ͼƬ�ھ�
	*/
	UPROPERTY(EditAnywhere)
		float CircleInnerRadius = 300.f;
	/*
	* �Ƿ�������Ҫ��Բ���ڲ���Ч
	*/
	UPROPERTY(EditAnywhere)
		bool bSelectionCheckRadius = false;
	UPROPERTY(Meta = (BindWidget)) 
		class UImage* CircleBgImage;

	int32 UpdateMouseCursorForIndex() override;
	void ConstructContainer() override;
	void SetItemWidgetTransform(int32 Index, UWidget* ItemWidget, class UPanelSlot* ItemWigetSlot);

	
};
