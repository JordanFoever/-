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
	
	InteractiveInformation->SetVisibility(ESlateVisibility::SelfHitTestInvisible);  // ���Ҳ����ʾ,ֻ�ǲ��ܱ�����⵽
	CurInteractiveActor = InteractiveActor;
	SetInteractiveInfo(Title, Summary, Tip); // ��ͼʵ�ָú���

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
		PC->ProjectWorldLocationToScreen(CurInteractiveActor->GetActorLocation(), ScreenLocation);  // double=> ����������Ͷ�䵽��Ļ��
		ScreenLocation /= UWidgetLayoutLibrary::GetViewportScale(GetWorld());                        // ��Ļ����ʱҲ�ڶ�Ӧ�ĵط� �ӿ����ű���
		InteractiveInformation->SetRenderTranslation(ScreenLocation);                                // double=>Translation and Transform
	}
}