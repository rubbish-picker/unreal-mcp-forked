#include "Commands/UnrealMCPEditorCommands.h"
#include "Commands/UnrealMCPCommonUtils.h"
#include "Editor.h"
#include "EditorViewportClient.h"
#include "LevelEditorViewport.h"
#include "ImageUtils.h"
#include "HighResScreenshot.h"
#include "Engine/GameViewportClient.h"
#include "Misc/FileHelper.h"
#include "Misc/App.h"
#include "GameFramework/Actor.h"
#include "Engine/Selection.h"
#include "Kismet/GameplayStatics.h"
#include "FileHelpers.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Engine/StaticMeshActor.h"
#include "Animation/SkeletalMeshActor.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "Camera/CameraActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/MeshComponent.h"
#include "EditorSubsystem.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "Components/PrimitiveComponent.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "EditorAssetLibrary.h"
#include "AssetImportTask.h"
#include "Factories/Factory.h"
#include "Factories/FbxFactory.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Factories/TextureFactory.h"
#include "UObject/ObjectRedirector.h"
#include "UObject/UObjectIterator.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Texture2D.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpression.h"
#include "Materials/MaterialExpressionAdd.h"
#include "Materials/MaterialExpressionAppendVector.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionConstant2Vector.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionConstant4Vector.h"
#include "Materials/MaterialExpressionLinearInterpolate.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionOneMinus.h"
#include "Materials/MaterialExpressionParameter.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionStaticSwitchParameter.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionTextureSampleParameter.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialParameters.h"
#include "MaterialEditingLibrary.h"
#include "SceneTypes.h"
#include "UObject/Package.h"

