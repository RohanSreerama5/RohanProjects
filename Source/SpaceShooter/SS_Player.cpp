// Fill out your copyright notice in the Description page of Project Settings.


#include "SS_Player.h"

// Sets default values
ASS_Player::ASS_Player()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0; //we need this to make the default pawn possess the player. We want the default pawn to be the spaceship, so it can be controled
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")); 

	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));

	ParticleSystems = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));

	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent")); //encapsulates the ship so we can detect collisions


	ExplosionFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ExplosionFX"));

	DeathExplosionSound = CreateDefaultSubobject<UAudioComponent>(TEXT("DeathExplosionSound"));

	//Calling function on ShipMesh pointer            //Keep Transform rules (in unreal engine)
	//Allows all components to move along with each other in the game (along w the parent)
	CollisionComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ShipMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ParticleSystems->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	DeathExplosionSound->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ExplosionFX->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);


	MaxVelocity = 250.0f; //add f to denote its a float as defined in SS_PLayer.h
	Current_X_Velocity = 0.0f;  //declares initial settings for velocity 
	Current_Y_Velocity = 0.0f;

	//when the player is spawned, this ASSPlayer() constructor will be called. When we create a new object - an ASS_Player object- this constructor will get called


	//starting defaults

	bIsFiring = false; 
	WeaponFireRate = 0.25f; 
	TimeSinceLastShot = 0.0f; 

	PlayerScore = 0.0f; 






}



// Called when the game starts or when spawned --> means player object is created --> constructor is called -->defaults are set
void ASS_Player::BeginPlay()
{
	Super::BeginPlay();

	//need to reference current location and current rotation 
	//this refers to the Current_Location object  referring to the pawn holding this entire script 
	Current_Location = this->GetActorLocation(); //gives us current position of spaceship in space; also "this" is a pointer
	Current_Rotation = this->GetActorRotation(); 

	bHit = false; 
	bDead = false; 

	ExplosionFX->Deactivate(); //turns off explosion for when game begins 
	DeathExplosionSound->Deactivate(); 

	Max_Health = 100.0f; 
	Current_Health = 100.0f; 
	
	Max_Armor = 100.0f; 
	Current_Armor = 100.0f; 

	OnActorBeginOverlap.AddDynamic(this, &ASS_Player::OnBeginOverlap); //binds the overlap func w player

	
}

// Called every frame - 60 frames in a second; time between frames is the deltaTIme 
void ASS_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//test velocity 

	if (Current_X_Velocity != 0.0f || Current_Y_Velocity != 0.0f) { //means we are pressing the buttons to move left and right

		New_Location = FVector(Current_Location.X + (Current_X_Velocity * DeltaTime), Current_Location.Y + (Current_Y_Velocity * DeltaTime), 0); 
		//0 for z-axis
		//mult by deltatime to smooth out. if we have really big Currentt x velocity, rocket will go super fast (that's bad) 

		this->SetActorLocation(New_Location);  //player's location is new location 
		//this refers to ASS_Player::BEhin Play class. The actor (spaceship) holds that class

		Current_Location = New_Location; //we are now in our new location. So this becomes the current location. and we set it equal 
		//in the next calculation, we need the new updated location, so we set current_location to new_location 

	}

	if (Current_Y_Velocity > 0.1f) { //means we have started moving in y dir up 
		

		//this will give spaceship feature to rotate when moving up or down rotates like 45 degrees
		this->SetActorRotation(FRotator(Current_Rotation.Pitch + 45.0f, Current_Rotation.Yaw, Current_Rotation.Roll)); 

		//In World outliner, if I click on spaceship, in the transform area, there is rotation x,y , and z degree measures
		//x is the roll, pitch is the y, and z is the yaw 
	}
	else if (Current_Y_Velocity < -0.1f) { //moving down
		this->SetActorRotation(FRotator(Current_Rotation.Pitch + -45.0f, Current_Rotation.Yaw, Current_Rotation.Roll));
	}
	else {

		this->SetActorRotation(FRotator(Current_Rotation)); //if not moving in y-dir, then resets player to defualt normal rotation 

	}

	//when we move up or down, the ship is getting rotated in the y direction, 45 when we go up and -45 when we go down 

	//this reffering to the spaceship or the object holding the class
	if (this->GetActorLocation().X > Field_Width) //this only takes care of the +x bound (the right side)
		Current_Location = FVector(Field_Width - 1, Current_Location.Y, Current_Location.Z);   //sets spaceship back, stopping it from going past boundary in x-dir
	
	if (this->GetActorLocation().X < -Field_Width) //this only takes care of the -x bound (the left side)
		Current_Location = FVector(-Field_Width + 1, Current_Location.Y, Current_Location.Z);

	if (this->GetActorLocation().Y > Field_Height) //limtis upper bound
		Current_Location = FVector(Current_Location.X, Field_Height - 1, Current_Location.Z);

	if (this->GetActorLocation().Y < -Field_Height) //limits lower bound
		Current_Location = FVector(Current_Location.X, -Field_Height + 1, Current_Location.Z);
}

// Called to bind functionality to input
void ASS_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	 //FName is the name of the function I want to bind. I'm going to bind it to THIS (This function is located in this class), lastly pass the function in 
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASS_Player::MoveRight);
	PlayerInputComponent->BindAxis(FName("MoveUp"), this, &ASS_Player::MoveUp);

}

void ASS_Player::MoveRight(float AxisValue) //this is used to bind the function to the axis mappings in the project settings 
{
	Current_X_Velocity = MaxVelocity * AxisValue; 
}

void ASS_Player::MoveUp(float AxisValue) //if W or A key is pressed, this func is called 
{
	Current_Y_Velocity = MaxVelocity * AxisValue;
}

void ASS_Player::OnBeginOverlap(AActor * PlayerActor, AActor * OtherActor) //this particular function is going to bind to an event tha
{
}
