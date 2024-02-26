// Easy Systems
// 

#include "Gameplay/ItemSystem/LootableItem.h"
#include "Kismet/GameplayStatics.h"
#include "Types/ItemsSysType.h"
#include "Utils/CommonUtilLibrary.h"
#include "Gameplay/Interfaces/AssociatedComponentGetter.h"
#include "Gameplay/ItemSystem/InventoryComponent.h"

// Sets default values
ALootableItem::ALootableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(RootComponent);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SkeletalMeshComp->SetupAttachment(RootComponent);
	SkeletalMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ALootableItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALootableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALootableItem::SpawnItemByRowNameAndCount(FName RowName, int32 Count)
{
	ItemCount = Count;
	ItemDataRowName = RowName;
	SetItemMesh(RowName);
}

#if WITH_EDITOR
void ALootableItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	
	if (PropertyChangedEvent.GetPropertyName() == TEXT("ItemDataRowName"))                  // PropertyChangedEvent.GetPropertyName() 
	{
		SetItemMesh(ItemDataRowName);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);         // ?????????›ö????
	//UKismetSystemLibrary::PrintString(GetWorld(), PropertyChangedEvent.GetPropertyName().ToString());
}
#endif

void ALootableItem::SetItemMesh(FName RowName)
{
	if (auto ItemData = UCommonUtilLibrary::GetTableRow<FItemDataTableRow>(TEXT("ItemTable"), RowName))
	{
		if (IsValid(ItemData->Model_SM))
		{
			StaticMeshComp->SetStaticMesh(ItemData->Model_SM);
			StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			StaticMeshComp->SetSimulatePhysics(true);
		}
		if (IsValid(ItemData->Model_Sk))
		{
			SkeletalMeshComp->SetSkeletalMesh(ItemData->Model_Sk);
			SkeletalMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			SkeletalMeshComp->SetSimulatePhysics(true);
		}
	}
	else
	{
		StaticMeshComp->SetStaticMesh(NULL);
		StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StaticMeshComp->SetSimulatePhysics(false);
		SkeletalMeshComp->SetSkeletalMesh(NULL);
		SkeletalMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SkeletalMeshComp->SetSimulatePhysics(false);
	}
}


void ALootableItem::BeginInteractive_Implementation(APlayerController* Controller)
{

}
void ALootableItem::EndInteractive_Implementation(APlayerController* Controller)
{
}
void ALootableItem::InteractiveResponse_Implementation(APlayerController* Controller)
{
	auto Pawn = Controller->GetPawn();
	if (IsValid(Pawn) && Pawn->Implements<UAssociatedComponentGetter>()) 
	{
		auto Inventory = IAssociatedComponentGetter::Execute_GetInventoryComponent(Pawn);
		auto Left = Inventory->AddItemToInventory(ItemDataRowName, ItemCount);
		if (Left == 0) 
		{
			Destroy();
			return;
		}
		ItemCount = Left;
	}
}
bool ALootableItem::GetInteractiveInformation_Implementation(FString& Title, FString& Summary, FString& Tip)
{
	if (auto ItemData = UCommonUtilLibrary::GetTableRow<FItemDataTableRow>(TEXT("ItemTable"), ItemDataRowName)) 
	{
		Title = FString::Printf(TEXT("%s (%d)"), *ItemData->ItemName.ToString(), ItemCount);
		Summary = ItemData->Description.ToString();
		Tip = TEXT("Press [F] to Pick Up");
		return true;
	}
	return false;
}