FUnrealMCPEditorCommands::FUnrealMCPEditorCommands()
{
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleCommand(const FString& CommandType, const TSharedPtr<FJsonObject>& Params)
{
    // Actor manipulation commands
    if (CommandType == TEXT("get_actors_in_level"))
    {
        return HandleGetActorsInLevel(Params);
    }
    else if (CommandType == TEXT("find_actors_by_name"))
    {
        return HandleFindActorsByName(Params);
    }
    else if (CommandType == TEXT("spawn_actor") || CommandType == TEXT("create_actor"))
    {
        if (CommandType == TEXT("create_actor"))
        {
            UE_LOG(LogTemp, Warning, TEXT("'create_actor' command is deprecated and will be removed in a future version. Please use 'spawn_actor' instead."));
        }
        return HandleSpawnActor(Params);
    }
    else if (CommandType == TEXT("delete_actor"))
    {
        return HandleDeleteActor(Params);
    }
    else if (CommandType == TEXT("set_actor_transform"))
    {
        return HandleSetActorTransform(Params);
    }
    else if (CommandType == TEXT("get_actor_properties"))
    {
        return HandleGetActorProperties(Params);
    }
    else if (CommandType == TEXT("set_actor_property"))
    {
        return HandleSetActorProperty(Params);
    }
    else if (CommandType == TEXT("inspect_object_properties"))
    {
        return HandleInspectObjectProperties(Params);
    }
    else if (CommandType == TEXT("inspect_selected_objects"))
    {
        return HandleInspectSelectedObjects(Params);
    }
    else if (CommandType == TEXT("inspect_component_collision"))
    {
        return HandleInspectComponentCollision(Params);
    }
    else if (CommandType == TEXT("find_asset_references"))
    {
        return HandleFindAssetReferences(Params);
    }
    else if (CommandType == TEXT("list_assets_by_class"))
    {
        return HandleListAssetsByClass(Params);
    }
    else if (CommandType == TEXT("create_content_folder"))
    {
        return HandleCreateContentFolder(Params);
    }
    else if (CommandType == TEXT("duplicate_asset"))
    {
        return HandleDuplicateAsset(Params);
    }
    else if (CommandType == TEXT("rename_asset"))
    {
        return HandleRenameAsset(Params);
    }
    else if (CommandType == TEXT("delete_asset"))
    {
        return HandleDeleteAsset(Params);
    }
    else if (CommandType == TEXT("save_asset"))
    {
        return HandleSaveAsset(Params);
    }
    else if (CommandType == TEXT("fixup_redirectors"))
    {
        return HandleFixupRedirectors(Params);
    }
    else if (CommandType == TEXT("import_asset"))
    {
        return HandleImportAsset(Params);
    }
    else if (CommandType == TEXT("import_assets_batch"))
    {
        return HandleImportAssetsBatch(Params);
    }
    else if (CommandType == TEXT("inspect_import_options"))
    {
        return HandleInspectImportOptions(Params);
    }
    else if (CommandType == TEXT("spawn_mesh_actor_from_asset"))
    {
        return HandleSpawnMeshActorFromAsset(Params);
    }
    else if (CommandType == TEXT("set_actor_mesh_asset"))
    {
        return HandleSetActorMeshAsset(Params);
    }
    else if (CommandType == TEXT("set_actor_component_property"))
    {
        return HandleSetActorComponentProperty(Params);
    }
    else if (CommandType == TEXT("set_actor_component_material"))
    {
        return HandleSetActorComponentMaterial(Params);
    }
    else if (CommandType == TEXT("create_material"))
    {
        return HandleCreateMaterial(Params);
    }
    else if (CommandType == TEXT("create_material_instance"))
    {
        return HandleCreateMaterialInstance(Params);
    }
    else if (CommandType == TEXT("inspect_material_parameters"))
    {
        return HandleInspectMaterialParameters(Params);
    }
    else if (CommandType == TEXT("set_material_parameter"))
    {
        return HandleSetMaterialParameter(Params);
    }
    else if (CommandType == TEXT("set_material_texture_parameter"))
    {
        return HandleSetMaterialTextureParameter(Params);
    }
    else if (CommandType == TEXT("set_material_static_switch_parameter"))
    {
        return HandleSetMaterialStaticSwitchParameter(Params);
    }
    else if (CommandType == TEXT("inspect_material_expressions"))
    {
        return HandleInspectMaterialExpressions(Params);
    }
    else if (CommandType == TEXT("add_material_expression_node"))
    {
        return HandleAddMaterialExpressionNode(Params);
    }
    else if (CommandType == TEXT("connect_material_expression_to_property"))
    {
        return HandleConnectMaterialExpressionToProperty(Params);
    }
    else if (CommandType == TEXT("connect_material_expressions"))
    {
        return HandleConnectMaterialExpressions(Params);
    }
    else if (CommandType == TEXT("recompile_material"))
    {
        return HandleRecompileMaterial(Params);
    }
    // Blueprint actor spawning
    else if (CommandType == TEXT("spawn_blueprint_actor"))
    {
        return HandleSpawnBlueprintActor(Params);
    }
    // Editor viewport commands
    else if (CommandType == TEXT("focus_viewport"))
    {
        return HandleFocusViewport(Params);
    }
    else if (CommandType == TEXT("take_screenshot"))
    {
        return HandleTakeScreenshot(Params);
    }
    else if (CommandType == TEXT("open_level"))
    {
        return HandleOpenLevel(Params);
    }
    else if (CommandType == TEXT("save_current_level"))
    {
        return HandleSaveCurrentLevel(Params);
    }
    
    return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Unknown editor command: %s"), *CommandType));
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleGetActorsInLevel(const TSharedPtr<FJsonObject>& Params)
{
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GWorld, AActor::StaticClass(), AllActors);
    
    TArray<TSharedPtr<FJsonValue>> ActorArray;
    for (AActor* Actor : AllActors)
    {
        if (Actor)
        {
            ActorArray.Add(FUnrealMCPCommonUtils::ActorToJson(Actor));
        }
    }
    
    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetArrayField(TEXT("actors"), ActorArray);
    
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleFindActorsByName(const TSharedPtr<FJsonObject>& Params)
{
    FString Pattern;
    if (!Params->TryGetStringField(TEXT("pattern"), Pattern))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'pattern' parameter"));
    }
    
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GWorld, AActor::StaticClass(), AllActors);
    
    TArray<TSharedPtr<FJsonValue>> MatchingActors;
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName().Contains(Pattern))
        {
            MatchingActors.Add(FUnrealMCPCommonUtils::ActorToJson(Actor));
        }
    }
    
    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetArrayField(TEXT("actors"), MatchingActors);
    
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleSpawnActor(const TSharedPtr<FJsonObject>& Params)
{
    // Get required parameters
    FString ActorType;
    if (!Params->TryGetStringField(TEXT("type"), ActorType))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'type' parameter"));
    }

    // Get actor name (required parameter)
    FString ActorName;
    if (!Params->TryGetStringField(TEXT("name"), ActorName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'name' parameter"));
    }

    // Get optional transform parameters
    FVector Location(0.0f, 0.0f, 0.0f);
    FRotator Rotation(0.0f, 0.0f, 0.0f);
    FVector Scale(1.0f, 1.0f, 1.0f);

    if (Params->HasField(TEXT("location")))
    {
        Location = FUnrealMCPCommonUtils::GetVectorFromJson(Params, TEXT("location"));
    }
    if (Params->HasField(TEXT("rotation")))
    {
        Rotation = FUnrealMCPCommonUtils::GetRotatorFromJson(Params, TEXT("rotation"));
    }
    if (Params->HasField(TEXT("scale")))
    {
        Scale = FUnrealMCPCommonUtils::GetVectorFromJson(Params, TEXT("scale"));
    }

    // Create the actor based on type
    AActor* NewActor = nullptr;
    UWorld* World = GEditor->GetEditorWorldContext().World();

    if (!World)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to get editor world"));
    }

    // Check if an actor with this name already exists
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), AllActors);
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName() == ActorName)
        {
            return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Actor with name '%s' already exists"), *ActorName));
        }
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = *ActorName;

    if (ActorType == TEXT("StaticMeshActor"))
    {
        NewActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation, SpawnParams);
    }
    else if (ActorType == TEXT("PointLight"))
    {
        NewActor = World->SpawnActor<APointLight>(APointLight::StaticClass(), Location, Rotation, SpawnParams);
    }
    else if (ActorType == TEXT("SpotLight"))
    {
        NewActor = World->SpawnActor<ASpotLight>(ASpotLight::StaticClass(), Location, Rotation, SpawnParams);
    }
    else if (ActorType == TEXT("DirectionalLight"))
    {
        NewActor = World->SpawnActor<ADirectionalLight>(ADirectionalLight::StaticClass(), Location, Rotation, SpawnParams);
    }
    else if (ActorType == TEXT("CameraActor"))
    {
        NewActor = World->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), Location, Rotation, SpawnParams);
    }
    else
    {
        TArray<FString> TypeCandidates;
        TypeCandidates.Add(ActorType);
        if (ActorType.StartsWith(TEXT("A")) && ActorType.Len() > 1)
        {
            TypeCandidates.Add(ActorType.RightChop(1));
        }
        else
        {
            TypeCandidates.Add(TEXT("A") + ActorType);
        }

        TArray<FString> ClassPaths;
        const FString ProjectModuleName = FApp::GetProjectName();
        for (const FString& Candidate : TypeCandidates)
        {
            if (Candidate.StartsWith(TEXT("/Script/")))
            {
                ClassPaths.Add(Candidate);
            }
            else
            {
                ClassPaths.Add(FString::Printf(TEXT("/Script/Engine.%s"), *Candidate));
                ClassPaths.Add(FString::Printf(TEXT("/Script/%s.%s"), *ProjectModuleName, *Candidate));
                ClassPaths.Add(FString::Printf(TEXT("/Script/gameproject.%s"), *Candidate));
            }
        }

        UClass* ActorClass = nullptr;
        for (const FString& ClassPath : ClassPaths)
        {
            ActorClass = LoadClass<AActor>(nullptr, *ClassPath);
            if (ActorClass && ActorClass->IsChildOf(AActor::StaticClass()))
            {
                break;
            }
            ActorClass = nullptr;
        }

        if (!ActorClass)
        {
            const auto NormalizeClassName = [](const FString& InName)
            {
                FString Result = InName;
                Result.ReplaceInline(TEXT("REINST_"), TEXT(""));
                Result.ReplaceInline(TEXT("HOTRELOADED_"), TEXT(""));
                Result.ReplaceInline(TEXT("SKEL_"), TEXT(""));
                if (Result.EndsWith(TEXT("_C")))
                {
                    Result.LeftChopInline(2);
                }
                return Result;
            };

            const FString NormalizedActorType = NormalizeClassName(ActorType);
            for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
            {
                UClass* CandidateClass = *ClassIt;
                if (!CandidateClass || !CandidateClass->IsChildOf(AActor::StaticClass()) || CandidateClass->HasAnyClassFlags(CLASS_Abstract))
                {
                    continue;
                }

                const FString CandidateName = NormalizeClassName(CandidateClass->GetName());
                const FString CandidateNameWithoutPrefix = CandidateName.StartsWith(TEXT("A"), ESearchCase::CaseSensitive) ? CandidateName.RightChop(1) : CandidateName;
                if (CandidateName.Equals(NormalizedActorType, ESearchCase::IgnoreCase) ||
                    CandidateNameWithoutPrefix.Equals(NormalizedActorType, ESearchCase::IgnoreCase))
                {
                    ActorClass = CandidateClass;
                    UE_LOG(LogTemp, Display, TEXT("Resolved actor type '%s' to loaded class '%s' via case-insensitive scan"), *ActorType, *CandidateClass->GetPathName());
                    break;
                }
            }
        }

        if (!ActorClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to resolve actor type '%s' after scanning loaded actor classes"), *ActorType);
            return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Unknown actor type: %s"), *ActorType));
        }

        NewActor = World->SpawnActor<AActor>(ActorClass, Location, Rotation, SpawnParams);
    }

    if (NewActor)
    {
        // Set scale (since SpawnActor only takes location and rotation)
        FTransform Transform = NewActor->GetTransform();
        Transform.SetScale3D(Scale);
        NewActor->SetActorTransform(Transform);

        // Return the created actor's details
        return FUnrealMCPCommonUtils::ActorToJsonObject(NewActor, true);
    }

    return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to create actor"));
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleDeleteActor(const TSharedPtr<FJsonObject>& Params)
{
    FString ActorName;
    if (!Params->TryGetStringField(TEXT("name"), ActorName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'name' parameter"));
    }

    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GWorld, AActor::StaticClass(), AllActors);
    
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName() == ActorName)
        {
            // Store actor info before deletion for the response
            TSharedPtr<FJsonObject> ActorInfo = FUnrealMCPCommonUtils::ActorToJsonObject(Actor);
            
            // Delete the actor
            Actor->Destroy();
            
            TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
            ResultObj->SetObjectField(TEXT("deleted_actor"), ActorInfo);
            return ResultObj;
        }
    }
    
    return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Actor not found: %s"), *ActorName));
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleSetActorTransform(const TSharedPtr<FJsonObject>& Params)
{
    // Get actor name
    FString ActorName;
    if (!Params->TryGetStringField(TEXT("name"), ActorName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'name' parameter"));
    }

    // Find the actor
    AActor* TargetActor = nullptr;
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GWorld, AActor::StaticClass(), AllActors);
    
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName() == ActorName)
        {
            TargetActor = Actor;
            break;
        }
    }

    if (!TargetActor)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Actor not found: %s"), *ActorName));
    }

    // Get transform parameters
    FTransform NewTransform = TargetActor->GetTransform();

    if (Params->HasField(TEXT("location")))
    {
        NewTransform.SetLocation(FUnrealMCPCommonUtils::GetVectorFromJson(Params, TEXT("location")));
    }
    if (Params->HasField(TEXT("rotation")))
    {
        NewTransform.SetRotation(FQuat(FUnrealMCPCommonUtils::GetRotatorFromJson(Params, TEXT("rotation"))));
    }
    if (Params->HasField(TEXT("scale")))
    {
        NewTransform.SetScale3D(FUnrealMCPCommonUtils::GetVectorFromJson(Params, TEXT("scale")));
    }

    // Set the new transform
    TargetActor->SetActorTransform(NewTransform);

    // Return updated actor info
    return FUnrealMCPCommonUtils::ActorToJsonObject(TargetActor, true);
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleGetActorProperties(const TSharedPtr<FJsonObject>& Params)
{
    // Get actor name
    FString ActorName;
    if (!Params->TryGetStringField(TEXT("name"), ActorName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'name' parameter"));
    }

    // Find the actor
    AActor* TargetActor = nullptr;
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GWorld, AActor::StaticClass(), AllActors);
    
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName() == ActorName)
        {
            TargetActor = Actor;
            break;
        }
    }

    if (!TargetActor)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Actor not found: %s"), *ActorName));
    }

    // Always return detailed properties for this command
    return FUnrealMCPCommonUtils::ActorToJsonObject(TargetActor, true);
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleSetActorProperty(const TSharedPtr<FJsonObject>& Params)
{
    // Get actor name
    FString ActorName;
    if (!Params->TryGetStringField(TEXT("name"), ActorName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'name' parameter"));
    }

    // Find the actor
    AActor* TargetActor = nullptr;
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GWorld, AActor::StaticClass(), AllActors);
    
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName() == ActorName)
        {
            TargetActor = Actor;
            break;
        }
    }

    if (!TargetActor)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Actor not found: %s"), *ActorName));
    }

    // Get property name
    FString PropertyName;
    if (!Params->TryGetStringField(TEXT("property_name"), PropertyName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'property_name' parameter"));
    }

    // Get property value
    if (!Params->HasField(TEXT("property_value")))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'property_value' parameter"));
    }
    
    TSharedPtr<FJsonValue> PropertyValue = Params->Values.FindRef(TEXT("property_value"));
    
    // Set the property using our utility function
    FString ErrorMessage;
    if (FUnrealMCPCommonUtils::SetObjectProperty(TargetActor, PropertyName, PropertyValue, ErrorMessage))
    {
        // Property set successfully
        TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
        ResultObj->SetStringField(TEXT("actor"), ActorName);
        ResultObj->SetStringField(TEXT("property"), PropertyName);
        ResultObj->SetBoolField(TEXT("success"), true);
        
        // Also include the full actor details
        ResultObj->SetObjectField(TEXT("actor_details"), FUnrealMCPCommonUtils::ActorToJsonObject(TargetActor, true));
        return ResultObj;
    }
    else
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(ErrorMessage);
    }
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleInspectObjectProperties(const TSharedPtr<FJsonObject>& Params)
{
    int32 MaxProperties = 200;
    Params->TryGetNumberField(TEXT("max_properties"), MaxProperties);

    UObject* TargetObject = nullptr;

    FString ObjectPath;
    if (Params->TryGetStringField(TEXT("object_path"), ObjectPath) && !ObjectPath.IsEmpty())
    {
        TargetObject = LoadObject<UObject>(nullptr, *ObjectPath);
        if (!TargetObject)
        {
            TargetObject = FindObject<UObject>(nullptr, *ObjectPath);
        }
    }

    FString ActorName;
    if (!TargetObject && Params->TryGetStringField(TEXT("actor_name"), ActorName))
    {
        TArray<AActor*> AllActors;
        UGameplayStatics::GetAllActorsOfClass(GWorld, AActor::StaticClass(), AllActors);
        for (AActor* Actor : AllActors)
        {
            if (Actor && Actor->GetName() == ActorName)
            {
                FString ComponentName;
                if (Params->TryGetStringField(TEXT("component_name"), ComponentName) && !ComponentName.IsEmpty())
                {
                    TArray<UActorComponent*> Components;
                    Actor->GetComponents(Components);
                    for (UActorComponent* Component : Components)
                    {
                        if (Component && Component->GetName() == ComponentName)
                        {
                            TargetObject = Component;
                            break;
                        }
                    }
                }
                else
                {
                    TargetObject = Actor;
                }
                break;
            }
        }
    }

    FString BlueprintName;
    if (!TargetObject && Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
    {
        UBlueprint* Blueprint = FUnrealMCPCommonUtils::FindBlueprint(BlueprintName);
        if (!Blueprint)
        {
            return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Blueprint not found: %s"), *BlueprintName));
        }

        FString ComponentName;
        if (Params->TryGetStringField(TEXT("component_name"), ComponentName) && !ComponentName.IsEmpty())
        {
            if (Blueprint->SimpleConstructionScript)
            {
                for (USCS_Node* Node : Blueprint->SimpleConstructionScript->GetAllNodes())
                {
                    if (Node && Node->GetVariableName() == FName(*ComponentName))
                    {
                        TargetObject = Node->ComponentTemplate;
                        break;
                    }
                }
            }
        }
        else
        {
            TargetObject = Blueprint->GeneratedClass ? Blueprint->GeneratedClass->GetDefaultObject() : Blueprint;
        }
    }

    if (!TargetObject)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Target object not found. Provide object_path, actor_name, or blueprint_name."));
    }

    TSharedPtr<FJsonObject> ResultObj = FUnrealMCPCommonUtils::ObjectToJsonObject(TargetObject, true, MaxProperties);
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleInspectSelectedObjects(const TSharedPtr<FJsonObject>& Params)
{
    int32 MaxProperties = 100;
    Params->TryGetNumberField(TEXT("max_properties"), MaxProperties);

    TArray<TSharedPtr<FJsonValue>> SelectedArray;

    if (GEditor)
    {
        USelection* SelectedActors = GEditor->GetSelectedActors();
        if (SelectedActors)
        {
            for (FSelectionIterator It(*SelectedActors); It; ++It)
            {
                if (UObject* SelectedObject = *It)
                {
                    SelectedArray.Add(MakeShared<FJsonValueObject>(FUnrealMCPCommonUtils::ObjectToJsonObject(SelectedObject, true, MaxProperties)));
                }
            }
        }

        USelection* SelectedObjects = GEditor->GetSelectedObjects();
        if (SelectedObjects)
        {
            for (FSelectionIterator It(*SelectedObjects); It; ++It)
            {
                if (UObject* SelectedObject = *It)
                {
                    SelectedArray.Add(MakeShared<FJsonValueObject>(FUnrealMCPCommonUtils::ObjectToJsonObject(SelectedObject, true, MaxProperties)));
                }
            }
        }
    }

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetArrayField(TEXT("selected_objects"), SelectedArray);
    ResultObj->SetNumberField(TEXT("count"), SelectedArray.Num());
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleInspectComponentCollision(const TSharedPtr<FJsonObject>& Params)
{
    FString ActorName;
    if (!Params->TryGetStringField(TEXT("actor_name"), ActorName))
    {
        Params->TryGetStringField(TEXT("name"), ActorName);
    }

    if (ActorName.IsEmpty())
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'actor_name' parameter"));
    }

    FString ComponentName;
    Params->TryGetStringField(TEXT("component_name"), ComponentName);

    AActor* TargetActor = nullptr;
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GWorld, AActor::StaticClass(), AllActors);
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName() == ActorName)
        {
            TargetActor = Actor;
            break;
        }
    }

    if (!TargetActor)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Actor not found: %s"), *ActorName));
    }

    TArray<TSharedPtr<FJsonValue>> ComponentArray;
    TArray<UActorComponent*> Components;
    TargetActor->GetComponents(Components);
    for (UActorComponent* Component : Components)
    {
        UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Component);
        if (!PrimitiveComponent)
        {
            continue;
        }
        if (!ComponentName.IsEmpty() && PrimitiveComponent->GetName() != ComponentName)
        {
            continue;
        }

        TSharedPtr<FJsonObject> ComponentObj = FUnrealMCPCommonUtils::ObjectToJsonObject(PrimitiveComponent, false);
        ComponentObj->SetObjectField(TEXT("collision"), FUnrealMCPCommonUtils::PrimitiveCollisionToJsonObject(PrimitiveComponent));
        ComponentArray.Add(MakeShared<FJsonValueObject>(ComponentObj));
    }

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("actor_name"), ActorName);
    ResultObj->SetArrayField(TEXT("components"), ComponentArray);
    ResultObj->SetNumberField(TEXT("count"), ComponentArray.Num());
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleFindAssetReferences(const TSharedPtr<FJsonObject>& Params)
{
    FString AssetPath;
    if (!Params->TryGetStringField(TEXT("asset_path"), AssetPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'asset_path' parameter"));
    }

    FString PackageName = AssetPath;
    if (AssetPath.Contains(TEXT(".")))
    {
        PackageName = FPackageName::ObjectPathToPackageName(AssetPath);
    }

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    TArray<FName> Referencers;
    TArray<FName> Dependencies;
    AssetRegistry.GetReferencers(FName(*PackageName), Referencers);
    AssetRegistry.GetDependencies(FName(*PackageName), Dependencies);

    TArray<TSharedPtr<FJsonValue>> ReferencerArray;
    for (const FName& Referencer : Referencers)
    {
        ReferencerArray.Add(MakeShared<FJsonValueString>(Referencer.ToString()));
    }

    TArray<TSharedPtr<FJsonValue>> DependencyArray;
    for (const FName& Dependency : Dependencies)
    {
        DependencyArray.Add(MakeShared<FJsonValueString>(Dependency.ToString()));
    }

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("asset_path"), AssetPath);
    ResultObj->SetStringField(TEXT("package_name"), PackageName);
    ResultObj->SetArrayField(TEXT("referencers"), ReferencerArray);
    ResultObj->SetArrayField(TEXT("dependencies"), DependencyArray);
    ResultObj->SetNumberField(TEXT("referencer_count"), ReferencerArray.Num());
    ResultObj->SetNumberField(TEXT("dependency_count"), DependencyArray.Num());
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleListAssetsByClass(const TSharedPtr<FJsonObject>& Params)
{
    FString ClassName;
    if (!Params->TryGetStringField(TEXT("class_name"), ClassName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'class_name' parameter"));
    }

    FString PackagePath = TEXT("/Game");
    Params->TryGetStringField(TEXT("path"), PackagePath);

    FString NameContains;
    Params->TryGetStringField(TEXT("name_contains"), NameContains);

    int32 MaxResults = 200;
    Params->TryGetNumberField(TEXT("max_results"), MaxResults);

    UClass* AssetClass = FUnrealMCPCommonUtils::FindClassByName(ClassName);
    if (!AssetClass)
    {
        if (ClassName.Equals(TEXT("Texture2D"), ESearchCase::IgnoreCase))
        {
            AssetClass = UTexture2D::StaticClass();
        }
        else if (ClassName.Equals(TEXT("StaticMesh"), ESearchCase::IgnoreCase))
        {
            AssetClass = UStaticMesh::StaticClass();
        }
        else if (ClassName.Equals(TEXT("SkeletalMesh"), ESearchCase::IgnoreCase))
        {
            AssetClass = USkeletalMesh::StaticClass();
        }
        else if (ClassName.Equals(TEXT("Material"), ESearchCase::IgnoreCase))
        {
            AssetClass = UMaterial::StaticClass();
        }
        else if (ClassName.Equals(TEXT("MaterialInstanceConstant"), ESearchCase::IgnoreCase))
        {
            AssetClass = UMaterialInstanceConstant::StaticClass();
        }
    }
    if (!AssetClass)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Asset class not found: %s"), *ClassName));
    }

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    FARFilter Filter;
    Filter.PackagePaths.Add(FName(*PackagePath));
    Filter.ClassPaths.Add(AssetClass->GetClassPathName());
    Filter.bRecursivePaths = true;
    Filter.bRecursiveClasses = true;

    TArray<FAssetData> Assets;
    AssetRegistry.GetAssets(Filter, Assets);

    TArray<TSharedPtr<FJsonValue>> AssetArray;
    for (const FAssetData& Asset : Assets)
    {
        if (!NameContains.IsEmpty() && !Asset.AssetName.ToString().Contains(NameContains))
        {
            continue;
        }

        TSharedPtr<FJsonObject> AssetObj = MakeShared<FJsonObject>();
        AssetObj->SetStringField(TEXT("name"), Asset.AssetName.ToString());
        AssetObj->SetStringField(TEXT("class"), Asset.AssetClassPath.ToString());
        AssetObj->SetStringField(TEXT("package_name"), Asset.PackageName.ToString());
        AssetObj->SetStringField(TEXT("object_path"), Asset.GetObjectPathString());
        AssetArray.Add(MakeShared<FJsonValueObject>(AssetObj));

        if (MaxResults > 0 && AssetArray.Num() >= MaxResults)
        {
            break;
        }
    }

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("class_name"), ClassName);
    ResultObj->SetStringField(TEXT("path"), PackagePath);
    ResultObj->SetArrayField(TEXT("assets"), AssetArray);
    ResultObj->SetNumberField(TEXT("count"), AssetArray.Num());
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleCreateContentFolder(const TSharedPtr<FJsonObject>& Params)
{
    FString FolderPath;
    if (!Params->TryGetStringField(TEXT("folder_path"), FolderPath) &&
        !Params->TryGetStringField(TEXT("path"), FolderPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'folder_path' parameter"));
    }

    if (!FolderPath.StartsWith(TEXT("/Game")))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("folder_path must be under /Game"));
    }

    const bool bExists = UEditorAssetLibrary::DoesDirectoryExist(FolderPath);
    const bool bCreated = bExists || UEditorAssetLibrary::MakeDirectory(FolderPath);

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("folder_path"), FolderPath);
    ResultObj->SetBoolField(TEXT("already_exists"), bExists);
    ResultObj->SetBoolField(TEXT("created"), bCreated);
    return bCreated ? ResultObj : FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Failed to create folder: %s"), *FolderPath));
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleDuplicateAsset(const TSharedPtr<FJsonObject>& Params)
{
    FString SourceAssetPath;
    if (!Params->TryGetStringField(TEXT("source_asset_path"), SourceAssetPath) &&
        !Params->TryGetStringField(TEXT("source"), SourceAssetPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'source_asset_path' parameter"));
    }

    FString DestinationAssetPath;
    if (!Params->TryGetStringField(TEXT("destination_asset_path"), DestinationAssetPath) &&
        !Params->TryGetStringField(TEXT("destination"), DestinationAssetPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'destination_asset_path' parameter"));
    }

    if (!UEditorAssetLibrary::DoesAssetExist(SourceAssetPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Source asset not found: %s"), *SourceAssetPath));
    }

    UObject* NewAsset = UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, DestinationAssetPath);
    if (!NewAsset)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to duplicate asset"));
    }

    TSharedPtr<FJsonObject> ResultObj = FUnrealMCPCommonUtils::ObjectToJsonObject(NewAsset, false);
    ResultObj->SetStringField(TEXT("source_asset_path"), SourceAssetPath);
    ResultObj->SetStringField(TEXT("destination_asset_path"), DestinationAssetPath);
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleRenameAsset(const TSharedPtr<FJsonObject>& Params)
{
    FString SourceAssetPath;
    if (!Params->TryGetStringField(TEXT("source_asset_path"), SourceAssetPath) &&
        !Params->TryGetStringField(TEXT("source"), SourceAssetPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'source_asset_path' parameter"));
    }

    FString DestinationAssetPath;
    if (!Params->TryGetStringField(TEXT("destination_asset_path"), DestinationAssetPath) &&
        !Params->TryGetStringField(TEXT("destination"), DestinationAssetPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'destination_asset_path' parameter"));
    }

    const bool bRenamed = UEditorAssetLibrary::RenameAsset(SourceAssetPath, DestinationAssetPath);
    if (!bRenamed)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to rename asset"));
    }

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("source_asset_path"), SourceAssetPath);
    ResultObj->SetStringField(TEXT("destination_asset_path"), DestinationAssetPath);
    ResultObj->SetBoolField(TEXT("renamed"), true);
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleDeleteAsset(const TSharedPtr<FJsonObject>& Params)
{
    FString AssetPath;
    if (!Params->TryGetStringField(TEXT("asset_path"), AssetPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'asset_path' parameter"));
    }

    const bool bDeleted = UEditorAssetLibrary::DeleteAsset(AssetPath);
    if (!bDeleted)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Failed to delete asset: %s"), *AssetPath));
    }

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("asset_path"), AssetPath);
    ResultObj->SetBoolField(TEXT("deleted"), true);
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleSaveAsset(const TSharedPtr<FJsonObject>& Params)
{
    FString AssetPath;
    if (!Params->TryGetStringField(TEXT("asset_path"), AssetPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'asset_path' parameter"));
    }

    bool bOnlyIfDirty = true;
    Params->TryGetBoolField(TEXT("only_if_dirty"), bOnlyIfDirty);

    const bool bSaved = UEditorAssetLibrary::SaveAsset(AssetPath, bOnlyIfDirty);
    if (!bSaved)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Failed to save asset: %s"), *AssetPath));
    }

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("asset_path"), AssetPath);
    ResultObj->SetBoolField(TEXT("saved"), true);
    ResultObj->SetBoolField(TEXT("only_if_dirty"), bOnlyIfDirty);
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleFixupRedirectors(const TSharedPtr<FJsonObject>& Params)
{
    FString FolderPath = TEXT("/Game");
    Params->TryGetStringField(TEXT("folder_path"), FolderPath);
    Params->TryGetStringField(TEXT("path"), FolderPath);

    if (!FolderPath.StartsWith(TEXT("/Game")))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("folder_path must be under /Game"));
    }

    bool bRecursive = true;
    Params->TryGetBoolField(TEXT("recursive"), bRecursive);

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    FARFilter Filter;
    Filter.PackagePaths.Add(*FolderPath);
    Filter.ClassPaths.Add(UObjectRedirector::StaticClass()->GetClassPathName());
    Filter.bRecursivePaths = bRecursive;

    TArray<FAssetData> RedirectorAssets;
    AssetRegistryModule.Get().GetAssets(Filter, RedirectorAssets);

    TArray<UObjectRedirector*> Redirectors;
    TArray<TSharedPtr<FJsonValue>> RedirectorArray;
    for (const FAssetData& AssetData : RedirectorAssets)
    {
        UObjectRedirector* Redirector = Cast<UObjectRedirector>(AssetData.GetAsset());
        if (!Redirector)
        {
            continue;
        }

        Redirectors.Add(Redirector);
        TSharedPtr<FJsonObject> RedirectorObj = MakeShared<FJsonObject>();
        RedirectorObj->SetStringField(TEXT("asset_name"), AssetData.AssetName.ToString());
        RedirectorObj->SetStringField(TEXT("package_name"), AssetData.PackageName.ToString());
        RedirectorObj->SetStringField(TEXT("object_path"), AssetData.GetObjectPathString());
        RedirectorArray.Add(MakeShared<FJsonValueObject>(RedirectorObj));
    }

    if (Redirectors.Num() > 0)
    {
        FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
        AssetToolsModule.Get().FixupReferencers(Redirectors, false);
    }

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("folder_path"), FolderPath);
    ResultObj->SetBoolField(TEXT("recursive"), bRecursive);
    ResultObj->SetNumberField(TEXT("redirector_count"), Redirectors.Num());
    ResultObj->SetArrayField(TEXT("redirectors"), RedirectorArray);
    ResultObj->SetBoolField(TEXT("fixed_up"), true);
    return ResultObj;
}

