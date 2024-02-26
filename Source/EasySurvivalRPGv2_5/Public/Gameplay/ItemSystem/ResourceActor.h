// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/ItemsSysType.h"

#include "ResourceActor.generated.h"

// �ɲ񼯵���Դ
UCLASS()
class EASYSURVIVALRPGV2_5_API AResourceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AResourceActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UFUNCTION()
		/// <summary>
		/// �ܵ��˺�ʱ�ͻ�����������
		/// </summary>
		/// <param name="DamagedActor"></param>
		/// <param name="Damage"></param>
		/// <param name="DamageType">�˺�����</param>
		/// <param name="InstigatedBy"></param>
		/// <param name="DamageCauser">�˺���������</param>
		void ResourceActorTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(EditDefaultsOnly)
		EToolType EffectiveToolType;      // ��Ч����������

	UPROPERTY(EditDefaultsOnly)
		float Hp = 100.f;                 // ��Դ������ֵ
	
	UPROPERTY(BlueprintReadWrite)
		float CurrentHp;
	
	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveToolDamage(float HpLeft,float TotalHp);

	UFUNCTION(BlueprintImplementableEvent)
		void ResourceExhaust();           // ��Դ�ľ�

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
