// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "GenericListWidget.generated.h"


// �ýӿ����ڸ��б��пؼ���ʵ��
UINTERFACE(MinimalAPI)
class UGenericListItemWidgetStyle : public UInterface
{
	GENERATED_BODY()
};

class EASYSURVIVALRPGV2_5_API IGenericListItemWidgetStyle
{
	GENERATED_BODY()

public:
	
	/*
	* �����뵽�б���ʵ�ָýӿڵ�ĳ���ؼ�ʱ�����
	*/
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		void MouseEnterItemWidget();

	/*
	* ����뿪
	*/
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		void MouseLeaveItemWidget();

	/*
	* ѡ���û��ѡ��
	* @Params bSelected �Ƿ�ѡ��
	*/
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		void SelectItemWidgetChanged(bool bSelected);
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FListWidgetSelectedDelegate, int32, SelectedIndex, int32 , lastSelectedIndex);

/**
*  ��ʼ���б��ί������
 * @ ItemWidget ��ʼ���Ŀؼ�
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FListWidgetInitiateDelegate, int32, SelectedIndex, UWidget*, ItemWidget);

/*
* ��굱ǰ�����Ŀؼ�������
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FListWidgetEnterDelegate, int32, EnterIndex);

/**
 * ͨ���б�ؼ�
 */
UCLASS(Blueprintable)
class EASYSURVIVALRPGV2_5_API UGenericListWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeogetry, float DeltaTime);
	/*
	* ��д�븸�� UUserwidget
	* @Params InGeometry ��ǰ�ؼ��������״��Ϣ
	* @Params InMouseEvent ��װ���������ȥ�ĸ������� 
	*/
	FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	/**
	 * �����ʱ����
	 */
	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/*
	* ���õ�ǰ������ڿؼ�������
	*/
	void SetCurrentMouseOnItemIndex(int32 Index);
	

protected:
	// Ϊ -1 ��ʾ�����κ�һ���ؼ���
	int32 CurrentMouseOnItemIndex = -1;
	// ��ǰ�Ѿ������ѡ�еĿؼ�������
	int32 CurrentSelectedItemIndex = -1;
	//�б������������з�ʽ
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EOrientation> ListOrientation;                

	//�����еĿؼ�
	UPROPERTY(EditAnywhere)
		TSubclassOf<UWidget> ItemWidgetClass = UImage::StaticClass();       
	
	UPROPERTY(EditAnywhere)
		bool bShouldUpdateExistChild = false;
	UPROPERTY(EditAnywhere)
		bool bUpdateWhenInGeometry = false;
	/*
	* ��갴�Ƿ���Ч
	*/
	UPROPERTY(EditAnywhere)
		bool bHandleMouseDownSelection = true;
	//�Ƿ�ʵ�� IAssociatedComponentGetter �ӿ�
	/*bool bStyleInferfaceType = false;*/

	// ����ͼ�еĿؼ�
	UPROPERTY(meta=(BindWidget))
		class UPanelWidget* RootSlot;                                           

	UPanelWidget* ContainerList;
	class UScrollBox* ScrollBox;

	virtual int32 UpdateMouseCursorForIndex();
	virtual void ConstructContainer();
	virtual void SetItemWidgetTransform(int32 Index, UWidget* ItemWidget,class UPanelSlot* ItemWigetSlot){}

public:

	// ����ί�б���,����ͼ���԰󶨸�ί��
	UPROPERTY(BlueprintAssignable)
		FListWidgetSelectedDelegate OnListItemSelected;

	// �����ʼ��ί�б���,����ͼ���԰󶨸�ί��
	UPROPERTY(BlueprintAssignable)
		FListWidgetInitiateDelegate  OnListItemWidgetInitiated;

	// ��ǰ��꾭���Ŀؼ�������
	UPROPERTY(BlueprintAssignable)
		FListWidgetEnterDelegate OnListWidgetEnterDelegate;

	/*
	* @Params ItemWidgetCount �б��г�ʼ���Ŀؼ�������
	* @Params bReconstruct �Ƿ����¹���,��ѡ��ĳһ���б�ʱΪ7��,����ѡ������һ��ʱΪ5��,��ʱӦ����δ����ǽ�
	*	ԭ����7��ɾ��������5������ɾ��ԭ����2��,��ԭ����5�����½��и�ֵ��                                                                                               
	*/
	UFUNCTION(BlueprintCallable)
		void RebuildItemWidget(int32 ItemWidgetCount, bool bReconstrcut = true);

};
