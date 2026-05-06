#include "UnrealMCPBridge.h"
#include "MCPServerRunnable.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "HAL/RunnableThread.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "Camera/CameraActor.h"
#include "EditorAssetLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "JsonObjectConverter.h"
#include "GameFramework/Actor.h"
#include "Engine/Selection.h"
#include "Kismet/GameplayStatics.h"
#include "Async/Async.h"
// Add Blueprint related includes
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Factories/BlueprintFactory.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_Event.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
// UE5.5 correct includes
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "UObject/Field.h"
#include "UObject/FieldPath.h"
// Blueprint Graph specific includes
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "K2Node_CallFunction.h"
#include "K2Node_InputAction.h"
#include "K2Node_Self.h"
#include "GameFramework/InputSettings.h"
#include "EditorSubsystem.h"
#include "Subsystems/EditorActorSubsystem.h"
// Include our new command handler classes
#include "Commands/UnrealMCPEditorCommands.h"
#include "Commands/UnrealMCPBlueprintCommands.h"
#include "Commands/UnrealMCPBlueprintNodeCommands.h"
#include "Commands/UnrealMCPProjectCommands.h"
#include "Commands/UnrealMCPCommonUtils.h"
#include "Commands/UnrealMCPUMGCommands.h"

// Default settings
#define MCP_SERVER_HOST "127.0.0.1"
#define MCP_SERVER_PORT 55557

UUnrealMCPBridge::UUnrealMCPBridge()
{
    EditorCommands = MakeShared<FUnrealMCPEditorCommands>();
    BlueprintCommands = MakeShared<FUnrealMCPBlueprintCommands>();
    BlueprintNodeCommands = MakeShared<FUnrealMCPBlueprintNodeCommands>();
    ProjectCommands = MakeShared<FUnrealMCPProjectCommands>();
    UMGCommands = MakeShared<FUnrealMCPUMGCommands>();
}

UUnrealMCPBridge::~UUnrealMCPBridge()
{
    EditorCommands.Reset();
    BlueprintCommands.Reset();
    BlueprintNodeCommands.Reset();
    ProjectCommands.Reset();
    UMGCommands.Reset();
}

