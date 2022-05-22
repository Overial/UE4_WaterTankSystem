// Fill out your copyright notice in the Description page of Project Settings.

#include "Waterfall.h"
#include "Engine/EngineTypes.h"
#include "UObject/NameTypes.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Components/DecalComponent.h"
#include "Sound/SoundBase.h"
#include "Math/Vector.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

// Assets
#include "WaterTank.h"
#include "WaterPuddle.h"

// Sets default values
AWaterfall::AWaterfall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setting water puddle flag
	this->bIsWaterPuddleDetected = false;

	// Setting collision flag
	this->bHasBeenCollision = false;

	// Creating waterfall PS component
	this->WaterfallParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("WaterfallParticleSystem"));
	RootComponent = this->WaterfallParticleSystemComponent;
	this->WaterfallParticleSystemComponent->OnParticleCollide.AddDynamic(this, &AWaterfall::OnPSCollide);

	// Creating waterfall box collision component
	this->WaterfallCollisionBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("WaterfallCollisionBox"));
	this->WaterfallCollisionBoxComponent->AttachToComponent(this->WaterfallParticleSystemComponent, FAttachmentTransformRules::KeepRelativeTransform);
	this->WaterfallCollisionBoxComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 0.25f));

	// Setting default params
	this->WaterPuddleInitialScale = FVector(0.2f, 0.2f, 0.2f);
	this->WaterfallMaxAngle = 60.0f;
	this->PSAccel = FVector(0.0f, 0.0f, -30000.0f);
	this->WorldNormalZ = FVector(0.0f, 0.0f, 1.0f);
}

// Called when the game starts or when spawned
void AWaterfall::BeginPlay()
{
	Super::BeginPlay();

	// Setting acceleration parameter
	this->WaterfallParticleSystemComponent->SetVectorParameter(TEXT("WAccel"), this->PSAccel);

	// Spawning waterfall sound
	this->WaterfallSoundComponent = UGameplayStatics::SpawnSoundAttached(this->WaterfallSound, this->WaterfallParticleSystemComponent);
}

// Called every frame
void AWaterfall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Calling sound managing function
	SoundManaging();

	// Setting location of puddle detector
	this->WaterfallCollisionBoxComponent->SetWorldLocation(this->CollideLocation);

	// Spawning water puddle
	SpawnWaterPuddle();

	// Setting water puddle flag
	SetWaterPuddleFlag();

	// Managing waterfall depenging on angle between actor and Z normal
	ManageWaterfallDependingOnAngle();

	// Managing waterfall depending on plane position in water tank
	ManageWaterfallDependingOnPlanePosition();

	// Managing waterfall depending on fill height in water tank
	ManageWaterfallDependingOnFillHeight();

	// After setting flag we should adjust waterfall acceleration and visibility
	SetPSAccelAtRuntime();
}

void AWaterfall::OnPSCollide(FName EventName, float EmitterTime, int32 ParticleTime, FVector Location, FVector Velocity, FVector Direction, FVector Normal, FName BoneName, UPhysicalMaterial* PhysMat)
{
	// Setting collision flag
	this->bHasBeenCollision = true;

	// Setting hit location
	this->CollideLocation = Location;

	// Setting hit normal
	this->CollideNormal = Normal;
}

void AWaterfall::SoundManaging()
{
	// Turning sound on or off depending on PS visibility
	if (this->WaterfallParticleSystemComponent->IsVisible())
	{
		if (this->WaterfallSoundComponent != nullptr)
		{
			this->WaterfallSoundComponent->SetPaused(false);
		}
	}
	else
	{
		if (this->WaterfallSoundComponent != nullptr)
		{
			this->WaterfallSoundComponent->SetPaused(true);
		}
	}
}

void AWaterfall::SpawnWaterPuddle()
{
	if (!(this->bIsWaterPuddleDetected))
	{
		if (this->WaterfallParticleSystemComponent->IsVisible())
		{
			if (this->bHasBeenCollision)
			{
				UWorld* const World = GetWorld();
				if (World != nullptr)
				{
					// Spawning water puddle if none were detected
					FActorSpawnParameters SpawnParams;
					AWaterPuddle* SpawnedWaterPuddle = World->SpawnActor<AWaterPuddle>(this->WaterPuddleToSpawn, this->CollideLocation, FRotator::ZeroRotator, SpawnParams);
					SpawnedWaterPuddle->SetActorScale3D(this->WaterPuddleInitialScale);
				}
			}
		}
	}
}

