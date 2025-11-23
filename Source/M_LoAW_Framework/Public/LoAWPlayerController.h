// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LoAWPlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LoAWPlayerController, Log, All);

/**
 * 
 */
UCLASS()
class M_LOAW_FRAMEWORK_API ALoAWPlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Input", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* CameraInputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Input", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* TerrainInputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Input", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* GameGridInputMappingContext;

public:
	ALoAWPlayerController();

protected:
	virtual void SetupInputComponent() override;
};
