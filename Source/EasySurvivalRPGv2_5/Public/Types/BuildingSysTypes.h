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
	SameMaterial,         // ��ͬ�Ĳ���
	SpecificMaterials,    // �ض��Ĳ���
	SpecificPartTypes,    // ��������

};

UENUM(BlueprintType)
enum EBuildingMaterial 
{
	Wood,
	Stone
};

// Բ�εĽ���˵�����
UENUM(BlueprintType)
enum ECircleBuildingMenuType
{
	WoodBuildingMenu,     // ľ�ƽ���˵�
	StoneBuildingMenu,    // ʯͷ����˵�
	PlaceableBuildingMenu // �ɷ��õĽ���˵�
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
	// �ɷ��õ��ߵ�����
	Campfire,
	Bed,
	Dummy,
	Table,
	Carpet,       // ��̺
	Paint,        // ��
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
* �������ݱ�ṹ
*/
USTRUCT(BlueprintType)
struct FBuildingTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UDestructibleMesh* BuildingPartDestructibleMesh;   // ���첿�������ٵ�����

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsHaveDestructibleMesh = true;							 // ���첿���Ƿ������

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bAttachToLandscape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bRotatorFourDirection = false;

	// �������ܷ���ת
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bAttachCanRotator = true;

	// �Ƿ��жϺ��ܷ�����ȥ�ļ�⽨�첿�����ص�����Ҫ���¥��
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
	* ������ÿ�����͵Ľ��첿�����Ծ�����������ò�������Щ�������
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<TEnumAsByte<EBuildingPartType>, FAttachTransformList> RelativeAttachTransformMap;

	// ����˵����� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<ECircleBuildingMenuType> CircleBuildingType;

	//  ���첿����Ҫ���������Լ����� ����ľ�ʵĵػ���Ҫ����ľͷ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FName, int32> RequestItemMap;
};

// �Ѿ������������Ҫ��������ͽṹ
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
		FName RowName = NAME_None;           // ����
	UPROPERTY(EditAnywhere)
		FVector PlaceLocation;   // ���첿�����õ�λ��
	UPROPERTY(EditAnywhere)
		FRotator PlaceRotator;   // ����
};