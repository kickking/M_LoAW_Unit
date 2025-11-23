// Fill out your copyright notice in the Description page of Project Settings.


#include "LoAWPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

DEFINE_LOG_CATEGORY(LoAWPlayerController);

ALoAWPlayerController::ALoAWPlayerController()
{
	bShowMouseCursor = true;
}

void ALoAWPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (CameraInputMappingContext && TerrainInputMappingContext && GameGridInputMappingContext) {
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		if (Subsystem) {
			Subsystem->AddMappingContext(CameraInputMappingContext, 0);
			Subsystem->AddMappingContext(TerrainInputMappingContext, 0);
			Subsystem->AddMappingContext(GameGridInputMappingContext, 0);
		}
	}
	else {
		UE_LOG(LoAWPlayerController, Warning, TEXT("Please setting Input Mapping Context first!"));
	}
}
