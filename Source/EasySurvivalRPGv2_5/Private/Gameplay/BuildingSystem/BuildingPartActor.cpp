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
	// ���ò�����λ��
	FVector PlaceLocation = FVector::ZeroVector;

	EBuildingAttachRule AttachRule = BuildingDataPtr->AttachRule;

	if (Hit.bBlockingHit) 
	{
		// �ڼ������ɽ��첿��
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
						// ��������ת��Ϊ��������(�������)
						auto LocalHit = UKismetMathLibrary::InverseTransformLocation(Hit.GetActor()->GetActorTransform(), Hit.Location);
						// ����������ĵ� note=> for the sort can have a better understand
						TransformArr.Sort([=](const FTransform& L, const FTransform& R)
							{
								return (L.GetLocation() - LocalHit).SizeSquared() < ((R.GetLocation() - LocalHit).SizeSquared());
							}
						);
						
						// ����ת����
						PlaceLocation = UKismetMathLibrary::TransformLocation(Hit.GetActor()->GetActorTransform(), TransformArr[0].GetLocation());

						if (!bAlreadyRotatored)
						{
							PlaceRotation = UKismetMathLibrary::TransformRotation(Hit.GetActor()->GetActorTransform(), TransformArr[0].Rotator());
						}
					}
					
					// �Ƿ�����������ɷ��������ص�
					if (!bOverlayWithOtherBuildingPart)  // û���ص��Ľ�������
					{
						CheckIsInAir();
					}
					else
						bShouldAttach = false;
				}
			}
		}
		else // ��⵽ʱ�ǽ�������,�� Landscape
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
	// չʾ��ʾ��Ϣ
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

	// �ж��Ƿ�ͷǽ��������ص�,�ص�ֱ�ӷ���
	for (auto&  Item: AllOverlayingComponents)
	{
		if (Item->GetCollisionObjectType() == UEngineTypes::ConvertToCollisionChannel(PlacementResponseObjectType)) continue;

		if (Item->GetCollisionResponseToChannel(UEngineTypes::ConvertToCollisionChannel(PlacementResponseObjectType)) == ECollisionResponse::ECR_Block) 
		{
			bOverlayWithOtherBuildingPart = true;
			return;
		}
	}

	if (ItemDataPtr->Category == EItemCategory::Building && BuildingDataPtr->bIsCheckOverlay)   // �ɽ���� 
	{
		if (AllOverlayingActors.Num() != 0)
			bOverlayWithOtherBuildingPart = true;
	}
	else               // �ɷ��õĽ���������Ҫ�޳����ܹ����ý��첿�� (����������ܷ��õĽ��첿�����ص�)�������� Building �� Placeblement��
	{ 
		for (auto& OverlayingItem : AllOverlayingActors)
		{
			auto HaveCanPlacementType = false;
			auto TempBuildingPartActor = Cast<ABuildingPartActor>(OverlayingItem);

			// �ص�������ĵĽ�������ֻҪ�ǺͿɷ��õ��������ܹ������ڽ��������е�һ����ͬ�Ͳ������ص�
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
	// �ɷ����������Ƿ�����
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
			if (!IsTrace)  // ����ͨ��������,ֻҪ��һ����û�м�⵽�ɷ��õ�����Ͳ���������
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
