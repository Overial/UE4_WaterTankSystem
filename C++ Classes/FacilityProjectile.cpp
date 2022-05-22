// Copyright Epic Games, Inc. All Rights Reserved.

#include "FacilityProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/EngineTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Sound/SoundCue.h"

// Assets
#include "WaterTank.h"
#include "GlassFeather.h"
#include "Waterfall.h"

AFacilityProjectile::AFacilityProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AFacilityProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AFacilityProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	OnSimPhysHit(OtherActor, OtherComp);
}

void AFacilityProjectile::OnSimPhysHit(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		// Default: 100.0f
		OtherComp->AddImpulseAtLocation(GetVelocity() * 50.0f, GetActorLocation());

		Destroy();
	}
}

/*
void AFacilityProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	// Setting logic when water container is hit
	AActor* OtherActorVar = Cast<AWaterTank>(Other);
	if (OtherActorVar != nullptr)
	{
		OnWaterContainerHit(OtherComp, HitLocation, HitNormal);
	}
}
*/

/*
void AFacilityProjectile::OnWaterContainerHit(UPrimitiveComponent* OtherComp, FVector HitLocation, FVector HitNormal)
{
	// Setting spawn location
	FVector Loc = HitLocation;
	// Setting spawn rotation
	FRotator Rot = HitNormal.Rotation();
	// Setting spawn params
	FActorSpawnParameters SpawnParams;
	// Setting hit component
	UPrimitiveComponent* OtherCompVar = OtherComp;
	// Setting transform rules
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);

	UWorld* const World = GetWorld();
	if (OtherCompVar->GetName() == "Glass")
	{
		if (World != nullptr)
		{
			// Spawning glass feather
			AGlassFeather* SpawnedGlassFeather = World->SpawnActor<AGlassFeather>(this->GlassFeatherToSpawn, Loc, Rot, SpawnParams);
			// Attaching glass feather to water container
			SpawnedGlassFeather->AttachToComponent(OtherCompVar, AttachmentRules);
			// Playing sound at hit location
			UGameplayStatics::PlaySoundAtLocation(this, GlassSmashSound, Loc);

			// Spawning waterfall
			AWaterfall* SpawnedWaterfall = World->SpawnActor<AWaterfall>(this->WaterfallToSpawn, Loc, Rot, SpawnParams);
			// Attaching waterfall to water container
			SpawnedWaterfall->AttachToComponent(OtherCompVar, AttachmentRules);

			if (!(OtherCompVar->IsSimulatingPhysics()))
			{
				Destroy();
			}
		}
	}
}
*/
