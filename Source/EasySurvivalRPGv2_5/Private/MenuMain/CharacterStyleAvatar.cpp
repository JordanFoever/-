// Easy Systems


#include "MenuMain/CharacterStyleAvatar.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Global/GlobalDelegateManager.h"
#include "MenuMain/MainMenuSaveGame.h"


// Sets default values
ACharacterStyleAvatar::ACharacterStyleAvatar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(RootComponent);
	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);
	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandsMesh"));
	HandsMesh->SetupAttachment(RootComponent);
	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegsMesh"));
	LegsMesh->SetupAttachment(RootComponent);
	FeetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FeetMesh"));
	FeetMesh->SetupAttachment(RootComponent);

	//FCharacterTypeOpteratorConfig::StaticStruct();                         // �õ��ṹ�����͵�����ָ������ǲ��ĸ���
}

// Called when the game starts or when spawned
void ACharacterStyleAvatar::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<FSoftObjectPath> InitiateAssetPathArray;							// �����첽����Ķ���
	InitiateAssetPathArray.Add(StyleTable.ToSoftObjectPath());
	for (auto&  Mesh: DefaultMeshes)
	{
		InitiateAssetPathArray.Add(Mesh.ToSoftObjectPath());
	}

	TableLoadHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(InitiateAssetPathArray, [=]()  // ����� = ���൱�� this
		{
			// ���� Table_CharacterStyleConfig ����е�����
			if (auto Table = Cast<UDataTable>(TableLoadHandle.Get()->GetLoadedAsset()))             // ����� lamdom ���ʽ�Ǽ��سɹ�֮����õ�
			{
				//UKismetSystemLibrary::PrintString(GetWorld(),LoadObject->GetName());
				InitiateAvatar(Table);
			}
			
			/*���س���ͷ����������Ĺ��������ʲ�, ��Ӧ�õ��������ֵ����������*/
			//start
			TArray<UObject*> AllAssets;
			TArray<USkeletalMeshComponent*> MeshComp = { BodyMesh,HandsMesh,LegsMesh,FeetMesh };
			TableLoadHandle.Get()->GetLoadedAssets(AllAssets);

			for (int i = 1; i < AllAssets.Num(); i++)
				if (auto Mesh = Cast<USkeletalMesh>(AllAssets[i])) 
					if (MeshComp.IsValidIndex(i-1))
						MeshComp[i - 1]->SetSkeletalMesh(Mesh);
			//end

			// ��������޸����е��ı����г�ʼ��
			ChangeStyle("HairStyle", 0, 0);
			ChangeStyle("HairColor", 0, 0);
			ChangeStyle("SkinColor", 0, 0);
			ChangeStyle("PantsColor", 0, 0);

			// ���ض���
			HeadMesh->SetAnimInstanceClass(AnimClass);
			BodyMesh->SetMasterPoseComponent(HeadMesh);
			HandsMesh->SetMasterPoseComponent(HeadMesh);
			LegsMesh->SetMasterPoseComponent(HeadMesh);
			FeetMesh->SetMasterPoseComponent(HeadMesh);

		});

	// ��ί��,����ҵ�� UI �мӺźͼ��Ÿı��������������ɫʱ����
	// ����ί���� MainHUD.cpp �е� ModifyChracterStates �ĺ�����
	GlobalDelegateManager::Get()->BindGenericDelegateByString<void(FString,int,int)>
		(MAINMENU_CHANGE_STYLE, this, &ACharacterStyleAvatar::ChangeStyle);
}

