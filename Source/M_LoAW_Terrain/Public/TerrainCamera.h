// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TerrainCamera.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(TerrainCamera, Log, All);

UCLASS(MinimalAPI)
class ATerrainCamera : public APawn
{
	GENERATED_BODY()

private:
	FTimerDynamicDelegate TerrainInputDelegate;
	FTimerDynamicDelegate TerrainInfoDelegate;
	FTimerDynamicDelegate ScrollScreenDelegate;

	class ATerrainInput* pTI = nullptr;
	class ATerrainGenerator* pTG = nullptr;

	FVector BoundaryMin;
	FVector BoundaryMax;

	float AltitudeMultiplier = 0.f;

	float TargetArmLength = -1.f;

protected:
	//Component
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class USpringArmComponent* CameraSpringArm;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Input")
	class UInputAction* CameraMoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Input")
	class UInputAction* CameraKeyMoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Input")
	class UInputAction* CameraRotateAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Input")
	class UInputAction* CameraKeyRotateAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Input")
	class UInputAction* CameraZoomInAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Input")
	class UInputAction* CameraZoomOutAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Timer")
	float TimingForWaitTerrain = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Timer")
	float TimingForScrollScreen = 0.03;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|TerrainInfo")
	float BoundaryScalar = 0.55;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|ScrollScreen")
	float ScrollScalar = 1.5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|ScrollScreen")
	float ScrollSpeedRatio = 0.7;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|ScrollScreen")
	float ScrollLeftLimitation = 0.005;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|ScrollScreen")
	float ScrollRightLimitation = 0.995;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|ScrollScreen")
	float ScrollUpLimitation = 0.005;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|ScrollScreen")
	float ScrollDownLimitation = 0.99;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Camera")
	float MoveScalar = 2.5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Camera")
	float KeyMoveScalar = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Camera")
	float RotateSpeed = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Camera")
	float KeyRotateSpeed = 0.2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Camera")
	float ZoomStep = 5000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Camera")
	float ZoomMin = 20000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Camera")
	float ZoomMinOffset = 2000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Camera")
	float ZoomMax = 50000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Camera")
	float InitArmLength = 30000.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Camera")
	float CameraLagSpeed = 10.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Camera")
	float CameraZoomSpeed = 10.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Camera")
	float ViewPitchMax = -60.0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Camera")
	float ViewPitchMin = -80.0;

public:
	// Sets default values for this pawn's properties
	ATerrainCamera();

private:
	void BindDelegate();

	bool InitCamera();

	UFUNCTION()
	void OnCameraMove(const FInputActionValue& Value);
	UFUNCTION()
	void OnCameraKeyMove(const FInputActionValue& Value);
	UFUNCTION()
	void OnCameraRotate(const FInputActionValue& Value);
	UFUNCTION()
	void OnCameraKeyRotate(const FInputActionValue& Value);
	UFUNCTION()
	void OnCameraZoomIn(const FInputActionValue& Value);
	UFUNCTION()
	void OnCameraZoomOut(const FInputActionValue& Value);

	UFUNCTION()
	void OnTerrainInput();
	UFUNCTION()
	void OnTerrainInfo();
	void FindActors(TSubclassOf<AActor> ActorClass, TArray<AActor*>& Actors);
	void CalZoomMin();

	UFUNCTION()
	void OnScrollScreen();

	void Scroll();
	void ScrollLeft();
	void ScrollRight();
	void ScrollUp();
	void ScrollDown();

	void Move(FVector2D MouseMov);
	void KeyMove(FVector2D KeyMov);
	void OffsetByVector(FVector offset, float scalar);

	void ChangeSpringArm(float DeltaTime);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
