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
		TMap<FName,UDataTable*> TableMap;					                        // ���ǵ��Ժ������Ҫ���غܶ಻ͬ�ı��,��˴˴�ʹ�� TMap ���Է����Ժ����չ		

public:
	template<typename RowType>
	static RowType* GetTableRow(FName TableName,FName RowName);                     // ��ȡ���е�ĳһ�� 

	// �õ�������е�����
	template<typename RowType>
	static TSet<FName> GetTableAllRow(FName TableName);
	
	/*
	* ���ֶ����Ա��浽 json �ļ���
	* @Params Obj ��������
	* @Params FileFullPath �ļ����ڵľ���·��
	* @Params Flags ���������һ�� 64 λ�ı���,Ĭ�ϸ�һ�� 0 
	*/
	static void SaveObjectPropertiesToJsonFile(UObject* Obj, const TCHAR* FileFullPath, int64 Flags = 0i64);

	/*
	* �� json �ļ��ж�ȡ����
	* 
	* @Params Obj ��������
	* @Params FileFullPath �ļ����ڵľ���·���ĵ�ַ
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

	if (auto Table = CDO->TableMap.Find(TableName))    //  Find ���͵õ�����ֵ���͵�ָ�� 
	{
		// ���Ͳ�ƥ��
		if (!(*Table)->GetRowStruct()->IsChildOf(RowType::StaticStruct()))  return NULL;  //  �Զ���ָ������õõ�������͵�һ��ָ��

		if (auto RowPtr = (*Table)->GetRowMap().Find(RowName))                       // uint8 * const* Ptr const �������(�ڶ���) * ��ʾ����ָ���ڴ�ʱ��ʾ����ʱ���Խ�����, ǰ����Ǹ� * ��ʾ��һ��ʱ�Ͳ��ܽ�������
												// �ڴ˴� Find �õ�һ���ֽ�ָ���ָ�� ������֮��õ���ǰ��Ĳ��� unit8* const										
			return (RowType*)(*RowPtr);         // �ٽ����õõ��ľ���ֵ��
	}

	return NULL;
}

template<typename RowType>
inline static TSet<FName>  UCommonUtilLibrary::GetTableAllRow(FName TableName)
{
	TSet<FName> EmptySet;
	auto CDO = GetBlueprintCDO();
	if (!CDO) return EmptySet;

	if (auto Table = CDO->TableMap.Find(TableName))    //  Find ���͵õ�����ֵ���͵�ָ�� 
	{
		// ���Ͳ�ƥ��
		if (!(*Table)->GetRowStruct()->IsChildOf(RowType::StaticStruct()))  return EmptySet;  //  �Զ���ָ������õõ�������͵�һ��ָ��

		TSet<FName> OutKeys;
		if (auto RowPtr = (*Table)->GetRowMap().GetKeys(OutKeys))
			return OutKeys;
			
	}
	return EmptySet;
}
