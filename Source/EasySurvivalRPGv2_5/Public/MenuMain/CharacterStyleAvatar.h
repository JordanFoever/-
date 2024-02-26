// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "CharacterStyleAvatar.generated.h"

USTRUCT(BlueprintType)
struct FCharacterTypeOpteratorConfig : public FTableRowBase 
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bChangeColor = false;								// 用于确定是修改发型的颜色还是资产

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TMap<FName, USkeletalMesh*> StyleMeshMap;				// 存储身体某部分的样式

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TMap<FName, FLinearColor> StyleColor;

};

UCLASS()
class EASYSURVIVALRPGV2_5_API ACharacterStyleAvatar : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACharacterStyleAvatar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
		USceneComponent* RootComp;

	UPROPERTY(EditDefaultsOnly)
		TSoftObjectPtr<UDataTable> StyleTable;

	UPROPERTY(EditDefaultsOnly)
		TArray<TSoftObjectPtr<USkeletalMesh>> DefaultMeshes;		 // 除了头以外的其它部件 手,身体等

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UAnimInstance> AnimClass;						 // 人物的动画蓝图

	UDataTable* ConfigDataTable;                                     // 保存从蓝图中加载的表的数据
	TSharedPtr<struct FStreamableHandle> TableLoadHandle;			 // struct 是前置声明
	TMap<FName, TArray<USkeletalMeshComponent*>> CharacterStyleComp; // FName 对应的是 StyleTable 表中的 RowName

	USkeletalMeshComponent* HeadMesh;
	USkeletalMeshComponent* BodyMesh;
	USkeletalMeshComponent* HandsMesh;
	USkeletalMeshComponent* LegsMesh;
	USkeletalMeshComponent* FeetMesh;

	class UMainMenuSaveGame* Save;									// 保存实时修改的人物装饰的数据					

	void InitiateAvatar(UDataTable* ConfigTable);					// 加载头
	void ChangeStyle(FString Tag,int PreIndex, int Index);
};
