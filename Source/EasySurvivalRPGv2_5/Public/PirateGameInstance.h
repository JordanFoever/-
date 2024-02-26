#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PirateGameInstance.generated.h"

UCLASS()
class EASYSURVIVALRPGV2_5_API UPirateGameInstance : public UGameInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly)
		TSoftObjectPtr<UWorld> TestLevel;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UUserWidget> LoadingWidgetClass;  // ���ػ���

	// �ڿ�ʼҳ�����Ƿ�ѡ�������Ϸ,���Ƿ���ش浵ʱʹ��
	bool bClickContinueGame = false;

	void AsyncOpenLevel(TSoftObjectPtr<UWorld> NewLevel);
	TSharedPtr<struct FStreamableHandle> Handle;
	UUserWidget* LoadingWidget;
public:
	UFUNCTION(BlueprintCallable)
		void OpenTestLevel();

	UFUNCTION(BlueprintCallable)
		void SetClickContinueGame(bool bInClickContinueGame);

	UFUNCTION(BlueprintCallable)
		bool GetClickContinueGame();

	void LoadLevelComplete();
};
