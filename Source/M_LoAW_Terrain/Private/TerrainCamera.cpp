// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainCamera.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "TerrainInput.h"
#include "TerrainGenerator.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"

DEFINE_LOG_CATEGORY(TerrainCamera);

// Sets default values
ATerrainCamera::ATerrainCamera()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(GetRootComponent());
	CameraSpringArm->bDoCollisionTest = 0;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraSpringArm);

	//AutoPossessPlayer = EAutoReceiveInput::Player0;
	BindDelegate();
}

void ATerrainCamera::BindDelegate()
{
	TerrainInputDelegate.BindUFunction(Cast<UObject>(this), TEXT("OnTerrainInput"));
	TerrainInfoDelegate.BindUFunction(Cast<UObject>(this), TEXT("OnTerrainInfo"));
	ScrollScreenDelegate.BindUFunction(Cast<UObject>(this), TEXT("OnScrollScreen"));
}

bool ATerrainCamera::InitCamera()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController) {
		CameraSpringArm->bUsePawnControlRotation = true;
		CameraSpringArm->TargetArmLength = InitArmLength;
		CameraSpringArm->bEnableCameraLag = true;
		CameraSpringArm->bEnableCameraRotationLag = true;
		CameraSpringArm->CameraLagSpeed = CameraLagSpeed;

		PlayerController->RotationInput.Pitch = PI / 180.0 * FMath::Abs(ViewPitchMax);
		PlayerController->PlayerCameraManager->ViewPitchMax = ViewPitchMax;
		PlayerController->PlayerCameraManager->ViewPitchMin = ViewPitchMin;

		return true;
	}
	UE_LOG(TerrainCamera, Warning, TEXT("InitCamera error!"));
	return false;
}

void ATerrainCamera::OnCameraMove(const FInputActionValue& Value)
{
	if (pTI != nullptr) {
		if (pTI->IsLeftHold()) {
			Move(Value.Get<FVector2D>());
		}
	}
}

void ATerrainCamera::OnCameraKeyMove(const FInputActionValue& Value)
{
	KeyMove(Value.Get<FVector2D>());
}

void ATerrainCamera::OnCameraRotate(const FInputActionValue& Value)
{
	if (pTI != nullptr) {
		if (pTI->IsRightHold()) {
			FVector2D Axis2D = Value.Get<FVector2D>();
			AddControllerYawInput(Axis2D.X * RotateSpeed);
			AddControllerPitchInput(-Axis2D.Y * RotateSpeed);
		}
	}
}

void ATerrainCamera::OnCameraKeyRotate(const FInputActionValue& Value)
{
	float Axis = Value.Get<float>();
	AddControllerYawInput(Axis * KeyRotateSpeed);
}

void ATerrainCamera::OnCameraZoomIn(const FInputActionValue& Value)
{
	TargetArmLength = FMath::Clamp(CameraSpringArm->TargetArmLength - ZoomStep, ZoomMin, ZoomMax);
}

void ATerrainCamera::OnCameraZoomOut(const FInputActionValue& Value)
{
	TargetArmLength = FMath::Clamp(CameraSpringArm->TargetArmLength + ZoomStep, ZoomMin, ZoomMax);
}

void ATerrainCamera::OnTerrainInput()
{
	TArray<AActor*> FoundActors;
	FindActors(ATerrainInput::StaticClass(), FoundActors);
	if (FoundActors.Num() == 1) {
		pTI = Cast<ATerrainInput>(FoundActors[0]);
	}
	else {
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, TerrainInputDelegate, TimingForWaitTerrain, false);
	}
}

void ATerrainCamera::OnTerrainInfo()
{
	TArray<AActor*> FoundActors;
	FindActors(ATerrainGenerator::StaticClass(), FoundActors);
	if (FoundActors.Num() == 1) {
		pTG = Cast<ATerrainGenerator>(FoundActors[0]);
		if (pTG->IsWorkFlowStepDone(Enum_TerrainGeneratorState::InitWorkflow)) {
			BoundaryMin.Set(-BoundaryScalar * pTG->GetSize(), -BoundaryScalar * pTG->GetSize(), -1);
			BoundaryMax.Set(BoundaryScalar * pTG->GetSize(), BoundaryScalar * pTG->GetSize(), 1);
			AltitudeMultiplier = pTG->GetTileAltitudeMultiplier();
			CalZoomMin();
		}
		else {
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, TerrainInfoDelegate, TimingForWaitTerrain, false);
		}
	}
	else {
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, TerrainInfoDelegate, TimingForWaitTerrain, false);
	}
}