TSharedPtr<FJsonObject> UUnrealMCPBridge::GetCommandCatalog() const
{
    auto MakeCommand = [](const FString& Name, const FString& Category, const FString& Description)
    {
        TSharedPtr<FJsonObject> CommandObj = MakeShared<FJsonObject>();
        CommandObj->SetStringField(TEXT("name"), Name);
        CommandObj->SetStringField(TEXT("category"), Category);
        CommandObj->SetStringField(TEXT("description"), Description);
        return MakeShared<FJsonValueObject>(CommandObj);
    };

    TArray<TSharedPtr<FJsonValue>> Commands;
    Commands.Add(MakeCommand(TEXT("ping"), TEXT("system"), TEXT("Check whether the Unreal MCP bridge is alive.")));
    Commands.Add(MakeCommand(TEXT("list_mcp_commands"), TEXT("system"), TEXT("Return the self-describing command catalog.")));

    Commands.Add(MakeCommand(TEXT("get_actors_in_level"), TEXT("editor"), TEXT("List actors in the current editor level.")));
    Commands.Add(MakeCommand(TEXT("find_actors_by_name"), TEXT("editor"), TEXT("Find actors by name substring.")));
    Commands.Add(MakeCommand(TEXT("spawn_actor"), TEXT("editor"), TEXT("Spawn a basic engine actor in the current level.")));
    Commands.Add(MakeCommand(TEXT("delete_actor"), TEXT("editor"), TEXT("Delete an actor by name.")));
    Commands.Add(MakeCommand(TEXT("set_actor_transform"), TEXT("editor"), TEXT("Set actor location, rotation, or scale.")));
    Commands.Add(MakeCommand(TEXT("get_actor_properties"), TEXT("editor"), TEXT("Get detailed actor data.")));
    Commands.Add(MakeCommand(TEXT("set_actor_property"), TEXT("editor"), TEXT("Set a property on a level actor.")));
    Commands.Add(MakeCommand(TEXT("inspect_object_properties"), TEXT("editor"), TEXT("Inspect Details-style properties for an object, actor, component, or Blueprint CDO.")));
    Commands.Add(MakeCommand(TEXT("inspect_selected_objects"), TEXT("editor"), TEXT("Inspect currently selected editor objects.")));
    Commands.Add(MakeCommand(TEXT("inspect_component_collision"), TEXT("editor"), TEXT("Inspect primitive component collision settings on an actor.")));
    Commands.Add(MakeCommand(TEXT("find_asset_references"), TEXT("editor"), TEXT("Find asset referencers and dependencies.")));
    Commands.Add(MakeCommand(TEXT("list_assets_by_class"), TEXT("editor"), TEXT("List assets by class, path, and optional name substring.")));
    Commands.Add(MakeCommand(TEXT("create_content_folder"), TEXT("editor"), TEXT("Create a Content Browser folder under /Game.")));
    Commands.Add(MakeCommand(TEXT("duplicate_asset"), TEXT("editor"), TEXT("Duplicate an asset to a destination path.")));
    Commands.Add(MakeCommand(TEXT("rename_asset"), TEXT("editor"), TEXT("Rename or move an asset.")));
    Commands.Add(MakeCommand(TEXT("delete_asset"), TEXT("editor"), TEXT("Delete an asset.")));
    Commands.Add(MakeCommand(TEXT("save_asset"), TEXT("editor"), TEXT("Save one loaded asset.")));
    Commands.Add(MakeCommand(TEXT("fixup_redirectors"), TEXT("editor"), TEXT("Fix redirectors under a Content Browser folder.")));
    Commands.Add(MakeCommand(TEXT("import_asset"), TEXT("editor"), TEXT("Import one local file into the Content Browser.")));
    Commands.Add(MakeCommand(TEXT("import_assets_batch"), TEXT("editor"), TEXT("Import multiple local files into the Content Browser.")));
    Commands.Add(MakeCommand(TEXT("spawn_mesh_actor_from_asset"), TEXT("editor"), TEXT("Spawn a StaticMeshActor or SkeletalMeshActor from a mesh asset.")));
    Commands.Add(MakeCommand(TEXT("set_actor_mesh_asset"), TEXT("editor"), TEXT("Set a static or skeletal mesh asset on an actor component.")));
    Commands.Add(MakeCommand(TEXT("set_actor_component_property"), TEXT("editor"), TEXT("Set a property on an actor component instance.")));
    Commands.Add(MakeCommand(TEXT("set_actor_component_material"), TEXT("editor"), TEXT("Set a material slot on an actor mesh component.")));
    Commands.Add(MakeCommand(TEXT("set_material_parameter"), TEXT("editor"), TEXT("Set scalar or vector parameters on a Material or MaterialInstanceConstant.")));
    Commands.Add(MakeCommand(TEXT("spawn_blueprint_actor"), TEXT("editor"), TEXT("Spawn an actor from a Blueprint.")));
    Commands.Add(MakeCommand(TEXT("open_level"), TEXT("editor"), TEXT("Open an editor level by package path or filename.")));
    Commands.Add(MakeCommand(TEXT("save_current_level"), TEXT("editor"), TEXT("Save the currently open editor level.")));
    Commands.Add(MakeCommand(TEXT("focus_viewport"), TEXT("editor"), TEXT("Focus the active viewport.")));
    Commands.Add(MakeCommand(TEXT("take_screenshot"), TEXT("editor"), TEXT("Capture the active viewport to a PNG file.")));

    Commands.Add(MakeCommand(TEXT("create_blueprint"), TEXT("blueprint"), TEXT("Create a Blueprint asset.")));
    Commands.Add(MakeCommand(TEXT("add_component_to_blueprint"), TEXT("blueprint"), TEXT("Add a component template to a Blueprint.")));
    Commands.Add(MakeCommand(TEXT("set_component_property"), TEXT("blueprint"), TEXT("Set a Blueprint component template property.")));
    Commands.Add(MakeCommand(TEXT("set_physics_properties"), TEXT("blueprint"), TEXT("Set common physics properties on a component template.")));
    Commands.Add(MakeCommand(TEXT("compile_blueprint"), TEXT("blueprint"), TEXT("Compile a Blueprint.")));
    Commands.Add(MakeCommand(TEXT("set_blueprint_property"), TEXT("blueprint"), TEXT("Set a Blueprint class default property.")));
    Commands.Add(MakeCommand(TEXT("set_static_mesh_properties"), TEXT("blueprint"), TEXT("Set static mesh asset on a component template.")));
    Commands.Add(MakeCommand(TEXT("set_skeletal_mesh_properties"), TEXT("blueprint"), TEXT("Set skeletal mesh asset on a component template.")));
    Commands.Add(MakeCommand(TEXT("set_blueprint_component_material"), TEXT("blueprint"), TEXT("Set a material slot on a Blueprint mesh component template.")));
    Commands.Add(MakeCommand(TEXT("attach_blueprint_component"), TEXT("blueprint"), TEXT("Attach one Blueprint scene component template under another.")));
    Commands.Add(MakeCommand(TEXT("set_pawn_properties"), TEXT("blueprint"), TEXT("Set common Pawn defaults.")));
    Commands.Add(MakeCommand(TEXT("inspect_blueprint_components"), TEXT("blueprint"), TEXT("Inspect Blueprint component tree, template properties, and collision.")));
    Commands.Add(MakeCommand(TEXT("inspect_blueprint_defaults"), TEXT("blueprint"), TEXT("Inspect Blueprint CDO defaults.")));
    Commands.Add(MakeCommand(TEXT("inspect_blueprint_timelines"), TEXT("blueprint"), TEXT("Inspect Timeline templates, tracks, curves, and keyframes.")));

    Commands.Add(MakeCommand(TEXT("connect_blueprint_nodes"), TEXT("blueprint_node"), TEXT("Connect two Blueprint graph pins.")));
    Commands.Add(MakeCommand(TEXT("add_blueprint_get_self_component_reference"), TEXT("blueprint_node"), TEXT("Add a self component reference node.")));
    Commands.Add(MakeCommand(TEXT("add_blueprint_self_reference"), TEXT("blueprint_node"), TEXT("Add a self reference node.")));
    Commands.Add(MakeCommand(TEXT("find_blueprint_nodes"), TEXT("blueprint_node"), TEXT("Find Blueprint nodes by type and event name.")));
    Commands.Add(MakeCommand(TEXT("inspect_blueprint_graph"), TEXT("blueprint_node"), TEXT("Inspect Blueprint graph nodes, pins, and links.")));
    Commands.Add(MakeCommand(TEXT("remove_blueprint_nodes"), TEXT("blueprint_node"), TEXT("Remove Blueprint graph nodes by GUID.")));
    Commands.Add(MakeCommand(TEXT("add_blueprint_event_node"), TEXT("blueprint_node"), TEXT("Add a Blueprint event node.")));
    Commands.Add(MakeCommand(TEXT("add_blueprint_input_action_node"), TEXT("blueprint_node"), TEXT("Add an input action event node.")));
    Commands.Add(MakeCommand(TEXT("add_blueprint_function_node"), TEXT("blueprint_node"), TEXT("Add a Blueprint function call node.")));
    Commands.Add(MakeCommand(TEXT("add_blueprint_variable"), TEXT("blueprint_node"), TEXT("Add a Blueprint member variable.")));
    Commands.Add(MakeCommand(TEXT("remove_blueprint_component_and_linked_nodes"), TEXT("blueprint_node"), TEXT("Remove a component template and directly linked generated nodes.")));
    Commands.Add(MakeCommand(TEXT("remove_blueprint_actor_overlap_nodes"), TEXT("blueprint_node"), TEXT("Remove legacy actor overlap door chains.")));

    Commands.Add(MakeCommand(TEXT("create_input_mapping"), TEXT("project"), TEXT("Create an action or axis input mapping.")));
    Commands.Add(MakeCommand(TEXT("remove_input_mapping"), TEXT("project"), TEXT("Remove action or axis input mappings by name and optional key.")));
    Commands.Add(MakeCommand(TEXT("inspect_input_mappings"), TEXT("project"), TEXT("List action and axis input mappings.")));

    Commands.Add(MakeCommand(TEXT("create_umg_widget_blueprint"), TEXT("umg"), TEXT("Create a Widget Blueprint.")));
    Commands.Add(MakeCommand(TEXT("add_text_block_to_widget"), TEXT("umg"), TEXT("Add a TextBlock widget.")));
    Commands.Add(MakeCommand(TEXT("add_button_to_widget"), TEXT("umg"), TEXT("Add a Button widget.")));
    Commands.Add(MakeCommand(TEXT("bind_widget_event"), TEXT("umg"), TEXT("Bind a widget event.")));
    Commands.Add(MakeCommand(TEXT("set_text_block_binding"), TEXT("umg"), TEXT("Bind a TextBlock property.")));
    Commands.Add(MakeCommand(TEXT("add_widget_to_viewport"), TEXT("umg"), TEXT("Add a widget instance to the viewport.")));

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetArrayField(TEXT("commands"), Commands);
    ResultObj->SetNumberField(TEXT("command_count"), Commands.Num());
    ResultObj->SetStringField(TEXT("note"), TEXT("This catalog is returned by the Unreal plugin itself, so it survives lost chat context."));
    return ResultObj;
}

