// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class M_LoAW_UnitEditorTarget : TargetRules
{
	public M_LoAW_UnitEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		ExtraModuleNames.Add("M_LoAW_Unit");
        ExtraModuleNames.Add("M_LoAW_GameGrid");
        ExtraModuleNames.Add("M_LoAW_GridData");
        ExtraModuleNames.Add("M_LoAW_Terrain");
    }
}
