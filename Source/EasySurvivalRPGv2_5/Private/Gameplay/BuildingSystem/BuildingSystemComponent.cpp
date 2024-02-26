#include "Gameplay/BuildingSystem/BuildingSystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Gameplay/BuildingSystem/BuildingPartActor.h"
#include "Gameplay/Interfaces/AssociatedComponentGetter.h"
#include "Gameplay/Interfaces/CameraDataGetter.h"
#include "Utils/CommonUtilLibrary.h"
#include "Types/ItemsSysType.h"
#include "PirateGameInstance.h"
#include "Global/GlobalDelegateManager.h"
#include "Gameplay/ItemSystem/InventoryComponent.h"


// Sets default values for this component's properties
UBuildingSystemComponent::UBuildingSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;	

	GlobalDelegateManager::Get()->BindGenericDelegateByString<void(ABuildingPartActor* DestoryBuildingPartActor)>(NOTIFY_DESTORY_BUILDINGPART,this, &UBuildingSystemComponent::DestoryBuildingPart);
}

// Called when the game starts
void UBuildingSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	InitBuildingRowNameArr();

	LoadDataFromJsonFile();
}

// Called every frame
void UBuildingSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateBuildingTrace();
}


void UBuildingSystemComponent::UpdateBuildingTrace()
{
	if (!bBuildingStarted)return;

	FVector WorldLocation; 
	FRotator Orientation;

	if (GetOwner()->Implements<UCameraDataGetter>())
		ICameraDataGetter::Execute_GetCameraProjectData(GetOwner(), WorldLocation, Orientation);

	auto Start = WorldLocation;
	auto End = Start + Orientation.Vector() * BuilingTraceRange;     // Orientation.Vector() 朝向的前方

	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, BuildingTraceChannel, false, {},
		bShouldDebug ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, OutHit, true);


	GetBuildingPartActor()->UpdatePlacement(OutHit);
}

void UBuildingSystemComponent::SavePlacementBuildingInfoToArray()
{
	FPlacementBuildingInfo PlacementBuildingInfo;

	// 设置 RowName
	if (GetBuildingPartActor()->ItemDataPtr->Category == EItemCategory::Building)
		PlacementBuildingInfo.RowName = BuildingRowNameArr[CurrentBuildingPartIndex];
	else
		PlacementBuildingInfo.RowName = PlaceBuildingRowNameArr[CurrentBuildingPartIndex];

	PlacementBuildingInfo.PlaceLocation = GetBuildingPartActor()->GetActorLocation();
	PlacementBuildingInfo.PlaceRotator = GetBuildingPartActor()->GetActorRotation();
	
	PlaceWoodBuildingArray.Add(PlacementBuildingInfo);
}

void UBuildingSystemComponent::InitBuildingRowNameArr()
{
	// 获取当前表中所有的 RowName
	auto RowNameSet = UCommonUtilLibrary::GetTableAllRow<FBuildingTableRow>(BUILDING_TABLE_NAME);
	if (RowNameSet.Num() == 0) return;

	for (auto& Item : RowNameSet)
	{
		auto Data = UCommonUtilLibrary::GetTableRow<FBuildingTableRow>(BUILDING_TABLE_NAME, Item);
		if (!Data) return;
		
		switch (Data->CircleBuildingType)
		{
			case ECircleBuildingMenuType::PlaceableBuildingMenu:
				PlaceBuildingRowNameArr.Add(Item);
				break;
			case ECircleBuildingMenuType::WoodBuildingMenu:
				BuildingRowNameArr.Add(Item);
				break;
			case ECircleBuildingMenuType::StoneBuildingMenu:
				StoneBuildingRowNameArr.Add(Item);
				break;
			default:
				break;
		}
	}
}