// Initialize subsystem
void UUnrealMCPBridge::Initialize(FSubsystemCollectionBase& Collection)
{
    UE_LOG(LogTemp, Display, TEXT("UnrealMCPBridge: Initializing"));
    
    bIsRunning = false;
    ListenerSocket = nullptr;
    ConnectionSocket = nullptr;
    ServerThread = nullptr;
    Port = MCP_SERVER_PORT;
    FIPv4Address::Parse(MCP_SERVER_HOST, ServerAddress);

    // Start the server automatically
    StartServer();
}

// Clean up resources when subsystem is destroyed
void UUnrealMCPBridge::Deinitialize()
{
    UE_LOG(LogTemp, Display, TEXT("UnrealMCPBridge: Shutting down"));
    StopServer();
}

// Start the MCP server
void UUnrealMCPBridge::StartServer()
{
    if (bIsRunning)
    {
        UE_LOG(LogTemp, Warning, TEXT("UnrealMCPBridge: Server is already running"));
        return;
    }

    // Create socket subsystem
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    if (!SocketSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("UnrealMCPBridge: Failed to get socket subsystem"));
        return;
    }

    // Create listener socket
    TSharedPtr<FSocket> NewListenerSocket = MakeShareable(SocketSubsystem->CreateSocket(NAME_Stream, TEXT("UnrealMCPListener"), false));
    if (!NewListenerSocket.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("UnrealMCPBridge: Failed to create listener socket"));
        return;
    }

    // Allow address reuse for quick restarts
    NewListenerSocket->SetReuseAddr(true);
    NewListenerSocket->SetNonBlocking(true);

    // Bind to address
    FIPv4Endpoint Endpoint(ServerAddress, Port);
    if (!NewListenerSocket->Bind(*Endpoint.ToInternetAddr()))
    {
        UE_LOG(LogTemp, Error, TEXT("UnrealMCPBridge: Failed to bind listener socket to %s:%d"), *ServerAddress.ToString(), Port);
        return;
    }

    // Start listening
    if (!NewListenerSocket->Listen(5))
    {
        UE_LOG(LogTemp, Error, TEXT("UnrealMCPBridge: Failed to start listening"));
        return;
    }

    ListenerSocket = NewListenerSocket;
    bIsRunning = true;
    UE_LOG(LogTemp, Display, TEXT("UnrealMCPBridge: Server started on %s:%d"), *ServerAddress.ToString(), Port);

    // Start server thread
    ServerThread = FRunnableThread::Create(
        new FMCPServerRunnable(this, ListenerSocket),
        TEXT("UnrealMCPServerThread"),
        0, TPri_Normal
    );

    if (!ServerThread)
    {
        UE_LOG(LogTemp, Error, TEXT("UnrealMCPBridge: Failed to create server thread"));
        StopServer();
        return;
    }
}

