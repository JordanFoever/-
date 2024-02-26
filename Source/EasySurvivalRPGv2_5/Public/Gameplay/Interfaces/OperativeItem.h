// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OperativeItem.generated.h"


// �ýӿ���������ɲ����ĵ���
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
		void MainActionOperative(USkeletalMeshComponent* OperatingActorMesh);    // USkeletalMeshComponent ʹ�����������Ҫ��Ϊ�˶���
	UFUNCTION(BlueprintNativeEvent)
		bool CanBeOperated();               // �Ƿ��ܹ�������
	UFUNCTION(BlueprintNativeEvent)
		void EndOperating();
};