void ATerrainCamera::FindActors(TSubclassOf<AActor> ActorClass, TArray<AActor*>& Actors)
{
	UWorld* World = GetWorld();
	if (World) {
		UGameplayStatics::GetAllActorsOfClass(World, ActorClass, Actors);
	}
}

void ATerrainCamera::CalZoomMin()
{
	float CameraArmLenMin = AltitudeMultiplier / FMath::Sin(PI / 180.0 * FMath::Abs(ViewPitchMax));
	ZoomMin = ZoomMin > (CameraArmLenMin + ZoomMinOffset) ? ZoomMin : CameraArmLenMin;

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController && InitArmLength < ZoomMin) {
		CameraSpringArm->TargetArmLength = ZoomMin;
	}

}

void ATerrainCamera::OnScrollScreen()
{
	Scroll();
}

void ATerrainCamera::Scroll()
{
	ScrollLeft();
	ScrollRight();
	ScrollUp();
	ScrollDown();
}

void ATerrainCamera::ScrollLeft()
{
	if (ULocalPlayer* LocalPlayer = Cast<APlayerController>(GetController())->GetLocalPlayer()) {
		FVector2D MousePos;
		if (LocalPlayer->ViewportClient->GetMousePosition(MousePos)) {
			FVector2D size;
			LocalPlayer->ViewportClient->GetViewportSize(size);
			float value = UKismetMathLibrary::NormalizeToRange(MousePos.X, 0.f, size.X * ScrollLeftLimitation);
			value = UKismetMathLibrary::FClamp(value, 0.f, 1.f);
			value = 1.f - value;
			if (value > 0.f) {
				OffsetByVector(FVector(0.f, size.X / size.Y * -ScrollSpeedRatio, 0.f), ScrollScalar);
			}
		}
	}
}

void ATerrainCamera::ScrollRight()
{
	if (ULocalPlayer* LocalPlayer = Cast<APlayerController>(GetController())->GetLocalPlayer()) {
		FVector2D MousePos;
		if (LocalPlayer->ViewportClient->GetMousePosition(MousePos)) {
			FVector2D size;
			LocalPlayer->ViewportClient->GetViewportSize(size);
			float value = UKismetMathLibrary::NormalizeToRange(MousePos.X, size.X * ScrollRightLimitation, size.X);
			value = UKismetMathLibrary::FClamp(value, 0.f, 1.f);
			if (value > 0.f) {
				OffsetByVector(FVector(0.f, size.X / size.Y * ScrollSpeedRatio, 0.f), ScrollScalar);
			}
		}
	}
}

void ATerrainCamera::ScrollUp()
{
	if (ULocalPlayer* LocalPlayer = Cast<APlayerController>(GetController())->GetLocalPlayer()) {
		FVector2D MousePos;
		if (LocalPlayer->ViewportClient->GetMousePosition(MousePos)) {
			FVector2D size;
			LocalPlayer->ViewportClient->GetViewportSize(size);
			float value = UKismetMathLibrary::NormalizeToRange(MousePos.Y, 0.f, size.Y * ScrollUpLimitation);
			value = UKismetMathLibrary::FClamp(value, 0.f, 1.f);
			value = 1.f - value;
			if (value > 0.f) {
				OffsetByVector(FVector(1.f, 0.f, 0.f), ScrollScalar);
			}
		}
	}
}

void ATerrainCamera::ScrollDown()
{
	if (ULocalPlayer* LocalPlayer = Cast<APlayerController>(GetController())->GetLocalPlayer()) {
		FVector2D MousePos;
		if (LocalPlayer->ViewportClient->GetMousePosition(MousePos)) {
			FVector2D size;
			LocalPlayer->ViewportClient->GetViewportSize(size);
			float value = UKismetMathLibrary::NormalizeToRange(MousePos.Y, size.Y * ScrollDownLimitation, size.Y);
			value = UKismetMathLibrary::FClamp(value, 0.f, 1.f);
			if (value > 0.f) {
				OffsetByVector(FVector(-1.f, 0.f, 0.f), ScrollScalar);
			}
		}
	}
}

