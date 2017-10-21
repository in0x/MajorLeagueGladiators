// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class MajorLeagueGladiator : ModuleRules
{
    public MajorLeagueGladiator(ReadOnlyTargetRules Target)
        : base(Target)
	{
        MinFilesUsingPrecompiledHeaderOverride = 1;
        bFasterWithoutUnity = true;

        Definitions.Add("WITH_OPEN_VR_EXPANSION=1");

        if (UEBuildConfiguration.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }

        PublicIncludePaths.AddRange(
			new string[] {
                "HeadMountedDisplay/Public",
                "Runtime/Engine/Classes/Kismet",
                "Runtime/Engine/Private/PhysicsEngine",
                "Runtime/GameplayAbilities",
                "Runtime/GameplayTasks"
            }
		);


        DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");

        PublicDependencyModuleNames.AddRange(
            new string[] 
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "PhysX",
                "HeadMountedDisplay",
                "RHI",
                "RenderCore",
                "ShaderCore",
                "NetworkReplayStreaming",
                "AIModule",
                "ProceduralMeshComponent",
                "GameplayAbilities",
                "GameplayTasks",
                "GameplayTags",
                "OnlineSubsystem",
                "OnlineSubsystemUtils"
            });

        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore"

				// ... add private dependencies that you statically link with here ...	
			}
			);

		AddThirdPartyPrivateStaticDependencies(Target, "Steamworks");
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		PrivateDependencyModuleNames.Add("Steamworks");

		// Don't load APEX on incompatible platforms
		if (
            Target.Platform != UnrealTargetPlatform.IOS &&
            Target.Platform != UnrealTargetPlatform.TVOS &&
            Target.Platform != UnrealTargetPlatform.Android &&
            Target.Platform != UnrealTargetPlatform.HTML5)
        {
            PublicDependencyModuleNames.AddRange(
            new string[]
            {                   
             "APEX"
            });
        }


        // Locking steamVR out of non windows builds
        if (Target.Platform == UnrealTargetPlatform.Win32 || Target.Platform == UnrealTargetPlatform.Win64)
        {

            PublicIncludePaths.AddRange(
            new string[] {
                        "../Plugins/Runtime/Steam/SteamVR/Source/SteamVR/Private" // This is dumb but it isn't very open

                // ... add public include paths required here ...
            }
            );


            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                "SteamVR",
                "OpenVR",
                "SteamVRController"
                });

            // AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenVR");
            //  PrivateDependencyModuleNames.AddRange(new string[] { "D3D11RHI" });     //@todo steamvr: multiplatform
        }
	}
}
