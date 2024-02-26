// Easy Systems

#include "Utils/CommonUtilLibrary.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"
#include "JsonUtilities/Public/JsonUtilities.h"


UCommonUtilLibrary* UCommonUtilLibrary::BlueprintCDO = NULL;

UCommonUtilLibrary* UCommonUtilLibrary::GetBlueprintCDO()
{
	if (!BlueprintCDO)
		// note => 除了加载蓝图外使用 LoadClass 其它的全部使用 LoadObject
		if (UClass *Class = LoadClass<UCommonUtilLibrary>(NULL, TEXT("Blueprint'/Game/PirateSurvival/Blueprints/GameFramework/BP_CommonUtilLibrary.BP_CommonUtilLibrary_C'"))) // 同步加载   
			return BlueprintCDO = Cast<UCommonUtilLibrary>(Class->GetDefaultObject());
	return BlueprintCDO;
}

void UCommonUtilLibrary::SaveObjectPropertiesToJsonFile(UObject* Obj, const TCHAR* FileFullPath, int64 Flags)
{
	/*
		note = > 或运算两个都为 0 才为0。下面这种写法就可以很好的通过 64 位的方式让 Flags 可以保存多个数据（也就是将枚举当成数组使用）。位运算的消耗会极低。
		int64 Flags = CPF_BlueprintCallable | CPF_BlueprintReadOnly;  CPF 是 Class Property Flag (类属性标记的简写)
			
	*/
	//UClass* Class = Obj->GetClass();
	TSharedRef<FJsonObject> JsonObj = MakeShareable(new FJsonObject);

	// 通过字段迭代器遍历字段 It 的 bool 值已经被重写了因此可以代表是否有效 
	for (TFieldIterator<FProperty> It(Obj->GetClass()); It; ++It) // 前面是构造器的简写方法
	{
		auto Property = *It;                                      // 对迭代器进行解引用得到需要迭代的东西
		if (Property->GetMetaData(TEXT("SaveToJson")) != TEXT("True")) continue;


		auto Value = FJsonObjectConverter::UPropertyToJsonValue(Property, Property->ContainerPtrToValuePtr<void>(Obj), Flags, 0i64);
		JsonObj->Values.Add(Property->GetName(), Value);
	}
	// 将 JsonObj 转换成 JsonStr 存到文件中
	FString JsonStr;
	FJsonSerializer::Serialize(JsonObj, TJsonWriterFactory<>::Create(&JsonStr));
	FFileHelper::SaveStringToFile(JsonStr, FileFullPath);
}

void UCommonUtilLibrary::LoadObjectPropertiesFromJsonFile(UObject* Obj, const TCHAR* FileFullPath)
{
	FString JsonStr;
	if (!FFileHelper::LoadFileToString(JsonStr, FileFullPath))return;

	TSharedPtr<FJsonObject> JsonObj;
	FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(JsonStr), JsonObj);

	for (TFieldIterator<FProperty> It(Obj->GetClass()); It; ++It) 
	{
		auto Property = *It;
		if (auto JsonValuePtr = JsonObj->Values.Find(Property->GetName())) 
		{
			void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Obj);
			FJsonObjectConverter::JsonValueToUProperty(*JsonValuePtr, Property, ValuePtr, 0, 0);
		}
	}
}

void UCommonUtilLibrary::ClearFile(const TCHAR* FileFullPath)
{
	FFileHelper::SaveStringToFile(TEXT(""), FileFullPath);
}

