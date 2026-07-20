// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UGE_ContentExamplesEditorTarget : TargetRules
{
	public UGE_ContentExamplesEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
		WindowsPlatform.Compiler = WindowsCompiler.VisualStudio2022;
		WindowsPlatform.CompilerVersion = "14.44.35228";
		ExtraModuleNames.Add("UGE_ContentExamples");
		ExtraModuleNames.Add("UGE_ContentExamplesEditor");
	}
}