void UBuildingSystemComponent::SetBuildingActorCollision()
{
	if (!BuildingPartActor) return;

	BuildingPartActor->BuildingPartMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BuildingPartActor->BuildingPartMesh->SetCollisionResponseToChannel(UEngineTypes::ConvertToCollisionChannel(BuildingTraceChannel),
		ECollisionResponse::ECR_Block);
	// 阻挡玩家
	BuildingPartActor->BuildingPartMesh->SetCollisionResponseToChannel(UEngineTypes::ConvertToCollisionChannel(BlockPlayerChannel),
		ECollisionResponse::ECR_Block);
	// 设置可以交互
	BuildingPartActor->BuildingPartMesh->SetCollisionResponseToChannel(UEngineTypes::ConvertToCollisionChannel(InteractionChannel),
		ECollisionResponse::ECR_Block);
	// 设置物体本身的 ObjectType
	BuildingPartActor->BuildingPartMesh->SetCollisionObjectType(UEngineTypes::ConvertToCollisionChannel(BuildingPartObjectType));

	BuildingPartActor->BuildingPartMesh->SetCollisionResponseToChannel(UEngineTypes::ConvertToCollisionChannel(BuildingPartObjectType),
		ECollisionResponse::ECR_Block);
	BuildingPartActor->BuildingPartMesh->SetCollisionResponseToChannel(UEngineTypes::ConvertToCollisionChannel(BlockFloorChannel),
		ECollisionResponse::ECR_Block);
}

void UBuildingSystemComponent::LoadDataFromJsonFile()
{
	UPirateGameInstance* PirateGameInstance = Cast<UPirateGameInstance>(GetWorld()->GetGameInstance());

	// 是否加载存档
	if (PirateGameInstance)
	{
		if (PirateGameInstance->GetClickContinueGame())
		{
			if (bSavePlacementBuildingToJson)
			{
				auto Path = FPaths::ProjectContentDir() + RelativeSavePath + JsonFileName;
				UCommonUtilLibrary::LoadObjectPropertiesFromJsonFile(this, *Path);
			}

			if (PlaceWoodBuildingArray.Num() == 0) return;

			for (auto& Item : PlaceWoodBuildingArray)
			{
				if (!BuildingPartActor)
				{
					UE_LOG(LogTemp, Warning, TEXT("BuildingPartActorClass pointer: %p"), BuildingPartActorClass.Get());

					CheckBuildingPartType(Item.RowName);

					if (BuildingPartActorClass && BuildingPartActorClass->IsChildOf(ABuildingPartActor::StaticClass()))
					{
						BuildingPartActor = GetWorld()->SpawnActor<ABuildingPartActor>(BuildingPartActorClass, Item.PlaceLocation, Item.PlaceRotator);
						if (Item.RowName == NAME_None) return;
						BuildingPartActor->SetUp(Item.RowName);
						SetBuildingActorCollision();
						BuildingPartActor = nullptr;

					}
				}
			}
		}
	}
}

void UBuildingSystemComponent::CheckBuildingPartType(FName RowName)
{
	if (RowName == NAME_None) return;
	// 设置门的蓝图类
	auto Data = UCommonUtilLibrary::GetTableRow<FBuildingTableRow>(BUILDING_TABLE_NAME, RowName);

	if (!Data) return;

	auto BuildingPartClass = GetBuildingPartClass(Data->Type);

	if (BuildingPartClass)
		BuildingPartActorClass = BuildingPartClass;
}

void UBuildingSystemComponent::DestoryBuildingPart(ABuildingPartActor* DestoryBuildingPartActor)
{
	FPlacementBuildingInfo PlacementInfo;

	TArray<FPlacementBuildingInfo> CurrentPlacementBuildingInfo;

	PlacementInfo.RowName = DestoryBuildingPartActor->BuildingPartRowName;
	PlacementInfo.PlaceLocation = DestoryBuildingPartActor->GetActorLocation();
	PlacementInfo.PlaceRotator = DestoryBuildingPartActor->GetActorRotation();

	if (DestoryBuildingPartActor->DestoryBuidlingPart()) 
	{
		for (auto& Item : PlaceWoodBuildingArray)
		{
			if (Item == PlacementInfo) continue;
			CurrentPlacementBuildingInfo.Add(Item);
		}

		PlaceWoodBuildingArray = CurrentPlacementBuildingInfo;
	}
}

