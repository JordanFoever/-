#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/Interfaces/InteractiveObject.h"
#include "DestructibleComponent.h"
#include "BuildingPartActor.generated.h"

class UDestructibleComponent;
//���ڹ����Ĳ��������������,���������ӵȽ���������Ǹ����Լ����������

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
	
	// �ܵ�ľ鳵��˺��ͻ���øú���
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
	
	// ���Ա�����Ĳ���
	UPROPERTY(EditAnywhere)
		UMaterialInterface* CanBeBuiltMat;
	
	// ���ܱ�����Ĳ���
	UPROPERTY(EditAnywhere)
		UMaterialInterface* CanNotBrBuiltMat;

	// �Ƿ񽻻���Ӧ
	UPROPERTY(EditAnywhere)
		bool BIsInteractResponse = false;


	UPROPERTY(EditAnywhere)
		bool bDebug = false;

	struct FItemDataTableRow* ItemDataPtr;
	struct FBuildingTableRow* BuildingDataPtr;
	FName BuildingPartRowName = NAME_None;

	bool bPlace = false;                    // �Ƿ��Ѿ����������˵���������ط���
	UMaterialInterface* OriginMat;            // ԭʼ����s
	// �ж��ܷ�����
	bool bShouldAttach;
	// ���������Ƿ��Ѿ���ת
	bool bAlreadyRotatored;
	FRotator PlaceRotation = FRotator::ZeroRotator;
	bool bRotatorFourDirection;
	bool bAttachCanRotator;
	// �Ƿ�������Ľ��첿���ص�
	bool bOverlayWithOtherBuildingPart = false;
	float PlaceableTraceRange = 100.f;
	// ������ѡ�������������ĳ����λʱ�Ƿ����ѡ��
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

	//��������Ƿ�������ص�
	void IsOverlayWithOther();

	// ���ɷ��õ������Ƿ�����,�紲��һ�������ڿ���
	void CheckIsInAir();

};