void ATerrainCamera::Move(FVector2D MouseMov)
{
	OffsetByVector(FVector(MouseMov.Y, MouseMov.X, 0.0), MoveScalar);
}

void ATerrainCamera::KeyMove(FVector2D KeyMov)
{
	OffsetByVector(FVector(KeyMov.Y, KeyMov.X, 0.0), KeyMoveScalar);
}

void ATerrainCamera::OffsetByVector(FVector offset, float scalar)
{
	FVector RotateVec = (CameraSpringArm->TargetArmLength / 100.0 * scalar) * offset;
	FRotator Rotator = FRotator(0.0, Camera->GetComponentRotation().Yaw, 0.0);
	FVector DeltaLoc = Rotator.RotateVector(RotateVec);
	AddActorLocalOffset(DeltaLoc);
	SetActorLocation(UKismetMathLibrary::Vector_BoundedToBox(GetActorLocation(), BoundaryMin, BoundaryMax));
}

void ATerrainCamera::ChangeSpringArm(float DeltaTime)
{
	if (TargetArmLength > 0 && CameraSpringArm->TargetArmLength != TargetArmLength)
	{
		CameraSpringArm->TargetArmLength = FMath::FInterpTo(CameraSpringArm->TargetArmLength,
			TargetArmLength,
			DeltaTime,
			CameraZoomSpeed);
	}
}

// Called when the game starts or when spawned
void ATerrainCamera::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) {
		UE_LOG(TerrainCamera, Warning, TEXT("AutoReceiveInput disabled."));
		return;
	}
	PlayerController->SetViewTargetWithBlend(this);

	if (!InitCamera()) {
		UE_LOG(TerrainCamera, Warning, TEXT("BeginPlay error!"));
		return;
	}

	FTimerHandle TimerHandleTI;
	GetWorldTimerManager().SetTimer(TimerHandleTI, TerrainInputDelegate, TimingForWaitTerrain, false);
	FTimerHandle TimerHandleTG;
	GetWorldTimerManager().SetTimer(TimerHandleTG, TerrainInfoDelegate, TimingForWaitTerrain, false);
	FTimerHandle TimerHandleSS;
	GetWorldTimerManager().SetTimer(TimerHandleSS, ScrollScreenDelegate, TimingForScrollScreen, true);
	
}

// Called every frame
void ATerrainCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ChangeSpringArm(DeltaTime);
}

// Called to bind functionality to input
void ATerrainCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (CameraMoveAction != nullptr && CameraKeyMoveAction != nullptr && 
			CameraRotateAction != nullptr && CameraKeyRotateAction != nullptr && 
			CameraZoomInAction != nullptr && CameraZoomOutAction != nullptr) {
			UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent);
			EnhancedInputComp->BindAction(CameraMoveAction, ETriggerEvent::Triggered, this,
				&ATerrainCamera::OnCameraMove);
			EnhancedInputComp->BindAction(CameraKeyMoveAction, ETriggerEvent::Triggered, this,
				&ATerrainCamera::OnCameraKeyMove);
			EnhancedInputComp->BindAction(CameraRotateAction, ETriggerEvent::Triggered, this,
				&ATerrainCamera::OnCameraRotate);
			EnhancedInputComp->BindAction(CameraKeyRotateAction, ETriggerEvent::Triggered, this,
				&ATerrainCamera::OnCameraKeyRotate);
			EnhancedInputComp->BindAction(CameraZoomInAction, ETriggerEvent::Triggered, this,
				&ATerrainCamera::OnCameraZoomIn);
			EnhancedInputComp->BindAction(CameraZoomOutAction, ETriggerEvent::Triggered, this,
				&ATerrainCamera::OnCameraZoomOut);
		}
		else {
			UE_LOG(TerrainCamera, Warning, TEXT("Pls setting Input Action first!"));
		}
	}
}

