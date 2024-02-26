// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameHUD.generated.h"

UCLASS()
class EASYSURVIVALRPGV2_5_API UGameHUD : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly,meta = (BindWidget))
		UUserWidget* InteractiveInformation;              // ����ʱ��ʾ�Ľ��������ʾ

	void SetUpInteractiveInfo(bool bShow, AActor* InteractiveActor, FString Title, FString Summary, FString Tip);

	UFUNCTION(BlueprintImplementableEvent)
		void SetInteractiveInfo(const FString& Title,const FString& Summary,const FString& Tip);    // note=> UFUNCTION�еĺ�������Ҫô�ǳ�������Ҫô����ͨ����

	UFUNCTION(BlueprintImplementableEvent)
		void UpdatePlayerInventorySlot(int32 Index);      // ����ͼʵ��
	 
private:
	bool bInteractiveInfoShow;  // �Ƿ���ʾ����Ϣ
	AActor* CurInteractiveActor;   // �����Ķ��� 

	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;  // override from UUserWidget
	void UpdateInteractiveInfo();
};
