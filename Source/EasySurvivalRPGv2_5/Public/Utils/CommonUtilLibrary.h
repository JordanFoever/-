// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

#include "CommonUtilLibrary.generated.h"

#define ITEM_TABLE_NAME TEXT("ItemTable")
#define WEAPON_TABLE_NAME TEXT("WeaponAndToolTable")
#define BUILDING_TABLE_NAME TEXT("BuildingTable")
/**
 * 
 */
UCLASS(Blueprintable)
class EASYSURVIVALRPGV2_5_API UCommonUtilLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static UCommonUtilLibrary* BlueprintCDO;
	static UCommonUtilLibrary* GetBlueprintCDO();

protected:
	UPROPERTY(EditAnywhere)
		TMap<FName,UDataTable*> TableMap;					                        // 考虑到以后可能需要加载很多不同的表格,因此此处使用 TMap 可以方便以后的拓展		

public:
	template<typename RowType>
	static RowType* GetTableRow(FName TableName,FName RowName);                     // 获取表中的某一行 

	// 得到表的所有的主键
	template<typename RowType>
	static TSet<FName> GetTableAllRow(FName TableName);
	
	/*
	* 将字段属性保存到 json 文件中
	* @Params Obj 对象类型
	* @Params FileFullPath 文件所在的绝对路径
	* @Params Flags 在这里代表一个 64 位的变量,默认给一个 0 
	*/
	static void SaveObjectPropertiesToJsonFile(UObject* Obj, const TCHAR* FileFullPath, int64 Flags = 0i64);

	/*
	* 从 json 文件中读取数据
	* 
	* @Params Obj 对象类型
	* @Params FileFullPath 文件所在的绝对路劲的地址
	*/
	static void LoadObjectPropertiesFromJsonFile(UObject* Obj, const TCHAR* FileFullPath);

	static void ClearFile(const TCHAR* FileFullPath);

};
template<typename RowType>
inline static RowType* UCommonUtilLibrary::GetTableRow(FName TableName, FName RowName)
{
	check(TableName != NAME_None);
	check(RowName != NAME_None);

	auto CDO = GetBlueprintCDO();
	if (!CDO) return NULL;

	if (auto Table = CDO->TableMap.Find(TableName))    //  Find 类型得到的是值类型的指针 
	{
		// 类型不匹配
		if (!(*Table)->GetRowStruct()->IsChildOf(RowType::StaticStruct()))  return NULL;  //  对二级指针解引用得到表格类型的一级指针

		if (auto RowPtr = (*Table)->GetRowMap().Find(RowName))                       // uint8 * const* Ptr const 后面这个(第二个) * 表示二级指针在此时表示二级时可以解引用, 前面的那个 * 表示到一级时就不能解引用了
												// 在此处 Find 得到一个字节指针的指针 解引用之后得到是前面的部分 unit8* const										
			return (RowType*)(*RowPtr);         // 再解引用得到的就是值了
	}

	return NULL;
}

template<typename RowType>
inline static TSet<FName>  UCommonUtilLibrary::GetTableAllRow(FName TableName)
{
	TSet<FName> EmptySet;
	auto CDO = GetBlueprintCDO();
	if (!CDO) return EmptySet;

	if (auto Table = CDO->TableMap.Find(TableName))    //  Find 类型得到的是值类型的指针 
	{
		// 类型不匹配
		if (!(*Table)->GetRowStruct()->IsChildOf(RowType::StaticStruct()))  return EmptySet;  //  对二级指针解引用得到表格类型的一级指针

		TSet<FName> OutKeys;
		if (auto RowPtr = (*Table)->GetRowMap().GetKeys(OutKeys))
			return OutKeys;
			
	}
	return EmptySet;
}
