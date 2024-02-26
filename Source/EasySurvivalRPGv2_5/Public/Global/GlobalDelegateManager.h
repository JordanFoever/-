#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"

// MainMenu 
#define MAINMENU_CHANGE_STYLE		FGameplayTag::RequestGameplayTag("GlobalEvent.MainMenu.MainMenuChangeStyle")
#define NOTIFY_STYLE_CHANGE			FGameplayTag::RequestGameplayTag("GlobalEvent.MainMenu.NotifyStyleChanged")	

// Game
#define INTERACTIVE_SHOW_INFO       FGameplayTag::RequestGameplayTag(TEXT("GlobalEvent.Interactive.ShowInteractiveInfo"))
#define PLAYERINVENTORY_UPDATE_SLOT FGameplayTag::RequestGameplayTag(TEXT("GlobalEvent.PlayerInventory.UpdateSlot"))   // �Ѿ�������ί������ķ�ʽ��

#define PLAYERINVENTORY_SWAP_SLOTS   FGameplayTag::RequestGameplayTag(TEXT("GlobalEvent.PlayerInventory.SwapSlots"))    // �Ѿ�������ί������ķ�ʽ��
#define PLAYERINVENTORY_SPLIT_ITEMS   FGameplayTag::RequestGameplayTag(TEXT("GlobalEvent.PlayerInventory.SplitItems"))  // �Ѿ�������ί������ķ�ʽ��

// Building
#define NOTIFY_DESTORY_BUILDINGPART FGameplayTag::RequestGameplayTag(TEXT("GlobalEvent.BuildingSystem.NotifyDestory"))  // ֪ͨ���ٽ������
/**
 * ȫ��ί�й�����
 */
class EASYSURVIVALRPGV2_5_API GlobalDelegateManager
{
private:
	GlobalDelegateManager(); 
	GlobalDelegateManager(const GlobalDelegateManager&);
	GlobalDelegateManager(GlobalDelegateManager&&);           // ��ֵ����

	
	static GlobalDelegateManager* Singleton;                  // ��̬��Ա�������������������г�ʼ��

	TMap<FGameplayTag, FSimpleDelegate> SimpleDelegateMap;          // TDelegate<void()>����д��FSimpleDelegate �޲��޷���һ���ܼ򵥵�ί��
	TMap<FGameplayTag, TSharedPtr<TDelegateBase<FDefaultDelegateUserPolicy>>> GenericDelegateMap; // ͨ��ί�� TSharedPtr ����ָ��
public:
	
	static	GlobalDelegateManager* Get();

	/// <summary>
	///  ��ί�� 
	/// </summary>
	/// <param name="KeyString"></param>
	/// <param name="Func"></param>
	/// <param name="bOverride">�Ƿ񸲸�,��Ϊ�ǵ���ί�У������ظ���Ĭ���Ǹ��ǵ�</param>
	template<class UserClass>
	void BindSimpleDelegateByString(FGameplayTag KeyString,UserClass* Obj, typename TMemFunPtrType<false, UserClass, void()>::Type Func, bool bOverride = true);

	template<typename DelegateSigniture,typename UserClass>  // note=> 1.typename <=> class     2.ParamsType...(�ɱ����) ������д��ǰ���ʾ�ɱ�����������ʾ����չ�������ֲ���ֻ��д�����
	void BindGenericDelegateByString(FGameplayTag KeyString, UserClass* Obj, typename TMemFunPtrType<false, UserClass, DelegateSigniture>::Type Func, bool bOverride = true);
	
	void CallSimpleDelegate(FGameplayTag KeyString);      // ����ί��

	template<typename RetType, typename... ParamsTypes>
	RetType CallGenericDelegate(FGameplayTag KeyString, ParamsTypes... Params);
};

template<class UserClass>
inline void GlobalDelegateManager::BindSimpleDelegateByString(FGameplayTag KeyString, UserClass* Obj,typename TMemFunPtrType<false,UserClass,void()>::Type Func, bool bOverride)
{
	//UKismetSystemLibrary::PrintString(GetWorld()); 
	// �����д��뱣֤ί���Ǵ��ڵ�
	auto DelegatePtr = SimpleDelegateMap.Find(KeyString); // �ҵ���һ��ί�е�ָ��

	// ˵���ǿյ�,����������ȥ
	if (!DelegatePtr) SimpleDelegateMap.Add(KeyString, FSimpleDelegate());   
	DelegatePtr = &SimpleDelegateMap[KeyString];

	if (DelegatePtr->IsBound() && bOverride) DelegatePtr->Unbind();  // ί���Ѿ�������,������Ҫ������,�Ǿ������
	if (!DelegatePtr->IsBound()) DelegatePtr->BindUObject(Obj, Func);
}

template<typename DelegateSigniture,typename UserClass>
inline void GlobalDelegateManager::BindGenericDelegateByString(FGameplayTag KeyString, UserClass* Obj, typename TMemFunPtrType<false, UserClass, DelegateSigniture>::Type Func, bool bOverride)
{
	typedef TDelegate<DelegateSigniture> ExactDelegateType;
	auto DelegatePtr = GenericDelegateMap.Find(KeyString);       // DelegatePtr ��ʱ��ָ����ָ���ָ�� 
	if (!DelegatePtr) GenericDelegateMap.Add(KeyString, MakeShareable(new ExactDelegateType));   // MakeShareable �����ݴ浽����ָ�����
	DelegatePtr = &GenericDelegateMap[KeyString];

	const auto SpecifyDelegate = (ExactDelegateType*)DelegatePtr->Get();

	if (SpecifyDelegate->IsBound() && bOverride) SpecifyDelegate->Unbind();
	if (!SpecifyDelegate->IsBound()) SpecifyDelegate->BindUObject(Obj, Func);
}

template<typename RetType, typename ...ParamsTypes>
inline RetType GlobalDelegateManager::CallGenericDelegate(FGameplayTag KeyString, ParamsTypes ...Params)
{
	if (auto DelegatePtr = GenericDelegateMap.Find(KeyString))
	{
		auto Delegate = (TDelegate<RetType(ParamsTypes...)>*)DelegatePtr->Get();
		if (Delegate->IsBound())
			return Delegate->Execute(Params...);
	}
	return RetType();
}
