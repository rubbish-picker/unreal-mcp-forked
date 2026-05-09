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

### create_content_folder

Create a Content Browser folder under `/Game`.

**Parameters:**
- `folder_path` (string) - Folder path, for example `/Game/MCP_Test`

### duplicate_asset

Duplicate an asset to a destination path.

**Parameters:**
- `source_asset_path` (string) - Source asset path
- `destination_asset_path` (string) - Destination asset path

### rename_asset

Rename or move an asset.

**Parameters:**
- `source_asset_path` (string) - Source asset path
- `destination_asset_path` (string) - Destination asset path

### delete_asset

Delete an asset.

**Parameters:**
- `asset_path` (string) - Asset path to delete

### save_asset

Save one loaded asset.

**Parameters:**
- `asset_path` (string) - Asset path to save
- `only_if_dirty` (boolean, optional) - Save only when dirty

### fixup_redirectors

Fix redirectors under a Content Browser folder.

**Parameters:**
- `folder_path` (string, optional) - Folder path, default `/Game`
- `recursive` (boolean, optional) - Include child folders

### import_asset

Import one local file into the Content Browser.

**Parameters:**
- `source_file` (string) - Absolute local file path to import
- `destination_path` (string, optional) - Content Browser folder, default `/Game`
- `destination_name` (string, optional) - Asset name override
- `replace_existing` (boolean, optional) - Replace an existing asset
- `save` (boolean, optional) - Save after import

### import_assets_batch

Import multiple local files into the Content Browser.

**Parameters:**
- `assets` (array) - Import specs using the same fields as `import_asset`

### inspect_import_options

Report import factories and supported extensions for a source file or extension.

**Parameters:**
- `source_file` (string, optional) - Local file path to probe
- `extension` (string, optional) - Extension to probe, for example `png`

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

### set_actor_component_property

Set a property on an actor component instance in the current level.

**Parameters:**
- `actor_name` (string) - Actor name
- `component_name` (string) - Component name on that actor
- `property_name` (string) - Property to set
- `property_value` (any) - New property value

### set_actor_component_material

Set a material slot on an actor mesh component.

**Parameters:**
- `actor_name` (string) - Actor name
- `material_path` (string) - Material or material instance path
- `component_name` (string, optional) - Mesh component name
- `material_index` (integer, optional) - Material slot index

### create_material

Create a Material asset.

**Parameters:**
- `asset_path` (string) - New material path under `/Game`
- `save` (boolean, optional) - Save after creation

### create_material_instance

Create a MaterialInstanceConstant from a parent material.

**Parameters:**
- `asset_path` (string) - New material instance path under `/Game`
- `parent_material_path` (string) - Parent Material or MaterialInstance path
- `save` (boolean, optional) - Save after creation

### inspect_material_parameters

List scalar, vector, texture, and static switch material parameters.

**Parameters:**
- `material_path` (string) - Material or MaterialInstance path

### set_material_parameter

Set scalar or vector parameters on a `Material` or `MaterialInstanceConstant`.

**Parameters:**
- `material_path` (string) - Material asset path
- `parameter_name` (string) - Existing parameter name, for example `Metallic`
- `parameter_type` (string, optional) - `scalar` or `vector`
- `value` (number or array) - Scalar number or `[R,G,B,A]`

### set_material_texture_parameter

Set a texture parameter on a Material or MaterialInstanceConstant.

**Parameters:**
- `material_path` (string) - Material or MaterialInstance path
- `parameter_name` (string) - Texture parameter name
- `texture_path` (string) - Texture asset path

### set_material_static_switch_parameter

Set a static switch parameter on a Material or MaterialInstance.

**Parameters:**
- `material_path` (string) - Material or MaterialInstance path
- `parameter_name` (string) - Static switch parameter name
- `value` (boolean) - Switch value

### inspect_material_expressions

Inspect material graph expression nodes, pins, and links.

**Parameters:**
- `material_path` (string) - Raw Material asset path

### add_material_expression_node

Add a low-level material graph expression node.

**Parameters:**
- `material_path` (string) - Raw Material asset path
- `expression_type` (string) - Expression type such as `ScalarParameter`, `VectorParameter`, `TextureSampleParameter2D`, `StaticSwitchParameter`, `Constant`, `Multiply`, `Add`, `Lerp`, or `TextureCoordinate`
- `parameter_name` (string, optional) - Parameter name for parameter expressions
- `default_value` (any, optional) - Default parameter value
- `value` (any, optional) - Constant value or alternate default value
- `texture_path` (string, optional) - Texture asset for texture parameter nodes
- `node_x` (integer, optional) - Material editor X position
- `node_y` (integer, optional) - Material editor Y position
- `description` (string, optional) - Node description
- `recompile` (boolean, optional) - Recompile after edit

### connect_material_expression_to_property

Connect a material expression output to a material property input.

**Parameters:**
- `material_path` (string) - Raw Material asset path
- `expression_id` (string) - Expression GUID returned by `add_material_expression_node` or `inspect_material_expressions`
- `property_name` (string) - Property such as `BaseColor`, `Metallic`, `Roughness`, `EmissiveColor`, `Opacity`, `OpacityMask`, `Normal`, `WorldPositionOffset`, or `AmbientOcclusion`
- `output_name` (string, optional) - Output pin name; empty uses the first output
- `recompile` (boolean, optional) - Recompile after edit

### connect_material_expressions

Connect two material expression nodes.

**Parameters:**
- `material_path` (string) - Raw Material asset path
- `from_expression_id` (string) - Source expression GUID
- `to_expression_id` (string) - Target expression GUID
- `from_output_name` (string, optional) - Source output pin; empty uses first output
- `to_input_name` (string, optional) - Target input pin; empty uses first input
- `recompile` (boolean, optional) - Recompile after edit

### recompile_material

Recompile and save a Material asset after graph edits.

**Parameters:**
- `material_path` (string) - Raw Material asset path

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
