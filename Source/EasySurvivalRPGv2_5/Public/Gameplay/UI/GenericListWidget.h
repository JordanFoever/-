// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "GenericListWidget.generated.h"


// 该接口用于给列表中控件向实现
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
	* 鼠标进入到列表中实现该接口的某个控件时将如何
	*/
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		void MouseEnterItemWidget();

	/*
	* 鼠标离开
	*/
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		void MouseLeaveItemWidget();

	/*
	* 选择和没有选中
	* @Params bSelected 是否选中
	*/
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		void SelectItemWidgetChanged(bool bSelected);
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FListWidgetSelectedDelegate, int32, SelectedIndex, int32 , lastSelectedIndex);

/**
*  初始化列表的委托声明
 * @ ItemWidget 初始化的控件
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FListWidgetInitiateDelegate, int32, SelectedIndex, UWidget*, ItemWidget);

/*
* 鼠标当前经过的控件的索引
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FListWidgetEnterDelegate, int32, EnterIndex);

/**
 * 通用列表控件
 */
UCLASS(Blueprintable)
class EASYSURVIVALRPGV2_5_API UGenericListWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeogetry, float DeltaTime);
	/*
	* 重写与父类 UUserwidget
	* @Params InGeometry 当前控件里面的形状信息
	* @Params InMouseEvent 封装了鼠标点击下去的各种属性 
	*/
	FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	/**
	 * 鼠标点击时触发
	 */
	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/*
	* 设置当前鼠标所在控件的索引
	*/
	void SetCurrentMouseOnItemIndex(int32 Index);
	

protected:
	// 为 -1 表示不在任何一个控件上
	int32 CurrentMouseOnItemIndex = -1;
	// 当前已经被鼠标选中的控件的索引
	int32 CurrentSelectedItemIndex = -1;
	//列表中容器的排列方式
	UPROPERTY(EditAnywhere)
		TEnumAsByte<EOrientation> ListOrientation;                

	//容器中的控件
	UPROPERTY(EditAnywhere)
		TSubclassOf<UWidget> ItemWidgetClass = UImage::StaticClass();       
	
	UPROPERTY(EditAnywhere)
		bool bShouldUpdateExistChild = false;
	UPROPERTY(EditAnywhere)
		bool bUpdateWhenInGeometry = false;
	/*
	* 鼠标按是否生效
	*/
	UPROPERTY(EditAnywhere)
		bool bHandleMouseDownSelection = true;
	//是否实现 IAssociatedComponentGetter 接口
	/*bool bStyleInferfaceType = false;*/

	// 绑定蓝图中的控件
	UPROPERTY(meta=(BindWidget))
		class UPanelWidget* RootSlot;                                           

	UPanelWidget* ContainerList;
	class UScrollBox* ScrollBox;

	virtual int32 UpdateMouseCursorForIndex();
	virtual void ConstructContainer();
	virtual void SetItemWidgetTransform(int32 Index, UWidget* ItemWidget,class UPanelSlot* ItemWigetSlot){}

public:

	// 声明委托变量,让蓝图可以绑定该委托
	UPROPERTY(BlueprintAssignable)
		FListWidgetSelectedDelegate OnListItemSelected;

	// 定义初始化委托变量,让蓝图可以绑定该委托
	UPROPERTY(BlueprintAssignable)
		FListWidgetInitiateDelegate  OnListItemWidgetInitiated;

	// 当前鼠标经过的控件的索引
	UPROPERTY(BlueprintAssignable)
		FListWidgetEnterDelegate OnListWidgetEnterDelegate;

	/*
	* @Params ItemWidgetCount 列表中初始化的控件的数量
	* @Params bReconstruct 是否重新构建,当选择某一个列表时为7个,但是选择另外一个时为5个,此时应该如何处理，是将
	*	原来的7个删除掉构建5个还是删掉原来的2个,将原来的5个重新进行赋值。                                                                                               
	*/
	UFUNCTION(BlueprintCallable)
		void RebuildItemWidget(int32 ItemWidgetCount, bool bReconstrcut = true);

};
