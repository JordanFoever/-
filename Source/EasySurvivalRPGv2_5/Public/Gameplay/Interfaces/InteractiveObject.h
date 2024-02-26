// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractiveObject.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractiveObject : public UInterface
{
	GENERATED_BODY()
};

class EASYSURVIVALRPGV2_5_API IInteractiveObject
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// note=> 虽然在此处的接口可以写实现,但是也不要写,不要破坏接口原本玩的设计理念
	// 该宏标记 C++ 和蓝图都可以实现该接口
	UFUNCTION(BlueprintNativeEvent)
		void BeginInteractive(APlayerController* Controller);        // 开始交互
	UFUNCTION(BlueprintNativeEvent)
		void EndInteractive(APlayerController* Controller);          // 结束交互
	UFUNCTION(BlueprintNativeEvent)
		void InteractiveResponse(APlayerController* Controller);     // 交互后的对面的反应
	UFUNCTION(BlueprintNativeEvent)
		bool GetInteractiveInformation(FString& Title,FString& Summary,FString& Tip);                            // 得到交互信息的文字提示
};
  