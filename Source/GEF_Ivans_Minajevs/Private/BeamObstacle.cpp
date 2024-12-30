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
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ABeamObstacle::OnOverlapEnd);
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
        //UE_LOG(LogTemp, Warning, TEXT("Damaged Player Character!"));
        //IDamagableInterface* Damagable = Cast<IDamagableInterface>(OtherActor);
    
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

    //TArray<UActorComponent*> Components;
    //Components = GetComponentsByInterface(UDamagableInterface::StaticClass());
    //
    //// Loop through each component to check if it implements the Blueprint Interface
    //for (UActorComponent* Component : Components)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("Damaged Player Character!"));
    //    // If the component implements the interface, proceed
    //    if (Component->GetClass()->ImplementsInterface(UDamagableInterface::StaticClass()))
    //    {
    //        IDamagableInterface* Damagable = Cast<IDamagableInterface>(Component);
    //        if (Damagable)
    //        {
    //            bool IsDead;
    //            Damagable->DoDamage(1, IsDead);
    //            UE_LOG(LogTemp, Warning, TEXT("Damaged Player Character!"));
    //            break;
    //        }
    //    }
    //}
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
    //if (OtherActor && OtherActor != this)
    //{
    //    // Get the class of the player character blueprint
    //    UClass* PlayerCharacterClass = FindObject<UClass>(nullptr, TEXT("/Game/Blueprints/BP_PlayerCharacter.BP_PlayerCharacter_C"));
    //    if (PlayerCharacterClass && OtherActor->IsA(PlayerCharacterClass))
    //    {
    //        // Get the health component from the player character
    //        UActorComponent* HealthComponent = OtherActor->GetComponentByClass(FindObject<UClass>(nullptr, TEXT("/Game/Blueprints/BPC_HealthComponent.BPC_HealthComponent_C")));
    //        if (HealthComponent)
    //        {
    //            // Call the DoDamage function using reflection
    //            UFunction* DoDamageFunction = HealthComponent->FindFunction(FName("DoDamage"));
    //            if (DoDamageFunction)
    //            {
    //                struct FDamageParams
    //                {
    //                    float DamageAmount;
    //                };
    //
    //                FDamageParams Params;
    //                Params.DamageAmount = 1.0f;
    //
    //                HealthComponent->ProcessEvent(DoDamageFunction, &Params);
    //            }
    //        }
    //    }
    //}
}

void ABeamObstacle::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // Handle end overlap if needed
}
