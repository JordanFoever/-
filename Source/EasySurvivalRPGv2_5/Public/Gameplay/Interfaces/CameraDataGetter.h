// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CameraDataGetter.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCameraDataGetter : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class EASYSURVIVALRPGV2_5_API ICameraDataGetter
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/*
	* 获取摄像机的投射数据
	* @Param WorldLocation 世界坐标
	* @Param  Orientation 朝向
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void GetCameraProjectData(FVector& WorldLocation, FRotator& Orientation);
};