struct FUnrealMCPImportSpec
{
    FString SourceFile;
    FString DestinationPath = TEXT("/Game");
    FString DestinationName;
    bool bReplaceExisting = true;
    bool bSave = true;
};

static bool ReadImportSpecFromParams(const TSharedPtr<FJsonObject>& Params, FUnrealMCPImportSpec& OutSpec, FString& OutError)
{
    if (!Params->TryGetStringField(TEXT("source_file"), OutSpec.SourceFile) &&
        !Params->TryGetStringField(TEXT("filename"), OutSpec.SourceFile) &&
        !Params->TryGetStringField(TEXT("file_path"), OutSpec.SourceFile))
    {
        OutError = TEXT("Missing 'source_file' parameter");
        return false;
    }

    if (!FPaths::FileExists(OutSpec.SourceFile))
    {
        OutError = FString::Printf(TEXT("Source file not found: %s"), *OutSpec.SourceFile);
        return false;
    }

    Params->TryGetStringField(TEXT("destination_path"), OutSpec.DestinationPath);
    Params->TryGetStringField(TEXT("folder_path"), OutSpec.DestinationPath);
    if (!OutSpec.DestinationPath.StartsWith(TEXT("/Game")))
    {
        OutError = TEXT("destination_path must be under /Game");
        return false;
    }

    Params->TryGetStringField(TEXT("destination_name"), OutSpec.DestinationName);
    Params->TryGetStringField(TEXT("asset_name"), OutSpec.DestinationName);
    Params->TryGetBoolField(TEXT("replace_existing"), OutSpec.bReplaceExisting);
    Params->TryGetBoolField(TEXT("save"), OutSpec.bSave);
    return true;
}

static UFactory* CreateLegacyImportFactory(const FString& SourceFile, const TSharedPtr<FJsonObject>& Params)
{
    FString FactoryName;
    Params->TryGetStringField(TEXT("factory_name"), FactoryName);
    Params->TryGetStringField(TEXT("factory"), FactoryName);
    const FString Extension = FPaths::GetExtension(SourceFile).ToLower();

    if (FactoryName.Equals(TEXT("none"), ESearchCase::IgnoreCase) ||
        FactoryName.Equals(TEXT("auto"), ESearchCase::IgnoreCase))
    {
        return nullptr;
    }

    if (FactoryName.Equals(TEXT("TextureFactory"), ESearchCase::IgnoreCase) ||
        FactoryName.Equals(TEXT("Texture"), ESearchCase::IgnoreCase) ||
        (FactoryName.IsEmpty() && (Extension == TEXT("png") || Extension == TEXT("jpg") || Extension == TEXT("jpeg") || Extension == TEXT("tga") || Extension == TEXT("bmp") || Extension == TEXT("exr") || Extension == TEXT("hdr"))))
    {
        UTextureFactory* TextureFactory = NewObject<UTextureFactory>();
        TextureFactory->AddToRoot();
        return TextureFactory;
    }

    if (FactoryName.Equals(TEXT("FbxFactory"), ESearchCase::IgnoreCase) ||
        FactoryName.Equals(TEXT("FBX"), ESearchCase::IgnoreCase) ||
        (FactoryName.IsEmpty() && Extension == TEXT("fbx")))
    {
        UFbxFactory* FbxFactory = NewObject<UFbxFactory>();
        FbxFactory->AddToRoot();
        return FbxFactory;
    }

    return nullptr;
}

