// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "SS_Player.generated.h"

UCLASS()
class SPACESHOOTER_API ASS_Player : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASS_Player();

	USceneComponent* DefaultSceneRoot; 

	UPROPERTY(EditAnywhere)
		UParticleSystemComponent* ParticleSystems;

	UPROPERTY(EditAnywhere)
		UParticleSystemComponent* ExplosionFX;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ShipMesh; 

	UPROPERTY(EditAnywhere)
		UCapsuleComponent* CollisionComponent; 

	UPROPERTY(EditAnywhere)
		UAudioComponent* DeathExplosionSound;

	UPROPERTY(EditAnywhere) //editanywhere vars can be modifed in World Outliner
		float Field_Width; //used to restrict movement of player --need to restrict player from going out of camera view 

	UPROPERTY(EditAnywhere)  //varaible appears anywhere -- in World Outliner 
		float Field_Height; 

	UPROPERTY(BlueprintReadOnly) //variable can only be read in blueprint  .. note: A blue print was used to put the spaceship in the game
		//need this to put this into the blueprint when we make UI
		float Max_Health;

	UPROPERTY(BlueprintReadOnly)
		float Current_Health; 

	UPROPERTY(BlueprintReadOnly)
		float Max_Armor;  //there is armor protecting the ship before we start to lose health. after armor is gone, health bar will start decreas

	UPROPERTY(BlueprintReadOnly) //Blueprintread vars will appear in UI 
		float Current_Armor;

	//functions for moving left and right

	void MoveRight(float AxisValue); //will implement function in SS_player.cpp -- axis value is values defined in axis mappings in proj settings
	void MoveUp(float AxisValue); 

	void FireWeapon();  //used to fire the weapon 
	void StartFiring();
	void StopFiring(); 

	UPROPERTY(EditAnywhere)
		float MaxVelocity; //max velocity player is allowed to move

	bool bIsFiring; 
	float WeaponFireRate; 
	float TimeSinceLastShot; //time since player last took a shot 

	bool bHit; //used to indicate if we are hit
	bool bDead; //used to indicate if we are dead

	float Current_X_Velocity;  //used to calculate curretn velocity 
	float Current_Y_Velocity; 

	UPROPERTY(BlueprintReadOnly) //indicates player score
		float PlayerScore; 

	FVector Current_Location; 
	FRotator Current_Rotation; 
	FVector New_Location; 

	UFUNCTION() //used to denote an event --thats going to be fired //means that this func is going to be bound to an event
		void OnBeginOverlap(AActor* PlayerActor, AActor* OtherActor);  //used to detect an overlap - when the spaceship hits something -an asteroid

	//UPROPERTY(EditAnywhere)
		//TsubclassOf<AProjectile> WeaponProjectile.BP;

	void CollectablePickup(); //function that is called when we pick up some collectible item 




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
