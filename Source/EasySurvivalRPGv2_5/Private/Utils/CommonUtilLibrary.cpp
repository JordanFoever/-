// Easy Systems

#include "Utils/CommonUtilLibrary.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"
#include "JsonUtilities/Public/JsonUtilities.h"


UCommonUtilLibrary* UCommonUtilLibrary::BlueprintCDO = NULL;

UCommonUtilLibrary* UCommonUtilLibrary::GetBlueprintCDO()
{
	if (!BlueprintCDO)
		// note => ���˼�����ͼ��ʹ�� LoadClass ������ȫ��ʹ�� LoadObject
		if (UClass *Class = LoadClass<UCommonUtilLibrary>(NULL, TEXT("Blueprint'/Game/PirateSurvival/Blueprints/GameFramework/BP_CommonUtilLibrary.BP_CommonUtilLibrary_C'"))) // ͬ������   
			return BlueprintCDO = Cast<UCommonUtilLibrary>(Class->GetDefaultObject());
	return BlueprintCDO;
}

void UCommonUtilLibrary::SaveObjectPropertiesToJsonFile(UObject* Obj, const TCHAR* FileFullPath, int64 Flags)
{
	/*
		note = > ������������Ϊ 0 ��Ϊ0����������д���Ϳ��Ժܺõ�ͨ�� 64 λ�ķ�ʽ�� Flags ���Ա��������ݣ�Ҳ���ǽ�ö�ٵ�������ʹ�ã���λ��������ĻἫ�͡�
		int64 Flags = CPF_BlueprintCallable | CPF_BlueprintReadOnly;  CPF �� Class Property Flag (�����Ա�ǵļ�д)
			
	*/
	//UClass* Class = Obj->GetClass();
	TSharedRef<FJsonObject> JsonObj = MakeShareable(new FJsonObject);

	// ͨ���ֶε����������ֶ� It �� bool ֵ�Ѿ�����д����˿��Դ����Ƿ���Ч 
	for (TFieldIterator<FProperty> It(Obj->GetClass()); It; ++It) // ǰ���ǹ������ļ�д����
	{
		auto Property = *It;                                      // �Ե��������н����õõ���Ҫ�����Ķ���
		if (Property->GetMetaData(TEXT("SaveToJson")) != TEXT("True")) continue;


		auto Value = FJsonObjectConverter::UPropertyToJsonValue(Property, Property->ContainerPtrToValuePtr<void>(Obj), Flags, 0i64);
		JsonObj->Values.Add(Property->GetName(), Value);
	}
	// �� JsonObj ת���� JsonStr �浽�ļ���
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

