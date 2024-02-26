// Easy Systems

#include "Gameplay/ItemSystem/WeaponAndTool.h"
#include "Utils/CommonUtilLibrary.h"
#include "Types/ItemsSysType.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWeaponAndTool::AWeaponAndTool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}

// Called when the game starts or when spawned
void AWeaponAndTool::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponAndTool::MainActionOperative_Implementation(USkeletalMeshComponent* OperatingActorMesh)
{	
	if (!WeaponDataPtr)return;

	CharacterMesh = OperatingActorMesh;  // ÿ�λӶ������񱣴�����
	OperatingActorMesh->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(WeaponDataPtr->MissAnim, MontageSlotName);
	bIsOperating = true;
}

void AWeaponAndTool::EndOperating_Implementation()
{
	bIsOperating = false;
}

void AWeaponAndTool::UpdateDamageTracing()
{
	if (!bDamageTracing) return;

	auto CurrentTracingLocations = GetTracingLocations();
	
	for (int32 i = 0; i < CurrentTracingLocations.Num(); i++)
	{
		auto Start = TracingLocations[i];
		auto End = CurrentTracingLocations[i];

		auto TraceDirection = (End - Start).GetSafeNormal();   // �õ��Ӷ��ķ���

		FHitResult OutHit;
		if(UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			Start,
			End + TraceDirection * WeaponDataPtr->TraceOffset,
			TraceChannel,
			false,
			{CharacterMesh->GetOwner()},
			bDebugTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
			OutHit,

			false
		))
		{
			TraceActors.Add(OutHit.GetActor());
			if (TraceActors.Num() > WeaponDataPtr->DamageCount) bDamageTracing = false;
		}
	}

	TracingLocations = GetTracingLocations();
}

TArray<FVector> AWeaponAndTool::GetTracingLocations()
{
	if (!CharacterMesh || !WeaponDataPtr) return TArray<FVector>();

	// ��Ҫ�õ��ֵ������Ĳ�ۣ�Ҳ������������㣩
	auto SocketTrans = CharacterMesh->GetSocketTransform(CharacterWeaponSocketName);
	TArray<FVector> TransPoints = { SocketTrans.GetLocation() };
	auto StepLength = WeaponDataPtr->Length / TraceStepNumber;         // ���ÿһ�εĳ���
	
	for (int32 i = 1; i <= TraceStepNumber; i++)
		TransPoints.Add(SocketTrans.GetLocation() + SocketTrans.GetRotation().GetUpVector() * StepLength * i );
	
	return TransPoints;
}

// Called every frame
void AWeaponAndTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateDamageTracing();
}

void AWeaponAndTool::Initiate(UStaticMesh* MeshAsset,FName WeaponToolRowName)
{
	if (WeaponToolRowName == NAME_None) 
		WeaponDataPtr = nullptr;
	else
		WeaponDataPtr = UCommonUtilLibrary::GetTableRow<FWeaponAndToolTableRow>(TEXT("WeaponAndToolTable"), WeaponToolRowName);
		
	Mesh->SetStaticMesh(MeshAsset);
}

void AWeaponAndTool::StartDamageTracing()
{
	bDamageTracing = true;
	TracingLocations = GetTracingLocations();
}

void AWeaponAndTool::EndDamageTracing()
{
	bDamageTracing = false;
	if (TraceActors.Num() == 0) return;       // ʲô��û�м�⵽

	if (IsValid(WeaponDataPtr->HitAnim))      // ���Ż��ж���
		CharacterMesh->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(WeaponDataPtr->HitAnim, MontageSlotName,
			0.05f, .2f, 1.f, 1, -1.f, WeaponDataPtr->HitAnimDelay);

	for (auto& Actor : TraceActors)
		UGameplayStatics::ApplyDamage(Actor, WeaponDataPtr->BaseDamage, 0, this, 0);      // this ������˺��Ĺ���
	
	TraceActors.Empty();
}
