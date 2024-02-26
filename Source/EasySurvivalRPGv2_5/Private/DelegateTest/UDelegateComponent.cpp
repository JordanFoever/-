// Easy Systems


#include "DelegateTest/UDelegateComponent.h"

// Sets default values for this component's properties
UDelegateComponent::UDelegateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDelegateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDelegateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDelegateComponent::BindDelegate(FGameplayTag DelegateTag, UObject* Obj, FName FunctionName)
{
	if (!DelegateMap.Contains(DelegateTag))
		DelegateMap.Add(DelegateTag, FCommonDelegate());
	
	auto& DynamicDelegate = DelegateMap[DelegateTag];

	DynamicDelegate.BindUFunction(Obj, FunctionName);
}

void UDelegateComponent::CallDelegate(FGameplayTag DelegateTag, UObject* payload)
{
	if (!DelegateMap.Contains(DelegateTag))return;

	DelegateMap[DelegateTag].ExecuteIfBound(payload);
}

