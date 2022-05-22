// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaterPuddle.generated.h"

UCLASS()
class FACILITY_API AWaterPuddle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaterPuddle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Water Puddle Components")
	USceneComponent* SceneRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Water Puddle Components")
	UDecalComponent* WaterPuddleDecalComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Water Puddle Components")
	class UBoxComponent* WaterPuddleCollisionBoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Puddle Options")
	bool IsAbleToFade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Puddle Options")
	float DeltaWaterPuddleScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Puddle Options")
	float DeltaWaterPuddleScaleStep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Puddle Options")
	float MaxWaterPuddleScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Puddle Options")
	float WaterPuddleStartDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Puddle Options")
	float WaterPuddleDuration;

	bool flag25;
	bool flag50;
	bool flag75;
	bool IsUnderWaterfall;
	// bool WasWaterPuddleRotated;

	int64 WaterfallCount;
	int64 VisibleWaterfallCount;

	// FRotator OtherActorRot;

protected:
	UFUNCTION()
	void ManageWaterPuddleScale();

	UFUNCTION()
	void ScaleWaterPuddle();

	// UFUNCTION()
	// void SetWaterPuddleRotation();

	UFUNCTION()
	void SetWaterfallFlag();
};
