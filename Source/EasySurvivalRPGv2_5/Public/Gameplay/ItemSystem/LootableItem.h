// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/Interfaces/InteractiveObject.h"
#include "LootableItem.generated.h"

UCLASS()
class EASYSURVIVALRPGV2_5_API ALootableItem : public AActor,public IInteractiveObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALootableItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComp;				// ��ʰȡ�ĵ��߿����Ǿ�̬����Ҳ�����ǹ�������,��˶�����������
	UPROPERTY(VisibleAnywhere) 
		USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditInstanceOnly)
		FName ItemDataRowName;								// ���߱��е�����
	UPROPERTY(EditInstanceOnly,meta = (ClampMin = 1))       // note=> ����������Сֵ,������Сֵ���ܼ�����С��
		int32 ItemCount = 1;                                // ��ʰȡ���ߵ�����

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ���ɵ���ͨ�� RowName
	UFUNCTION(BlueprintCallable)
		void SpawnItemByRowNameAndCount(FName RowName, int32 Count);	

// ģ�� UObject ����д��
#if WITH_EDITOR
private:
	// override from UOject
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override; 
#endif

private:
	void SetItemMesh(FName RowName);

	void BeginInteractive_Implementation(APlayerController* Controller) override;
	void EndInteractive_Implementation(APlayerController* Controller) override;
	void InteractiveResponse_Implementation(APlayerController* Controller) override;
	bool GetInteractiveInformation_Implementation(FString& Title,FString& Summary,FString& Tip) override;
};
