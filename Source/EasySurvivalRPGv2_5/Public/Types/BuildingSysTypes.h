// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DestructibleComponent.h"
#include "BuildingSysTypes.generated.h"

UENUM(BlueprintType)
enum EBuildingAttachRule 
{
	None,
	SameMaterial,         // 相同的材质
	SpecificMaterials,    // 特定的材质
	SpecificPartTypes,    // 部件类型

};

UENUM(BlueprintType)
enum EBuildingMaterial 
{
	Wood,
	Stone
};

// 圆形的建造菜单类型
UENUM(BlueprintType)
enum ECircleBuildingMenuType
{
	WoodBuildingMenu,     // 木制建造菜单
	StoneBuildingMenu,    // 石头建造菜单
	PlaceableBuildingMenu // 可放置的建造菜单
};

UENUM(BlueprintType)
enum EBuildingPartType
{
	Ceiling,
	TriCeiling,
	DoorFrame,
	Door,
	Fence,
	Foundation,
	TriFoundation,
	Roof,
	Ramp,
	Stair,
	Wall,
	TriTopRoofWall,
	TriLeftRoofWall,
	TriRightRoofWall,
	WindowFrame,
	Window,
	// 可放置道具的类型
	Campfire,
	Bed,
	Dummy,
	Table,
	Carpet,       // 地毯
	Paint,        // 画
};

USTRUCT(BlueprintType)
struct FAttachTransformList 
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FTransform> TransformList;
};

/*
* 建造数据表结构
*/
USTRUCT(BlueprintType)
struct FBuildingTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UDestructibleMesh* BuildingPartDestructibleMesh;   // 建造部件可销毁的网格

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsHaveDestructibleMesh = true;							 // 建造部件是否可销毁

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bAttachToLandscape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bRotatorFourDirection = false;

	// 吸附后能否旋转
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bAttachCanRotator = true;

	// 是否判断和能放置上去的检测建造部件的重叠，主要针对楼梯
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsCheckOverlay = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EBuildingAttachRule>  AttachRule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EBuildingMaterial>  Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EBuildingPartType> Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSet<TEnumAsByte<EBuildingMaterial>> AttachMaterialSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSet<TEnumAsByte<EBuildingPartType>> AttachTypeSet;

	/*
	* 吸附到每个类型的建造部件可以具体的吸附到该部件中哪些相对坐标
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<TEnumAsByte<EBuildingPartType>, FAttachTransformList> RelativeAttachTransformMap;

	// 建造菜单类型 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<ECircleBuildingMenuType> CircleBuildingType;

	//  建造部件需要的配件组合以及数量 比如木质的地基需要多少木头
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FName, int32> RequestItemMap;
};

// 已经建造的物体需要保存的类型结构
USTRUCT(BlueprintType)
struct FPlacementBuildingInfo 
{
	GENERATED_BODY()

public:
	FPlacementBuildingInfo() {}

	FPlacementBuildingInfo(FName InRowName, FVector InPlaceLocation, FRotator InPlaceRotator) :
		RowName(InRowName), PlaceLocation(InPlaceLocation), PlaceRotator(InPlaceRotator) {} 

	bool operator==(FPlacementBuildingInfo PlacementBuildingInfo)
	{
		if (PlacementBuildingInfo.RowName == this->RowName && PlacementBuildingInfo.PlaceLocation == this->PlaceLocation
			&& PlacementBuildingInfo.PlaceRotator == this->PlaceRotator)
			return true;
		return false;
		
	}

	UPROPERTY(EditAnywhere)
		FName RowName = NAME_None;           // 主键
	UPROPERTY(EditAnywhere)
		FVector PlaceLocation;   // 建造部件放置的位置
	UPROPERTY(EditAnywhere)
		FRotator PlaceRotator;   // 方向
};