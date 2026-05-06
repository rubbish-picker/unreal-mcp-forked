# Unreal MCP Editor Tools

This document provides detailed information about the editor tools available in the Unreal MCP integration.

## Overview

Editor tools allow you to control the Unreal Editor viewport and other editor functionality through MCP commands. These tools are particularly useful for automating tasks like focusing the camera on specific actors or locations.

## Editor Tools

### list_unreal_mcp_commands

List the commands supported by the Unreal-side MCP bridge. Use this first when chat context is lost, when the Python MCP tool list may be stale, or after adding new plugin commands.

**Returns:**
- `commands` - Array of command names, categories, and descriptions
- `command_count` - Number of registered Unreal-side commands

### inspect_object_properties

Inspect Details-style properties for an object, actor, actor component, or Blueprint class default object.

**Parameters:**
- `object_path` (string, optional) - Full object path to inspect
- `actor_name` (string, optional) - Actor name in the current level
- `component_name` (string, optional) - Component name when inspecting an actor or Blueprint component
- `blueprint_name` (string, optional) - Blueprint asset name or path; inspects the CDO unless `component_name` is supplied
- `max_properties` (integer, optional) - Maximum number of properties to return

### inspect_selected_objects

Inspect the objects currently selected in the editor.

**Parameters:**
- `max_properties` (integer, optional) - Maximum number of properties per object

### inspect_component_collision

Inspect collision settings for primitive components on an actor.

**Parameters:**
- `actor_name` (string) - Actor name in the current level
- `component_name` (string, optional) - Primitive component name; omitted returns all primitive components

### find_asset_references

Find package referencers and dependencies for an asset.

**Parameters:**
- `asset_path` (string) - Asset package path such as `/Game/Project101/BluePrint/BP_Door`

### list_assets_by_class

List assets by Unreal class, path, and optional name substring. Useful before choosing a mesh, material, animation, or Blueprint asset.

**Parameters:**
- `class_name` (string) - Class name such as `StaticMesh`, `SkeletalMesh`, `Material`, or `MaterialInstanceConstant`
- `path` (string, optional) - Package path to search, default `/Game`
- `name_contains` (string, optional) - Optional asset-name substring filter
- `max_results` (integer, optional) - Maximum number of assets to return

### spawn_mesh_actor_from_asset

Spawn a `StaticMeshActor` or `SkeletalMeshActor` from a mesh asset.

**Parameters:**
- `asset_path` (string) - StaticMesh or SkeletalMesh asset path
- `actor_name` (string, optional) - New actor name
- `location`, `rotation`, `scale` (arrays, optional) - Actor transform

### set_actor_mesh_asset

Set a StaticMesh or SkeletalMesh asset on an actor mesh component.

**Parameters:**
- `actor_name` (string) - Actor name
- `asset_path` (string) - StaticMesh or SkeletalMesh asset path
- `component_name` (string, optional) - Mesh component name

### set_actor_component_material

Set a material slot on an actor mesh component.

**Parameters:**
- `actor_name` (string) - Actor name
- `material_path` (string) - Material or material instance path
- `component_name` (string, optional) - Mesh component name
- `material_index` (integer, optional) - Material slot index

### set_material_parameter

Set scalar or vector parameters on a `Material` or `MaterialInstanceConstant`.

**Parameters:**
- `material_path` (string) - Material asset path
- `parameter_name` (string) - Existing parameter name, for example `Metallic`
- `parameter_type` (string, optional) - `scalar` or `vector`
- `value` (number or array) - Scalar number or `[R,G,B,A]`

### open_level

Open a level in the editor.

**Parameters:**
- `level_name` (string) - Level package path such as `/Game/Project101/Maps/Lv1`, or a map filename

### save_current_level

Save the currently open editor level.

### focus_viewport

Focus the viewport on a specific actor or location.

**Parameters:**
- `target` (string, optional) - Name of the actor to focus on (if provided, location is ignored)
- `location` (array, optional) - [X, Y, Z] coordinates to focus on (used if target is None)
- `distance` (float, optional) - Distance from the target/location (default: 1000.0)
- `orientation` (array, optional) - [Pitch, Yaw, Roll] for the viewport camera

**Returns:**
- Response from Unreal Engine containing the result of the focus operation

**Example:**
```json
{
  "command": "focus_viewport",
  "params": {
    "target": "PlayerStart",
    "distance": 500,
    "orientation": [0, 180, 0]
  }
}
```

### take_screenshot

Capture a screenshot of the viewport.

**Parameters:**
- `filename` (string, optional) - Name of the file to save the screenshot as (default: "screenshot.png")
- `show_ui` (boolean, optional) - Whether to include UI elements in the screenshot (default: false)
- `resolution` (array, optional) - [Width, Height] for the screenshot

**Returns:**
- Result of the screenshot operation

**Example:**
```json
{
  "command": "take_screenshot",
  "params": {
    "filename": "my_scene.png",
    "show_ui": false,
    "resolution": [1920, 1080]
  }
}
```

## Error Handling

All command responses include a "status" field indicating whether the operation succeeded, and an optional "message" field with details in case of failure.

```json
{
  "status": "error",
  "message": "Failed to get active viewport"
}
```

## Usage Examples

### Python Example

```python
from unreal_mcp_server import get_unreal_connection

# Get connection to Unreal Engine
unreal = get_unreal_connection()

# Focus on a specific actor
focus_response = unreal.send_command("focus_viewport", {
    "target": "PlayerStart",
    "distance": 500,
    "orientation": [0, 180, 0]
})
print(focus_response)

# Take a screenshot
screenshot_response = unreal.send_command("take_screenshot", {"filename": "my_scene.png"})
print(screenshot_response)
```

## Troubleshooting

- **Command fails with "Failed to get active viewport"**: Make sure Unreal Editor is running and has an active viewport.
- **Actor not found**: Verify that the actor name is correct and the actor exists in the current level.
- **Invalid parameters**: Ensure that location and orientation arrays contain exactly 3 values (X, Y, Z for location; Pitch, Yaw, Roll for orientation).

## Future Enhancements

- Support for setting viewport display mode (wireframe, lit, etc.)
- Camera animation paths for cinematic viewport control
- Support for multiple viewports
