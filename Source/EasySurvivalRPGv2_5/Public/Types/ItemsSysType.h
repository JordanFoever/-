// Easy Systems 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "ItemsSysType.generated.h"


// types of posture
UENUM(BlueprintType)
enum class EWeaponAndToolStanceType : uint8
{
	Common, OneHanded, TwoHanded, Spear
};


// 武器类型,用于确定一个可柴集的东西可以让那些武器柴集
UENUM(BlueprintType)
enum class EToolType : uint8
{
	None,            // None 表示它不是一个武器
	Hatchet, 
	Pickaxe, 
	Shovel,
	Stone,
	Mallet          // 木槌，用于拆建造物体
};


// make Item have a category,according to type to confirm gestures 
UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	None,
	Consumable,
	Tool,
	Component,
	Building,
	Placeable, // 建造时可以放置的物体
	Weapon, 
	Equipment, 
	Ammo, 
	All
};


// 物品有一些其它的物品组合建造出来 工艺列表的表结构
USTRUCT(BlueprintType)
struct FCraftingItemInfo : public FTableRowBase 
{
	GENERATED_BODY()
public:
	/*
	* the name of craft
	*/
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		FName CraftingItemRowName;

	/*
	* 该种工艺品由哪几个物品构建出来,以及每种物品的数量
	*/
	UPROPERTY(BlueprintReadOnly,EditAnywhere)
		TMap<FName,int32> RequestItemMap;

	/**
	* How much time does it take to build all items
	*/
	UPROPERTY(BlueprintReadOnly,EditAnywhere)
		float CraftingCostTime;
};

USTRUCT(BlueprintType) 
struct FItemDataTableRow : public FTableRowBase
{
	GENERATED_BODY()
		
public:
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		FName ItemName;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		FText Description;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		UStaticMesh* Model_SM;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		USkeletalMesh* Model_Sk;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		UTexture2D* IconTexture; 	 
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		int32 MaxStack;              // 最大堆叠数量 
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		EItemCategory Category;      // 道具的类型
};

//  table struct of weapon and tool
USTRUCT(BlueprintType) 
struct FWeaponAndToolTableRow : public FTableRowBase
{
	GENERATED_BODY()
		
public:
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		EWeaponAndToolStanceType StanceType;        // 姿势类型
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		UAnimSequence* MissAnim;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		UAnimSequence* HitAnim;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		float HitAnimDelay = .37f;                   // 击中动画延长时间
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		float Length;                                // 武器的长度
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		float TraceOffset;                           // 直接往检测的方向上进行偏移
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		float BaseDamage = 1.f;                      // 基础伤害
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		int DamageCount = 1;                          // 同时可以对几个敌人造成伤害
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		EToolType ToolType;                           // 武器类型

	/*UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UDestructibleMesh* DestrutibleMesh;*/
};

USTRUCT(BlueprintType)
struct FInventoryItemData 
{
	GENERATED_BODY()

public:
	FInventoryItemData() {}

	FInventoryItemData(FName ItemTableRowName, int32 ItemCount, float ItemDuraction = -1.f) 
		: TableRowName(ItemTableRowName), Count(ItemCount), Duration(ItemDuraction) {}

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		FName TableRowName = NAME_None;                 // 道具的名字（编号）
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		int32 Count = 0;                                // 数量
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		float Duration = -1.f;                          // 耐久度,负数表明没有该属性

	static const FInventoryItemData Empty;              // 声明该变量是为了可读性更强,并没有具体的意义,定义在源文件中

	FORCEINLINE bool IsEmpty() { return Count == 0 || TableRowName == NAME_None; }
};