static TArray<UObject*> ImportWithAssetTask(
    const FUnrealMCPImportSpec& ImportSpec,
    UFactory* Factory)
{
    UAssetImportTask* ImportTask = NewObject<UAssetImportTask>();
    ImportTask->Filename = ImportSpec.SourceFile;
    ImportTask->DestinationPath = ImportSpec.DestinationPath;
    ImportTask->DestinationName = ImportSpec.DestinationName;
    ImportTask->bReplaceExisting = ImportSpec.bReplaceExisting;
    ImportTask->bReplaceExistingSettings = ImportSpec.bReplaceExisting;
    ImportTask->bAutomated = true;
    ImportTask->bSave = false;
    ImportTask->bAsync = false;
    ImportTask->Factory = Factory;

    TArray<UAssetImportTask*> Tasks;
    Tasks.Add(ImportTask);

    FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
    AssetToolsModule.Get().ImportAssetTasks(Tasks);

    TArray<UObject*> ImportedObjects;
    for (UObject* ImportedObject : ImportTask->GetObjects())
    {
        if (ImportedObject)
        {
            ImportedObjects.Add(ImportedObject);
        }
    }
    return ImportedObjects;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleImportAsset(const TSharedPtr<FJsonObject>& Params)
{
    FString Error;
    FUnrealMCPImportSpec ImportSpec;
    if (!ReadImportSpecFromParams(Params, ImportSpec, Error))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(Error);
    }

    UFactory* Factory = CreateLegacyImportFactory(ImportSpec.SourceFile, Params);
    TArray<UObject*> ImportedObjects = ImportWithAssetTask(ImportSpec, Factory);

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("source_file"), ImportSpec.SourceFile);
    ResultObj->SetStringField(TEXT("destination_path"), ImportSpec.DestinationPath);
    ResultObj->SetStringField(TEXT("destination_name"), ImportSpec.DestinationName);
    ResultObj->SetBoolField(TEXT("replace_existing"), ImportSpec.bReplaceExisting);
    ResultObj->SetBoolField(TEXT("saved"), false);

    TArray<TSharedPtr<FJsonValue>> ImportedObjectsJson;
    TArray<TSharedPtr<FJsonValue>> ImportedPaths;
    TArray<UPackage*> PackagesToSave;
    for (UObject* ImportedObject : ImportedObjects)
    {
        if (!ImportedObject)
        {
            continue;
        }
        ImportedObjectsJson.Add(MakeShared<FJsonValueObject>(FUnrealMCPCommonUtils::ObjectToJsonObject(ImportedObject, false)));
        ImportedPaths.Add(MakeShared<FJsonValueString>(ImportedObject->GetPathName()));
        PackagesToSave.AddUnique(ImportedObject->GetOutermost());
    }

    if (ImportSpec.bSave && PackagesToSave.Num() > 0)
    {
        UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, true);
        ResultObj->SetBoolField(TEXT("saved"), true);
    }

    ResultObj->SetArrayField(TEXT("imported_object_paths"), ImportedPaths);
    ResultObj->SetArrayField(TEXT("imported_objects"), ImportedObjectsJson);
    ResultObj->SetNumberField(TEXT("imported_count"), ImportedObjectsJson.Num());

    if (ImportedObjectsJson.Num() <= 0)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Import failed: %s"), *ImportSpec.SourceFile));
    }

    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleImportAssetsBatch(const TSharedPtr<FJsonObject>& Params)
{
    const TArray<TSharedPtr<FJsonValue>>* AssetSpecs = nullptr;
    if (!Params->TryGetArrayField(TEXT("assets"), AssetSpecs) || !AssetSpecs)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'assets' array parameter"));
    }

    TArray<TPair<FUnrealMCPImportSpec, TSharedPtr<FJsonObject>>> Tasks;
    TArray<TSharedPtr<FJsonValue>> ValidationErrors;
    for (const TSharedPtr<FJsonValue>& AssetSpecValue : *AssetSpecs)
    {
        const TSharedPtr<FJsonObject>* AssetSpecPtr = nullptr;
        if (!AssetSpecValue->TryGetObject(AssetSpecPtr) || !AssetSpecPtr || !AssetSpecPtr->IsValid())
        {
            ValidationErrors.Add(MakeShared<FJsonValueString>(TEXT("Asset spec must be an object")));
            continue;
        }

        FString Error;
        FUnrealMCPImportSpec ImportSpec;
        if (!ReadImportSpecFromParams(*AssetSpecPtr, ImportSpec, Error))
        {
            ValidationErrors.Add(MakeShared<FJsonValueString>(Error));
            continue;
        }
        Tasks.Add(TPair<FUnrealMCPImportSpec, TSharedPtr<FJsonObject>>(ImportSpec, *AssetSpecPtr));
    }

    if (Tasks.Num() == 0)
    {
        TSharedPtr<FJsonObject> ErrorObj = FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("No valid import tasks"));
        ErrorObj->SetArrayField(TEXT("validation_errors"), ValidationErrors);
        return ErrorObj;
    }

    TArray<TSharedPtr<FJsonValue>> ResultArray;
    int32 ImportedTotal = 0;
    for (const TPair<FUnrealMCPImportSpec, TSharedPtr<FJsonObject>>& TaskPair : Tasks)
    {
        const FUnrealMCPImportSpec& ImportSpec = TaskPair.Key;
        UFactory* Factory = CreateLegacyImportFactory(ImportSpec.SourceFile, TaskPair.Value);
        TArray<UObject*> ImportedObjects = ImportWithAssetTask(ImportSpec, Factory);

        TSharedPtr<FJsonObject> TaskObj = MakeShared<FJsonObject>();
        TaskObj->SetStringField(TEXT("source_file"), ImportSpec.SourceFile);
        TaskObj->SetStringField(TEXT("destination_path"), ImportSpec.DestinationPath);
        TaskObj->SetStringField(TEXT("destination_name"), ImportSpec.DestinationName);
        TaskObj->SetBoolField(TEXT("replace_existing"), ImportSpec.bReplaceExisting);
        TaskObj->SetBoolField(TEXT("saved"), false);

        TArray<TSharedPtr<FJsonValue>> ImportedObjectsJson;
        TArray<TSharedPtr<FJsonValue>> ImportedPaths;
        TArray<UPackage*> PackagesToSave;
        for (UObject* ImportedObject : ImportedObjects)
        {
            if (!ImportedObject)
            {
                continue;
            }
            ImportedObjectsJson.Add(MakeShared<FJsonValueObject>(FUnrealMCPCommonUtils::ObjectToJsonObject(ImportedObject, false)));
            ImportedPaths.Add(MakeShared<FJsonValueString>(ImportedObject->GetPathName()));
            PackagesToSave.AddUnique(ImportedObject->GetOutermost());
        }
        if (ImportSpec.bSave && PackagesToSave.Num() > 0)
        {
            UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, true);
            TaskObj->SetBoolField(TEXT("saved"), true);
        }
        TaskObj->SetArrayField(TEXT("imported_object_paths"), ImportedPaths);
        TaskObj->SetArrayField(TEXT("imported_objects"), ImportedObjectsJson);
        TaskObj->SetNumberField(TEXT("imported_count"), ImportedObjectsJson.Num());
        ImportedTotal += ImportedObjectsJson.Num();
        ResultArray.Add(MakeShared<FJsonValueObject>(TaskObj));
    }

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetNumberField(TEXT("task_count"), Tasks.Num());
    ResultObj->SetNumberField(TEXT("imported_total"), ImportedTotal);
    ResultObj->SetArrayField(TEXT("results"), ResultArray);
    ResultObj->SetArrayField(TEXT("validation_errors"), ValidationErrors);
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleInspectImportOptions(const TSharedPtr<FJsonObject>& Params)
{
    FString SourceFile;
    Params->TryGetStringField(TEXT("source_file"), SourceFile);
    Params->TryGetStringField(TEXT("filename"), SourceFile);
    Params->TryGetStringField(TEXT("file_path"), SourceFile);

    FString Extension;
    Params->TryGetStringField(TEXT("extension"), Extension);
    if (Extension.IsEmpty() && !SourceFile.IsEmpty())
    {
        Extension = FPaths::GetExtension(SourceFile);
    }
    Extension.RemoveFromStart(TEXT("."));
    Extension = Extension.ToLower();

    TArray<UFactory*> Factories;
    for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
    {
        UClass* FactoryClass = *ClassIt;
        if (!FactoryClass || FactoryClass->HasAnyClassFlags(CLASS_Abstract) || !FactoryClass->IsChildOf(UFactory::StaticClass()))
        {
            continue;
        }

        UFactory* Factory = NewObject<UFactory>(GetTransientPackage(), FactoryClass);
        if (Factory && Factory->GetFormats().Num() > 0)
        {
            Factories.Add(Factory);
        }
    }

    TArray<TSharedPtr<FJsonValue>> FactoryArray;
    for (UFactory* Factory : Factories)
    {
        bool bMatchesExtension = Extension.IsEmpty();
        TArray<TSharedPtr<FJsonValue>> FormatArray;
        for (const FString& Format : Factory->GetFormats())
        {
            FString FormatExtension;
            FString Description;
            Format.Split(TEXT(";"), &FormatExtension, &Description);
            FormatExtension = FormatExtension.ToLower();
            if (!Extension.IsEmpty() && FormatExtension == Extension)
            {
                bMatchesExtension = true;
            }

            TSharedPtr<FJsonObject> FormatObj = MakeShared<FJsonObject>();
            FormatObj->SetStringField(TEXT("extension"), FormatExtension);
            FormatObj->SetStringField(TEXT("description"), Description);
            FormatObj->SetStringField(TEXT("raw"), Format);
            FormatArray.Add(MakeShared<FJsonValueObject>(FormatObj));
        }

        if (!bMatchesExtension)
        {
            continue;
        }

        TSharedPtr<FJsonObject> FactoryObj = MakeShared<FJsonObject>();
        FactoryObj->SetStringField(TEXT("factory_class"), Factory->GetClass()->GetName());
        FactoryObj->SetStringField(TEXT("factory_path"), Factory->GetClass()->GetPathName());
        UClass* SupportedClass = Factory->ResolveSupportedClass();
        FactoryObj->SetStringField(TEXT("supported_class"), SupportedClass ? SupportedClass->GetName() : TEXT(""));
        FactoryObj->SetBoolField(TEXT("can_create_new"), Factory->CanCreateNew());
        FactoryObj->SetBoolField(TEXT("can_import_source_file"), !SourceFile.IsEmpty() ? Factory->FactoryCanImport(SourceFile) : false);
        FactoryObj->SetArrayField(TEXT("formats"), FormatArray);
        FactoryArray.Add(MakeShared<FJsonValueObject>(FactoryObj));
    }

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("source_file"), SourceFile);
    ResultObj->SetStringField(TEXT("extension"), Extension);
    ResultObj->SetArrayField(TEXT("factories"), FactoryArray);
    ResultObj->SetNumberField(TEXT("factory_count"), FactoryArray.Num());
    ResultObj->SetStringField(TEXT("note"), TEXT("This reports import factories and supported extensions. Per-format UI option objects are factory-specific and not fully enumerated yet."));
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleSpawnMeshActorFromAsset(const TSharedPtr<FJsonObject>& Params)
{
    FString AssetPath;
    if (!Params->TryGetStringField(TEXT("asset_path"), AssetPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'asset_path' parameter"));
    }

    FString ActorName;
    if (!Params->TryGetStringField(TEXT("actor_name"), ActorName))
    {
        Params->TryGetStringField(TEXT("name"), ActorName);
    }
    if (ActorName.IsEmpty())
    {
        ActorName = FPackageName::GetShortName(AssetPath);
    }

    FVector Location(0.0f, 0.0f, 0.0f);
    FRotator Rotation(0.0f, 0.0f, 0.0f);
    FVector Scale(1.0f, 1.0f, 1.0f);
    if (Params->HasField(TEXT("location")))
    {
        Location = FUnrealMCPCommonUtils::GetVectorFromJson(Params, TEXT("location"));
    }
    if (Params->HasField(TEXT("rotation")))
    {
        Rotation = FUnrealMCPCommonUtils::GetRotatorFromJson(Params, TEXT("rotation"));
    }
    if (Params->HasField(TEXT("scale")))
    {
        Scale = FUnrealMCPCommonUtils::GetVectorFromJson(Params, TEXT("scale"));
    }

    UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
    if (!World)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to get editor world"));
    }

    UObject* Asset = UEditorAssetLibrary::LoadAsset(AssetPath);
    AActor* NewActor = nullptr;
    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = *ActorName;

    if (UStaticMesh* StaticMesh = Cast<UStaticMesh>(Asset))
    {
        AStaticMeshActor* StaticActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation, SpawnParams);
        if (StaticActor && StaticActor->GetStaticMeshComponent())
        {
            StaticActor->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);
            NewActor = StaticActor;
        }
    }
    else if (USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(Asset))
    {
        ASkeletalMeshActor* SkeletalActor = World->SpawnActor<ASkeletalMeshActor>(ASkeletalMeshActor::StaticClass(), Location, Rotation, SpawnParams);
        if (SkeletalActor && SkeletalActor->GetSkeletalMeshComponent())
        {
            SkeletalActor->GetSkeletalMeshComponent()->SetSkeletalMesh(SkeletalMesh);
            NewActor = SkeletalActor;
        }
    }
    else
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Asset is not a StaticMesh or SkeletalMesh: %s"), *AssetPath));
    }

    if (!NewActor)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to spawn mesh actor"));
    }

    NewActor->SetActorScale3D(Scale);
    return FUnrealMCPCommonUtils::ActorToJsonObject(NewActor, true);
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleSetActorMeshAsset(const TSharedPtr<FJsonObject>& Params)
{
    FString ActorName;
    if (!Params->TryGetStringField(TEXT("actor_name"), ActorName))
    {
        Params->TryGetStringField(TEXT("name"), ActorName);
    }

    FString AssetPath;
    if (!Params->TryGetStringField(TEXT("asset_path"), AssetPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'asset_path' parameter"));
    }

    FString ComponentName;
    Params->TryGetStringField(TEXT("component_name"), ComponentName);

    AActor* TargetActor = nullptr;
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GWorld, AActor::StaticClass(), AllActors);
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName() == ActorName)
        {
            TargetActor = Actor;
            break;
        }
    }

    if (!TargetActor)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Actor not found: %s"), *ActorName));
    }

    UObject* Asset = UEditorAssetLibrary::LoadAsset(AssetPath);
    bool bSet = false;
    TArray<UActorComponent*> Components;
    TargetActor->GetComponents(Components);
    for (UActorComponent* Component : Components)
    {
        if (!ComponentName.IsEmpty() && Component->GetName() != ComponentName)
        {
            continue;
        }
        if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Component))
        {
            if (UStaticMesh* StaticMesh = Cast<UStaticMesh>(Asset))
            {
                StaticMeshComponent->SetStaticMesh(StaticMesh);
                bSet = true;
                break;
            }
        }
        if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Component))
        {
            if (USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(Asset))
            {
                SkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
                bSet = true;
                break;
            }
        }
    }

    if (!bSet)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("No compatible mesh component found or asset type mismatched"));
    }

    return FUnrealMCPCommonUtils::ActorToJsonObject(TargetActor, true);
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleSetActorComponentProperty(const TSharedPtr<FJsonObject>& Params)
{
    FString ActorName;
    if (!Params->TryGetStringField(TEXT("actor_name"), ActorName))
    {
        Params->TryGetStringField(TEXT("name"), ActorName);
    }
    if (ActorName.IsEmpty())
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'actor_name' parameter"));
    }

    FString ComponentName;
    if (!Params->TryGetStringField(TEXT("component_name"), ComponentName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'component_name' parameter"));
    }

    FString PropertyName;
    if (!Params->TryGetStringField(TEXT("property_name"), PropertyName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'property_name' parameter"));
    }

    if (!Params->HasField(TEXT("property_value")))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'property_value' parameter"));
    }

    AActor* TargetActor = nullptr;
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GWorld, AActor::StaticClass(), AllActors);
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName() == ActorName)
        {
            TargetActor = Actor;
            break;
        }
    }

    if (!TargetActor)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Actor not found: %s"), *ActorName));
    }

    UActorComponent* TargetComponent = nullptr;
    TArray<UActorComponent*> Components;
    TargetActor->GetComponents(Components);
    for (UActorComponent* Component : Components)
    {
        if (Component && Component->GetName() == ComponentName)
        {
            TargetComponent = Component;
            break;
        }
    }

    if (!TargetComponent)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Component not found: %s"), *ComponentName));
    }

    FString ErrorMessage;
    if (!FUnrealMCPCommonUtils::SetObjectProperty(TargetComponent, PropertyName, Params->Values.FindRef(TEXT("property_value")), ErrorMessage))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(ErrorMessage);
    }

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("actor_name"), ActorName);
    ResultObj->SetStringField(TEXT("component_name"), ComponentName);
    ResultObj->SetStringField(TEXT("property_name"), PropertyName);
    ResultObj->SetObjectField(TEXT("component"), FUnrealMCPCommonUtils::ObjectToJsonObject(TargetComponent, true, 100));
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleSetActorComponentMaterial(const TSharedPtr<FJsonObject>& Params)
{
    FString ActorName;
    if (!Params->TryGetStringField(TEXT("actor_name"), ActorName))
    {
        Params->TryGetStringField(TEXT("name"), ActorName);
    }

    FString ComponentName;
    Params->TryGetStringField(TEXT("component_name"), ComponentName);

    FString MaterialPath;
    if (!Params->TryGetStringField(TEXT("material_path"), MaterialPath))
    {
        Params->TryGetStringField(TEXT("material"), MaterialPath);
    }

    int32 MaterialIndex = 0;
    Params->TryGetNumberField(TEXT("material_index"), MaterialIndex);

    UMaterialInterface* Material = Cast<UMaterialInterface>(UEditorAssetLibrary::LoadAsset(MaterialPath));
    if (!Material)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Material not found: %s"), *MaterialPath));
    }

    AActor* TargetActor = nullptr;
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GWorld, AActor::StaticClass(), AllActors);
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName() == ActorName)
        {
            TargetActor = Actor;
            break;
        }
    }

    if (!TargetActor)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Actor not found: %s"), *ActorName));
    }

    bool bSet = false;
    TArray<UActorComponent*> Components;
    TargetActor->GetComponents(Components);
    for (UActorComponent* Component : Components)
    {
        UMeshComponent* MeshComponent = Cast<UMeshComponent>(Component);
        if (!MeshComponent)
        {
            continue;
        }
        if (!ComponentName.IsEmpty() && MeshComponent->GetName() != ComponentName)
        {
            continue;
        }
        MeshComponent->SetMaterial(MaterialIndex, Material);
        bSet = true;
        break;
    }

    if (!bSet)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("No matching mesh component found"));
    }

    return FUnrealMCPCommonUtils::ActorToJsonObject(TargetActor, true);
}

