# Unreal MCP Project Tools

Project tools edit project-wide settings such as input mappings.

## create_input_mapping

Create an action or axis input mapping.

**Parameters:**
- `action_name` (string) - Action or axis name
- `key` (string) - Key name, for example `E`, `MouseX`, or `MouseY`
- `input_type` (string, optional) - `Action` or `Axis`
- `scale` (number, optional) - Axis scale, commonly `1.0` or `-1.0`

## inspect_input_mappings

List project action and axis input mappings.

## remove_input_mapping

Remove action or axis input mappings by name and optional key.

**Parameters:**
- `action_name` (string) - Action or axis name
- `key` (string, optional) - Key to remove; omitted removes all mappings with that name
- `input_type` (string, optional) - `Action` or `Axis`
