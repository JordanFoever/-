// Easy Systems 


#include "Gameplay/PiratePlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Interfaces/InteractiveObject.h"
#include "Global/GlobalDelegateManager.h"
#include "Gameplay/Interfaces/AssociatedComponentGetter.h"
#include "Gameplay/Interfaces/CameraDataGetter.h"


void APiratePlayerController::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);


	UpdateInteractiveTrace();
}

void APiratePlayerController::SetupInputComponent() 
{
	Super::SetupInputComponent();

	InputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Pressed,this,&APiratePlayerController::Interact);
}

void APiratePlayerController::Interact()
{
	if (IsValid(InteractiveActor) && InteractiveActor->Implements<UInteractiveObject>())
		IInteractiveObject::Execute_InteractiveResponse(InteractiveActor, this);
}

void APiratePlayerController::UpdateInteractiveTrace()
{
	/*auto SpringArm = Cast<USpringArmComponent>(GetPawn()->GetComponentByClass(USpringArmComponent::StaticClass()));
	if (!SpringArm) return;*/
	FVector WorldLocation;
	FRotator Orientation;

	if (GetPawn()->Implements<UCameraDataGetter>())
		ICameraDataGetter::Execute_GetCameraProjectData(GetPawn(), WorldLocation, Orientation);

	auto Start = WorldLocation;
	auto End = Start + Orientation.Vector() * InteractiveTraceRange;     // Orientation.Vector() 朝向的前方

	//auto Start = SpringArm->GetComponentLocation();
	////auto End = Start + SpringArm->GetForwardVector()*InteractiveTraceRange;
	//auto End = Start + GetControlRotation().Vector()*InteractiveTraceRange;
	FHitResult OutHit;

	AActor* BeginInteractiveActor = NULL;           // 开始交互的 AActor
	AActor* EndInteractiveActor = NULL;				// 结束交互的 AActor
	AActor* TracingInteractiveActor = NULL;			// 每一帧正在交互的 AActor

	
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, InteractiveTraceChannal, false, { GetPawn() }, EDrawDebugTrace::ForOneFrame, OutHit, true))
	{
		TracingInteractiveActor = OutHit.GetActor();
		if (InteractiveActor != TracingInteractiveActor)       // 检测到了不一样的 AActor
		{
			BeginInteractiveActor = TracingInteractiveActor;   // 新的 AActor 开始交互
			EndInteractiveActor = InteractiveActor;
		}
	}
	else
		EndInteractiveActor = InteractiveActor;

	if (EndInteractiveActor && EndInteractiveActor->Implements<UInteractiveObject>())
	{
		IInteractiveObject::Execute_EndInteractive(EndInteractiveActor, this);        // 第一个参数为调用主体
		GlobalDelegateManager::Get()->CallGenericDelegate<void, bool, AActor*, FString, FString, FString>
			(INTERACTIVE_SHOW_INFO, false, NULL, "", "", "");
	}
	
	// BeginInteractiveActor->Implements<UInteractiveObject>() 用于判断是否实现某个接口
	if (IsValid(BeginInteractiveActor) && BeginInteractiveActor->Implements<UInteractiveObject>()) 
	{
		IInteractiveObject::Execute_BeginInteractive(BeginInteractiveActor, this); // request=> can see  again
		FString Title, Summary, Tip;
		auto bHasInfo = IInteractiveObject::Execute_GetInteractiveInformation(BeginInteractiveActor, Title, Summary, Tip); // 返回值是否显示交互信息

		/*GlobalDelegateManager::Get()->CallGenericDelegate<void, bool, AActor*,FString, FString, FString>
			(PLAYERINVENTORY_UPDATE_SLOT,bHasInfo, BeginInteractiveActor, Title, Summary, Tip);*/

		GlobalDelegateManager::Get()->CallGenericDelegate<void, bool, AActor*,FString, FString, FString>
			(INTERACTIVE_SHOW_INFO,bHasInfo, BeginInteractiveActor, Title, Summary, Tip);
	}
	
	SetActorOutLine(BeginInteractiveActor, true);
	SetActorOutLine(EndInteractiveActor, false);

	InteractiveActor = TracingInteractiveActor;
}

void APiratePlayerController::SetActorOutLine(AActor* Actor, bool bShowOutLine)
{
	if (!IsValid(Actor)) return;
	TArray<UMeshComponent*> MeshComps;
	Actor->GetComponents(MeshComps);

	for (auto& Comp : MeshComps)
	{
		if(!bShowOutLine)
		{
			Comp->SetRenderCustomDepth(false);
			continue;
		}
		Comp->SetRenderCustomDepth(true);
		Comp->SetCustomDepthStencilValue(1); 
	}
}