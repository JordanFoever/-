// Easy Systems

#include "MenuMain/MainHUD.h"
#include "Global/GlobalDelegateManager.h"	

void UMainHUD::ModifyChracterStates(int ValueBeforeModify, int Value, const FString& StateTag)
{

	// 调用泛型委托    <FString,FString,int, int> 第一个是返回值,后面的都是参数
	// 绑定委托在 CharacterStyleAvatar.cpp 的 BeginPlay 函数中
	GlobalDelegateManager::Get()->CallGenericDelegate<void,FString,int, int>
		(MAINMENU_CHANGE_STYLE,StateTag, ValueBeforeModify,Value); // 括号里面是调用标记
}

bool UMainHUD::Initialize() 
{
	bool bSuper = Super::Initialize();

	// 绑定委托,角色的身体的样式改变时调用，调用委托在 CharacterStyleAvatar.cpp 中 
	GlobalDelegateManager::Get()->BindGenericDelegateByString<void(const FString&,const FString&)>
		(NOTIFY_STYLE_CHANGE, this, &UMainHUD::OnCharacterStyleChanged);
	return bSuper;
} 