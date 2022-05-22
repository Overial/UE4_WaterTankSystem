// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FacilityProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class AGlassFeather;
class AWaterfall;

UCLASS(config=Game)
class AFacilityProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

public:
	AFacilityProjectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

public:
	// Called when projectile hits smth simulating physics
	UFUNCTION()
	void OnSimPhysHit(AActor* OtherActor, UPrimitiveComponent* OtherComp);

	// I temporarily disabled it for BP implementation of projectiles logic
	// Event when this actor bumps into a blocking object, or blocks another actor that bumps into it
	// UFUNCTION()
	// void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	// I temporarily disabled it for BP implementation of projectiles logic
	// Called when water container is hit by projectile
	// UFUNCTION()
	// void OnWaterContainerHit(class UPrimitiveComponent* OtherComp, FVector HitLocation, FVector HitNormal);

public:
	// Glass feather to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Projectile Hit FX")
	TSubclassOf<AGlassFeather> GlassFeatherToSpawn;

	// Waterfall to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Projectile Hit FX")
	TSubclassOf<AWaterfall> WaterfallToSpawn;

	// Glass smash sound to play
	UPROPERTY(EditAnywhere, Category = "Projectile Hit FX")
	USoundBase* GlassSmashSound;
};
