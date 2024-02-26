// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "UDelegateComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FCommonDelegate, UObject*, Payload);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EASYSURVIVALRPGV2_5_API UDelegateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDelegateComponent();

protected:
	// Called when the game starts  
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		TMap<FGameplayTag, FCommonDelegate> DelegateMap;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		/// <summary>
		/// ��ί��
		/// </summary>
		/// <param name="DelegateTag">ί�е�����</param>
		/// <param name="Obj">���ú���������,��Ϊ������ͼ���Ͷ��� UObject ������</param>
		/// <param name="FunctionName">���ں�����ָ�벻֧�� UFUNCTION ���ֱ��ʹ�ú�����������Ƕ�̬ί�бȽ�����ĵط�</param>
		void BindDelegate(FGameplayTag DelegateTag, UObject* Obj, FName FunctionName);
	UFUNCTION(BlueprintCallable)
		void CallDelegate(FGameplayTag DelegateTag,UObject* payload);
};

// �����һ�����������ڻ�ȡ��ͼ�е�����
template<typename PropertyType>
PropertyType GetPropertyFromUObject(UObject* Obj,FString PropertyName) 
{

	/*
	* note=>
	*  1.������ UPROPERTY ��ǵ��ֶεķ�����Ϣ�������ڸ��ֶ��У�����������ģ�
	*	�Ǹ�������,����FIntProperty(�����������͵ķ�����Ϣ),�������ﲻ֪���������ʲô��������ø������͵ĵ�����
	*  2. ExcludeSuper ��ʾ����������ģ���������Ļ�����Ҫ����������̫����,û��Ҫ
	*/
	for (TFieldIterator<FProperty> It(Obj->GetClass(), EFieldIteratorFlags::ExcludeSuper);It; ++It)
	{
		auto PropertyPtr = *It;
		if (PropertyPtr->GetName() == PropertyName)
		{
			if (auto ValuePtr = PropertyPtr->ContainerPtrToValuePtr<PropertyType>(Obj))
				return *ValuePtr;
		}
	}

	return  PropertyType();			  
}

#define PROPERTY_GET(Type,Indentifier,Obj) Type Indentifier;\
Indentifier = GetPropertyFromUObject<Type>(Obj,TEXT(#Indentifier))            // #Indentifier ���Ǻ�ֻ��Ϊ��ת��Ϊ�ַ���
