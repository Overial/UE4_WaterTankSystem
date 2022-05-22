// Fill out your copyright notice in the Description page of Project Settings.

#include "WaterPuddle.h"
#include "Components/SceneComponent.h"
#include "Components/DecalComponent.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/Material.h"
#include "Engine/EngineTypes.h"

// Assets
#include "Waterfall.h"
#include "WaterTank.h"

// Sets default values
AWaterPuddle::AWaterPuddle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creating scene root
	this->SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = this->SceneRootComponent;

	// Creating water puddle decal
	this->WaterPuddleDecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("WaterPuddleDecal"));
	this->WaterPuddleDecalComponent->AttachToComponent(this->SceneRootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	this->WaterPuddleDecalComponent->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	this->WaterPuddleDecalComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));

	// Creating water puddle collision box
	this->WaterPuddleCollisionBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("WaterPuddleCollisionBox"));
	this->WaterPuddleCollisionBoxComponent->AttachToComponent(this->SceneRootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	this->WaterPuddleCollisionBoxComponent->SetRelativeScale3D(FVector(2.0f, 2.0f, 0.05f));
	this->WaterPuddleCollisionBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	this->WaterPuddleCollisionBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	// Setting default variables
	this->DeltaWaterPuddleScale = 0.005f;
	this->DeltaWaterPuddleScaleStep = 0.001f;
	this->MaxWaterPuddleScale = 3.0f;
	this->WaterPuddleStartDelay = 3.0f;
	this->WaterPuddleDuration = 5.0f;
	this->IsAbleToFade = false;
	this->flag25 = false;
	this->flag50 = false;
	this->flag75 = false;
}

// Called when the game starts or when spawned
void AWaterPuddle::BeginPlay()
{
	Super::BeginPlay();
	
	// Setting waterfall detector flag
	this->IsUnderWaterfall = false;

	// Setting water puddle fade function
	if (this->IsAbleToFade)
	{
		this->WaterPuddleDecalComponent->SetFadeOut(this->WaterPuddleStartDelay, this->WaterPuddleDuration, true);
	}
}

// Called every frame
void AWaterPuddle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Setting waterfall flag
	SetWaterfallFlag();

	// Scaling puddle under waterfall
	ScaleWaterPuddle();

	// Managing water puddle scale
	ManageWaterPuddleScale();

	// Setting water puddle rotation depending on actor under it
	// SetWaterPuddleRotation();

	// Fixing water puddle collision box scale
	FVector CurrentWPCBCScale = this->WaterPuddleCollisionBoxComponent->GetRelativeScale3D();
	this->WaterPuddleCollisionBoxComponent->SetRelativeScale3D(FVector(CurrentWPCBCScale.X, CurrentWPCBCScale.Y, 0.05f));
}

void AWaterPuddle::ManageWaterPuddleScale()
{
	// Get current water scale
	FVector CurrentWaterPuddleScale = GetActorScale3D();

	if (!this->flag25)
	{
		if ((CurrentWaterPuddleScale.X >= this->MaxWaterPuddleScale * 0.25) &&
			(CurrentWaterPuddleScale.Y >= this->MaxWaterPuddleScale * 0.25) &&
			(CurrentWaterPuddleScale.Z >= this->MaxWaterPuddleScale * 0.25))
		{
			// Decrease water puddle scale
			if (this->DeltaWaterPuddleScale > 0.001f)
			{
				this->DeltaWaterPuddleScale -= this->DeltaWaterPuddleScaleStep;
				this->flag25 = true;
			}
		}
	}

	if (!this->flag50)
	{
		if ((CurrentWaterPuddleScale.X >= this->MaxWaterPuddleScale * 0.5) &&
			(CurrentWaterPuddleScale.Y >= this->MaxWaterPuddleScale * 0.5) &&
			(CurrentWaterPuddleScale.Z >= this->MaxWaterPuddleScale * 0.5))
		{
			// Decrease water puddle scale
			if (this->DeltaWaterPuddleScale > 0.001f)
			{
				this->DeltaWaterPuddleScale -= this->DeltaWaterPuddleScaleStep;
				this->flag50 = true;
			}
		}
	}

	if (!this->flag75)
	{
		if ((CurrentWaterPuddleScale.X >= this->MaxWaterPuddleScale * 0.75) &&
			(CurrentWaterPuddleScale.Y >= this->MaxWaterPuddleScale * 0.75) &&
			(CurrentWaterPuddleScale.Z >= this->MaxWaterPuddleScale * 0.75))
		{
			// Decrease water puddle scale
			if (this->DeltaWaterPuddleScale > 0.001f)
			{
				this->DeltaWaterPuddleScale -= this->DeltaWaterPuddleScaleStep;
				this->flag75 = true;
			}
		}
	}
}

