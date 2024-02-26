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
		bool bChangeColor = false;								// ����ȷ�����޸ķ��͵���ɫ�����ʲ�

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TMap<FName, USkeletalMesh*> StyleMeshMap;				// �洢����ĳ���ֵ���ʽ

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
		TArray<TSoftObjectPtr<USkeletalMesh>> DefaultMeshes;		 // ����ͷ������������� ��,�����

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UAnimInstance> AnimClass;						 // ����Ķ�����ͼ

	UDataTable* ConfigDataTable;                                     // �������ͼ�м��صı������
	TSharedPtr<struct FStreamableHandle> TableLoadHandle;			 // struct ��ǰ������
	TMap<FName, TArray<USkeletalMeshComponent*>> CharacterStyleComp; // FName ��Ӧ���� StyleTable ���е� RowName

	USkeletalMeshComponent* HeadMesh;
	USkeletalMeshComponent* BodyMesh;
	USkeletalMeshComponent* HandsMesh;
	USkeletalMeshComponent* LegsMesh;
	USkeletalMeshComponent* FeetMesh;

	class UMainMenuSaveGame* Save;									// ����ʵʱ�޸ĵ�����װ�ε�����					

	void InitiateAvatar(UDataTable* ConfigTable);					// ����ͷ
	void ChangeStyle(FString Tag,int PreIndex, int Index);
};
