// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"

#include "Private/SteamVRPrivate.h"

// UNREAL
#include "PhysXIncludes.h"
#include "PhysicsPublic.h"
#include "PhysXPublic.h" 
#include "Runtime/Engine/Private/PhysicsEngine/PhysXSupport.h"
#include "UnrealNetwork.h"
#include "MessageEndpointBuilder.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

// STL
#include <algorithm>
#include <memory>

// VREXPANSION
#include "VRExpansion/VRPlayerController.h"
#include "VRExpansion/VRRootComponent.h" 
#include "VRExpansion/SimpleChar/VRSimpleCharacter.h"
#include "VRExpansion/SimpleChar/VRSimpleCharacterMovementComponent.h"
#include "VRExpansion/VRStereoWidgetComponent.h"
#include "VRExpansion/VRTrackedParentInterface.h"
#include "VRExpansion/GripMotionControllerComponent.h"
#include "VRExpansion/Grippables/GrippableBoxComponent.h"
#include "VRExpansion/Grippables/GrippableCapsuleComponent.h"
#include "VRExpansion/Grippables/GrippableSkeletalMeshActor.h"
#include "VRExpansion/Grippables/GrippableSkeletalMeshComponent.h"
#include "VRExpansion/Grippables/GrippableSphereComponent.h"
#include "VRExpansion/Grippables/GrippableStaticMeshActor.h"
#include "VRExpansion/Grippables/GrippableStaticMeshComponent.h"
#include "VRExpansion/ParentRelativeAttachmentComponent.h"
#include "VRExpansion/ReplicatedVRCameraComponent.h"
#include "VRExpansion/VRAIController.h"
#include "VRExpansion/VRBaseCharacter.h"
#include "VRExpansion/VRBaseCharacterMovementComponent.h"
#include "VRExpansion/VRBPDatatypes.h"
#include "VRExpansion/Interactibles/VRButtonComponent.h"
#include "VRExpansion/VRCharacter.h"
#include "VRExpansion/VRCharacterMovementComponent.h"
#include "VRExpansion/VRExpansionFunctionLibrary.h"
#include "VRExpansion/VRGripInterface.h"
#include "VRExpansion/Misc/VRLogComponent.h"
#include "VRExpansion/VRPathFollowingComponent.h"

//VRExpansion/OpenVRExpansion
#include "VRExpansion/OpenVRExpansionPlugin/OpenVRExpansionFunctionLibrary.h"
#include "VRExpansion/OpenVRExpansionPlugin/SteamVRKeyboardComponent.h"

// LOG
DECLARE_LOG_CATEGORY_EXTERN(DebugLog, Log, All);

bool g_IsVREnabled();
