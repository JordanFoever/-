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

	//FCharacterTypeOpteratorConfig::StaticStruct();                         // 得到结构体类型的引用指针和类是差不多的概念
}

// Called when the game starts or when spawned
void ACharacterStyleAvatar::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<FSoftObjectPath> InitiateAssetPathArray;							// 用于异步请求的对象
	InitiateAssetPathArray.Add(StyleTable.ToSoftObjectPath());
	for (auto&  Mesh: DefaultMeshes)
	{
		InitiateAssetPathArray.Add(Mesh.ToSoftObjectPath());
	}

	TableLoadHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(InitiateAssetPathArray, [=]()  // 这里的 = 号相当于 this
		{
			// 加载 Table_CharacterStyleConfig 表格中的数据
			if (auto Table = Cast<UDataTable>(TableLoadHandle.Get()->GetLoadedAsset()))             // 这里的 lamdom 表达式是加载成功之后调用的
			{
				//UKismetSystemLibrary::PrintString(GetWorld(),LoadObject->GetName());
				InitiateAvatar(Table);
			}
			
			/*加载除了头以外的其它的骨骼网格资产, 并应用到各个部分的网格组件上*/
			//start
			TArray<UObject*> AllAssets;
			TArray<USkeletalMeshComponent*> MeshComp = { BodyMesh,HandsMesh,LegsMesh,FeetMesh };
			TableLoadHandle.Get()->GetLoadedAssets(AllAssets);

			for (int i = 1; i < AllAssets.Num(); i++)
				if (auto Mesh = Cast<USkeletalMesh>(AllAssets[i])) 
					if (MeshComp.IsValidIndex(i-1))
						MeshComp[i - 1]->SetSkeletalMesh(Mesh);
			//end

			// 对人物的修改器中的文本进行初始化
			ChangeStyle("HairStyle", 0, 0);
			ChangeStyle("HairColor", 0, 0);
			ChangeStyle("SkinColor", 0, 0);
			ChangeStyle("PantsColor", 0, 0);

			// 加载动画
			HeadMesh->SetAnimInstanceClass(AnimClass);
			BodyMesh->SetMasterPoseComponent(HeadMesh);
			HandsMesh->SetMasterPoseComponent(HeadMesh);
			LegsMesh->SetMasterPoseComponent(HeadMesh);
			FeetMesh->SetMasterPoseComponent(HeadMesh);

		});

	// 绑定委托,当玩家点击 UI 中加号和减号改变人物骨骼或者颜色时调用
	// 调用委托在 MainHUD.cpp 中的 ModifyChracterStates 的函数中
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
	// 无效或者 表行的结构不是 FCharacterTypeOpteratorConfig 对应的结构体类型直接返回
	if (!IsValid(ConfigTable) || !(ConfigTable->GetRowStruct()->IsChildOf(FCharacterTypeOpteratorConfig::StaticStruct())))return;

	ConfigDataTable = ConfigTable;

	auto RowMap = ConfigTable->GetRowMap();

	TArray<FName> RowNames;																// 得到每一行名字的数组
	RowMap.GetKeys(RowNames); 

	TArray<TArray<USkeletalMeshComponent*>> MeshComp = { {HeadMesh},{HeadMesh},{HeadMesh,BodyMesh,HandsMesh,FeetMesh},{LegsMesh}};

	for (int i = 0; i < RowNames.Num(); i++)
	{
		CharacterStyleComp.Add(RowNames[i], MeshComp[i]);                                  // 将名字和骨骼网格一一对应起来
	}

	/*if (auto RowPtr = RowMap.Find(TEXT("HairStyle")))
	{
		auto HairRowPtr = (FCharacterTypeOpteratorConfig*)(*RowPtr);                        //   *RowPtr 指向表格中某一行的指针

		for (auto& Pair : HairRowPtr->StyleMeshMap)
		{
			HeadMesh->SetSkeletalMesh(Pair.Value);                                          //  Pair.Value 是骨骼网格定位指针
		}
	}*/
}

void ACharacterStyleAvatar::ChangeStyle(FString Tag, int PreIndex,int Index)
{
	if (!IsValid(ConfigDataTable))return;
	 
	// 获取表格中 RowName 为 Tag（eg:HairStyle,HairColor）
	if (auto RowPtr = ConfigDataTable->GetRowMap().Find(FName(Tag)))
	{
		// 将修改过的样式的数据保存到 GameSave 中的一个 TMap 中
		if (!Save) Save = Cast<UMainMenuSaveGame>(UGameplayStatics::CreateSaveGameObject(UMainMenuSaveGame::StaticClass()));
		Save->StyleConfigs.Add(FName(Tag), Index);							
		UGameplayStatics::SaveGameToSlot(Save, "MainMenu", 0);				// 最后一个层参数可以理解为存的数据的索引

		auto MeshComps = CharacterStyleComp.Find(FName(Tag));				// 返回的是一个数组的指针,解引用之后才变成数组
		auto RowData = (FCharacterTypeOpteratorConfig*)(*RowPtr);			// 得到的是行结构的指针
		if (RowData->bChangeColor)											// 通过材质修改颜色
		{
			TArray<FName> ColorNames;
			RowData->StyleColor.GetKeys(ColorNames);
			FString ColorParamsName = "";
			if (Tag.Contains("Hair")) ColorParamsName = "Hair Tint";
			if (Tag.Contains("Skin")) ColorParamsName = "Skin Tint";
			if (Tag.Contains("Pants")) ColorParamsName = "Pants Tint";

			for (auto& MeshComp : *MeshComps)                              // 将每个网格都换成想要的颜色
				MeshComp->CreateDynamicMaterialInstance(0)->SetVectorParameterValue(FName(ColorParamsName),
					RowData->StyleColor[ColorNames[Index]]);               // (0) 这里的 - 表示第一个，因为只有一个材质实例插槽	 ->StyleColor[] 字典运算符和Find()的区别就是前者会自动验证		

			// 调用委托，通知 UI 样式已经改变,需要 UI 进行同步更新,绑定委托在 MainHUD.cpp 中
			GlobalDelegateManager::Get()->CallGenericDelegate<void, const FString&, const FString&>
				(NOTIFY_STYLE_CHANGE, Tag, ColorNames[Index].ToString());  //<void,FString,FString> 无返回值,标签,内容
		}
		else                                                               // 改变网格
		{
			TArray<FName> StyleNames;
			RowData->StyleMeshMap.GetKeys(StyleNames);

			for (auto& MeshComp : *MeshComps)
				MeshComp->SetSkeletalMesh(RowData->StyleMeshMap[StyleNames[Index]]);

			// 调用委托，通知 UI 样式数据已经改变,需要 UI进行同步的更新绑定委托在 MainHUD.cpp 中
			GlobalDelegateManager::Get()->CallGenericDelegate<void, const FString&, const FString&>
				(NOTIFY_STYLE_CHANGE, Tag, StyleNames[Index].ToString());
		}
	}
}
