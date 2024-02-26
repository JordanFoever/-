// Easy Systems


#include "Gameplay/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "MenuMain/MainMenuSaveGame.h"
#include "Utils/CommonUtilLibrary.h"
#include "MenuMain/CharacterStyleAvatar.h"
#include "Gameplay/ItemSystem/InventoryComponent.h"
#include "DelegateTest/UDelegateComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utils/CommonUtilLibrary.h"
#include "Gameplay/ItemSystem/WeaponAndTool.h"
#include "Gameplay/Interfaces/OperativeItem.h"
#include "Gameplay/PiratePlayerController.h"
#include "Gameplay/BuildingSystem/BuildingPartActor.h"
#include "Gameplay/BuildingSystem/BuildingPartActor.h"
#include "Global/GlobalDelegateManager.h"	

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(GetMesh());
	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetMesh());
	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandsMesh"));
	HandsMesh->SetupAttachment(GetMesh());
	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegsMesh"));
	LegsMesh->SetupAttachment(GetMesh());
	FeetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FeetMesh"));
	FeetMesh->SetupAttachment(GetMesh());
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// ���ض���
	BodyMesh->SetMasterPoseComponent(HeadMesh);
	HandsMesh->SetMasterPoseComponent(HeadMesh);
	LegsMesh->SetMasterPoseComponent(HeadMesh);
	FeetMesh->SetMasterPoseComponent(HeadMesh);

	LoadSavedStyle();
	/*GlobalDelegateManager::Get()->
	<void(int, int)>(PLAYERINVENTORY_SWAP_SLOTS, this, &APlayerCharacter::PlayerSwapItems);
	GlobalDelegateManager::Get()->BindGenericDelegateByString<void(int, int,int)>(PLAYERINVENTORY_SPLIT_ITEMS, this, &APlayerCharacter::PlayerSplitItems);*/
	if(this->Implements<UAssociatedComponentGetter>())
		if (auto DelegateComp = IAssociatedComponentGetter::Execute_GetDelegateComponent(this))
		{
			//��ί��  =>����ί������ͼ�е� WBP_PlayerInventoy OnClicked������HandleDragAndDrop��
			DelegateComp->BindDelegate(SwapItemEventTag, this, TEXT("PlayerSwapItems"));
			DelegateComp->BindDelegate(SplitItemEventTag, this, TEXT("PlayerSplitItems"));
		}
}

//void APlayerCharacter::PlayerSwapItems(int32 FromIndex, int32 ToIndex)
//{
// 
// 
//	if (!this->Implements<UAssociatedComponentGetter>()) return;
//
//	if (auto InventoryComp = IAssociatedComponentGetter::Execute_GetInventoryComponent(this))
//		InventoryComp->SwapItems(FromIndex, ToIndex);
//}

//void APlayerCharacter::PlayerSplitItems(int32 FromIndex, int32 ToIndex, int32 SplitCount)
//{
//	if (!this->Implements<UAssociatedComponentGetter>()) return;
//
//	if (auto InventoryComp = IAssociatedComponentGetter::Execute_GetInventoryComponent(this))
//		InventoryComp->SplitItems(FromIndex, ToIndex, SplitCount);
//}

void APlayerCharacter::PlayerSwapItems(UObject* Payload)
{
	PROPERTY_GET(int32, From, Payload);  // �����ʶ�� From �� To ����Ҫ����ͼ�� UObject �е�������һ��,��Ȼ�Ҳ���
	PROPERTY_GET(int32, To, Payload);
	
	if (auto InventoryComp = IAssociatedComponentGetter::Execute_GetInventoryComponent(this))
	InventoryComp->SwapItems(From, To);
	
}

void APlayerCharacter::PlayerSplitItems(UObject* Payload)
{
	PROPERTY_GET(int32, From, Payload);  
	PROPERTY_GET(int32, To, Payload);
	PROPERTY_GET(int32, SplitCount, Payload);

	if (auto InventoryComp = IAssociatedComponentGetter::Execute_GetInventoryComponent(this))
		InventoryComp->SplitItems(From, To, SplitCount);
}

