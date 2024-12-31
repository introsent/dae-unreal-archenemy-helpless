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
    virtual void Tick(float DeltaTime) override;

    // Timer for laser toggle
    FTimerHandle LaserTimerHandle;

    // Laser state
    bool bIsLaserActive;

    // Laser timing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Settings")
    float LaserOnTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Settings")
    float LaserOffTime;

    // Target position
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger box")
    FVector TargetPosition;

    // Trigger box
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Beam Settings")
    UBoxComponent* TriggerBox;

    // Damage state
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool didDamageCharacter;

    // Damage on character
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int damageAmount;

    // Niagara beam effect
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Beam Settings")
    UNiagaraComponent* BeamFX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Settings")
    UNiagaraSystem* BeamNiagaraSystem;

    // Target position offset for the beam
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Settings")
    FVector TargetPositionOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<ACharacter> TargetCharacterBlueprint;

    // Toggle laser state
    void ToggleLaser();

    void PerformLineTrace(FVector StartPos, FVector EndPos);

    void CallDoDamage(AActor* OtherActor);
};