#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/Interfaces/InteractiveObject.h"
#include "DestructibleComponent.h"
#include "BuildingPartActor.generated.h"

class UDestructibleComponent;
//用于构建的部件必须改类类型,即构建房子等建筑物必须是该类以及该类的子类

UCLASS()
class EASYSURVIVALRPGV2_5_API ABuildingPartActor : public AActor, public IInteractiveObject
{
	GENERATED_BODY()

		friend class UBuildingSystemComponent;
	
public:	
	// Sets default values for this actor's properties
	ABuildingPartActor();
	
protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SetUp(FName RowName);
	void RecoveryMaterial();
	void UpdatePlacement(FHitResult Hit);
	void RotateBuilding(float RotatorAngle);

	UFUNCTION(BlueprintImplementableEvent)
		 UDestructibleComponent* AddDestructibleCompToActor(AActor* Actor,FTransform InTransform);
	
	// 受到木槌的伤害就会调用该函数
	/*UFUNCTION()
		void BuildingActorTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);*/

	UPROPERTY(EditAnywhere)
		TEnumAsByte<EObjectTypeQuery> PlacementResponseObjectType;

	UPROPERTY(EditAnywhere)
		TEnumAsByte<EObjectTypeQuery> DestructibleComToPawnResponseChannel;

	UPROPERTY(EditAnywhere)
		TEnumAsByte<ETraceTypeQuery> DestructibleComToCameraResponseChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* BuildingPartMesh;
	
	// 可以被建造的材质
	UPROPERTY(EditAnywhere)
		UMaterialInterface* CanBeBuiltMat;
	
	// 不能被建造的材质
	UPROPERTY(EditAnywhere)
		UMaterialInterface* CanNotBrBuiltMat;

	// 是否交互响应
	UPROPERTY(EditAnywhere)
		bool BIsInteractResponse = false;


	UPROPERTY(EditAnywhere)
		bool bDebug = false;

	struct FItemDataTableRow* ItemDataPtr;
	struct FBuildingTableRow* BuildingDataPtr;
	FName BuildingPartRowName = NAME_None;

	bool bPlace = false;                    // 是否已经被设置在了地面或其它地方了
	UMaterialInterface* OriginMat;            // 原始材质s
	// 判断能否吸附
	bool bShouldAttach;
	// 在吸附后是否已经旋转
	bool bAlreadyRotatored;
	FRotator PlaceRotation = FRotator::ZeroRotator;
	bool bRotatorFourDirection;
	bool bAttachCanRotator;
	// 是否和其它的建造部件重叠
	bool bOverlayWithOtherBuildingPart = false;
	float PlaceableTraceRange = 100.f;
	// 在吸附选择吸附到具体的某个方位时是否可以选择
	float bCanRotatorBuildingPart;

public:
	
	bool DestoryBuidlingPart();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BeginInteractive_Implementation(APlayerController* Controller) override;
	void EndInteractive_Implementation(APlayerController* Controller) override;
	void InteractiveResponse_Implementation(APlayerController* Controller) override;
	bool GetInteractiveInformation_Implementation(FString& Title, FString& Summary, FString& Tip) override;

private:

	//建造组件是否和其它重叠
	void IsOverlayWithOther();

	// 检查可放置的物体是否悬空,如床的一个角悬在空中
	void CheckIsInAir();

};