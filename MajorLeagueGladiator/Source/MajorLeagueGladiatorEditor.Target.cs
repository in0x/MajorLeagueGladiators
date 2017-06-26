// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class MajorLeagueGladiatorEditorTarget : TargetRules
{
	public MajorLeagueGladiatorEditorTarget(TargetInfo Target)
        : base(Target)
	{
		Type = TargetType.Editor;
        ExtraModuleNames.Add("MajorLeagueGladiator");
    }

    //
    // TargetRules interface.
    //

 //   public override void SetupBinaries(
	//	TargetInfo Target,
	//	ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
	//	ref List<string> OutExtraModuleNames
	//	)
	//{
	//	OutExtraModuleNames.AddRange( new string[] { "MajorLeagueGladiator" } );
	//}
}
