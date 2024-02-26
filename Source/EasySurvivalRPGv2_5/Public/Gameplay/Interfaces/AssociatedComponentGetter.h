// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AssociatedComponentGetter.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAssociatedComponentGetter : public UInterface
{
	GENERATED_BODY()
};

class EASYSURVIVALRPGV2_5_API IAssociatedComponentGetter
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		class UInventoryComponent* GetInventoryComponent();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	    class UInventoryComponent* GetHotBarComponent();// 得到快捷栏组件

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		class UDelegateComponent* GetDelegateComponent(); 

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
		class UCraftingComponent* GetCraftingComponent();	
	
};
 