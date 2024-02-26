// Easy Systems
#include "Gameplay/BuildingSystem/BuildingPartActor.h"
#include "Utils/CommonUtilLibrary.h"
#include "Types/ItemsSysType.h"
#include "Types/BuildingSysTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Gameplay/ItemSystem/WeaponAndTool.h"

// Sets default values
ABuildingPartActor::ABuildingPartActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BuildingPartMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingPartMesh"));
	
	SetRootComponent(BuildingPartMesh);
}

// Called when the game starts or when spawned
void ABuildingPartActor::BeginPlay()
{
	Super::BeginPlay();

}

void ABuildingPartActor::SetUp(FName RowName)
{
	
	BuildingPartRowName = RowName;
	ItemDataPtr = UCommonUtilLibrary::GetTableRow<FItemDataTableRow>(ITEM_TABLE_NAME, RowName);

	if (!ItemDataPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get data for RowName: %s"), *RowName.ToString());
		return;
	}

	BuildingDataPtr = UCommonUtilLibrary::GetTableRow<FBuildingTableRow>(BUILDING_TABLE_NAME, RowName);

	bRotatorFourDirection = BuildingDataPtr->bRotatorFourDirection;
	bAttachCanRotator = BuildingDataPtr->bAttachCanRotator;
	
	if (!ItemDataPtr || !BuildingDataPtr) return;

	BuildingPartMesh->SetStaticMesh(ItemDataPtr->Model_SM);
	OriginMat = ItemDataPtr->Model_SM->GetMaterial(0);
}

void ABuildingPartActor::RecoveryMaterial()
{
	BuildingPartMesh->SetMaterial(0, OriginMat);
}

void ABuildingPartActor::UpdatePlacement(FHitResult Hit)
{
	bOverlayWithOtherBuildingPart = false;
	bCanRotatorBuildingPart = false;

	BuildingPartMesh->SetMaterial(0, CanNotBrBuiltMat);

	IsOverlayWithOther();
	// 放置部件的位置
	FVector PlaceLocation = FVector::ZeroVector;

	EBuildingAttachRule AttachRule = BuildingDataPtr->AttachRule;

	if (Hit.bBlockingHit) 
	{
		// 在检测点生成建造部件
		PlaceLocation = Hit.Location;  

		if (auto AnotherPart = Cast<ABuildingPartActor>(Hit.GetActor()))
		{	
			switch (AttachRule)
			{
			case SameMaterial:
				bShouldAttach = AnotherPart->BuildingDataPtr->Material == BuildingDataPtr->Material;
				break;
			case SpecificMaterials:
				bShouldAttach = BuildingDataPtr->AttachMaterialSet.Contains(AnotherPart->BuildingDataPtr->Material);
				break;
			case SpecificPartTypes:
				bShouldAttach = BuildingDataPtr->AttachTypeSet.Contains(AnotherPart->BuildingDataPtr->Type);
				break;
			}
			
			if (bShouldAttach)
			{
				bCanRotatorBuildingPart = true;

				auto AttachType = AnotherPart->BuildingDataPtr->Type;

				if (auto AttachTransformListPtr = BuildingDataPtr->RelativeAttachTransformMap.Find(AttachType))
				{
					auto TransformArr = AttachTransformListPtr->TransformList;
					if (TransformArr.Num() > 0)
					{
						// 世界坐标转换为本地坐标(相对坐标)
						auto LocalHit = UKismetMathLibrary::InverseTransformLocation(Hit.GetActor()->GetActorTransform(), Hit.Location);
						// 吸附到最近的点 note=> for the sort can have a better understand
						TransformArr.Sort([=](const FTransform& L, const FTransform& R)
							{
								return (L.GetLocation() - LocalHit).SizeSquared() < ((R.GetLocation() - LocalHit).SizeSquared());
							}
						);
						
						// 本地转世界
						PlaceLocation = UKismetMathLibrary::TransformLocation(Hit.GetActor()->GetActorTransform(), TransformArr[0].GetLocation());

						if (!bAlreadyRotatored)
						{
							PlaceRotation = UKismetMathLibrary::TransformRotation(Hit.GetActor()->GetActorTransform(), TransformArr[0].Rotator());
						}
					}
					
					// 是否和其它建造或可放置物体重叠
					if (!bOverlayWithOtherBuildingPart)  // 没有重叠的建造物体
					{
						CheckIsInAir();
					}
					else
						bShouldAttach = false;
				}
			}
		}
		else // 检测到时非建造物体,如 Landscape
		{
			if (Hit.GetActor()->ActorHasTag(TEXT("Landscape")) && BuildingDataPtr->bAttachToLandscape && !bOverlayWithOtherBuildingPart)
			{
				bShouldAttach = true;
				BuildingPartMesh->SetMaterial(0, CanBeBuiltMat);
			}
			else
				bShouldAttach = false;
		}		
	}
	else
	{
		bShouldAttach = false;
		PlaceLocation = Hit.TraceEnd;
	}
	
	SetActorLocationAndRotation(PlaceLocation, PlaceRotation);
}

void ABuildingPartActor::RotateBuilding(float RotatorAngle)
{
	PlaceRotation = GetActorRotation();
	PlaceRotation.Yaw += RotatorAngle;	
}