// Called every frame
void ACharacterStyleAvatar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACharacterStyleAvatar::InitiateAvatar(UDataTable* ConfigTable)
{
	// ��Ч���� ���еĽṹ���� FCharacterTypeOpteratorConfig ��Ӧ�Ľṹ������ֱ�ӷ���
	if (!IsValid(ConfigTable) || !(ConfigTable->GetRowStruct()->IsChildOf(FCharacterTypeOpteratorConfig::StaticStruct())))return;

	ConfigDataTable = ConfigTable;

	auto RowMap = ConfigTable->GetRowMap();

	TArray<FName> RowNames;																// �õ�ÿһ�����ֵ�����
	RowMap.GetKeys(RowNames); 

	TArray<TArray<USkeletalMeshComponent*>> MeshComp = { {HeadMesh},{HeadMesh},{HeadMesh,BodyMesh,HandsMesh,FeetMesh},{LegsMesh}};

	for (int i = 0; i < RowNames.Num(); i++)
	{
		CharacterStyleComp.Add(RowNames[i], MeshComp[i]);                                  // �����ֺ͹�������һһ��Ӧ����
	}

	/*if (auto RowPtr = RowMap.Find(TEXT("HairStyle")))
	{
		auto HairRowPtr = (FCharacterTypeOpteratorConfig*)(*RowPtr);                        //   *RowPtr ָ������ĳһ�е�ָ��

		for (auto& Pair : HairRowPtr->StyleMeshMap)
		{
			HeadMesh->SetSkeletalMesh(Pair.Value);                                          //  Pair.Value �ǹ�������λָ��
		}
	}*/
}

void ACharacterStyleAvatar::ChangeStyle(FString Tag, int PreIndex,int Index)
{
	if (!IsValid(ConfigDataTable))return;
	 
	// ��ȡ����� RowName Ϊ Tag��eg:HairStyle,HairColor��
	if (auto RowPtr = ConfigDataTable->GetRowMap().Find(FName(Tag)))
	{
		// ���޸Ĺ�����ʽ�����ݱ��浽 GameSave �е�һ�� TMap ��
		if (!Save) Save = Cast<UMainMenuSaveGame>(UGameplayStatics::CreateSaveGameObject(UMainMenuSaveGame::StaticClass()));
		Save->StyleConfigs.Add(FName(Tag), Index);							
		UGameplayStatics::SaveGameToSlot(Save, "MainMenu", 0);				// ���һ��������������Ϊ������ݵ�����

		auto MeshComps = CharacterStyleComp.Find(FName(Tag));				// ���ص���һ�������ָ��,������֮��ű������
		auto RowData = (FCharacterTypeOpteratorConfig*)(*RowPtr);			// �õ������нṹ��ָ��
		if (RowData->bChangeColor)											// ͨ�������޸���ɫ
		{
			TArray<FName> ColorNames;
			RowData->StyleColor.GetKeys(ColorNames);
			FString ColorParamsName = "";
			if (Tag.Contains("Hair")) ColorParamsName = "Hair Tint";
			if (Tag.Contains("Skin")) ColorParamsName = "Skin Tint";
			if (Tag.Contains("Pants")) ColorParamsName = "Pants Tint";

			for (auto& MeshComp : *MeshComps)                              // ��ÿ�����񶼻�����Ҫ����ɫ
				MeshComp->CreateDynamicMaterialInstance(0)->SetVectorParameterValue(FName(ColorParamsName),
					RowData->StyleColor[ColorNames[Index]]);               // (0) ����� - ��ʾ��һ������Ϊֻ��һ������ʵ�����	 ->StyleColor[] �ֵ��������Find()���������ǰ�߻��Զ���֤		

			// ����ί�У�֪ͨ UI ��ʽ�Ѿ��ı�,��Ҫ UI ����ͬ������,��ί���� MainHUD.cpp ��
			GlobalDelegateManager::Get()->CallGenericDelegate<void, const FString&, const FString&>
				(NOTIFY_STYLE_CHANGE, Tag, ColorNames[Index].ToString());  //<void,FString,FString> �޷���ֵ,��ǩ,����
		}
		else                                                               // �ı�����
		{
			TArray<FName> StyleNames;
			RowData->StyleMeshMap.GetKeys(StyleNames);

			for (auto& MeshComp : *MeshComps)
				MeshComp->SetSkeletalMesh(RowData->StyleMeshMap[StyleNames[Index]]);

			// ����ί�У�֪ͨ UI ��ʽ�����Ѿ��ı�,��Ҫ UI����ͬ���ĸ��°�ί���� MainHUD.cpp ��
			GlobalDelegateManager::Get()->CallGenericDelegate<void, const FString&, const FString&>
				(NOTIFY_STYLE_CHANGE, Tag, StyleNames[Index].ToString());
		}
	}
}