void AWaterPuddle::ScaleWaterPuddle()
{
	if (this->IsUnderWaterfall)
	{
		// Updating water puddle fade function
		if (this->IsAbleToFade)
		{
			this->WaterPuddleDecalComponent->SetFadeOut(this->WaterPuddleStartDelay, this->WaterPuddleDuration, true);
		}

		// Getting current water puddle scale
		FVector CurrentWaterPuddleScale = GetActorScale3D();

		// New actor scale to set
		FVector NewActorScale(0.0f, 0.0f, 0.0f);

		// Setting new water puddle scale
		if ((CurrentWaterPuddleScale.X < this->MaxWaterPuddleScale) &&
			(CurrentWaterPuddleScale.Y < this->MaxWaterPuddleScale) &&
			(CurrentWaterPuddleScale.Z < this->MaxWaterPuddleScale))
		{
			NewActorScale.X = CurrentWaterPuddleScale.X + (this->DeltaWaterPuddleScale * this->VisibleWaterfallCount);
			NewActorScale.Y = CurrentWaterPuddleScale.Y + (this->DeltaWaterPuddleScale * this->VisibleWaterfallCount);
			NewActorScale.Z = CurrentWaterPuddleScale.Z + (this->DeltaWaterPuddleScale * this->VisibleWaterfallCount);

			SetActorScale3D(NewActorScale);
		}
	}
}

/*
void AWaterPuddle::SetWaterPuddleRotation()
{
	// Get overlapping actors array
	TArray<AActor*> OverlappingActorsArray;
	GetOverlappingActors(OverlappingActorsArray);
	int64 LenOAA = OverlappingActorsArray.Num();

	for (int64 i = 0; i < LenOAA; ++i)
	{
		if (this->WasWaterPuddleRotated)
		{
			break;
		}

		AWaterPuddle* WaterPuddleActor = Cast<AWaterPuddle>(OverlappingActorsArray[i]);
		
		// We ignore water puddles, waterfalls and water tanks
		if (!(WaterPuddleActor))
		{
			AWaterfall* WaterfallActor = Cast<AWaterfall>(OverlappingActorsArray[i]);
			if (!(WaterfallActor))
			{
				AWaterTank* WaterTankActor = Cast<AWaterTank>(OverlappingActorsArray[i]);
				if (!(WaterTankActor))
				{
					// Getting rotation of actor under water puddle
					this->OtherActorRot = OverlappingActorsArray[i]->GetActorRotation();

					// Adjust water puddle Y rot
					this->OtherActorRot.Pitch = this->OtherActorRot.Pitch * (-1.0f);

					// Set this water puddle rotation using rotator of other actor
					SetActorRotation(this->OtherActorRot);

					// Setting water puddle rotation flag
					this->WasWaterPuddleRotated = true;
				}
			}
		}
	}
}
*/

void AWaterPuddle::SetWaterfallFlag()
{
	// Setting counters
	this->VisibleWaterfallCount = 0;
	this->WaterfallCount = 0;

	// Setting array with overlapping actors
	TArray<AActor*> OverlappingActorsArray;
	GetOverlappingActors(OverlappingActorsArray);
	int64 LenOAA = OverlappingActorsArray.Num();

	// Detecting waterfall actors
	for (int64 i = 0; i < LenOAA; ++i)
	{
		// Getting waterfall actor
		AWaterfall* WaterfallActor = Cast<AWaterfall>(OverlappingActorsArray[i]);
		if (WaterfallActor != nullptr)
		{
			// Checking if waterfall is visible
			UParticleSystemComponent* PSComp = Cast<UParticleSystemComponent>(WaterfallActor->GetDefaultSubobjectByName(TEXT("WaterfallParticleSystem")));
			if ((PSComp != nullptr) && (PSComp->IsVisible()))
			{
				++this->VisibleWaterfallCount;
			}
		}
	}

	// Setting waterfall flag
	if (this->VisibleWaterfallCount > 0)
	{
		// Setting array with overlapping components
		TArray<UPrimitiveComponent*> OverlappingComponentsArray;
		GetOverlappingComponents(OverlappingComponentsArray);
		int64 LenOCA = OverlappingComponentsArray.Num();

		// Detecting visible overlapping waterfalls
		for (int64 i = 0; i < LenOCA; ++i)
		{
			// Detecting waterfall collision components
			UBoxComponent* BoxComp = Cast<UBoxComponent>(OverlappingComponentsArray[i]);
			if ((BoxComp != nullptr) && (BoxComp->GetName() == "WaterfallCollisionBox"))
			{
				++this->WaterfallCount;
			}
		}

		if (this->WaterfallCount > 0)
		{
			this->IsUnderWaterfall = true;
		}
		else
		{
			this->IsUnderWaterfall = false;
		}
	}
	else
	{
		this->IsUnderWaterfall = false;
	}
}
