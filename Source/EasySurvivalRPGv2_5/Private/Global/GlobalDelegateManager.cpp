// Easy Systems


#include "Global/GlobalDelegateManager.h"



GlobalDelegateManager* GlobalDelegateManager::Singleton = NULL;


GlobalDelegateManager::GlobalDelegateManager()
{

}

GlobalDelegateManager* GlobalDelegateManager::Get()
{
	return Singleton == NULL ? Singleton = new GlobalDelegateManager : Singleton;
}

void GlobalDelegateManager::CallSimpleDelegate(FGameplayTag KeyString)
{
	if (auto Delegate = SimpleDelegateMap.Find(KeyString))
		Delegate->ExecuteIfBound();
}
