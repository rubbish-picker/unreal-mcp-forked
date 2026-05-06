#include "Commands/UnrealMCPEditorCommands.h"
#include "Commands/UnrealMCPCommonUtils.h"
#include "Editor.h"
#include "EditorViewportClient.h"
#include "LevelEditorViewport.h"
#include "ImageUtils.h"
#include "HighResScreenshot.h"
#include "Engine/GameViewportClient.h"
#include "Misc/FileHelper.h"
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
#include "Factories/TextureFactory.h"
#include "UObject/ObjectRedirector.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Texture2D.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInterface.h"

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
    else if (CommandType == TEXT("set_material_parameter"))
    {
        return HandleSetMaterialParameter(Params);
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
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Unknown actor type: %s"), *ActorType));
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

    Asset->MarkPackageDirty();
    UEditorAssetLibrary::SaveLoadedAsset(Asset, false);

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetStringField(TEXT("material_path"), MaterialPath);
    ResultObj->SetStringField(TEXT("parameter_name"), ParameterName);
    ResultObj->SetStringField(TEXT("parameter_type"), ParameterType);
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
