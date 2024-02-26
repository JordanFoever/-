// Easy Systems

#include "MenuMain/MainHUD.h"
#include "Global/GlobalDelegateManager.h"	

void UMainHUD::ModifyChracterStates(int ValueBeforeModify, int Value, const FString& StateTag)
{

	// ���÷���ί��    <FString,FString,int, int> ��һ���Ƿ���ֵ,����Ķ��ǲ���
	// ��ί���� CharacterStyleAvatar.cpp �� BeginPlay ������
	GlobalDelegateManager::Get()->CallGenericDelegate<void,FString,int, int>
		(MAINMENU_CHANGE_STYLE,StateTag, ValueBeforeModify,Value); // ���������ǵ��ñ��
}

bool UMainHUD::Initialize() 
{
	bool bSuper = Super::Initialize();

	// ��ί��,��ɫ���������ʽ�ı�ʱ���ã�����ί���� CharacterStyleAvatar.cpp �� 
	GlobalDelegateManager::Get()->BindGenericDelegateByString<void(const FString&,const FString&)>
		(NOTIFY_STYLE_CHANGE, this, &UMainHUD::OnCharacterStyleChanged);
	return bSuper;
} 