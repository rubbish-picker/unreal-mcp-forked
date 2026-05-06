#include "Commands/UnrealMCPProjectCommands.h"
#include "Commands/UnrealMCPCommonUtils.h"
#include "GameFramework/InputSettings.h"

FUnrealMCPProjectCommands::FUnrealMCPProjectCommands()
{
}

TSharedPtr<FJsonObject> FUnrealMCPProjectCommands::HandleCommand(const FString& CommandType, const TSharedPtr<FJsonObject>& Params)
{
    if (CommandType == TEXT("create_input_mapping"))
    {
        return HandleCreateInputMapping(Params);
    }
    else if (CommandType == TEXT("inspect_input_mappings"))
    {
        return HandleInspectInputMappings(Params);
    }
    
    return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Unknown project command: %s"), *CommandType));
}

TSharedPtr<FJsonObject> FUnrealMCPProjectCommands::HandleCreateInputMapping(const TSharedPtr<FJsonObject>& Params)
{
    // Get required parameters
    FString ActionName;
    if (!Params->TryGetStringField(TEXT("action_name"), ActionName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'action_name' parameter"));
    }

    FString Key;
    if (!Params->TryGetStringField(TEXT("key"), Key))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'key' parameter"));
    }

    // Get the input settings
    UInputSettings* InputSettings = GetMutableDefault<UInputSettings>();
    if (!InputSettings)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to get input settings"));
    }

    FString InputType = TEXT("Action");
    Params->TryGetStringField(TEXT("input_type"), InputType);

    // Add modifiers if provided
    if (Params->HasField(TEXT("shift")))
    {
        // Modifiers are only valid for action mappings.
    }

    if (InputType.Equals(TEXT("Axis"), ESearchCase::IgnoreCase))
    {
        float Scale = 1.0f;
        double ScaleNumber = 1.0;
        if (Params->TryGetNumberField(TEXT("scale"), ScaleNumber))
        {
            Scale = (float)ScaleNumber;
        }

        FInputAxisKeyMapping AxisMapping;
        AxisMapping.AxisName = FName(*ActionName);
        AxisMapping.Key = FKey(*Key);
        AxisMapping.Scale = Scale;
        InputSettings->AddAxisMapping(AxisMapping);
        InputSettings->SaveConfig();

        TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
        ResultObj->SetStringField(TEXT("axis_name"), ActionName);
        ResultObj->SetStringField(TEXT("key"), Key);
        ResultObj->SetNumberField(TEXT("scale"), Scale);
        return ResultObj;
    }

    FInputActionKeyMapping ActionMapping;
    ActionMapping.ActionName = FName(*ActionName);
    ActionMapping.Key = FKey(*Key);

    if (Params->HasField(TEXT("shift")))
    {
        ActionMapping.bShift = Params->GetBoolField(TEXT("shift"));
    }
    if (Params->HasField(TEXT("ctrl")))
    {
        ActionMapping.bCtrl = Params->GetBoolField(TEXT("ctrl"));
    }
    if (Params->HasField(TEXT("alt")))
    {
        ActionMapping.bAlt = Params->GetBoolField(TEXT("alt"));
    }
    if (Params->HasField(TEXT("cmd")))
    {
        ActionMapping.bCmd = Params->GetBoolField(TEXT("cmd"));
    }

    // Add the mapping
    InputSettings->AddActionMapping(ActionMapping);
    InputSettings->SaveConfig();

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("action_name"), ActionName);
    ResultObj->SetStringField(TEXT("key"), Key);
    return ResultObj;
} 

TSharedPtr<FJsonObject> FUnrealMCPProjectCommands::HandleInspectInputMappings(const TSharedPtr<FJsonObject>& Params)
{
    UInputSettings* InputSettings = GetMutableDefault<UInputSettings>();
    if (!InputSettings)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to get input settings"));
    }

    TArray<TSharedPtr<FJsonValue>> ActionArray;
    for (const FInputActionKeyMapping& Mapping : InputSettings->GetActionMappings())
    {
        TSharedPtr<FJsonObject> MappingObj = MakeShared<FJsonObject>();
        MappingObj->SetStringField(TEXT("action_name"), Mapping.ActionName.ToString());
        MappingObj->SetStringField(TEXT("key"), Mapping.Key.ToString());
        MappingObj->SetBoolField(TEXT("shift"), Mapping.bShift);
        MappingObj->SetBoolField(TEXT("ctrl"), Mapping.bCtrl);
        MappingObj->SetBoolField(TEXT("alt"), Mapping.bAlt);
        MappingObj->SetBoolField(TEXT("cmd"), Mapping.bCmd);
        ActionArray.Add(MakeShared<FJsonValueObject>(MappingObj));
    }

    TArray<TSharedPtr<FJsonValue>> AxisArray;
    for (const FInputAxisKeyMapping& Mapping : InputSettings->GetAxisMappings())
    {
        TSharedPtr<FJsonObject> MappingObj = MakeShared<FJsonObject>();
        MappingObj->SetStringField(TEXT("axis_name"), Mapping.AxisName.ToString());
        MappingObj->SetStringField(TEXT("key"), Mapping.Key.ToString());
        MappingObj->SetNumberField(TEXT("scale"), Mapping.Scale);
        AxisArray.Add(MakeShared<FJsonValueObject>(MappingObj));
    }

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetArrayField(TEXT("actions"), ActionArray);
    ResultObj->SetArrayField(TEXT("axes"), AxisArray);
    ResultObj->SetNumberField(TEXT("action_count"), ActionArray.Num());
    ResultObj->SetNumberField(TEXT("axis_count"), AxisArray.Num());
    return ResultObj;
}