bool ABuildingPartActor::DestoryBuidlingPart()
{
	if (!IsValid(BuildingPartMesh)) return false;
	
	if (!BuildingDataPtr->bIsHaveDestructibleMesh) 
	{
		BuildingPartMesh->SetSimulatePhysics(true);
		SetLifeSpan(2.f);
		Destroy();
		return false;
	}

	if (!BuildingDataPtr->BuildingPartDestructibleMesh) return false;

	auto ActorCurrentTransform = GetActorTransform();

	BuildingPartMesh->DestroyComponent();
	auto SpawnActor = GetWorld()->SpawnActor<AActor>();
	SpawnActor->SetLifeSpan(2.f);

	UDestructibleComponent* DestrictibleComp = AddDestructibleCompToActor(SpawnActor, ActorCurrentTransform);

	DestrictibleComp->SetDestructibleMesh(BuildingDataPtr->BuildingPartDestructibleMesh);

	DestrictibleComp->SetCollisionResponseToChannel(UEngineTypes::ConvertToCollisionChannel(DestructibleComToPawnResponseChannel),
		ECollisionResponse::ECR_Ignore);
	DestrictibleComp->SetCollisionResponseToChannel(UEngineTypes::ConvertToCollisionChannel(DestructibleComToCameraResponseChannel),
		ECollisionResponse::ECR_Ignore);

	DestrictibleComp->SetSimulatePhysics(true);

	return true;
}

void ABuildingPartActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* FRotator NewRotator = GetActorRotation();
	NewRotator.Yaw += DeltaTime * 50.f;
	SetActorRotation(NewRotator); */
}

void ABuildingPartActor::BeginInteractive_Implementation(APlayerController* Controller)
{
	
}

void ABuildingPartActor::EndInteractive_Implementation(APlayerController* Controller)
{
	
}

void ABuildingPartActor::InteractiveResponse_Implementation(APlayerController* Controller)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Prent"));
}

bool ABuildingPartActor::GetInteractiveInformation_Implementation(FString& Title, FString& Summary, FString& Tip)
{
	// 展示提示信息
	if (ItemDataPtr)
	{
		Title = FString::Printf(TEXT("%s"), *ItemDataPtr->ItemName.ToString());

		Summary = ItemDataPtr->Description.ToString();

		if (BuildingDataPtr->Type == Door)
			Tip = TEXT("Press [F] to Open or Close the Door");
		else
			Tip = TEXT("");

		return true;
	}
	return false;
}

void ABuildingPartActor::IsOverlayWithOther()
{
	bOverlayWithOtherBuildingPart = false;

	TSet<AActor*> AllOverlayingActors;
	TSet<UPrimitiveComponent*> AllOverlayingComponents;
	BuildingPartMesh->GetOverlappingActors(AllOverlayingActors);
	BuildingPartMesh->GetOverlappingComponents(AllOverlayingComponents);

	// 判断是否和非建造物体重叠,重叠直接返回
	for (auto&  Item: AllOverlayingComponents)
	{
		if (Item->GetCollisionObjectType() == UEngineTypes::ConvertToCollisionChannel(PlacementResponseObjectType)) continue;

		if (Item->GetCollisionResponseToChannel(UEngineTypes::ConvertToCollisionChannel(PlacementResponseObjectType)) == ECollisionResponse::ECR_Block) 
		{
			bOverlayWithOtherBuildingPart = true;
			return;
		}
	}

	if (ItemDataPtr->Category == EItemCategory::Building && BuildingDataPtr->bIsCheckOverlay)   // 可建造的 
	{
		if (AllOverlayingActors.Num() != 0)
			bOverlayWithOtherBuildingPart = true;
	}
	else               // 可放置的建造物体需要剔除它能够放置建造部件 (不检测它和能放置的建造部件的重叠)（类型是 Building 或 Placeblement）
	{ 
		for (auto& OverlayingItem : AllOverlayingActors)
		{
			auto HaveCanPlacementType = false;
			auto TempBuildingPartActor = Cast<ABuildingPartActor>(OverlayingItem);

			// 重叠的物体的的建造类型只要是和可放置的物体所能够放置在建造类型中的一种相同就不考虑重叠
			for (auto& ItemType : BuildingDataPtr->AttachTypeSet)
			{
				if (TempBuildingPartActor->BuildingDataPtr->Type == ItemType)
				{
					HaveCanPlacementType = true;
					break;
				}
			}

			if (!HaveCanPlacementType)
			{
				bOverlayWithOtherBuildingPart = true;
				return;
			}
		}
	}
}

void ABuildingPartActor::CheckIsInAir()
{
	// 可放置物体检测是否悬空
	if (ItemDataPtr->Category == EItemCategory::Placeable)
	{
		TArray<FName> AllSocketArr = BuildingPartMesh->GetAllSocketNames();
		FVector End = FVector::ZeroVector;
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		TArray<AActor*> ActorToIngore;
		ObjectTypes.Add(PlacementResponseObjectType);
		ActorToIngore.Add(this);
		FHitResult OutHit;
		bShouldAttach = true;
		for (auto& Item : AllSocketArr)
		{
			FVector Start = BuildingPartMesh->GetSocketLocation(Item);
			End = Start + Start.ZAxisVector * (-PlaceableTraceRange);
			bool IsTrace = UKismetSystemLibrary::LineTraceSingleForObjects(
				GetWorld(),
				Start,
				End,
				ObjectTypes,
				false,
				ActorToIngore,
				bDebug ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
				OutHit,
				true
			);
			if (!IsTrace)  // 这里通过床举例,只要有一个角没有检测到可放置的物体就不能吸附。
			{
				bShouldAttach = false;
				break;
			}
		}
		if (bShouldAttach)
			BuildingPartMesh->SetMaterial(0, CanBeBuiltMat);
	}
	else
	{
		BuildingPartMesh->SetMaterial(0, CanBeBuiltMat);
		bShouldAttach = true;
	}
}
