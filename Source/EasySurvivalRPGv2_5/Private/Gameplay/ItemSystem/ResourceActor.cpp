// Easy Systems


#include "Gameplay/ItemSystem/ResourceActor.h"
#include "Gameplay/ItemSystem/WeaponAndTool.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AResourceActor::AResourceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 绑定动态委托
	OnTakeAnyDamage.AddDynamic(this,&AResourceActor::ResourceActorTakeDamage);
}

// Called when the game starts or when spawned
void AResourceActor::BeginPlay()
{
	Super::BeginPlay();
	CurrentHp = Hp;
}

void AResourceActor::ResourceActorTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("DamageCall"));
	auto Tool = Cast<AWeaponAndTool>(DamageCauser);
	if (!Tool) return;

	auto DamageValue = 1.f;
	if (Tool->GetWeaponData().ToolType == EffectiveToolType) DamageValue = Tool->GetWeaponData().BaseDamage;        // 匹配就正常伤害,否者就刮痧

	CurrentHp -= DamageValue;
	ReceiveToolDamage(CurrentHp, Hp);

	if (CurrentHp <= 0)  ResourceExhaust();
}

// Called every frame
void AResourceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
