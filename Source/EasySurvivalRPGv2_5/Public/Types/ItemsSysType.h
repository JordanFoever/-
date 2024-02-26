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


// ��������,����ȷ��һ���ɲ񼯵Ķ�����������Щ������
UENUM(BlueprintType)
enum class EToolType : uint8
{
	None,            // None ��ʾ������һ������
	Hatchet, 
	Pickaxe, 
	Shovel,
	Stone,
	Mallet          // ľ鳣����ڲ�������
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
	Placeable, // ����ʱ���Է��õ�����
	Weapon, 
	Equipment, 
	Ammo, 
	All
};


// ��Ʒ��һЩ��������Ʒ��Ͻ������ �����б�ı�ṹ
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
	* ���ֹ���Ʒ���ļ�����Ʒ��������,�Լ�ÿ����Ʒ������
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
		int32 MaxStack;              // ���ѵ����� 
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		EItemCategory Category;      // ���ߵ�����
};

//  table struct of weapon and tool
USTRUCT(BlueprintType) 
struct FWeaponAndToolTableRow : public FTableRowBase
{
	GENERATED_BODY()
		
public:
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		EWeaponAndToolStanceType StanceType;        // ��������
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		UAnimSequence* MissAnim;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		UAnimSequence* HitAnim;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		float HitAnimDelay = .37f;                   // ���ж����ӳ�ʱ��
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		float Length;                                // �����ĳ���
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		float TraceOffset;                           // ֱ�������ķ����Ͻ���ƫ��
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		float BaseDamage = 1.f;                      // �����˺�
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		int DamageCount = 1;                          // ͬʱ���ԶԼ�����������˺�
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		EToolType ToolType;                           // ��������

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
		FName TableRowName = NAME_None;                 // ���ߵ����֣���ţ�
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		int32 Count = 0;                                // ����
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
		float Duration = -1.f;                          // �;ö�,��������û�и�����

	static const FInventoryItemData Empty;              // �����ñ�����Ϊ�˿ɶ��Ը�ǿ,��û�о��������,������Դ�ļ���

	FORCEINLINE bool IsEmpty() { return Count == 0 || TableRowName == NAME_None; }
};