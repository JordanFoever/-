// Easy Systems


#include "Gameplay/UI/GameHUD.h"
#include "Global/GlobalDelegateManager.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UGameHUD::SetUpInteractiveInfo(bool bShow, AActor* InteractiveActor, FString Title, FString Summary, FString Tip)
{
	bInteractiveInfoShow = bShow;
	if (!bShow) 
	{
		InteractiveInformation->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	
	InteractiveInformation->SetVisibility(ESlateVisibility::SelfHitTestInvisible);  // 这个也是显示,只是不能被鼠标检测到
	CurInteractiveActor = InteractiveActor;
	SetInteractiveInfo(Title, Summary, Tip); // 蓝图实现该函数

}

void UGameHUD::NativeConstruct()
{
	Super::NativeConstruct();
	GlobalDelegateManager::Get()->BindGenericDelegateByString<void(bool, AActor*, FString, FString, FString)>
		(INTERACTIVE_SHOW_INFO, this, &UGameHUD::SetUpInteractiveInfo);

	/*GlobalDelegateManager::Get()->BindGenericDelegateByString<void(int)>
		(PLAYERINVENTORY_UPDATE_SLOT, this, &UGameHUD::UpdatePlayerInventorySlot);*/

}

void UGameHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) 
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateInteractiveInfo();
}

void UGameHUD::UpdateInteractiveInfo()
{
	if (!bInteractiveInfoShow || !IsValid(CurInteractiveActor)) return;

	if (auto PC = GetOwningPlayer()) 
	{
		FVector2D ScreenLocation;
		PC->ProjectWorldLocationToScreen(CurInteractiveActor->GetActorLocation(), ScreenLocation);  // double=> 将世界坐标投射到屏幕上
		ScreenLocation /= UWidgetLayoutLibrary::GetViewportScale(GetWorld());                        // 屏幕缩放时也在对应的地方 视口缩放比例
		InteractiveInformation->SetRenderTranslation(ScreenLocation);                                // double=>Translation and Transform
	}
}