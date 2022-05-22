// Fill out your copyright notice in the Description page of Project Settings.

#include "WaterTank.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "KismetProceduralMeshLibrary.h"
#include "Math/Vector.h"
#include "Math/UnrealMathUtility.h"
#include "Containers/Array.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystem.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"

// Assets
#include "GlassFeather.h"
#include "Waterfall.h"
#include "WaterPuddle.h"

// Sets default values
AWaterTank::AWaterTank()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creating glass component
	this->GlassComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Glass"));
	RootComponent = this->GlassComponent;
	this->GlassComponent->SetWorldScale3D(FVector(1.5f, 1.5f, 1.5f));
	this->GlassComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	this->GlassComponent->SetSimulatePhysics(false);

	// Creating liquid static mesh component
	this->LiquidStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LiquidStaticMesh"));
	this->LiquidStaticMeshComponent->AttachToComponent(this->GlassComponent, FAttachmentTransformRules::KeepRelativeTransform);
	this->LiquidStaticMeshComponent->SetRelativeScale3D(FVector(0.9f, 0.9f, 0.9f));
	this->LiquidStaticMeshComponent->SetHiddenInGame(true);

	// Creating liquid procedural mesh component
	this->LiquidProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("LiquidProceduralMesh"));
	this->LiquidProceduralMeshComponent->AttachToComponent(this->GlassComponent, FAttachmentTransformRules::KeepRelativeTransform);
	this->LiquidProceduralMeshComponent->SetSimulatePhysics(true);
	this->LiquidProceduralMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	this->LiquidProceduralMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	// Creating surface plane component
	this->SurfacePlaneComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SurfacePlane"));
	this->SurfacePlaneComponent->AttachToComponent(this->GlassComponent, FAttachmentTransformRules::KeepRelativeTransform);
	this->SurfacePlaneComponent->SetRelativeScale3D(FVector(1.2f, 1.2f, 1.2f));
	this->SurfacePlaneComponent->SetGenerateOverlapEvents(false);
	this->SurfacePlaneComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	this->SurfacePlaneComponent->SetVisibility(false);
	this->SurfacePlaneComponent->SetHiddenInGame(true);
	
	// Setting default params
	this->FillHeight = 50.0f;
	this->Viscosity = 90.0f;
	this->GlassThickness = 1.5f;
	this->LargeWaterPuddleScale = FVector(4.0f, 4.0f, 4.0f);
	this->MediumWaterPuddleScale = FVector(2.0f, 2.0f, 2.0f);
	this->SmallWaterPuddleScale = FVector(1.0f, 1.0f, 1.0f);
	this->WorldNormalZ = FVector(0.0f, 0.0f, 1.0f);
}

// Called when the game starts or when spawned
void AWaterTank::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWaterTank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Setting plane pos and rot
	SetPlanePositionAndRotation();

	// Updating liquid
	UpdateLiquid();

	// Checking if we should destroy water tank every frame
	DestroyWaterTank();

	// Depleting water tank if there are any visible waterfalls
	DepleteWaterTank();
}

FVector AWaterTank::GetPlaneNormal()
{
	FRotator SPCRot = this->SurfacePlaneComponent->GetComponentRotation();

	float X = UKismetMathLibrary::DegTan(SPCRot.Roll);
	float Y = UKismetMathLibrary::DegTan(SPCRot.Pitch);
	float Z = UKismetMathLibrary::DegCos(SPCRot.Yaw);

	FVector PlaneNormal(Y, -X, -Z);

	return PlaneNormal;
}

float AWaterTank::GetContainerZBound()
{
	FVector Origin;
	FVector BoxExtent;
	float SphereRadius;
	UKismetSystemLibrary::GetComponentBounds(this->GlassComponent, Origin, BoxExtent, SphereRadius);

	float A = BoxExtent.Z / 1.0f;

	float B = (this->GlassComponent->GetComponentScale().Z / 2.0f) * 50.0f;

	float ContainerZBound = A / B;

	return ContainerZBound;
}

