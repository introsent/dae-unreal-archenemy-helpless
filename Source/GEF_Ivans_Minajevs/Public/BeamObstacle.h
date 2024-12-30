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

    // Laser timing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Settings")
    float LaserOnTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Settings")
    float LaserOffTime;

    // Trigger box
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Beam Settings")
    UBoxComponent* TriggerBox;

    // Trigger box
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int damageAmount;

    // Niagara beam effect
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Beam Settings")
    UNiagaraComponent* BeamFX;

    // Niagara system for beam
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Settings")
    UNiagaraSystem* BeamNiagaraSystem;

    // Target position offset for the beam
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Beam Settings")
    FVector TargetPositionOffset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<ACharacter> TargetCharacterBlueprint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UInterface> TargetDamagableInterface;

    // Toggle laser state
    void ToggleLaser();

    void CallDoDamage(AActor* OtherActor);

    // Handle overlap
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};