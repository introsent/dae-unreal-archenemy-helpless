#pragma once

#include <Components/BoxComponent.h>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "BeamObstacle.generated.h"


UCLASS()
class GEF_IVANS_MINAJEVS_API ABeamObstacle : public AActor
{
    GENERATED_BODY()

public:
    ABeamObstacle();

protected:
    virtual void BeginPlay() override;

    // Timer for laser toggle
    FTimerHandle LaserTimerHandle;

    // Laser state
    bool bIsLaserActive;

    // Damage amount
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Settings")
    float DamageAmount;

    // Laser timing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Settings")
    float LaserOnTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Settings")
    float LaserOffTime;

    // Trigger box
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Beam Settings")
    UBoxComponent* TriggerBox;

    // Niagara beam effect
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Beam Settings")
    UNiagaraComponent* BeamFX;

    // Niagara system for beam
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Settings")
    UNiagaraSystem* BeamNiagaraSystem;

    // Target position offset for the beam
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Settings")
    FVector TargetPositionOffset;

    // Toggle laser state
    void ToggleLaser();

    // Handle overlap
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    // Update Niagara system parameters
    void UpdateBeamFX();
};