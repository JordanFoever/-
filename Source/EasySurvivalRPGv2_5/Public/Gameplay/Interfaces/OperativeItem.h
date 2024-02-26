// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OperativeItem.generated.h"


// 该接口用来处理可操作的道具
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOperativeItem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class EASYSURVIVALRPGV2_5_API IOperativeItem
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
		void MainActionOperative(USkeletalMeshComponent* OperatingActorMesh);    // USkeletalMeshComponent 使用这个参数主要是为了动画
	UFUNCTION(BlueprintNativeEvent)
		bool CanBeOperated();               // 是否能够操作它
	UFUNCTION(BlueprintNativeEvent)
		void EndOperating();
};
