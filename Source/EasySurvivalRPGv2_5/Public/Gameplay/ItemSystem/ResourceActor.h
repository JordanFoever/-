// Easy Systems

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/ItemsSysType.h"

#include "ResourceActor.generated.h"

// 可柴集的资源
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
		/// 受到伤害时就会调用这个函数
		/// </summary>
		/// <param name="DamagedActor"></param>
		/// <param name="Damage"></param>
		/// <param name="DamageType">伤害类型</param>
		/// <param name="InstigatedBy"></param>
		/// <param name="DamageCauser">伤害的引起者</param>
		void ResourceActorTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(EditDefaultsOnly)
		EToolType EffectiveToolType;      // 有效的武器类型

	UPROPERTY(EditDefaultsOnly)
		float Hp = 100.f;                 // 资源的生命值
	
	UPROPERTY(BlueprintReadWrite)
		float CurrentHp;
	
	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveToolDamage(float HpLeft,float TotalHp);

	UFUNCTION(BlueprintImplementableEvent)
		void ResourceExhaust();           // 资源耗尽

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