bool UBuildingSystemComponent::CheckBuildingItemIsEnough()
{
	UInventoryComponent* InventoryComponent = nullptr;
	
	if (GetOwner()->Implements<UAssociatedComponentGetter>())
		InventoryComponent = IAssociatedComponentGetter::Execute_GetInventoryComponent(GetOwner());

	if (!IsValid(InventoryComponent)) return false;

	// 目前不考虑快捷栏中的道具

	if (auto BuildingInfoPtr = UCommonUtilLibrary::GetTableRow<FBuildingTableRow>(BUILDING_TABLE_NAME, CurrentBuildingPartRowName)) 
	{
		for (auto& Request : BuildingInfoPtr->RequestItemMap) 
		{
			//只要有一个道具不够就是不够
			if (!InventoryComponent->IsItemEnouph(Request.Key, Request.Value)) 
				return false;
		}
	}
	
	return true;
}

void UBuildingSystemComponent::UpdateInventoryBuildingItem()
{
	UInventoryComponent* InventoryComponent = nullptr;

	if (GetOwner()->Implements<UAssociatedComponentGetter>())
		InventoryComponent = IAssociatedComponentGetter::Execute_GetInventoryComponent(GetOwner());

	if (auto BuildingInfoPtr = UCommonUtilLibrary::GetTableRow<FBuildingTableRow>(BUILDING_TABLE_NAME, CurrentBuildingPartRowName)) 
	{
		for (auto& Request : BuildingInfoPtr->RequestItemMap)
			InventoryComponent->RemoveItemFromInventory(Request.Key, Request.Value);
	}
}

void UBuildingSystemComponent::SaveDataToJsonFile()
{
	auto Path = FPaths::ProjectContentDir() + RelativeSavePath + JsonFileName;
	UCommonUtilLibrary::SaveObjectPropertiesToJsonFile(this, *Path);
}

ABuildingPartActor* UBuildingSystemComponent::GetBuildingPartActor()
{
	if (!BuildingPartActor) 
	{
		CheckBuildingPartType(BuildingRowNameArr[CurrentBuildingPartIndex]);

		BuildingPartActor = GetWorld()->SpawnActor<ABuildingPartActor>(BuildingPartActorClass);

		switch (CurrentOpenCircleMenuType)
		{
			case WoodBuildingMenu:
				BuildingPartActor->SetUp(BuildingRowNameArr[CurrentBuildingPartIndex]);
				break;
			case PlaceableBuildingMenu:
				BuildingPartActor->SetUp(PlaceBuildingRowNameArr[CurrentBuildingPartIndex]);
				break;
			case StoneBuildingMenu:
				BuildingPartActor->SetUp(StoneBuildingRowNameArr[CurrentBuildingPartIndex]);
				break;
		}
		
		//BuildingPartActor->BuildingPartMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		BuildingPartActor->BuildingPartMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		BuildingPartActor->BuildingPartMesh->SetCollisionResponseToChannel(UEngineTypes::ConvertToCollisionChannel(BuildingPartObjectType),
			ECollisionResponse::ECR_Overlap);
		BuildingPartActor->BuildingPartMesh->SetCollisionResponseToChannel(UEngineTypes::ConvertToCollisionChannel(OverlayWorldStaticChannel),
			ECollisionResponse::ECR_Overlap);
		BuildingPartActor->BuildingPartMesh->SetCollisionResponseToChannel(UEngineTypes::ConvertToCollisionChannel(OverlayWorldDynamicChannel),
			ECollisionResponse::ECR_Overlap);
		BuildingPartActor->BuildingPartMesh->SetCollisionResponseToChannel(UEngineTypes::ConvertToCollisionChannel(OverlayWorldPawnChannel),
			ECollisionResponse::ECR_Overlap);
	}

	return BuildingPartActor;
}

void UBuildingSystemComponent::CancelBuilding()
{
	if (BuildingPartActor) 
	{
		BuildingPartActor->Destroy();
		BuildingPartActor = 0;
	}
		
	bBuildingStarted = false;
}

