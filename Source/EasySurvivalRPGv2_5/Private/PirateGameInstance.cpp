// Easy Systems


#include "PirateGameInstance.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "MenuMain/MainMenuSaveGame.h"
#include "MenuMain/CharacterStyleAvatar.h"

void UPirateGameInstance::AsyncOpenLevel(TSoftObjectPtr<UWorld> NewLevel)
{
	/*if (!IsValid(LoadingWidget)) LoadingWidget = CreateWidget<UUserWidget>(this, LoadingWidgetClass);
	LoadingWidget->AddToViewport();*/
	FStreamableDelegate Delegate;
	Delegate.BindUObject(this, &UPirateGameInstance::LoadLevelComplete);
	// “Ï≤Ωº”‘ÿ
	Handle = UAssetManager::GetStreamableManager().RequestAsyncLoad(NewLevel.ToSoftObjectPath(), Delegate);
}

void UPirateGameInstance::OpenTestLevel()
{
	AsyncOpenLevel(TestLevel);
}

void UPirateGameInstance::SetClickContinueGame(bool bInClickContinueGame)
{
	bClickContinueGame = bInClickContinueGame;
}

bool UPirateGameInstance::GetClickContinueGame()
{
	return  bClickContinueGame;
}

void UPirateGameInstance::LoadLevelComplete()
{
	if (!Handle.IsValid())return;

	if (auto Level = Cast<UWorld>(Handle.Get()->GetLoadedAsset()))
	{
		UKismetSystemLibrary::PrintString(GetWorld(), Level->GetName());
		UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), Level);
	}
}