float AWaterfall::GetAngleBetweenVectorsD(FVector A, FVector B)
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

void AWaterfall::SetWaterPuddleFlag()
{
	// Setting counters
	this->WaterPuddleActorCount = 0;
	this->WaterPuddleCompCount = 0;

	// Setting array with overlapping actors
	TArray<AActor*> OverlappingActorsArray;
	GetOverlappingActors(OverlappingActorsArray);
	int64 LenOAA = OverlappingActorsArray.Num();

	// Detecting water puddle actors
	for (int64 i = 0; i < LenOAA; ++i)
	{
		// Getting water puddle actor
		AWaterPuddle* WaterPuddleActor = Cast<AWaterPuddle>(OverlappingActorsArray[i]);
		if (WaterPuddleActor != nullptr)
		{
			++this->WaterPuddleActorCount;
		}
	}

	// Setting water puddle flag
	if (this->WaterPuddleActorCount > 0)
	{
		// Setting array with overlapping components
		TArray<UPrimitiveComponent*> OverlappingComponentsArray;
		GetOverlappingComponents(OverlappingComponentsArray);
		int64 LenOCA = OverlappingComponentsArray.Num();

		// Detecting water puddle components
		for (int64 i = 0; i < LenOCA; ++i)
		{
			// Getting water puddle component
			UBoxComponent* WaterPuddleCollisionComponent = Cast<UBoxComponent>(OverlappingComponentsArray[i]);
			if ((WaterPuddleCollisionComponent != nullptr) && (WaterPuddleCollisionComponent->GetName() == "WaterPuddleCollisionBox"))
			{
				++this->WaterPuddleCompCount;
			}
		}

		if (this->WaterPuddleCompCount > 0)
		{
			this->bIsWaterPuddleDetected = true;
		}
		else
		{
			this->bIsWaterPuddleDetected = false;
		}
	}
	else
	{
		this->bIsWaterPuddleDetected = false;
	}
}

void AWaterfall::ManageWaterfallDependingOnAngle()
{
	FVector WaterfallForwardVector = GetActorForwardVector();
	float WaterfallAngle = GetAngleBetweenVectorsD(WaterfallForwardVector, this->WorldNormalZ);

	if ((WaterfallAngle >= -this->WaterfallMaxAngle) && (WaterfallAngle <= this->WaterfallMaxAngle))
	{
		this->bIsWaterfallVisible = false;
	}
	else
	{
		this->bIsWaterfallVisible = true;
	}
}

void AWaterfall::ManageWaterfallDependingOnPlanePosition()
{
	AWaterTank* AttachParentActor = Cast<AWaterTank>(GetAttachParentActor());
	if (AttachParentActor != nullptr)
	{
		FVector WTPlanePos = AttachParentActor->PlanePosition;
		FVector WaterfallPos = GetActorLocation();

		if (WTPlanePos.Z <= WaterfallPos.Z)
		{
			this->bIsWaterfallVisible = false;
		}
	}
}

void AWaterfall::ManageWaterfallDependingOnFillHeight()
{
	AWaterTank* AttachParentActor = Cast<AWaterTank>(GetAttachParentActor());
	if (AttachParentActor != nullptr)
	{
		if (AttachParentActor->FillHeight <= 0.0f)
		{
			this->bIsWaterfallVisible = false;
		}
	}
}

void AWaterfall::SetPSAccelAtRuntime()
{
	if (!(this->bIsWaterfallVisible))
	{
		if (this->PSAccel != FVector(0.0f, 0.0f, -30000.0f))
		{
			this->PSAccel -= FVector(0.0f, 0.0f, 1000.0f);
			this->WaterfallParticleSystemComponent->SetVectorParameter(TEXT("WAccel"), this->PSAccel);
		}
		else
		{
			this->WaterfallParticleSystemComponent->SetVisibility(false);
		}
	}
	else
	{
		this->WaterfallParticleSystemComponent->SetVisibility(true);

		if (this->PSAccel != FVector(0.0f, 0.0f, -5000.0f))
		{
			this->PSAccel += FVector(0.0f, 0.0f, 1000.0f);
			this->WaterfallParticleSystemComponent->SetVectorParameter(TEXT("WAccel"), this->PSAccel);
		}
	}
}

void AWaterfall::Destroyed()
{
	// Disabling sound when waterfall is destroyed
	if (this->WaterfallSoundComponent != nullptr)
	{
		WaterfallSoundComponent->ToggleActive();
	}
}