static bool SplitAssetPathForCreation(const FString& AssetPath, FString& OutPackagePath, FString& OutAssetName, FString& OutError)
{
    if (!AssetPath.StartsWith(TEXT("/Game/")))
    {
        OutError = TEXT("asset_path must be under /Game");
        return false;
    }

    OutPackagePath = FPackageName::GetLongPackagePath(AssetPath);
    OutAssetName = FPackageName::GetLongPackageAssetName(AssetPath);
    if (OutPackagePath.IsEmpty() || OutAssetName.IsEmpty())
    {
        OutError = FString::Printf(TEXT("Invalid asset_path: %s"), *AssetPath);
        return false;
    }
    return true;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleCreateMaterial(const TSharedPtr<FJsonObject>& Params)
{
    FString AssetPath;
    if (!Params->TryGetStringField(TEXT("asset_path"), AssetPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'asset_path' parameter"));
    }

    FString PackagePath;
    FString AssetName;
    FString Error;
    if (!SplitAssetPathForCreation(AssetPath, PackagePath, AssetName, Error))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(Error);
    }

    if (!UEditorAssetLibrary::DoesDirectoryExist(PackagePath))
    {
        UEditorAssetLibrary::MakeDirectory(PackagePath);
    }

    UMaterialFactoryNew* Factory = NewObject<UMaterialFactoryNew>();
    FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
    UObject* NewAsset = AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, UMaterial::StaticClass(), Factory);
    UMaterial* Material = Cast<UMaterial>(NewAsset);
    if (!Material)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Failed to create material: %s"), *AssetPath));
    }

    Material->MarkPackageDirty();
    bool bSave = true;
    Params->TryGetBoolField(TEXT("save"), bSave);
    if (bSave)
    {
        UEditorAssetLibrary::SaveLoadedAsset(Material, false);
    }

    TSharedPtr<FJsonObject> ResultObj = FUnrealMCPCommonUtils::ObjectToJsonObject(Material, false);
    ResultObj->SetStringField(TEXT("asset_path"), AssetPath);
    ResultObj->SetBoolField(TEXT("saved"), bSave);
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleCreateMaterialInstance(const TSharedPtr<FJsonObject>& Params)
{
    FString AssetPath;
    if (!Params->TryGetStringField(TEXT("asset_path"), AssetPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'asset_path' parameter"));
    }

    FString ParentMaterialPath;
    if (!Params->TryGetStringField(TEXT("parent_material_path"), ParentMaterialPath) &&
        !Params->TryGetStringField(TEXT("parent_path"), ParentMaterialPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'parent_material_path' parameter"));
    }

    UMaterialInterface* ParentMaterial = Cast<UMaterialInterface>(UEditorAssetLibrary::LoadAsset(ParentMaterialPath));
    if (!ParentMaterial)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Parent material not found: %s"), *ParentMaterialPath));
    }

    FString PackagePath;
    FString AssetName;
    FString Error;
    if (!SplitAssetPathForCreation(AssetPath, PackagePath, AssetName, Error))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(Error);
    }

    if (!UEditorAssetLibrary::DoesDirectoryExist(PackagePath))
    {
        UEditorAssetLibrary::MakeDirectory(PackagePath);
    }

    UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
    Factory->InitialParent = ParentMaterial;

    FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
    UObject* NewAsset = AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, UMaterialInstanceConstant::StaticClass(), Factory);
    UMaterialInstanceConstant* MaterialInstance = Cast<UMaterialInstanceConstant>(NewAsset);
    if (!MaterialInstance)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Failed to create material instance: %s"), *AssetPath));
    }

    MaterialInstance->SetParentEditorOnly(ParentMaterial);
    MaterialInstance->PostEditChange();
    MaterialInstance->MarkPackageDirty();

    bool bSave = true;
    Params->TryGetBoolField(TEXT("save"), bSave);
    if (bSave)
    {
        UEditorAssetLibrary::SaveLoadedAsset(MaterialInstance, false);
    }

    TSharedPtr<FJsonObject> ResultObj = FUnrealMCPCommonUtils::ObjectToJsonObject(MaterialInstance, false);
    ResultObj->SetStringField(TEXT("asset_path"), AssetPath);
    ResultObj->SetStringField(TEXT("parent_material_path"), ParentMaterialPath);
    ResultObj->SetBoolField(TEXT("saved"), bSave);
    return ResultObj;
}

static TSharedPtr<FJsonObject> MaterialParameterInfoToJson(const FMaterialParameterInfo& ParameterInfo, const FGuid& ParameterId)
{
    TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
    Obj->SetStringField(TEXT("name"), ParameterInfo.Name.ToString());
    Obj->SetStringField(TEXT("association"), StaticEnum<EMaterialParameterAssociation>()->GetNameStringByValue(ParameterInfo.Association));
    Obj->SetNumberField(TEXT("index"), ParameterInfo.Index);
    Obj->SetStringField(TEXT("id"), ParameterId.ToString(EGuidFormats::DigitsWithHyphens));
    return Obj;
}

static void AddMaterialParameterArray(
    UMaterialInterface* Material,
    const FString& TypeName,
    const TArray<FMaterialParameterInfo>& Infos,
    const TArray<FGuid>& Ids,
    TSharedPtr<FJsonObject> ResultObj)
{
    TArray<TSharedPtr<FJsonValue>> Parameters;
    for (int32 Index = 0; Index < Infos.Num(); ++Index)
    {
        const FMaterialParameterInfo& Info = Infos[Index];
        const FGuid Id = Ids.IsValidIndex(Index) ? Ids[Index] : FGuid();
        TSharedPtr<FJsonObject> ParamObj = MaterialParameterInfoToJson(Info, Id);

        if (TypeName == TEXT("scalar"))
        {
            float Value = 0.0f;
            if (Material->GetScalarParameterValue(FHashedMaterialParameterInfo(Info), Value))
            {
                ParamObj->SetNumberField(TEXT("value"), Value);
            }
        }
        else if (TypeName == TEXT("vector"))
        {
            FLinearColor Value;
            if (Material->GetVectorParameterValue(FHashedMaterialParameterInfo(Info), Value))
            {
                TArray<TSharedPtr<FJsonValue>> ColorArray;
                ColorArray.Add(MakeShared<FJsonValueNumber>(Value.R));
                ColorArray.Add(MakeShared<FJsonValueNumber>(Value.G));
                ColorArray.Add(MakeShared<FJsonValueNumber>(Value.B));
                ColorArray.Add(MakeShared<FJsonValueNumber>(Value.A));
                ParamObj->SetArrayField(TEXT("value"), ColorArray);
            }
        }
        else if (TypeName == TEXT("texture"))
        {
            UTexture* Value = nullptr;
            if (Material->GetTextureParameterValue(FHashedMaterialParameterInfo(Info), Value))
            {
                ParamObj->SetStringField(TEXT("value"), Value ? Value->GetPathName() : TEXT(""));
            }
        }
        else if (TypeName == TEXT("static_switch"))
        {
            bool Value = false;
            FGuid ExpressionGuid;
            if (Material->GetStaticSwitchParameterValue(FHashedMaterialParameterInfo(Info), Value, ExpressionGuid))
            {
                ParamObj->SetBoolField(TEXT("value"), Value);
                ParamObj->SetStringField(TEXT("expression_guid"), ExpressionGuid.ToString(EGuidFormats::DigitsWithHyphens));
            }
        }

        Parameters.Add(MakeShared<FJsonValueObject>(ParamObj));
    }
    ResultObj->SetArrayField(TypeName + TEXT("_parameters"), Parameters);
    ResultObj->SetNumberField(TypeName + TEXT("_count"), Parameters.Num());
}

static bool MaterialHasParameterOfType(UMaterialInterface* Material, EMaterialParameterType Type, const FName& ParameterName)
{
    if (!Material)
    {
        return false;
    }

    TArray<FMaterialParameterInfo> Infos;
    TArray<FGuid> Ids;
    Material->GetAllParameterInfoOfType(Type, Infos, Ids);
    for (const FMaterialParameterInfo& Info : Infos)
    {
        if (Info.Name == ParameterName)
        {
            return true;
        }
    }
    return false;
}

static UMaterialExpression* FindMaterialExpressionById(UMaterial* Material, const FString& ExpressionId)
{
    if (!Material)
    {
        return nullptr;
    }

    FGuid TargetGuid;
    const bool bHasGuid = FGuid::Parse(ExpressionId, TargetGuid);
    for (UMaterialExpression* Expression : Material->GetExpressions())
    {
        if (!Expression)
        {
            continue;
        }

        if ((bHasGuid && Expression->MaterialExpressionGuid == TargetGuid) ||
            Expression->GetName() == ExpressionId ||
            Expression->GetPathName() == ExpressionId)
        {
            return Expression;
        }
    }
    return nullptr;
}

static TSubclassOf<UMaterialExpression> ResolveMaterialExpressionClass(const FString& ExpressionType)
{
    const FString Normalized = ExpressionType.Replace(TEXT(" "), TEXT("")).Replace(TEXT("_"), TEXT(""));
    if (Normalized.Equals(TEXT("ScalarParameter"), ESearchCase::IgnoreCase) ||
        Normalized.Equals(TEXT("MaterialExpressionScalarParameter"), ESearchCase::IgnoreCase))
    {
        return UMaterialExpressionScalarParameter::StaticClass();
    }
    if (Normalized.Equals(TEXT("VectorParameter"), ESearchCase::IgnoreCase) ||
        Normalized.Equals(TEXT("MaterialExpressionVectorParameter"), ESearchCase::IgnoreCase))
    {
        return UMaterialExpressionVectorParameter::StaticClass();
    }
    if (Normalized.Equals(TEXT("TextureParameter"), ESearchCase::IgnoreCase) ||
        Normalized.Equals(TEXT("TextureSampleParameter2D"), ESearchCase::IgnoreCase) ||
        Normalized.Equals(TEXT("MaterialExpressionTextureSampleParameter2D"), ESearchCase::IgnoreCase))
    {
        return UMaterialExpressionTextureSampleParameter2D::StaticClass();
    }
    if (Normalized.Equals(TEXT("StaticSwitchParameter"), ESearchCase::IgnoreCase) ||
        Normalized.Equals(TEXT("MaterialExpressionStaticSwitchParameter"), ESearchCase::IgnoreCase))
    {
        return UMaterialExpressionStaticSwitchParameter::StaticClass();
    }
    if (Normalized.Equals(TEXT("Constant"), ESearchCase::IgnoreCase) ||
        Normalized.Equals(TEXT("Constant1"), ESearchCase::IgnoreCase) ||
        Normalized.Equals(TEXT("MaterialExpressionConstant"), ESearchCase::IgnoreCase))
    {
        return UMaterialExpressionConstant::StaticClass();
    }
    if (Normalized.Equals(TEXT("Constant2Vector"), ESearchCase::IgnoreCase) ||
        Normalized.Equals(TEXT("Constant2"), ESearchCase::IgnoreCase))
    {
        return UMaterialExpressionConstant2Vector::StaticClass();
    }
    if (Normalized.Equals(TEXT("Constant3Vector"), ESearchCase::IgnoreCase) ||
        Normalized.Equals(TEXT("Constant3"), ESearchCase::IgnoreCase))
    {
        return UMaterialExpressionConstant3Vector::StaticClass();
    }
    if (Normalized.Equals(TEXT("Constant4Vector"), ESearchCase::IgnoreCase) ||
        Normalized.Equals(TEXT("Constant4"), ESearchCase::IgnoreCase))
    {
        return UMaterialExpressionConstant4Vector::StaticClass();
    }
    if (Normalized.Equals(TEXT("Multiply"), ESearchCase::IgnoreCase))
    {
        return UMaterialExpressionMultiply::StaticClass();
    }
    if (Normalized.Equals(TEXT("Add"), ESearchCase::IgnoreCase))
    {
        return UMaterialExpressionAdd::StaticClass();
    }
    if (Normalized.Equals(TEXT("LinearInterpolate"), ESearchCase::IgnoreCase) ||
        Normalized.Equals(TEXT("Lerp"), ESearchCase::IgnoreCase))
    {
        return UMaterialExpressionLinearInterpolate::StaticClass();
    }
    if (Normalized.Equals(TEXT("AppendVector"), ESearchCase::IgnoreCase))
    {
        return UMaterialExpressionAppendVector::StaticClass();
    }
    if (Normalized.Equals(TEXT("OneMinus"), ESearchCase::IgnoreCase))
    {
        return UMaterialExpressionOneMinus::StaticClass();
    }
    if (Normalized.Equals(TEXT("TextureCoordinate"), ESearchCase::IgnoreCase) ||
        Normalized.Equals(TEXT("TexCoord"), ESearchCase::IgnoreCase))
    {
        return UMaterialExpressionTextureCoordinate::StaticClass();
    }
    return nullptr;
}

