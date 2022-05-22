// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "WaterTank.generated.h"

class AWaterPuddle;

UCLASS()
class FACILITY_API AWaterTank : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaterTank();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Water Container Components")
	UStaticMeshComponent* GlassComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Water Container Components")
	UStaticMeshComponent* LiquidStaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Water Container Components")
	UProceduralMeshComponent* LiquidProceduralMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Water Container Components")
	UStaticMeshComponent* SurfacePlaneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Container Options")
	float FillHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Container Options")
	float Viscosity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Container Options")
	float GlassThickness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Container Options")
	FVector LargeWaterPuddleScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Container Options")
	FVector MediumWaterPuddleScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Container Options")
	FVector SmallWaterPuddleScale;

	UPROPERTY(EditDefaultsOnly, Category = "Water Container Assets")
	USoundBase* ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Water Container Assets")
	UParticleSystem* ExplosionPS;

	UPROPERTY(EditDefaultsOnly, Category = "Water Container Assets")
	TSubclassOf<AWaterPuddle> WaterPuddleToSpawn;

	int VisibleWaterfallCount;
	int WaterfallCount;

	FVector PlanePosition;
	FVector LastPosition;
	FVector LiquidVelocity;
	FVector WorldNormalZ;

protected:
	UFUNCTION()
	FVector GetPlaneNormal();

	UFUNCTION()
	float GetContainerZBound();

	UFUNCTION()
	float GetAngleBetweenVectorsD(FVector A, FVector B);

	UFUNCTION()
	void SetPlanePositionAndRotation();

	UFUNCTION()
	void UpdateLiquid();

	UFUNCTION()
	void DestroyWaterTank();

	UFUNCTION()
	void DepleteWaterTank();
};