void APlayerCharacter::LoadSavedStyle()
{
	if (auto Save = Cast<UMainMenuSaveGame>(UGameplayStatics::LoadGameFromSlot("MainMenu", 0))) 
	{
		TArray<FName> StyleKeys;
		Save->StyleConfigs.GetKeys(StyleKeys);

		TArray<TArray<USkeletalMeshComponent*>> MeshCompsArr = { {HeadMesh},{HeadMesh},{HeadMesh,BodyMesh,HandsMesh,FeetMesh},{LegsMesh} };

		for (int i = 0; i < StyleKeys.Num(); i++)
		{
			auto Index = Save->StyleConfigs[StyleKeys[i]];
			if (auto RowPtr = UCommonUtilLibrary::GetTableRow<FCharacterTypeOpteratorConfig>("CharacterStyleTable", StyleKeys[i])) 
			{
				if (RowPtr->bChangeColor) 
				{
					TArray<FName> ColorNames;
					RowPtr->StyleColor.GetKeys(ColorNames);
					FString ColorParamsName = "";
					if (StyleKeys[i].ToString().Contains("Hair")) ColorParamsName = "Hair Tint";
					if (StyleKeys[i].ToString().Contains("Skin")) ColorParamsName = "Skin Tint";
					if (StyleKeys[i].ToString().Contains("Pants")) ColorParamsName = "Pants Tint";

					for (auto& MeshComp : MeshCompsArr[i])                           
						MeshComp->CreateDynamicMaterialInstance(0)->SetVectorParameterValue(FName(ColorParamsName),RowPtr->StyleColor[ColorNames[Index]]);               
				}
				else                                                              
				{
					TArray<FName> StyleNames;
					RowPtr->StyleMeshMap.GetKeys(StyleNames);

					for (auto& MeshComp : MeshCompsArr[i])
						MeshComp->SetSkeletalMesh(RowPtr->StyleMeshMap[StyleNames[Index]]);
				}
			}
		}
	}
}


void APlayerCharacter::HandleFastMovingSwitch() 
{
	if (bFastMoving &&
		(GetInputAxisValue(TEXT("MoveForward")) < 0 || (GetInputAxisValue(TEXT("MoveForward")) == 0 && GetInputAxisValue(TEXT("MoveRight")) == 0)))
		SpringOrFastSwim();
}

void APlayerCharacter::HandleWeaponAndToolSelected(const FItemDataTableRow& ItemData, FName RowName)
{
	if (!IsValid(HoldingWeapon)) 
	{
		HoldingWeapon = GetWorld()->SpawnActor<AWeaponAndTool>(WeaponToolClass);
		HoldingWeapon->AttachToComponent(HandsMesh, FAttachmentTransformRules::SnapToTargetIncludingScale,WeaponSocketName);
	}

	HoldingWeapon->Initiate(ItemData.Model_SM, RowName);
	HotBarItem = HoldingWeapon;
}

// Called every frame 
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HandleFastMovingSwitch();
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayerCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APlayerCharacter::LookUp);

	PlayerInputComponent->BindAction(TEXT("Sprint/FastSwim"), EInputEvent::IE_Pressed, this, &APlayerCharacter::SpringOrFastSwim);

	//PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed,this, &APlayerCharacter::Jump);
	//̧�� �ո���Ծ
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::StopJumping);
	PlayerInputComponent->BindAction("NumbericShotcut", IE_Released, this, &APlayerCharacter::SelectHotBar);
	PlayerInputComponent->BindAction("HotBarMainAction", IE_Released, this, &APlayerCharacter::MainAction); 
	
}

void APlayerCharacter::LookUp(float Axis)
{
	this->AddControllerPitchInput(Axis);
}