static bool ResolveMaterialProperty(const FString& PropertyName, EMaterialProperty& OutProperty)
{
    const FString Normalized = PropertyName.Replace(TEXT(" "), TEXT("")).Replace(TEXT("_"), TEXT(""));
    if (Normalized.Equals(TEXT("BaseColor"), ESearchCase::IgnoreCase) || Normalized.Equals(TEXT("MPBaseColor"), ESearchCase::IgnoreCase))
    {
        OutProperty = MP_BaseColor;
        return true;
    }
    if (Normalized.Equals(TEXT("Metallic"), ESearchCase::IgnoreCase) || Normalized.Equals(TEXT("MPMetallic"), ESearchCase::IgnoreCase))
    {
        OutProperty = MP_Metallic;
        return true;
    }
    if (Normalized.Equals(TEXT("Specular"), ESearchCase::IgnoreCase) || Normalized.Equals(TEXT("MPSpecular"), ESearchCase::IgnoreCase))
    {
        OutProperty = MP_Specular;
        return true;
    }
    if (Normalized.Equals(TEXT("Roughness"), ESearchCase::IgnoreCase) || Normalized.Equals(TEXT("MPRoughness"), ESearchCase::IgnoreCase))
    {
        OutProperty = MP_Roughness;
        return true;
    }
    if (Normalized.Equals(TEXT("EmissiveColor"), ESearchCase::IgnoreCase) || Normalized.Equals(TEXT("Emissive"), ESearchCase::IgnoreCase) || Normalized.Equals(TEXT("MPEmissiveColor"), ESearchCase::IgnoreCase))
    {
        OutProperty = MP_EmissiveColor;
        return true;
    }
    if (Normalized.Equals(TEXT("Opacity"), ESearchCase::IgnoreCase) || Normalized.Equals(TEXT("MPOpacity"), ESearchCase::IgnoreCase))
    {
        OutProperty = MP_Opacity;
        return true;
    }
    if (Normalized.Equals(TEXT("OpacityMask"), ESearchCase::IgnoreCase) || Normalized.Equals(TEXT("MPOpacityMask"), ESearchCase::IgnoreCase))
    {
        OutProperty = MP_OpacityMask;
        return true;
    }
    if (Normalized.Equals(TEXT("Normal"), ESearchCase::IgnoreCase) || Normalized.Equals(TEXT("MPNormal"), ESearchCase::IgnoreCase))
    {
        OutProperty = MP_Normal;
        return true;
    }
    if (Normalized.Equals(TEXT("WorldPositionOffset"), ESearchCase::IgnoreCase) || Normalized.Equals(TEXT("WPO"), ESearchCase::IgnoreCase))
    {
        OutProperty = MP_WorldPositionOffset;
        return true;
    }
    if (Normalized.Equals(TEXT("AmbientOcclusion"), ESearchCase::IgnoreCase) || Normalized.Equals(TEXT("AO"), ESearchCase::IgnoreCase))
    {
        OutProperty = MP_AmbientOcclusion;
        return true;
    }
    return false;
}

static bool ReadLinearColorField(const TSharedPtr<FJsonObject>& Params, const FString& FieldName, FLinearColor& OutColor)
{
    const TArray<TSharedPtr<FJsonValue>>* ValueArray = nullptr;
    if (!Params->TryGetArrayField(FieldName, ValueArray) || !ValueArray || ValueArray->Num() < 3)
    {
        return false;
    }

    OutColor.R = (float)(*ValueArray)[0]->AsNumber();
    OutColor.G = (float)(*ValueArray)[1]->AsNumber();
    OutColor.B = (float)(*ValueArray)[2]->AsNumber();
    OutColor.A = ValueArray->Num() > 3 ? (float)(*ValueArray)[3]->AsNumber() : 1.0f;
    return true;
}

