#include "BeamObstacle.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ActorComponent.h"
#include "NiagaraComponent.h"

#include "UObject/ConstructorHelpers.h"
#include "UObject/Interface.h"

#include "TimerManager.h"

#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"

#include "NiagaraSystem.h"
#include "Engine/World.h"

#include "DamagableInterface.h"

class UNiagaraSystem;
class UNiagaraComponent;
ABeamObstacle::ABeamObstacle()
{
    PrimaryActorTick.bCanEverTick = true;

    damageAmount = 1;
    LaserOnTime = 1.0f;
    LaserOffTime = 2.0f;
    bIsLaserActive = false;
    didDamageCharacter = false;

    // Create components
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;

    BeamFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BeamFX"));
    BeamFX->SetupAttachment(RootComponent);
    BeamFX->SetAutoActivate(true);
}


// BeginPlay
void ABeamObstacle::BeginPlay()
{
    Super::BeginPlay();

    if (BeamNiagaraSystem)
    {
        BeamFX->SetAsset(BeamNiagaraSystem);
        BeamFX->Activate();
    }

    // Start laser toggle timer
    GetWorldTimerManager().SetTimer(LaserTimerHandle, this, &ABeamObstacle::ToggleLaser, LaserOffTime, true);
}

// Toggle laser state
void ABeamObstacle::ToggleLaser()
{
    bIsLaserActive = !bIsLaserActive;

    if (bIsLaserActive && didDamageCharacter)
    {
        didDamageCharacter = false;
    }

    // Update Niagara system visibility
    if (BeamFX)
    {
        BeamFX->SetVisibility(bIsLaserActive);
    }

    // Adjust timer interval
    float NextInterval = bIsLaserActive ? LaserOnTime : LaserOffTime;
    GetWorldTimerManager().SetTimer(LaserTimerHandle, this, &ABeamObstacle::ToggleLaser, NextInterval, false);
}


void ABeamObstacle::PerformLineTrace(FVector StartPos, FVector EndPos)
{
    // Perform line trace
    FHitResult HitResult;
    FCollisionQueryParams TraceParams(FName(TEXT("BeamTrace")), true, this);
    TraceParams.bReturnPhysicalMaterial = false;

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartPos,
        EndPos,
        ECC_Visibility,
        TraceParams
    );

    // If hit, apply damage
    if (bHit && HitResult.GetActor())
    {
        UE_LOG(LogTemp, Warning, TEXT("Beam hit: %s"), *HitResult.GetActor()->GetName());
        if (ACharacter* OverlappingCharacter = Cast<ACharacter>(HitResult.GetActor()))
        {
            if (TargetCharacterBlueprint && OverlappingCharacter->IsA(TargetCharacterBlueprint))
            {
                UE_LOG(LogTemp, Warning, TEXT("Overlapped with the target Blueprint Character!"));
                didDamageCharacter = true;
                CallDoDamage(HitResult.GetActor());
               
            }
        }
    }
}

void ABeamObstacle::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!BeamFX)
        return;

    // If laser active and character is not damaged, perform line trace
    if (bIsLaserActive && !didDamageCharacter)
    {
        PerformLineTrace(BeamFX->GetComponentLocation(), BeamFX->GetComponentLocation() + TargetPosition);
    }
}

void ABeamObstacle::CallDoDamage(AActor* OtherActor)
{
    if (OtherActor->GetClass()->ImplementsInterface(UDamagableInterface::StaticClass()))
    {
        bool IsDead;
        IDamagableInterface::Execute_DoDamage(OtherActor, damageAmount, IsDead);

    }

    TArray<UActorComponent*> Components;
    Components = OtherActor->GetComponentsByInterface(UDamagableInterface::StaticClass());

    for (UActorComponent* Component : Components)
    {
        if (Component->GetClass()->ImplementsInterface(UDamagableInterface::StaticClass()))
        {

            bool IsDead;
            IDamagableInterface::Execute_DoDamage(Component, damageAmount, IsDead);
            break;
            
        }
    }
}