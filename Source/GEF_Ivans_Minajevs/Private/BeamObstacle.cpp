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
    PrimaryActorTick.bCanEverTick = false;

    damageAmount = 1;
    LaserOnTime = 1.0f;
    LaserOffTime = 2.0f;
    bIsLaserActive = false;

    // Create components
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABeamObstacle::OnOverlapBegin);
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

    // Update Niagara system visibility
    if (BeamFX)
    {
        BeamFX->SetVisibility(bIsLaserActive);
    }

    // Adjust timer interval
    float NextInterval = bIsLaserActive ? LaserOnTime : LaserOffTime;
    GetWorldTimerManager().SetTimer(LaserTimerHandle, this, &ABeamObstacle::ToggleLaser, NextInterval, false);
}

void ABeamObstacle::CallDoDamage(AActor* OtherActor)
{
    if (OtherActor->GetClass()->ImplementsInterface(UDamagableInterface::StaticClass()))
    {
        bool IsDead;
        IDamagableInterface::Execute_DoDamage(OtherActor, 1, IsDead);

    }

    TArray<UActorComponent*> Components;
    Components = OtherActor->GetComponentsByInterface(UDamagableInterface::StaticClass());

    for (UActorComponent* Component : Components)
    {
        if (Component->GetClass()->ImplementsInterface(UDamagableInterface::StaticClass()))
        {

            bool IsDead;
            IDamagableInterface::Execute_DoDamage(Component, 1, IsDead);
            break;
            
        }
    }
}



// Handle overlap
void ABeamObstacle::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    if (OtherActor && (OtherActor != this))
    {
        // Log the name of the overlapping actor
        UE_LOG(LogTemp, Warning, TEXT("Overlapped with: %s"), *OtherActor->GetName());

        // Check if OtherActor is a character
        if (ACharacter* OverlappingCharacter = Cast<ACharacter>(OtherActor))
        {
            // Further check if it's the specific Blueprint class
            if (TargetCharacterBlueprint && OverlappingCharacter->IsA(TargetCharacterBlueprint))
            {
                UE_LOG(LogTemp, Warning, TEXT("Overlapped with the target Blueprint Character!"));
                CallDoDamage(OtherActor);
            }
        }
    }
}