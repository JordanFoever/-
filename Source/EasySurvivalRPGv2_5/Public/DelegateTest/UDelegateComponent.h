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
		/// 绑定委托
		/// </summary>
		/// <param name="DelegateTag">委托的名字</param>
		/// <param name="Obj">调用函数的主体,因为所有蓝图类型都是 UObject 的子类</param>
		/// <param name="FunctionName">由于函数的指针不支持 UFUNCTION 因此直接使用函数名，这个是动态委托比较特殊的地方</param>
		void BindDelegate(FGameplayTag DelegateTag, UObject* Obj, FName FunctionName);
	UFUNCTION(BlueprintCallable)
		void CallDelegate(FGameplayTag DelegateTag,UObject* payload);
};

// 这个是一个工具类用于获取蓝图中的数据
template<typename PropertyType>
PropertyType GetPropertyFromUObject(UObject* Obj,FString PropertyName) 
{

	/*
	* note=>
	*  1.所用用 UPROPERTY 标记的字段的反射信息都保存在该字段中，它是有子类的，
	*	是各种类型,比如FIntProperty(保存整数类型的反射信息),由于这里不知道传入的是什么类型因此用父类类型的迭代器
	*  2. ExcludeSuper 表示不包含父类的，包含父类的话，需要迭代的数据太多了,没必要
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
Indentifier = GetPropertyFromUObject<Type>(Obj,TEXT(#Indentifier))            // #Indentifier 加星号只是为了转换为字符串