void APlayerCharacter::Turn(float Axis)
{
	this->AddControllerYawInput(Axis);
}

void APlayerCharacter::MoveForward(float Axis)
{
	AddMovementInput(FRotator(0, GetControlRotation().Yaw, 0).Quaternion().GetForwardVector(), Axis);
}

void APlayerCharacter::MoveRight(float Axis)
{
	AddMovementInput(FRotator(0, GetControlRotation().Yaw, 0).Quaternion().GetRightVector(), Axis);
}

void APlayerCharacter::SpringOrFastSwim() 
{
	bFastMoving = !bFastMoving;

	if (bFastMoving)
		GetCharacterMovement()->MaxWalkSpeed = SpringSpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed; 
}

void APlayerCharacter::SelectHotBar(FKey Key)
{
	// ֪ͨ UI ������ʾ��ѡ��Ŀ�����ɵ���
	auto HotBar = IAssociatedComponentGetter::Execute_GetHotBarComponent(this);
	if (!IsValid(HotBar))return;

	TArray<FKey> HotBarKeys = {
		EKeys::One,
		EKeys::Two,
		EKeys::Three,
		EKeys::Four,
		EKeys::Five,
		EKeys::Six,
		EKeys::Seven,
		EKeys::Eight,
	};

	auto Index = HotBarKeys.Find(Key);

	if (Index < 0 || Index == HotBarIndex)return; // Ҫô��Ч,Ҫô���ϴ�ѡ����һ����

	HotBarIndex = Index;
	HotBarSelected(HotBar, Index);
		
	if (IsValid(HoldingWeapon)) HoldingWeapon->Initiate(NULL,NAME_None);

	FInventoryItemData Data;
	if(HotBar->GetItemData(Index,Data))
	{
		auto RowPtr = UCommonUtilLibrary::GetTableRow<FItemDataTableRow>(TEXT("ItemTable"), Data.TableRowName);
		if (RowPtr) 
		{
			auto Category = RowPtr->Category;
			switch (Category)
			{
			case EItemCategory::Consumable:
				break;
			case EItemCategory::Weapon:
			case EItemCategory::Tool:
				HandleWeaponAndToolSelected(*RowPtr,Data.TableRowName);
				break;
			case EItemCategory::Component:
				break;
			case EItemCategory::Building:
				break;
			default:
				break;
			}
		}
	}
}

void APlayerCharacter::MainAction()
{
	auto HotBar = IAssociatedComponentGetter::Execute_GetHotBarComponent(this);
	if (!IsValid(HotBar))return;

	FInventoryItemData Data;
	HotBar->GetItemData(HotBarIndex, Data);

	if (Data.IsEmpty()) return;

	if (!IsValid(HotBarItem)) return;

	// �����⵽���岢���ǽ������������ľ鳲ſ���,��ʱͨ��
	if (auto PlayerController = Cast<APiratePlayerController>(GetController())) 
	{
		if (auto BuildingPartActor = Cast<ABuildingPartActor>(PlayerController->InteractiveActor)) 
		{
			auto RowName = UCommonUtilLibrary::GetTableRow<FWeaponAndToolTableRow>(WEAPON_TABLE_NAME, Data.TableRowName);
			if (RowName->ToolType == EToolType::Mallet)
				// ��ί���� BuildingSystemComponent �����
				GlobalDelegateManager::Get()->CallGenericDelegate<void, ABuildingPartActor*>(NOTIFY_DESTORY_BUILDINGPART, BuildingPartActor);
			return;
		}
	}

	if (HotBarItem->Implements<UOperativeItem>() && IOperativeItem::Execute_CanBeOperated(HotBarItem))
		IOperativeItem::Execute_MainActionOperative(HotBarItem, HeadMesh);
	
}

void APlayerCharacter::EndOperatingItem()
{
	if (IsValid(HotBarItem) && HotBarItem->Implements<UOperativeItem>())
		IOperativeItem::Execute_EndOperating(HotBarItem);
}
