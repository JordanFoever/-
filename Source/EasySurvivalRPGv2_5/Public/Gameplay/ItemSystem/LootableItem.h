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
		UStaticMeshComponent* StaticMeshComp;				// 可拾取的道具可能是静态网格也可能是骨骼网格,因此定义两个变量
	UPROPERTY(VisibleAnywhere) 
		USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditInstanceOnly)
		FName ItemDataRowName;								// 道具表行的名字
	UPROPERTY(EditInstanceOnly,meta = (ClampMin = 1))       // note=> 用于限制最小值,到达最小值后不能继续减小了
		int32 ItemCount = 1;                                // 可拾取道具的数量

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 生成道具通过 RowName
	UFUNCTION(BlueprintCallable)
		void SpawnItemByRowNameAndCount(FName RowName, int32 Count);	

// 模仿 UObject 父类写的
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
