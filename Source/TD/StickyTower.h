// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DefenseTowerBase.h"
#include "StickyTower.generated.h"


UCLASS()
class TD_API AStickyTower : public ADefenseTowerBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStickyTower();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SlowingRate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Overlap begin function
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* HitComp, class AActor* OtherActor, 
						class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult & SweepResult);

	// Overlap end function
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* HitComp, class AActor* OtherActor, 
					  class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Function to allow Player to set this Tower down */
	void Deploy() override;

	/** To get the blueprint-defined collision box */
	UFUNCTION(BlueprintImplementableEvent)
	class UBoxComponent *GetCollisionBox();
};