void UBuildingSystemComponent::RotateBuildingPart(float AxisValue)
{
	// 没有开始建造直接返回,避免和其它模块的鼠标的滚轮进行混淆导致不必要的麻烦
	if (!bBuildingStarted) return;

	if (AxisValue == 0.f) return;
	
	float RotatorAngle = 0.f;

	if (GetBuildingPartActor()->bCanRotatorBuildingPart)
	{
		if (!GetBuildingPartActor()->bAttachCanRotator) return;

		GetBuildingPartActor()->bAlreadyRotatored = true;
		if (AxisValue > 0)
			if(GetBuildingPartActor()->bRotatorFourDirection)
				RotatorAngle = 90.f;
			else
				RotatorAngle = 180.f;
		else
			if (GetBuildingPartActor()->bRotatorFourDirection)
				RotatorAngle = -90.f;
			else
				RotatorAngle = -180.f;	
	}
	else
	{
		GetBuildingPartActor()->bAlreadyRotatored = false;
		if (AxisValue > 0)
			RotatorAngle = 22.5f;
		if (AxisValue < 0)
			RotatorAngle = -20.5f;
	}

	GetBuildingPartActor()->RotateBuilding(RotatorAngle);
}

void UBuildingSystemComponent::SetBuildingPartIndex(int32 Index)
{
	// 保存当前建造部件的 RowName
	switch (CurrentOpenCircleMenuType)
	{
		case WoodBuildingMenu:
			if (!BuildingRowNameArr.IsValidIndex(Index)) return;
			CurrentBuildingPartRowName = BuildingRowNameArr[Index];
			break;
		case StoneBuildingMenu:
			if (!StoneBuildingRowNameArr.IsValidIndex(Index)) return;
			CurrentBuildingPartRowName = StoneBuildingRowNameArr[Index];
			break;
		case PlaceableBuildingMenu:
			if (!PlaceBuildingRowNameArr.IsValidIndex(Index)) return;
			CurrentBuildingPartRowName = PlaceBuildingRowNameArr[Index];
			break;
		default:
			break;
	}
	
	CurrentBuildingPartIndex = Index;
}

void UBuildingSystemComponent::PlaceBuildingPart()
{
	//不能放置直接返回
	if (!GetBuildingPartActor()->bShouldAttach) 
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Can't put here!"),true,false,FLinearColor::Red,1.f);
		OnShowBuildingWarnAndTipInfo.Broadcast("Can't Building here", false,false);
		return;
	}

	GetBuildingPartActor()->bPlace = true;

	GetBuildingPartActor()->RecoveryMaterial();

	SetBuildingActorCollision();

	//GetBuildingPartActor()->BuildingPartMesh->SetSimulatePhysics(true);

	SavePlacementBuildingInfoToArray();

	// 放下去之后就将当前的设置成空指针此时放在地下不动了,将会生成一个新的跟随着
	BuildingPartActor = 0;

	// 更新背包中建造所需材料的数量
	UpdateInventoryBuildingItem();

	// 判断建造部件需要的建造材料是否足够
	if (!CheckBuildingItemIsEnough())
	{
		bBuildingStarted = false;
		OnShowBuildingWarnAndTipInfo.Broadcast("Building requirements are no Completed", false, true);
	}
}

void UBuildingSystemComponent::ToggleBuildingPart(int32 Index)
{
	SetBuildingPartIndex(Index);
	
	//如果不够直接返回
	if (!CheckBuildingItemIsEnough()) 
	{
		// 给 UI 一个提示
		OnShowBuildingWarnAndTipInfo.Broadcast("Building requirements are no Completed", false, true);
		
		// 展示建造提示文字
		return;
	}

	OnShowBuildingWarnAndTipInfo.Broadcast(CurrentBuildingPartRowName, true, true);
	
	//BuildingPartActor->SetUp(BuildingRowNameArr[CurrentBuildingPartIndex]);
	bBuildingStarted = true;
}

bool UBuildingSystemComponent::CanStartBuild()
{
	return bBuildingStarted;
}