float AWaterTank::GetAngleBetweenVectorsD(FVector A, FVector B)
{
	// Normalizing vectors
	A.Normalize(0.0001f);
	B.Normalize(0.0001f);

	// Getting dot product
	float DPResult = FVector::DotProduct(A, B);

	// Calculating acos
	float AngleR = UKismetMathLibrary::Acos(DPResult);

	// Getting angle in degrees
	float AngleD = FMath::RadiansToDegrees(AngleR);

	return AngleD;
}

void AWaterTank::SetPlanePositionAndRotation()
{
	float A = (this->FillHeight - 50.0f) * GetContainerZBound();

	FVector Origin;
	FVector BoxExtent;
	float SphereRadius;
	UKismetSystemLibrary::GetComponentBounds(this->SurfacePlaneComponent, Origin, BoxExtent, SphereRadius);

	float B = BoxExtent.Z * ((1.0f - (this->FillHeight / 10.0f)) / (this->FillHeight + 1.0f));

	float C = A - B;

	FVector NewPlanePos = this->SurfacePlaneComponent->GetComponentLocation() + FVector(0.0f, 0.0f, C);

	// Setting plane position
	this->PlanePosition = NewPlanePos;
	
	FVector A1 = this->LiquidVelocity / 100.0f;
	float B1 = 20.0f - (0.2f * this->Viscosity);
	FVector C1 = A1 * B1;

	FRotator NewPlaneRot = UKismetMathLibrary::MakeRotator(C1.X, C1.Y, C1.Z);

	// Setting plane rotation
	this->SurfacePlaneComponent->SetWorldRotation(NewPlaneRot);
}

void AWaterTank::UpdateLiquid()
{
	UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(this->LiquidStaticMeshComponent, 0, this->LiquidProceduralMeshComponent, false);

	UProceduralMeshComponent* OutOtherHalfProcMesh;
	UKismetProceduralMeshLibrary::SliceProceduralMesh(this->LiquidProceduralMeshComponent,
													  this->PlanePosition,
													  GetPlaneNormal(),
													  false,
													  OutOtherHalfProcMesh,
													  EProcMeshSliceCapOption::UseLastSectionForCap,
													  this->LiquidStaticMeshComponent->GetMaterial(0));

	this->LastPosition = this->LiquidProceduralMeshComponent->GetComponentLocation();
	this->LiquidProceduralMeshComponent->SetWorldTransform(this->GlassComponent->GetComponentTransform());
	FVector NewPMScale = this->GlassComponent->GetComponentScale() / ((this->GlassThickness * 0.1f) + 1.0f);
	this->LiquidProceduralMeshComponent->SetRelativeScale3D(NewPMScale);

	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		FVector A = this->LiquidProceduralMeshComponent->GetComponentLocation() - this->LastPosition;
		FVector NewLV = A / World->GetDeltaSeconds();
		this->LiquidVelocity = NewLV;
	}
}

