#include "BeamObstacle.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"

class UNiagaraSystem;
class UNiagaraComponent;
ABeamObstacle::ABeamObstacle()
{
    PrimaryActorTick.bCanEverTick = false;

    DamageAmount = 20.0f;
    LaserOnTime = 1.0f;
    LaserOffTime = 2.0f;
    bIsLaserActive = false;

    // Create components
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;

    BeamFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BeamFX"));
    BeamFX->SetupAttachment(RootComponent);
    BeamFX->SetAutoActivate(false);

    TargetPositionOffset = FVector(0.0f, 0.0f, 200.0f); // Default target offset
}


// BeginPlay
void ABeamObstacle::BeginPlay()
{
    Super::BeginPlay();

    if (BeamNiagaraSystem)
    {
        BeamFX->SetAsset(BeamNiagaraSystem);
        BeamFX->Activate();

        // Set initial target position
        UpdateBeamFX();
    }

    // Start laser toggle timer
    GetWorldTimerManager().SetTimer(LaserTimerHandle, this, &ABeamObstacle::ToggleLaser, LaserOffTime, true);
}

// Toggle laser state
void ABeamObstacle::ToggleLaser()
{
    bIsLaserActive = !bIsLaserActive;

    // Update Niagara system visibility
    if (BeamFX)
    {
        BeamFX->SetVisibility(bIsLaserActive);
    }

    // Adjust timer interval
    float NextInterval = bIsLaserActive ? LaserOnTime : LaserOffTime;
    GetWorldTimerManager().SetTimer(LaserTimerHandle, this, &ABeamObstacle::ToggleLaser, NextInterval, false);
}

void ABeamObstacle::UpdateBeamFX()
{
    if (BeamFX)
    {
        FVector StartPosition = GetActorLocation();
        FVector TargetPosition = StartPosition + TargetPositionOffset;

        // Update the TargetPosition parameter
        BeamFX->SetVectorParameter(TEXT("TargetPosition"), TargetPosition);
    }
}


// Handle overlap
void ABeamObstacle::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (bIsLaserActive && OtherActor)
    {
        // Check if the overlapping actor is a character
        ACharacter* Character = Cast<ACharacter>(OtherActor);
        if (Character)
        {
            TArray<UActorComponent*> Components;
            Character->GetComponents(Components);

            for (UActorComponent* Component : Components)
            {
                // Check if the component is of the Blueprint class "BPC_HealthComponent"
                if (Component && Component->GetClass()->GetName() == "BPC_HealthComponent")
                {
                    // Call the Blueprint function "DoDamage" on the component
                    UFunction* DoDamageFunction = Component->FindFunction(FName("DoDamage"));
                    if (DoDamageFunction)
                    {
                        struct FDamageParams
                        {
                            float Damage;
                        };

                        FDamageParams Params;
                        Params.Damage = 1;

                        Component->ProcessEvent(DoDamageFunction, &Params);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("DoDamage function not found on BPC_HealthComponent!"));
                    }

                    return; // Stop searching after finding the first matching component
                }
            }
        }
    }
}
