#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"

// MainMenu 
#define MAINMENU_CHANGE_STYLE		FGameplayTag::RequestGameplayTag("GlobalEvent.MainMenu.MainMenuChangeStyle")
#define NOTIFY_STYLE_CHANGE			FGameplayTag::RequestGameplayTag("GlobalEvent.MainMenu.NotifyStyleChanged")	

// Game
#define INTERACTIVE_SHOW_INFO       FGameplayTag::RequestGameplayTag(TEXT("GlobalEvent.Interactive.ShowInteractiveInfo"))
#define PLAYERINVENTORY_UPDATE_SLOT FGameplayTag::RequestGameplayTag(TEXT("GlobalEvent.PlayerInventory.UpdateSlot"))   // 已经换成了委托组件的方式了

#define PLAYERINVENTORY_SWAP_SLOTS   FGameplayTag::RequestGameplayTag(TEXT("GlobalEvent.PlayerInventory.SwapSlots"))    // 已经换成了委托组件的方式了
#define PLAYERINVENTORY_SPLIT_ITEMS   FGameplayTag::RequestGameplayTag(TEXT("GlobalEvent.PlayerInventory.SplitItems"))  // 已经换成了委托组件的方式了

// Building
#define NOTIFY_DESTORY_BUILDINGPART FGameplayTag::RequestGameplayTag(TEXT("GlobalEvent.BuildingSystem.NotifyDestory"))  // 通知销毁建造组件
/**
 * 全局委托管理器
 */
class EASYSURVIVALRPGV2_5_API GlobalDelegateManager
{
private:
	GlobalDelegateManager(); 
	GlobalDelegateManager(const GlobalDelegateManager&);
	GlobalDelegateManager(GlobalDelegateManager&&);           // 右值引用

	
	static GlobalDelegateManager* Singleton;                  // 静态成员变量必须在类的外面进行初始化

	TMap<FGameplayTag, FSimpleDelegate> SimpleDelegateMap;          // TDelegate<void()>或者写成FSimpleDelegate 无参无返回一个很简单的委托
	TMap<FGameplayTag, TSharedPtr<TDelegateBase<FDefaultDelegateUserPolicy>>> GenericDelegateMap; // 通用委托 TSharedPtr 共享指针
public:
	
	static	GlobalDelegateManager* Get();

	/// <summary>
	///  绑定委托 
	/// </summary>
	/// <param name="KeyString"></param>
	/// <param name="Func"></param>
	/// <param name="bOverride">是否覆盖,因为是单波委托，避免重复，默认是覆盖的</param>
	template<class UserClass>
	void BindSimpleDelegateByString(FGameplayTag KeyString,UserClass* Obj, typename TMemFunPtrType<false, UserClass, void()>::Type Func, bool bOverride = true);

	template<typename DelegateSigniture,typename UserClass>  // note=> 1.typename <=> class     2.ParamsType...(可变参数) 三个点写在前面表示可变参数，后面表示参数展开，这种参数只能写在最后
	void BindGenericDelegateByString(FGameplayTag KeyString, UserClass* Obj, typename TMemFunPtrType<false, UserClass, DelegateSigniture>::Type Func, bool bOverride = true);
	
	void CallSimpleDelegate(FGameplayTag KeyString);      // 调用委托

	template<typename RetType, typename... ParamsTypes>
	RetType CallGenericDelegate(FGameplayTag KeyString, ParamsTypes... Params);
};

template<class UserClass>
inline void GlobalDelegateManager::BindSimpleDelegateByString(FGameplayTag KeyString, UserClass* Obj,typename TMemFunPtrType<false,UserClass,void()>::Type Func, bool bOverride)
{
	//UKismetSystemLibrary::PrintString(GetWorld()); 
	// 这三行代码保证委托是存在的
	auto DelegatePtr = SimpleDelegateMap.Find(KeyString); // 找到的一个委托的指针

	// 说明是空的,并将其加入进去
	if (!DelegatePtr) SimpleDelegateMap.Add(KeyString, FSimpleDelegate());   
	DelegatePtr = &SimpleDelegateMap[KeyString];

	if (DelegatePtr->IsBound() && bOverride) DelegatePtr->Unbind();  // 委托已经被绑定了,并且我要覆盖它,那就清除绑定
	if (!DelegatePtr->IsBound()) DelegatePtr->BindUObject(Obj, Func);
}

template<typename DelegateSigniture,typename UserClass>
inline void GlobalDelegateManager::BindGenericDelegateByString(FGameplayTag KeyString, UserClass* Obj, typename TMemFunPtrType<false, UserClass, DelegateSigniture>::Type Func, bool bOverride)
{
	typedef TDelegate<DelegateSigniture> ExactDelegateType;
	auto DelegatePtr = GenericDelegateMap.Find(KeyString);       // DelegatePtr 此时是指向共享指针的指针 
	if (!DelegatePtr) GenericDelegateMap.Add(KeyString, MakeShareable(new ExactDelegateType));   // MakeShareable 将数据存到共享指针库中
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