static TSharedPtr<FJsonObject> MaterialExpressionToJson(UMaterialExpression* Expression)
{
    TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
    if (!Expression)
    {
        return Obj;
    }

    Obj->SetStringField(TEXT("id"), Expression->MaterialExpressionGuid.ToString(EGuidFormats::DigitsWithHyphens));
    Obj->SetStringField(TEXT("name"), Expression->GetName());
    Obj->SetStringField(TEXT("class"), Expression->GetClass()->GetName());
    Obj->SetNumberField(TEXT("node_x"), Expression->MaterialExpressionEditorX);
    Obj->SetNumberField(TEXT("node_y"), Expression->MaterialExpressionEditorY);
    Obj->SetStringField(TEXT("description"), Expression->Desc);
    Obj->SetBoolField(TEXT("is_parameter"), Expression->HasAParameterName());
    if (Expression->HasAParameterName())
    {
        Obj->SetStringField(TEXT("parameter_name"), Expression->GetParameterName().ToString());
    }

    if (UMaterialExpressionScalarParameter* Scalar = Cast<UMaterialExpressionScalarParameter>(Expression))
    {
        Obj->SetNumberField(TEXT("default_value"), Scalar->DefaultValue);
    }
    else if (UMaterialExpressionVectorParameter* Vector = Cast<UMaterialExpressionVectorParameter>(Expression))
    {
        TArray<TSharedPtr<FJsonValue>> ColorArray;
        ColorArray.Add(MakeShared<FJsonValueNumber>(Vector->DefaultValue.R));
        ColorArray.Add(MakeShared<FJsonValueNumber>(Vector->DefaultValue.G));
        ColorArray.Add(MakeShared<FJsonValueNumber>(Vector->DefaultValue.B));
        ColorArray.Add(MakeShared<FJsonValueNumber>(Vector->DefaultValue.A));
        Obj->SetArrayField(TEXT("default_value"), ColorArray);
    }
    else if (UMaterialExpressionTextureSampleParameter2D* TextureParameter = Cast<UMaterialExpressionTextureSampleParameter2D>(Expression))
    {
        Obj->SetStringField(TEXT("texture"), TextureParameter->Texture ? TextureParameter->Texture->GetPathName() : TEXT(""));
    }
    else if (UMaterialExpressionStaticSwitchParameter* StaticSwitch = Cast<UMaterialExpressionStaticSwitchParameter>(Expression))
    {
        Obj->SetBoolField(TEXT("default_value"), StaticSwitch->DefaultValue != 0);
    }
    else if (UMaterialExpressionConstant* Constant = Cast<UMaterialExpressionConstant>(Expression))
    {
        Obj->SetNumberField(TEXT("value"), Constant->R);
    }
    else if (UMaterialExpressionConstant2Vector* Constant2 = Cast<UMaterialExpressionConstant2Vector>(Expression))
    {
        TArray<TSharedPtr<FJsonValue>> Values;
        Values.Add(MakeShared<FJsonValueNumber>(Constant2->R));
        Values.Add(MakeShared<FJsonValueNumber>(Constant2->G));
        Obj->SetArrayField(TEXT("value"), Values);
    }
    else if (UMaterialExpressionConstant3Vector* Constant3 = Cast<UMaterialExpressionConstant3Vector>(Expression))
    {
        TArray<TSharedPtr<FJsonValue>> Values;
        Values.Add(MakeShared<FJsonValueNumber>(Constant3->Constant.R));
        Values.Add(MakeShared<FJsonValueNumber>(Constant3->Constant.G));
        Values.Add(MakeShared<FJsonValueNumber>(Constant3->Constant.B));
        Obj->SetArrayField(TEXT("value"), Values);
    }
    else if (UMaterialExpressionConstant4Vector* Constant4 = Cast<UMaterialExpressionConstant4Vector>(Expression))
    {
        TArray<TSharedPtr<FJsonValue>> Values;
        Values.Add(MakeShared<FJsonValueNumber>(Constant4->Constant.R));
        Values.Add(MakeShared<FJsonValueNumber>(Constant4->Constant.G));
        Values.Add(MakeShared<FJsonValueNumber>(Constant4->Constant.B));
        Values.Add(MakeShared<FJsonValueNumber>(Constant4->Constant.A));
        Obj->SetArrayField(TEXT("value"), Values);
    }

    TArray<TSharedPtr<FJsonValue>> Inputs;
    const int32 InputCount = Expression->CountInputs();
    for (int32 Index = 0; Index < InputCount; ++Index)
    {
        FExpressionInput* Input = Expression->GetInput(Index);
        TSharedPtr<FJsonObject> InputObj = MakeShared<FJsonObject>();
        InputObj->SetNumberField(TEXT("index"), Index);
        InputObj->SetStringField(TEXT("name"), Expression->GetInputName(Index).ToString());
        if (Input && Input->Expression)
        {
            InputObj->SetStringField(TEXT("connected_expression_id"), Input->Expression->MaterialExpressionGuid.ToString(EGuidFormats::DigitsWithHyphens));
            InputObj->SetStringField(TEXT("connected_expression_name"), Input->Expression->GetName());
            InputObj->SetNumberField(TEXT("output_index"), Input->OutputIndex);
        }
        Inputs.Add(MakeShared<FJsonValueObject>(InputObj));
    }
    Obj->SetArrayField(TEXT("inputs"), Inputs);

    TArray<TSharedPtr<FJsonValue>> Outputs;
    TArray<FExpressionOutput>& ExpressionOutputs = Expression->GetOutputs();
    for (int32 Index = 0; Index < ExpressionOutputs.Num(); ++Index)
    {
        TSharedPtr<FJsonObject> OutputObj = MakeShared<FJsonObject>();
        OutputObj->SetNumberField(TEXT("index"), Index);
        OutputObj->SetStringField(TEXT("name"), ExpressionOutputs[Index].OutputName.ToString());
        Outputs.Add(MakeShared<FJsonValueObject>(OutputObj));
    }
    Obj->SetArrayField(TEXT("outputs"), Outputs);
    return Obj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleInspectMaterialParameters(const TSharedPtr<FJsonObject>& Params)
{
    FString MaterialPath;
    if (!Params->TryGetStringField(TEXT("material_path"), MaterialPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'material_path' parameter"));
    }

    UMaterialInterface* Material = Cast<UMaterialInterface>(UEditorAssetLibrary::LoadAsset(MaterialPath));
    if (!Material)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Material asset not found: %s"), *MaterialPath));
    }

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("material_path"), MaterialPath);
    ResultObj->SetStringField(TEXT("class"), Material->GetClass()->GetName());

    TArray<FMaterialParameterInfo> Infos;
    TArray<FGuid> Ids;

    Infos.Reset();
    Ids.Reset();
    Material->GetAllScalarParameterInfo(Infos, Ids);
    AddMaterialParameterArray(Material, TEXT("scalar"), Infos, Ids, ResultObj);

    Infos.Reset();
    Ids.Reset();
    Material->GetAllVectorParameterInfo(Infos, Ids);
    AddMaterialParameterArray(Material, TEXT("vector"), Infos, Ids, ResultObj);

    Infos.Reset();
    Ids.Reset();
    Material->GetAllTextureParameterInfo(Infos, Ids);
    AddMaterialParameterArray(Material, TEXT("texture"), Infos, Ids, ResultObj);

    Infos.Reset();
    Ids.Reset();
    Material->GetAllStaticSwitchParameterInfo(Infos, Ids);
    AddMaterialParameterArray(Material, TEXT("static_switch"), Infos, Ids, ResultObj);

    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleSetMaterialParameter(const TSharedPtr<FJsonObject>& Params)
{
    FString MaterialPath;
    if (!Params->TryGetStringField(TEXT("material_path"), MaterialPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'material_path' parameter"));
    }

    FString ParameterName;
    if (!Params->TryGetStringField(TEXT("parameter_name"), ParameterName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'parameter_name' parameter"));
    }

    UObject* Asset = UEditorAssetLibrary::LoadAsset(MaterialPath);
    if (!Asset)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Material asset not found: %s"), *MaterialPath));
    }

    FString ParameterType = TEXT("scalar");
    Params->TryGetStringField(TEXT("parameter_type"), ParameterType);

    bool bSet = false;
    if (ParameterType.Equals(TEXT("scalar"), ESearchCase::IgnoreCase))
    {
        UMaterialInterface* MaterialInterface = Cast<UMaterialInterface>(Asset);
        if (!MaterialHasParameterOfType(MaterialInterface, EMaterialParameterType::Scalar, FName(*ParameterName)))
        {
            return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Scalar parameter not found: %s"), *ParameterName));
        }

        double NumberValue = 0.0;
        if (!Params->TryGetNumberField(TEXT("value"), NumberValue))
        {
            return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing numeric 'value' for scalar parameter"));
        }

        if (UMaterialInstanceConstant* MIC = Cast<UMaterialInstanceConstant>(Asset))
        {
            MIC->SetScalarParameterValueEditorOnly(FMaterialParameterInfo(FName(*ParameterName)), (float)NumberValue);
            bSet = true;
        }
        else if (UMaterial* Material = Cast<UMaterial>(Asset))
        {
            bSet = Material->SetScalarParameterValueEditorOnly(FName(*ParameterName), (float)NumberValue);
        }
    }
    else if (ParameterType.Equals(TEXT("vector"), ESearchCase::IgnoreCase))
    {
        UMaterialInterface* MaterialInterface = Cast<UMaterialInterface>(Asset);
        if (!MaterialHasParameterOfType(MaterialInterface, EMaterialParameterType::Vector, FName(*ParameterName)))
        {
            return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Vector parameter not found: %s"), *ParameterName));
        }

        const TArray<TSharedPtr<FJsonValue>>* ValueArray = nullptr;
        if (!Params->TryGetArrayField(TEXT("value"), ValueArray) || !ValueArray || ValueArray->Num() < 3)
        {
            return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Vector parameter 'value' must be [R,G,B] or [R,G,B,A]"));
        }

        const float R = (float)(*ValueArray)[0]->AsNumber();
        const float G = (float)(*ValueArray)[1]->AsNumber();
        const float B = (float)(*ValueArray)[2]->AsNumber();
        const float A = ValueArray->Num() > 3 ? (float)(*ValueArray)[3]->AsNumber() : 1.0f;
        const FLinearColor ColorValue(R, G, B, A);

        if (UMaterialInstanceConstant* MIC = Cast<UMaterialInstanceConstant>(Asset))
        {
            MIC->SetVectorParameterValueEditorOnly(FMaterialParameterInfo(FName(*ParameterName)), ColorValue);
            bSet = true;
        }
        else if (UMaterial* Material = Cast<UMaterial>(Asset))
        {
            bSet = Material->SetVectorParameterValueEditorOnly(FName(*ParameterName), ColorValue);
        }
    }

    if (!bSet)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to set material parameter. For raw materials, the named parameter must already exist."));
    }

    Asset->PostEditChange();
    Asset->MarkPackageDirty();
    UEditorAssetLibrary::SaveLoadedAsset(Asset, false);

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("material_path"), MaterialPath);
    ResultObj->SetStringField(TEXT("parameter_name"), ParameterName);
    ResultObj->SetStringField(TEXT("parameter_type"), ParameterType);
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleSetMaterialTextureParameter(const TSharedPtr<FJsonObject>& Params)
{
    FString MaterialPath;
    if (!Params->TryGetStringField(TEXT("material_path"), MaterialPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'material_path' parameter"));
    }

    FString ParameterName;
    if (!Params->TryGetStringField(TEXT("parameter_name"), ParameterName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'parameter_name' parameter"));
    }

    FString TexturePath;
    if (!Params->TryGetStringField(TEXT("texture_path"), TexturePath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'texture_path' parameter"));
    }

    UObject* Asset = UEditorAssetLibrary::LoadAsset(MaterialPath);
    if (!Asset)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Material asset not found: %s"), *MaterialPath));
    }

    UTexture* Texture = Cast<UTexture>(UEditorAssetLibrary::LoadAsset(TexturePath));
    if (!Texture)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Texture asset not found: %s"), *TexturePath));
    }

    bool bSet = false;
    UMaterialInterface* MaterialInterface = Cast<UMaterialInterface>(Asset);
    if (!MaterialHasParameterOfType(MaterialInterface, EMaterialParameterType::Texture, FName(*ParameterName)))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Texture parameter not found: %s"), *ParameterName));
    }

    if (UMaterialInstanceConstant* MIC = Cast<UMaterialInstanceConstant>(Asset))
    {
        MIC->SetTextureParameterValueEditorOnly(FMaterialParameterInfo(FName(*ParameterName)), Texture);
        bSet = true;
    }
    else if (UMaterial* Material = Cast<UMaterial>(Asset))
    {
        bSet = Material->SetTextureParameterValueEditorOnly(FName(*ParameterName), Texture);
    }

    if (!bSet)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to set texture parameter. For raw materials, the named parameter must already exist."));
    }

    Asset->PostEditChange();
    Asset->MarkPackageDirty();
    UEditorAssetLibrary::SaveLoadedAsset(Asset, false);

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("material_path"), MaterialPath);
    ResultObj->SetStringField(TEXT("parameter_name"), ParameterName);
    ResultObj->SetStringField(TEXT("texture_path"), TexturePath);
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleSetMaterialStaticSwitchParameter(const TSharedPtr<FJsonObject>& Params)
{
    FString MaterialPath;
    if (!Params->TryGetStringField(TEXT("material_path"), MaterialPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'material_path' parameter"));
    }

    FString ParameterName;
    if (!Params->TryGetStringField(TEXT("parameter_name"), ParameterName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'parameter_name' parameter"));
    }

    bool Value = false;
    if (!Params->TryGetBoolField(TEXT("value"), Value))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing boolean 'value' parameter"));
    }

    UObject* Asset = UEditorAssetLibrary::LoadAsset(MaterialPath);
    if (!Asset)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Material asset not found: %s"), *MaterialPath));
    }

    bool bSet = false;
    UMaterialInterface* MaterialInterface = Cast<UMaterialInterface>(Asset);
    if (!MaterialHasParameterOfType(MaterialInterface, EMaterialParameterType::StaticSwitch, FName(*ParameterName)))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Static switch parameter not found: %s"), *ParameterName));
    }

    if (UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(Asset))
    {
        MaterialInstance->SetStaticSwitchParameterValueEditorOnly(FMaterialParameterInfo(FName(*ParameterName)), Value);
        bSet = true;
    }
    else if (UMaterial* Material = Cast<UMaterial>(Asset))
    {
        TArray<FMaterialParameterInfo> Infos;
        TArray<FGuid> Ids;
        Material->GetAllStaticSwitchParameterInfo(Infos, Ids);
        FGuid ExpressionGuid;
        for (int32 Index = 0; Index < Infos.Num(); ++Index)
        {
            if (Infos[Index].Name == FName(*ParameterName))
            {
                ExpressionGuid = Ids.IsValidIndex(Index) ? Ids[Index] : FGuid();
                break;
            }
        }
        bSet = Material->SetStaticSwitchParameterValueEditorOnly(FName(*ParameterName), Value, ExpressionGuid);
    }

    if (!bSet)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to set static switch parameter. The named static switch must exist."));
    }

    Asset->PostEditChange();
    Asset->MarkPackageDirty();
    UEditorAssetLibrary::SaveLoadedAsset(Asset, false);

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("material_path"), MaterialPath);
    ResultObj->SetStringField(TEXT("parameter_name"), ParameterName);
    ResultObj->SetBoolField(TEXT("value"), Value);
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleInspectMaterialExpressions(const TSharedPtr<FJsonObject>& Params)
{
    FString MaterialPath;
    if (!Params->TryGetStringField(TEXT("material_path"), MaterialPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'material_path' parameter"));
    }

    UMaterial* Material = Cast<UMaterial>(UEditorAssetLibrary::LoadAsset(MaterialPath));
    if (!Material)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Material asset not found or is not a raw Material: %s"), *MaterialPath));
    }

    TArray<TSharedPtr<FJsonValue>> Expressions;
    for (UMaterialExpression* Expression : Material->GetExpressions())
    {
        Expressions.Add(MakeShared<FJsonValueObject>(MaterialExpressionToJson(Expression)));
    }

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("material_path"), MaterialPath);
    ResultObj->SetArrayField(TEXT("expressions"), Expressions);
    ResultObj->SetNumberField(TEXT("expression_count"), Expressions.Num());
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleAddMaterialExpressionNode(const TSharedPtr<FJsonObject>& Params)
{
    FString MaterialPath;
    if (!Params->TryGetStringField(TEXT("material_path"), MaterialPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'material_path' parameter"));
    }

    FString ExpressionType;
    if (!Params->TryGetStringField(TEXT("expression_type"), ExpressionType) &&
        !Params->TryGetStringField(TEXT("expression_class"), ExpressionType))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'expression_type' parameter"));
    }

    UMaterial* Material = Cast<UMaterial>(UEditorAssetLibrary::LoadAsset(MaterialPath));
    if (!Material)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Material asset not found or is not a raw Material: %s"), *MaterialPath));
    }

    TSubclassOf<UMaterialExpression> ExpressionClass = ResolveMaterialExpressionClass(ExpressionType);
    if (!ExpressionClass)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Unsupported material expression type: %s"), *ExpressionType));
    }

    int32 NodeX = 0;
    int32 NodeY = 0;
    Params->TryGetNumberField(TEXT("node_x"), NodeX);
    Params->TryGetNumberField(TEXT("node_y"), NodeY);

    UMaterialExpression* Expression = UMaterialEditingLibrary::CreateMaterialExpression(Material, ExpressionClass, NodeX, NodeY);
    if (!Expression)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Failed to create material expression: %s"), *ExpressionType));
    }

    FString Description;
    if (Params->TryGetStringField(TEXT("description"), Description))
    {
        Expression->Desc = Description;
    }

    FString ParameterName;
    if (Params->TryGetStringField(TEXT("parameter_name"), ParameterName))
    {
        if (UMaterialExpressionParameter* ParameterExpression = Cast<UMaterialExpressionParameter>(Expression))
        {
            ParameterExpression->SetParameterName(FName(*ParameterName));
            if (!ParameterExpression->ExpressionGUID.IsValid())
            {
                ParameterExpression->ExpressionGUID = FGuid::NewGuid();
            }
        }
        else if (UMaterialExpressionTextureSampleParameter* TextureParameter = Cast<UMaterialExpressionTextureSampleParameter>(Expression))
        {
            TextureParameter->SetParameterName(FName(*ParameterName));
            if (!TextureParameter->ExpressionGUID.IsValid())
            {
                TextureParameter->ExpressionGUID = FGuid::NewGuid();
            }
        }
    }

    double NumberValue = 0.0;
    if (UMaterialExpressionScalarParameter* Scalar = Cast<UMaterialExpressionScalarParameter>(Expression))
    {
        if (Params->TryGetNumberField(TEXT("default_value"), NumberValue) || Params->TryGetNumberField(TEXT("value"), NumberValue))
        {
            Scalar->DefaultValue = (float)NumberValue;
        }
    }
    else if (UMaterialExpressionVectorParameter* Vector = Cast<UMaterialExpressionVectorParameter>(Expression))
    {
        FLinearColor ColorValue;
        if (ReadLinearColorField(Params, TEXT("default_value"), ColorValue) || ReadLinearColorField(Params, TEXT("value"), ColorValue))
        {
            Vector->DefaultValue = ColorValue;
        }
    }
    else if (UMaterialExpressionTextureSampleParameter2D* TextureParameter = Cast<UMaterialExpressionTextureSampleParameter2D>(Expression))
    {
        FString TexturePath;
        if (Params->TryGetStringField(TEXT("texture_path"), TexturePath))
        {
            UTexture* Texture = Cast<UTexture>(UEditorAssetLibrary::LoadAsset(TexturePath));
            if (!Texture)
            {
                return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Texture asset not found: %s"), *TexturePath));
            }
            TextureParameter->Texture = Texture;
            TextureParameter->AutoSetSampleType();
        }
    }
    else if (UMaterialExpressionStaticSwitchParameter* StaticSwitch = Cast<UMaterialExpressionStaticSwitchParameter>(Expression))
    {
        bool BoolValue = false;
        if (Params->TryGetBoolField(TEXT("default_value"), BoolValue) || Params->TryGetBoolField(TEXT("value"), BoolValue))
        {
            StaticSwitch->DefaultValue = BoolValue;
        }
    }
    else if (UMaterialExpressionConstant* Constant = Cast<UMaterialExpressionConstant>(Expression))
    {
        if (Params->TryGetNumberField(TEXT("value"), NumberValue) || Params->TryGetNumberField(TEXT("default_value"), NumberValue))
        {
            Constant->R = (float)NumberValue;
        }
    }
    else if (UMaterialExpressionConstant2Vector* Constant2 = Cast<UMaterialExpressionConstant2Vector>(Expression))
    {
        const TArray<TSharedPtr<FJsonValue>>* ValueArray = nullptr;
        if (Params->TryGetArrayField(TEXT("value"), ValueArray) && ValueArray && ValueArray->Num() >= 2)
        {
            Constant2->R = (float)(*ValueArray)[0]->AsNumber();
            Constant2->G = (float)(*ValueArray)[1]->AsNumber();
        }
    }
    else if (UMaterialExpressionConstant3Vector* Constant3 = Cast<UMaterialExpressionConstant3Vector>(Expression))
    {
        FLinearColor ColorValue;
        if (ReadLinearColorField(Params, TEXT("value"), ColorValue) || ReadLinearColorField(Params, TEXT("default_value"), ColorValue))
        {
            Constant3->Constant = ColorValue;
        }
    }
    else if (UMaterialExpressionConstant4Vector* Constant4 = Cast<UMaterialExpressionConstant4Vector>(Expression))
    {
        FLinearColor ColorValue;
        if (ReadLinearColorField(Params, TEXT("value"), ColorValue) || ReadLinearColorField(Params, TEXT("default_value"), ColorValue))
        {
            Constant4->Constant = ColorValue;
        }
    }
    else if (UMaterialExpressionMultiply* Multiply = Cast<UMaterialExpressionMultiply>(Expression))
    {
        double ConstA = 0.0;
        double ConstB = 1.0;
        if (Params->TryGetNumberField(TEXT("const_a"), ConstA))
        {
            Multiply->ConstA = (float)ConstA;
        }
        if (Params->TryGetNumberField(TEXT("const_b"), ConstB))
        {
            Multiply->ConstB = (float)ConstB;
        }
    }

    bool bRecompile = true;
    Params->TryGetBoolField(TEXT("recompile"), bRecompile);
    if (bRecompile)
    {
        UMaterialEditingLibrary::RecompileMaterial(Material);
    }

    Material->PostEditChange();
    Material->MarkPackageDirty();
    UEditorAssetLibrary::SaveLoadedAsset(Material, false);

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("material_path"), MaterialPath);
    ResultObj->SetObjectField(TEXT("expression"), MaterialExpressionToJson(Expression));
    ResultObj->SetBoolField(TEXT("recompiled"), bRecompile);
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleConnectMaterialExpressionToProperty(const TSharedPtr<FJsonObject>& Params)
{
    FString MaterialPath;
    if (!Params->TryGetStringField(TEXT("material_path"), MaterialPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'material_path' parameter"));
    }

    FString ExpressionId;
    if (!Params->TryGetStringField(TEXT("expression_id"), ExpressionId))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'expression_id' parameter"));
    }

    FString PropertyName;
    if (!Params->TryGetStringField(TEXT("property_name"), PropertyName) &&
        !Params->TryGetStringField(TEXT("material_property"), PropertyName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'property_name' parameter"));
    }

    UMaterial* Material = Cast<UMaterial>(UEditorAssetLibrary::LoadAsset(MaterialPath));
    if (!Material)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Material asset not found or is not a raw Material: %s"), *MaterialPath));
    }

    UMaterialExpression* Expression = FindMaterialExpressionById(Material, ExpressionId);
    if (!Expression)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Expression not found: %s"), *ExpressionId));
    }

    EMaterialProperty MaterialProperty;
    if (!ResolveMaterialProperty(PropertyName, MaterialProperty))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Unsupported material property: %s"), *PropertyName));
    }

    FString OutputName;
    Params->TryGetStringField(TEXT("output_name"), OutputName);
    const bool bConnected = UMaterialEditingLibrary::ConnectMaterialProperty(Expression, OutputName, MaterialProperty);
    if (!bConnected)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to connect material expression to property"));
    }

    bool bRecompile = true;
    Params->TryGetBoolField(TEXT("recompile"), bRecompile);
    if (bRecompile)
    {
        UMaterialEditingLibrary::RecompileMaterial(Material);
    }

    Material->PostEditChange();
    Material->MarkPackageDirty();
    UEditorAssetLibrary::SaveLoadedAsset(Material, false);

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("material_path"), MaterialPath);
    ResultObj->SetStringField(TEXT("expression_id"), Expression->MaterialExpressionGuid.ToString(EGuidFormats::DigitsWithHyphens));
    ResultObj->SetStringField(TEXT("property_name"), PropertyName);
    ResultObj->SetStringField(TEXT("output_name"), OutputName);
    ResultObj->SetBoolField(TEXT("recompiled"), bRecompile);
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleConnectMaterialExpressions(const TSharedPtr<FJsonObject>& Params)
{
    FString MaterialPath;
    if (!Params->TryGetStringField(TEXT("material_path"), MaterialPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'material_path' parameter"));
    }

    FString FromExpressionId;
    if (!Params->TryGetStringField(TEXT("from_expression_id"), FromExpressionId))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'from_expression_id' parameter"));
    }

    FString ToExpressionId;
    if (!Params->TryGetStringField(TEXT("to_expression_id"), ToExpressionId))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'to_expression_id' parameter"));
    }

    UMaterial* Material = Cast<UMaterial>(UEditorAssetLibrary::LoadAsset(MaterialPath));
    if (!Material)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Material asset not found or is not a raw Material: %s"), *MaterialPath));
    }

    UMaterialExpression* FromExpression = FindMaterialExpressionById(Material, FromExpressionId);
    UMaterialExpression* ToExpression = FindMaterialExpressionById(Material, ToExpressionId);
    if (!FromExpression)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("From expression not found: %s"), *FromExpressionId));
    }
    if (!ToExpression)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("To expression not found: %s"), *ToExpressionId));
    }

    FString FromOutputName;
    FString ToInputName;
    Params->TryGetStringField(TEXT("from_output_name"), FromOutputName);
    Params->TryGetStringField(TEXT("to_input_name"), ToInputName);

    const bool bConnected = UMaterialEditingLibrary::ConnectMaterialExpressions(FromExpression, FromOutputName, ToExpression, ToInputName);
    if (!bConnected)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to connect material expressions"));
    }

    bool bRecompile = true;
    Params->TryGetBoolField(TEXT("recompile"), bRecompile);
    if (bRecompile)
    {
        UMaterialEditingLibrary::RecompileMaterial(Material);
    }

    Material->PostEditChange();
    Material->MarkPackageDirty();
    UEditorAssetLibrary::SaveLoadedAsset(Material, false);

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("material_path"), MaterialPath);
    ResultObj->SetStringField(TEXT("from_expression_id"), FromExpression->MaterialExpressionGuid.ToString(EGuidFormats::DigitsWithHyphens));
    ResultObj->SetStringField(TEXT("to_expression_id"), ToExpression->MaterialExpressionGuid.ToString(EGuidFormats::DigitsWithHyphens));
    ResultObj->SetStringField(TEXT("from_output_name"), FromOutputName);
    ResultObj->SetStringField(TEXT("to_input_name"), ToInputName);
    ResultObj->SetBoolField(TEXT("recompiled"), bRecompile);
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleRecompileMaterial(const TSharedPtr<FJsonObject>& Params)
{
    FString MaterialPath;
    if (!Params->TryGetStringField(TEXT("material_path"), MaterialPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'material_path' parameter"));
    }

    UMaterial* Material = Cast<UMaterial>(UEditorAssetLibrary::LoadAsset(MaterialPath));
    if (!Material)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Material asset not found or is not a raw Material: %s"), *MaterialPath));
    }

    UMaterialEditingLibrary::RecompileMaterial(Material);
    Material->PostEditChange();
    Material->MarkPackageDirty();
    UEditorAssetLibrary::SaveLoadedAsset(Material, false);

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("material_path"), MaterialPath);
    ResultObj->SetBoolField(TEXT("recompiled"), true);
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleSpawnBlueprintActor(const TSharedPtr<FJsonObject>& Params)
{
    // Get required parameters
    FString BlueprintName;
    if (!Params->TryGetStringField(TEXT("blueprint_name"), BlueprintName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'blueprint_name' parameter"));
    }

    FString ActorName;
    if (!Params->TryGetStringField(TEXT("actor_name"), ActorName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'actor_name' parameter"));
    }

    // Find the blueprint
    if (BlueprintName.IsEmpty())
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Blueprint name is empty"));
    }

    FString Root      = TEXT("/Game/Blueprints/");
    FString AssetPath = Root + BlueprintName;

    if (!FPackageName::DoesPackageExist(AssetPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Blueprint '%s' not found – it must reside under /Game/Blueprints"), *BlueprintName));
    }

    UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *AssetPath);
    if (!Blueprint)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Blueprint not found: %s"), *BlueprintName));
    }

    // Get transform parameters
    FVector Location(0.0f, 0.0f, 0.0f);
    FRotator Rotation(0.0f, 0.0f, 0.0f);
    FVector Scale(1.0f, 1.0f, 1.0f);

    if (Params->HasField(TEXT("location")))
    {
        Location = FUnrealMCPCommonUtils::GetVectorFromJson(Params, TEXT("location"));
    }
    if (Params->HasField(TEXT("rotation")))
    {
        Rotation = FUnrealMCPCommonUtils::GetRotatorFromJson(Params, TEXT("rotation"));
    }
    if (Params->HasField(TEXT("scale")))
    {
        Scale = FUnrealMCPCommonUtils::GetVectorFromJson(Params, TEXT("scale"));
    }

    // Spawn the actor
    UWorld* World = GEditor->GetEditorWorldContext().World();
    if (!World)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to get editor world"));
    }

    FTransform SpawnTransform;
    SpawnTransform.SetLocation(Location);
    SpawnTransform.SetRotation(FQuat(Rotation));
    SpawnTransform.SetScale3D(Scale);

    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = *ActorName;

    AActor* NewActor = World->SpawnActor<AActor>(Blueprint->GeneratedClass, SpawnTransform, SpawnParams);
    if (NewActor)
    {
        return FUnrealMCPCommonUtils::ActorToJsonObject(NewActor, true);
    }

    return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to spawn blueprint actor"));
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleFocusViewport(const TSharedPtr<FJsonObject>& Params)
{
    // Get target actor name if provided
    FString TargetActorName;
    bool HasTargetActor = Params->TryGetStringField(TEXT("target"), TargetActorName);

    // Get location if provided
    FVector Location(0.0f, 0.0f, 0.0f);
    bool HasLocation = false;
    if (Params->HasField(TEXT("location")))
    {
        Location = FUnrealMCPCommonUtils::GetVectorFromJson(Params, TEXT("location"));
        HasLocation = true;
    }

    // Get distance
    float Distance = 1000.0f;
    if (Params->HasField(TEXT("distance")))
    {
        Distance = Params->GetNumberField(TEXT("distance"));
    }

    // Get orientation if provided
    FRotator Orientation(0.0f, 0.0f, 0.0f);
    bool HasOrientation = false;
    if (Params->HasField(TEXT("orientation")))
    {
        Orientation = FUnrealMCPCommonUtils::GetRotatorFromJson(Params, TEXT("orientation"));
        HasOrientation = true;
    }

    // Get the active viewport
    FLevelEditorViewportClient* ViewportClient = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
    if (!ViewportClient)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to get active viewport"));
    }

    // If we have a target actor, focus on it
    if (HasTargetActor)
    {
        // Find the actor
        AActor* TargetActor = nullptr;
        TArray<AActor*> AllActors;
        UGameplayStatics::GetAllActorsOfClass(GWorld, AActor::StaticClass(), AllActors);
        
        for (AActor* Actor : AllActors)
        {
            if (Actor && Actor->GetName() == TargetActorName)
            {
                TargetActor = Actor;
                break;
            }
        }

        if (!TargetActor)
        {
            return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Actor not found: %s"), *TargetActorName));
        }

        // Focus on the actor
        ViewportClient->SetViewLocation(TargetActor->GetActorLocation() - FVector(Distance, 0.0f, 0.0f));
    }
    // Otherwise use the provided location
    else if (HasLocation)
    {
        ViewportClient->SetViewLocation(Location - FVector(Distance, 0.0f, 0.0f));
    }
    else
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Either 'target' or 'location' must be provided"));
    }

    // Set orientation if provided
    if (HasOrientation)
    {
        ViewportClient->SetViewRotation(Orientation);
    }

    // Force viewport to redraw
    ViewportClient->Invalidate();

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetBoolField(TEXT("success"), true);
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleTakeScreenshot(const TSharedPtr<FJsonObject>& Params)
{
    // Get file path parameter
    FString FilePath;
    if (!Params->TryGetStringField(TEXT("filepath"), FilePath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'filepath' parameter"));
    }
    
    // Ensure the file path has a proper extension
    if (!FilePath.EndsWith(TEXT(".png")))
    {
        FilePath += TEXT(".png");
    }

    // Get the active viewport
    if (GEditor && GEditor->GetActiveViewport())
    {
        FViewport* Viewport = GEditor->GetActiveViewport();
        TArray<FColor> Bitmap;
        FIntRect ViewportRect(0, 0, Viewport->GetSizeXY().X, Viewport->GetSizeXY().Y);
        
        if (Viewport->ReadPixels(Bitmap, FReadSurfaceDataFlags(), ViewportRect))
        {
            TArray<uint8> CompressedBitmap;
            FImageUtils::CompressImageArray(Viewport->GetSizeXY().X, Viewport->GetSizeXY().Y, Bitmap, CompressedBitmap);
            
            if (FFileHelper::SaveArrayToFile(CompressedBitmap, *FilePath))
            {
                TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
                ResultObj->SetStringField(TEXT("filepath"), FilePath);
                return ResultObj;
            }
        }
    }
    
    return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to take screenshot"));
} 

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleOpenLevel(const TSharedPtr<FJsonObject>& Params)
{
    FString LevelName;
    if (!Params->TryGetStringField(TEXT("level_name"), LevelName) &&
        !Params->TryGetStringField(TEXT("level_path"), LevelName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'level_name' parameter"));
    }

    if (LevelName.IsEmpty())
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("'level_name' cannot be empty"));
    }

    FString Filename = LevelName;
    if (LevelName.StartsWith(TEXT("/Game/")))
    {
        Filename = FPackageName::LongPackageNameToFilename(LevelName, FPackageName::GetMapPackageExtension());
    }
    else if (!FPaths::FileExists(Filename))
    {
        FString PackageName = LevelName;
        if (!PackageName.StartsWith(TEXT("/")))
        {
            PackageName = FString::Printf(TEXT("/Game/%s"), *LevelName);
        }
        Filename = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetMapPackageExtension());
    }

    if (!FPaths::FileExists(Filename))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Level file not found: %s"), *Filename));
    }

    const bool bLoadAsTemplate = false;
    const bool bShowProgress = false;
    const bool bOpened = FEditorFileUtils::LoadMap(Filename, bLoadAsTemplate, bShowProgress);
    if (!bOpened)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Failed to open level: %s"), *LevelName));
    }

    UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("level_name"), LevelName);
    ResultObj->SetStringField(TEXT("filename"), Filename);
    if (World)
    {
        ResultObj->SetStringField(TEXT("current_world"), World->GetName());
    }
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPEditorCommands::HandleSaveCurrentLevel(const TSharedPtr<FJsonObject>& Params)
{
    UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
    if (!World || !World->PersistentLevel)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to get current editor level"));
    }

    const bool bSaved = FEditorFileUtils::SaveLevel(World->PersistentLevel);
    if (!bSaved)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Failed to save level: %s"), *World->GetName()));
    }

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("world"), World->GetName());
    if (UPackage* Package = World->GetOutermost())
    {
        ResultObj->SetStringField(TEXT("package"), Package->GetName());
    }
    ResultObj->SetBoolField(TEXT("saved"), true);
    return ResultObj;
}
