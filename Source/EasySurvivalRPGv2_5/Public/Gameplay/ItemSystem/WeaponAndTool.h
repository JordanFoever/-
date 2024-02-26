// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/Interfaces/OperativeItem.h"
#include "Types/ItemsSysType.h"

#include "WeaponAndTool.generated.h"

UCLASS()
class EASYSURVIVALRPGV2_5_API AWeaponAndTool : public AActor,public IOperativeItem
{
	GENERATED_BODY()
public:	
	
	// Sets default values for this actor's properties
	AWeaponAndTool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MainActionOperative_Implementation(USkeletalMeshComponent* OperatingActorMesh) override;  
	bool CanBeOperated_Implementation() override { return !bIsOperating; }                  
	void EndOperating_Implementation() override;

	void UpdateDamageTracing();           // ���¼��
	TArray<FVector> GetTracingLocations(); // ��ȡ����

	UStaticMeshComponent* Mesh;
	USkeletalMeshComponent* CharacterMesh;
	FWeaponAndToolTableRow* WeaponDataPtr; // ������ĵó������ݣ������ظ����
	bool bDamageTracing = false;
	TArray<FVector> TracingLocations;       // ������������һ�ε�λ��
	TSet<AActor*> TraceActors;

	UPROPERTY(BlueprintReadWrite)
		bool bIsOperating = false;
	UPROPERTY(EditAnywhere)
		FName MontageSlotName;                        // ��̫��۵�����
	UPROPERTY(EditAnywhere)
		FName CharacterWeaponSocketName;              // ��ɫ���ϵ������Ĳ�۵�����
	UPROPERTY(EditAnywhere)
		TEnumAsByte<ETraceTypeQuery> TraceChannel;                 // ���������ͨ��    
	UPROPERTY(EditAnywhere,meta=(ClampMin=2))
		int32 TraceStepNumber = 5;                    // �������ֳɶ��ٶ�
	UPROPERTY(EditAnywhere)
		bool bDebugTrace = false;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Initiate(UStaticMesh* MeshAsset, FName WeaponToolRowName);
	FORCEINLINE FWeaponAndToolTableRow GetWeaponData() { return WeaponDataPtr ? *WeaponDataPtr : FWeaponAndToolTableRow(); }

	UFUNCTION(BlueprintCallable)
		void StartDamageTracing(); // �ɶ����¼�����
	UFUNCTION(BlueprintCallable)
		void EndDamageTracing();   // �ɶ����¼�����
};
