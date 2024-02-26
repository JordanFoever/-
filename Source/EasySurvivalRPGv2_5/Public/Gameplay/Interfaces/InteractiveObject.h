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
	// note=> ��Ȼ�ڴ˴��Ľӿڿ���дʵ��,����Ҳ��Ҫд,��Ҫ�ƻ��ӿ�ԭ������������
	// �ú��� C++ ����ͼ������ʵ�ָýӿ�
	UFUNCTION(BlueprintNativeEvent)
		void BeginInteractive(APlayerController* Controller);        // ��ʼ����
	UFUNCTION(BlueprintNativeEvent)
		void EndInteractive(APlayerController* Controller);          // ��������
	UFUNCTION(BlueprintNativeEvent)
		void InteractiveResponse(APlayerController* Controller);     // ������Ķ���ķ�Ӧ
	UFUNCTION(BlueprintNativeEvent)
		bool GetInteractiveInformation(FString& Title,FString& Summary,FString& Tip);                            // �õ�������Ϣ��������ʾ
};
  