void AWaterTank::DestroyWaterTank()
{
	// Resetting glass feather counter
	this->WaterfallCount = 0;

	// Getting array with attached actors
	TArray<AActor*> AttachedActorsArray;
	GetAttachedActors(AttachedActorsArray);
	int64 LenAAA = AttachedActorsArray.Num();

	// Counting amount of glass feathers
	for (int64 i = 0; i < LenAAA; ++i)
	{
		AWaterfall* WaterfallActor = Cast<AWaterfall>(AttachedActorsArray[i]);
		if (WaterfallActor != nullptr)
		{
			++this->WaterfallCount;
		}
	}

	// Destroying water tank if there are more than 5 glass feathers
	if (this->WaterfallCount >= 5)
	{
		for (int64 i = LenAAA - 1; i >= 0; --i)
		{
			AttachedActorsArray[i]->Destroy();
		}

		this->Destroy();

		// FX
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			// Sound
			UGameplayStatics::PlaySoundAtLocation(World, this->ExplosionSound, GetActorLocation());
			
			// Explosion
			UGameplayStatics::SpawnEmitterAtLocation(World, this->ExplosionPS, GetActorLocation(), FRotator::ZeroRotator, FVector(1.0f, 1.0f, 1.0f));

			// Large water puddle (water volume > 70%)
			if (this->FillHeight >= 70.0f)
			{
				// FVector WaterPuddleSpawnLocation = GetActorLocation() - FVector(0.0f, 0.0f, GetActorLocation().Z);
				FHitResult OutHit;
				World->LineTraceSingleByChannel(OutHit,
										 		GetActorLocation(),
										 		GetActorLocation() - FVector(0.0f, 0.0f, 200.0f),
										 		ECollisionChannel::ECC_WorldStatic);
				FVector WaterPuddleSpawnLocation = OutHit.Location;
				FActorSpawnParameters SpawnParams;
				AWaterPuddle* SpawnedWaterPuddle = World->SpawnActor<AWaterPuddle>(this->WaterPuddleToSpawn, WaterPuddleSpawnLocation, FRotator::ZeroRotator, SpawnParams);
				SpawnedWaterPuddle->SetActorScale3D(this->LargeWaterPuddleScale);
			}
			
			// Medium water puddle (40% <= water volume < 70%)
			if ((this->FillHeight >= 40.0f) && (this->FillHeight < 70.0f))
			{
				// FVector WaterPuddleSpawnLocation = GetActorLocation() - FVector(0.0f, 0.0f, GetActorLocation().Z);
				FHitResult OutHit;
				World->LineTraceSingleByChannel(OutHit,
										 		GetActorLocation(),
										 		GetActorLocation() - FVector(0.0f, 0.0f, 200.0f),
										 		ECollisionChannel::ECC_WorldStatic);
				FVector WaterPuddleSpawnLocation = OutHit.Location;
				FActorSpawnParameters SpawnParams;
				AWaterPuddle* SpawnedWaterPuddle = World->SpawnActor<AWaterPuddle>(this->WaterPuddleToSpawn, WaterPuddleSpawnLocation, FRotator::ZeroRotator, SpawnParams);
				SpawnedWaterPuddle->SetActorScale3D(FVector(this->MediumWaterPuddleScale));
			}

			// Small water puddle (10% <= water volume < 40%)
			if ((this->FillHeight >= 10.0f) && (this->FillHeight < 40.0f))
			{
				// FVector WaterPuddleSpawnLocation = GetActorLocation() - FVector(0.0f, 0.0f, GetActorLocation().Z);
				FHitResult OutHit;
				World->LineTraceSingleByChannel(OutHit,
										 		GetActorLocation(),
										 		GetActorLocation() - FVector(0.0f, 0.0f, 200.0f),
										 		ECollisionChannel::ECC_WorldStatic);
				FVector WaterPuddleSpawnLocation = OutHit.Location;
				FActorSpawnParameters SpawnParams;
				AWaterPuddle* SpawnedWaterPuddle = World->SpawnActor<AWaterPuddle>(this->WaterPuddleToSpawn, WaterPuddleSpawnLocation, FRotator::ZeroRotator, SpawnParams);
				SpawnedWaterPuddle->SetActorScale3D(FVector(this->SmallWaterPuddleScale));
			}

			// No water puddle (water volume = 0%)
		}
	}
}

void AWaterTank::DepleteWaterTank()
{
	// Resetting visible waterfall counter every frame
	this->VisibleWaterfallCount = 0;

	// Getting array with attached actors
	TArray<AActor*> AttachedActorsArray;
	GetAttachedActors(AttachedActorsArray);
	int64 LenAAA = AttachedActorsArray.Num();

	// Counting visible waterfalls
	for (int64 i = 0; i < LenAAA; ++i)
	{
		AWaterfall* WaterfallActor = Cast<AWaterfall>(AttachedActorsArray[i]);
		if (WaterfallActor != nullptr)
		{
			if (WaterfallActor->WaterfallParticleSystemComponent->IsVisible())
			{
				++this->VisibleWaterfallCount;
			}
		}
	}

	// Depleting water tank depending on amount of visible waterfalls
	if (this->VisibleWaterfallCount > 0)
	{
		this->FillHeight -= this->VisibleWaterfallCount * 0.1f;
	}
}