// Stop the MCP server
void UUnrealMCPBridge::StopServer()
{
    if (!bIsRunning)
    {
        return;
    }

    bIsRunning = false;

    // Clean up thread
    if (ServerThread)
    {
        ServerThread->Kill(true);
        delete ServerThread;
        ServerThread = nullptr;
    }

    // Close sockets
    if (ConnectionSocket.IsValid())
    {
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket.Get());
        ConnectionSocket.Reset();
    }

    if (ListenerSocket.IsValid())
    {
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenerSocket.Get());
        ListenerSocket.Reset();
    }

    UE_LOG(LogTemp, Display, TEXT("UnrealMCPBridge: Server stopped"));
}

// Execute a command received from a client
FString UUnrealMCPBridge::ExecuteCommand(const FString& CommandType, const TSharedPtr<FJsonObject>& Params)
{
    UE_LOG(LogTemp, Display, TEXT("UnrealMCPBridge: Executing command: %s"), *CommandType);
    
    // Create a promise to wait for the result
    TPromise<FString> Promise;
    TFuture<FString> Future = Promise.GetFuture();
    
    // Queue execution on Game Thread
    AsyncTask(ENamedThreads::GameThread, [this, CommandType, Params, Promise = MoveTemp(Promise)]() mutable
    {
        TSharedPtr<FJsonObject> ResponseJson = MakeShareable(new FJsonObject);
        
        try
        {
            TSharedPtr<FJsonObject> ResultJson;
            
            if (CommandType == TEXT("ping"))
            {
                ResultJson = MakeShareable(new FJsonObject);
                ResultJson->SetStringField(TEXT("message"), TEXT("pong"));
            }
            else if (CommandType == TEXT("list_mcp_commands"))
            {
                ResultJson = GetCommandCatalog();
            }
            // Editor Commands (including actor manipulation)
            else if (CommandType == TEXT("get_actors_in_level") || 
                     CommandType == TEXT("find_actors_by_name") ||
                     CommandType == TEXT("spawn_actor") ||
                     CommandType == TEXT("create_actor") ||
                     CommandType == TEXT("delete_actor") || 
                     CommandType == TEXT("set_actor_transform") ||
                     CommandType == TEXT("get_actor_properties") ||
                     CommandType == TEXT("set_actor_property") ||
                     CommandType == TEXT("inspect_object_properties") ||
                     CommandType == TEXT("inspect_selected_objects") ||
                     CommandType == TEXT("inspect_component_collision") ||
                     CommandType == TEXT("find_asset_references") ||
                     CommandType == TEXT("list_assets_by_class") ||
                     CommandType == TEXT("create_content_folder") ||
                     CommandType == TEXT("duplicate_asset") ||
                     CommandType == TEXT("rename_asset") ||
                     CommandType == TEXT("delete_asset") ||
                     CommandType == TEXT("save_asset") ||
                     CommandType == TEXT("fixup_redirectors") ||
                     CommandType == TEXT("import_asset") ||
                     CommandType == TEXT("import_assets_batch") ||
                     CommandType == TEXT("spawn_mesh_actor_from_asset") ||
                     CommandType == TEXT("set_actor_mesh_asset") ||
                     CommandType == TEXT("set_actor_component_property") ||
                     CommandType == TEXT("set_actor_component_material") ||
                     CommandType == TEXT("set_material_parameter") ||
                     CommandType == TEXT("spawn_blueprint_actor") ||
                     CommandType == TEXT("open_level") ||
                     CommandType == TEXT("save_current_level") ||
                     CommandType == TEXT("focus_viewport") || 
                     CommandType == TEXT("take_screenshot"))
            {
                ResultJson = EditorCommands->HandleCommand(CommandType, Params);
            }
            // Blueprint Commands
            else if (CommandType == TEXT("create_blueprint") || 
                     CommandType == TEXT("add_component_to_blueprint") || 
                     CommandType == TEXT("set_component_property") || 
                     CommandType == TEXT("set_physics_properties") || 
                     CommandType == TEXT("compile_blueprint") || 
                     CommandType == TEXT("set_blueprint_property") || 
                     CommandType == TEXT("set_static_mesh_properties") ||
                     CommandType == TEXT("set_skeletal_mesh_properties") ||
                     CommandType == TEXT("set_blueprint_component_material") ||
                     CommandType == TEXT("attach_blueprint_component") ||
                     CommandType == TEXT("set_pawn_properties") ||
                     CommandType == TEXT("inspect_blueprint_components") ||
                     CommandType == TEXT("inspect_blueprint_defaults") ||
                     CommandType == TEXT("inspect_blueprint_timelines"))
            {
                ResultJson = BlueprintCommands->HandleCommand(CommandType, Params);
            }
            // Blueprint Node Commands
            else if (CommandType == TEXT("connect_blueprint_nodes") || 
                     CommandType == TEXT("add_blueprint_get_self_component_reference") ||
                     CommandType == TEXT("add_blueprint_self_reference") ||
                     CommandType == TEXT("find_blueprint_nodes") ||
                     CommandType == TEXT("inspect_blueprint_graph") ||
                     CommandType == TEXT("remove_blueprint_nodes") ||
                     CommandType == TEXT("add_blueprint_event_node") ||
                     CommandType == TEXT("add_blueprint_input_action_node") ||
                     CommandType == TEXT("add_blueprint_function_node") ||
                     CommandType == TEXT("add_blueprint_get_component_node") ||
                     CommandType == TEXT("remove_blueprint_component_and_linked_nodes") ||
                     CommandType == TEXT("remove_blueprint_actor_overlap_nodes") ||
                     CommandType == TEXT("add_blueprint_variable"))
            {
                ResultJson = BlueprintNodeCommands->HandleCommand(CommandType, Params);
            }
            // Project Commands
            else if (CommandType == TEXT("create_input_mapping") ||
                     CommandType == TEXT("remove_input_mapping") ||
                     CommandType == TEXT("inspect_input_mappings"))
            {
                ResultJson = ProjectCommands->HandleCommand(CommandType, Params);
            }
            // UMG Commands
            else if (CommandType == TEXT("create_umg_widget_blueprint") ||
                     CommandType == TEXT("add_text_block_to_widget") ||
                     CommandType == TEXT("add_button_to_widget") ||
                     CommandType == TEXT("bind_widget_event") ||
                     CommandType == TEXT("set_text_block_binding") ||
                     CommandType == TEXT("add_widget_to_viewport"))
            {
                ResultJson = UMGCommands->HandleCommand(CommandType, Params);
            }
            else
            {
                ResponseJson->SetStringField(TEXT("status"), TEXT("error"));
                ResponseJson->SetStringField(TEXT("error"), FString::Printf(TEXT("Unknown command: %s"), *CommandType));
                
                FString ResultString;
                TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResultString);
                FJsonSerializer::Serialize(ResponseJson.ToSharedRef(), Writer);
                Promise.SetValue(ResultString);
                return;
            }
            
            // Check if the result contains an error
            bool bSuccess = true;
            FString ErrorMessage;
            
            if (ResultJson->HasField(TEXT("success")))
            {
                bSuccess = ResultJson->GetBoolField(TEXT("success"));
                if (!bSuccess && ResultJson->HasField(TEXT("error")))
                {
                    ErrorMessage = ResultJson->GetStringField(TEXT("error"));
                }
            }
            
            if (bSuccess)
            {
                // Set success status and include the result
                ResponseJson->SetStringField(TEXT("status"), TEXT("success"));
                ResponseJson->SetObjectField(TEXT("result"), ResultJson);
            }
            else
            {
                // Set error status and include the error message
                ResponseJson->SetStringField(TEXT("status"), TEXT("error"));
                ResponseJson->SetStringField(TEXT("error"), ErrorMessage);
            }
        }
        catch (const std::exception& e)
        {
            ResponseJson->SetStringField(TEXT("status"), TEXT("error"));
            ResponseJson->SetStringField(TEXT("error"), UTF8_TO_TCHAR(e.what()));
        }
        
        FString ResultString;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResultString);
        FJsonSerializer::Serialize(ResponseJson.ToSharedRef(), Writer);
        Promise.SetValue(ResultString);
    });
    
    return Future.Get();
}
