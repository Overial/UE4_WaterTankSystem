// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Waterfall.generated.h"

class AWaterPuddle;

UCLASS()
class FACILITY_API AWaterfall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaterfall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Waterfall Components")
	class UParticleSystemComponent* WaterfallParticleSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Waterfall Components")
	class UBoxComponent* WaterfallCollisionBoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waterfall Options")
	FVector WaterPuddleInitialScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waterfall Options")
	float WaterfallMaxAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waterfall Options")
	FVector PSAccel;

	UPROPERTY(EditDefaultsOnly, Category = "Waterfall Assets")
	TSubclassOf<AWaterPuddle> WaterPuddleToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "Waterfall Assets")
	USoundBase* WaterfallSound;

	UPROPERTY(EditDefaultsOnly, Category = "Waterfall Assets")
	class UAudioComponent* WaterfallSoundComponent;

	bool bIsWaterfallVisible;
	bool bHasBeenCollision;
	bool bIsWaterPuddleDetected;

	int64 WaterPuddleActorCount;
	int64 WaterPuddleCompCount;

	FVector CollideLocation;
	FVector CollideNormal;
	FVector WorldNormalZ;

protected:
	UFUNCTION()
	void OnPSCollide(FName EventName, float EmitterTime, int32 ParticleTime, FVector Location, FVector Velocity, FVector Direction, FVector Normal, FName BoneName, UPhysicalMaterial* PhysMat);

	UFUNCTION()
	void SoundManaging();

	UFUNCTION()
	void SpawnWaterPuddle();

	UFUNCTION()
	float GetAngleBetweenVectorsD(FVector A, FVector B);

	UFUNCTION()
	void SetWaterPuddleFlag();

	UFUNCTION()
	void ManageWaterfallDependingOnAngle();

	UFUNCTION()
	void ManageWaterfallDependingOnPlanePosition();

	UFUNCTION()
	void ManageWaterfallDependingOnFillHeight();

	UFUNCTION()
	void SetPSAccelAtRuntime();

	UFUNCTION()
	void Destroyed() override;
};
