"""
Editor Tools for Unreal MCP.

This module provides tools for controlling the Unreal Editor viewport and other editor functionality.
"""

import logging
from typing import Dict, List, Any, Optional
from mcp.server.fastmcp import FastMCP, Context

# Get logger
logger = logging.getLogger("UnrealMCP")

def register_editor_tools(mcp: FastMCP):
    """Register editor tools with the MCP server."""

    @mcp.tool()
    def list_unreal_mcp_commands(ctx: Context) -> Dict[str, Any]:
        """
        List commands supported by the Unreal-side MCP bridge.

        Use this as the first recovery step when chat context is lost or when the
        Python MCP tool list may be stale after plugin changes.
        """
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                logger.error("Failed to connect to Unreal Engine")
                return {"success": False, "message": "Failed to connect to Unreal Engine"}

            response = unreal.send_command("list_mcp_commands", {})
            return response or {}

        except Exception as e:
            error_msg = f"Error listing Unreal MCP commands: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}
    
    @mcp.tool()
    def get_actors_in_level(ctx: Context) -> List[Dict[str, Any]]:
        """Get a list of all actors in the current level."""
        from unreal_mcp_server import get_unreal_connection
        
        try:
            unreal = get_unreal_connection()
            if not unreal:
                logger.warning("Failed to connect to Unreal Engine")
                return []
                
            response = unreal.send_command("get_actors_in_level", {})
            
            if not response:
                logger.warning("No response from Unreal Engine")
                return []
                
            # Log the complete response for debugging
            logger.info(f"Complete response from Unreal: {response}")
            
            # Check response format
            if "result" in response and "actors" in response["result"]:
                actors = response["result"]["actors"]
                logger.info(f"Found {len(actors)} actors in level")
                return actors
            elif "actors" in response:
                actors = response["actors"]
                logger.info(f"Found {len(actors)} actors in level")
                return actors
                
            logger.warning(f"Unexpected response format: {response}")
            return []
            
        except Exception as e:
            logger.error(f"Error getting actors: {e}")
            return []

    @mcp.tool()
    def find_actors_by_name(ctx: Context, pattern: str) -> List[str]:
        """Find actors by name pattern."""
        from unreal_mcp_server import get_unreal_connection
        
        try:
            unreal = get_unreal_connection()
            if not unreal:
                logger.warning("Failed to connect to Unreal Engine")
                return []
                
            response = unreal.send_command("find_actors_by_name", {
                "pattern": pattern
            })
            
            if not response:
                return []
                
            return response.get("actors", [])
            
        except Exception as e:
            logger.error(f"Error finding actors: {e}")
            return []
    
    @mcp.tool()
    def spawn_actor(
        ctx: Context,
        name: str,
        type: str,
        location: List[float] = [0.0, 0.0, 0.0],
        rotation: List[float] = [0.0, 0.0, 0.0]
    ) -> Dict[str, Any]:
        """Create a new actor in the current level.
        
        Args:
            ctx: The MCP context
            name: The name to give the new actor (must be unique)
            type: The type of actor to create (e.g. StaticMeshActor, PointLight)
            location: The [x, y, z] world location to spawn at
            rotation: The [pitch, yaw, roll] rotation in degrees
            
        Returns:
            Dict containing the created actor's properties
        """
        from unreal_mcp_server import get_unreal_connection
        
        try:
            unreal = get_unreal_connection()
            if not unreal:
                logger.error("Failed to connect to Unreal Engine")
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
            
            # Ensure all parameters are properly formatted
            params = {
                "name": name,
                "type": type.upper(),  # Make sure type is uppercase
                "location": location,
                "rotation": rotation
            }
            
            # Validate location and rotation formats
            for param_name in ["location", "rotation"]:
                param_value = params[param_name]
                if not isinstance(param_value, list) or len(param_value) != 3:
                    logger.error(f"Invalid {param_name} format: {param_value}. Must be a list of 3 float values.")
                    return {"success": False, "message": f"Invalid {param_name} format. Must be a list of 3 float values."}
                # Ensure all values are float
                params[param_name] = [float(val) for val in param_value]
            
            logger.info(f"Creating actor '{name}' of type '{type}' with params: {params}")
            response = unreal.send_command("spawn_actor", params)
            
            if not response:
                logger.error("No response from Unreal Engine")
                return {"success": False, "message": "No response from Unreal Engine"}
            
            # Log the complete response for debugging
            logger.info(f"Actor creation response: {response}")
            
            # Handle error responses correctly
            if response.get("status") == "error":
                error_message = response.get("error", "Unknown error")
                logger.error(f"Error creating actor: {error_message}")
                return {"success": False, "message": error_message}
            
            return response
            
        except Exception as e:
            error_msg = f"Error creating actor: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}
    
    @mcp.tool()
    def delete_actor(ctx: Context, name: str) -> Dict[str, Any]:
        """Delete an actor by name."""
        from unreal_mcp_server import get_unreal_connection
        
        try:
            unreal = get_unreal_connection()
            if not unreal:
                logger.error("Failed to connect to Unreal Engine")
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
                
            response = unreal.send_command("delete_actor", {
                "name": name
            })
            return response or {}
            
        except Exception as e:
            logger.error(f"Error deleting actor: {e}")
            return {}
    
    @mcp.tool()
    def set_actor_transform(
        ctx: Context,
        name: str,
        location: List[float]  = None,
        rotation: List[float]  = None,
        scale: List[float] = None
    ) -> Dict[str, Any]:
        """Set the transform of an actor."""
        from unreal_mcp_server import get_unreal_connection
        
        try:
            unreal = get_unreal_connection()
            if not unreal:
                logger.error("Failed to connect to Unreal Engine")
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
                
            params = {"name": name}
            if location is not None:
                params["location"] = location
            if rotation is not None:
                params["rotation"] = rotation
            if scale is not None:
                params["scale"] = scale
                
            response = unreal.send_command("set_actor_transform", params)
            return response or {}
            
        except Exception as e:
            logger.error(f"Error setting transform: {e}")
            return {}
    
    @mcp.tool()
    def get_actor_properties(ctx: Context, name: str) -> Dict[str, Any]:
        """Get all properties of an actor."""
        from unreal_mcp_server import get_unreal_connection
        
        try:
            unreal = get_unreal_connection()
            if not unreal:
                logger.error("Failed to connect to Unreal Engine")
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
                
            response = unreal.send_command("get_actor_properties", {
                "name": name
            })
            return response or {}
            
        except Exception as e:
            logger.error(f"Error getting properties: {e}")
            return {}

    @mcp.tool()
    def set_actor_property(
        ctx: Context,
        name: str,
        property_name: str,
        property_value,
    ) -> Dict[str, Any]:
        """
        Set a property on an actor.
        
        Args:
            name: Name of the actor
            property_name: Name of the property to set
            property_value: Value to set the property to
            
        Returns:
            Dict containing response from Unreal with operation status
        """
        from unreal_mcp_server import get_unreal_connection
        
        try:
            unreal = get_unreal_connection()
            if not unreal:
                logger.error("Failed to connect to Unreal Engine")
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
                
            response = unreal.send_command("set_actor_property", {
                "name": name,
                "property_name": property_name,
                "property_value": property_value
            })
            
            if not response:
                logger.error("No response from Unreal Engine")
                return {"success": False, "message": "No response from Unreal Engine"}
            
            logger.info(f"Set actor property response: {response}")
            return response
            
        except Exception as e:
            error_msg = f"Error setting actor property: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    # @mcp.tool() commented out because it's buggy
    def focus_viewport(
        ctx: Context,
        target: str = None,
        location: List[float] = None,
        distance: float = 1000.0,
        orientation: List[float] = None
    ) -> Dict[str, Any]:
        """
        Focus the viewport on a specific actor or location.
        
        Args:
            target: Name of the actor to focus on (if provided, location is ignored)
            location: [X, Y, Z] coordinates to focus on (used if target is None)
            distance: Distance from the target/location
            orientation: Optional [Pitch, Yaw, Roll] for the viewport camera
            
        Returns:
            Response from Unreal Engine
        """
        from unreal_mcp_server import get_unreal_connection
        
        try:
            unreal = get_unreal_connection()
            if not unreal:
                logger.error("Failed to connect to Unreal Engine")
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
                
            params = {}
            if target:
                params["target"] = target
            elif location:
                params["location"] = location
            
            if distance:
                params["distance"] = distance
                
            if orientation:
                params["orientation"] = orientation
                
            response = unreal.send_command("focus_viewport", params)
            return response or {}
            
        except Exception as e:
            logger.error(f"Error focusing viewport: {e}")
            return {"status": "error", "message": str(e)}

    @mcp.tool()
    def spawn_blueprint_actor(
        ctx: Context,
        blueprint_name: str,
        actor_name: str,
        location: List[float] = [0.0, 0.0, 0.0],
        rotation: List[float] = [0.0, 0.0, 0.0]
    ) -> Dict[str, Any]:
        """Spawn an actor from a Blueprint.
        
        Args:
            ctx: The MCP context
            blueprint_name: Name of the Blueprint to spawn from
            actor_name: Name to give the spawned actor
            location: The [x, y, z] world location to spawn at
            rotation: The [pitch, yaw, roll] rotation in degrees
            
        Returns:
            Dict containing the spawned actor's properties
        """
        from unreal_mcp_server import get_unreal_connection
        
        try:
            unreal = get_unreal_connection()
            if not unreal:
                logger.error("Failed to connect to Unreal Engine")
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
            
            # Ensure all parameters are properly formatted
            params = {
                "blueprint_name": blueprint_name,
                "actor_name": actor_name,
                "location": location or [0.0, 0.0, 0.0],
                "rotation": rotation or [0.0, 0.0, 0.0]
            }
            
            # Validate location and rotation formats
            for param_name in ["location", "rotation"]:
                param_value = params[param_name]
                if not isinstance(param_value, list) or len(param_value) != 3:
                    logger.error(f"Invalid {param_name} format: {param_value}. Must be a list of 3 float values.")
                    return {"success": False, "message": f"Invalid {param_name} format. Must be a list of 3 float values."}
                # Ensure all values are float
                params[param_name] = [float(val) for val in param_value]
            
            logger.info(f"Spawning blueprint actor with params: {params}")
            response = unreal.send_command("spawn_blueprint_actor", params)
            
            if not response:
                logger.error("No response from Unreal Engine")
                return {"success": False, "message": "No response from Unreal Engine"}
            
            logger.info(f"Spawn blueprint actor response: {response}")
            return response
            
        except Exception as e:
            error_msg = f"Error spawning blueprint actor: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def open_level(ctx: Context, level_name: str) -> Dict[str, Any]:
        """
        Open a level in the Unreal Editor.

        Args:
            level_name: Level asset path such as /Game/Maps/Lv1, or a map filename

        Returns:
            Response containing the opened level filename and current world
        """
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                logger.error("Failed to connect to Unreal Engine")
                return {"success": False, "message": "Failed to connect to Unreal Engine"}

            response = unreal.send_command("open_level", {
                "level_name": level_name
            })
            return response or {}

        except Exception as e:
            error_msg = f"Error opening level: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def save_current_level(ctx: Context) -> Dict[str, Any]:
        """
        Save the currently open editor level.

        Returns:
            Response containing the saved world and package
        """
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                logger.error("Failed to connect to Unreal Engine")
                return {"success": False, "message": "Failed to connect to Unreal Engine"}

            response = unreal.send_command("save_current_level", {})
            return response or {}

        except Exception as e:
            error_msg = f"Error saving current level: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def inspect_object_properties(
        ctx: Context,
        object_path: str = "",
        actor_name: str = "",
        component_name: str = "",
        blueprint_name: str = "",
        max_properties: int = 200
    ) -> Dict[str, Any]:
        """Inspect Details-style properties for an object, actor, component, or Blueprint CDO."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}

            response = unreal.send_command("inspect_object_properties", {
                "object_path": object_path,
                "actor_name": actor_name,
                "component_name": component_name,
                "blueprint_name": blueprint_name,
                "max_properties": max_properties
            })
            return response or {}

        except Exception as e:
            error_msg = f"Error inspecting object properties: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def inspect_selected_objects(ctx: Context, max_properties: int = 100) -> Dict[str, Any]:
        """Inspect currently selected editor objects and their Details-style properties."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}

            response = unreal.send_command("inspect_selected_objects", {
                "max_properties": max_properties
            })
            return response or {}

        except Exception as e:
            error_msg = f"Error inspecting selected objects: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def inspect_component_collision(
        ctx: Context,
        actor_name: str,
        component_name: str = ""
    ) -> Dict[str, Any]:
        """Inspect collision settings for primitive components on an actor."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}

            response = unreal.send_command("inspect_component_collision", {
                "actor_name": actor_name,
                "component_name": component_name
            })
            return response or {}

        except Exception as e:
            error_msg = f"Error inspecting component collision: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def find_asset_references(ctx: Context, asset_path: str) -> Dict[str, Any]:
        """Find package referencers and dependencies for an asset."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}

            response = unreal.send_command("find_asset_references", {
                "asset_path": asset_path
            })
            return response or {}

        except Exception as e:
            error_msg = f"Error finding asset references: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def list_assets_by_class(
        ctx: Context,
        class_name: str,
        path: str = "/Game",
        name_contains: str = "",
        max_results: int = 200
    ) -> Dict[str, Any]:
        """List assets by Unreal class name, such as StaticMesh, SkeletalMesh, or MaterialInstanceConstant."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}

            response = unreal.send_command("list_assets_by_class", {
                "class_name": class_name,
                "path": path,
                "name_contains": name_contains,
                "max_results": max_results
            })
            return response or {}

        except Exception as e:
            error_msg = f"Error listing assets: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def create_content_folder(ctx: Context, folder_path: str) -> Dict[str, Any]:
        """Create a Content Browser folder under /Game."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
            return unreal.send_command("create_content_folder", {"folder_path": folder_path}) or {}
        except Exception as e:
            error_msg = f"Error creating content folder: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def duplicate_asset(ctx: Context, source_asset_path: str, destination_asset_path: str) -> Dict[str, Any]:
        """Duplicate an asset to a destination path."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
            return unreal.send_command("duplicate_asset", {
                "source_asset_path": source_asset_path,
                "destination_asset_path": destination_asset_path
            }) or {}
        except Exception as e:
            error_msg = f"Error duplicating asset: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def rename_asset(ctx: Context, source_asset_path: str, destination_asset_path: str) -> Dict[str, Any]:
        """Rename or move an asset."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
            return unreal.send_command("rename_asset", {
                "source_asset_path": source_asset_path,
                "destination_asset_path": destination_asset_path
            }) or {}
        except Exception as e:
            error_msg = f"Error renaming asset: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def delete_asset(ctx: Context, asset_path: str) -> Dict[str, Any]:
        """Delete an asset."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
            return unreal.send_command("delete_asset", {"asset_path": asset_path}) or {}
        except Exception as e:
            error_msg = f"Error deleting asset: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def save_asset(ctx: Context, asset_path: str, only_if_dirty: bool = True) -> Dict[str, Any]:
        """Save one loaded asset."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
            return unreal.send_command("save_asset", {
                "asset_path": asset_path,
                "only_if_dirty": only_if_dirty
            }) or {}
        except Exception as e:
            error_msg = f"Error saving asset: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def fixup_redirectors(ctx: Context, folder_path: str = "/Game", recursive: bool = True) -> Dict[str, Any]:
        """Fix redirectors under a Content Browser folder."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
            return unreal.send_command("fixup_redirectors", {
                "folder_path": folder_path,
                "recursive": recursive
            }) or {}
        except Exception as e:
            error_msg = f"Error fixing redirectors: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def import_asset(
        ctx: Context,
        source_file: str,
        destination_path: str = "/Game",
        destination_name: str = "",
        replace_existing: bool = True,
        save: bool = True
    ) -> Dict[str, Any]:
        """Import one local file into the Content Browser."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
            return unreal.send_command("import_asset", {
                "source_file": source_file,
                "destination_path": destination_path,
                "destination_name": destination_name,
                "replace_existing": replace_existing,
                "save": save
            }) or {}
        except Exception as e:
            error_msg = f"Error importing asset: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def import_assets_batch(ctx: Context, assets: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Import multiple local files into the Content Browser."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
            return unreal.send_command("import_assets_batch", {"assets": assets}) or {}
        except Exception as e:
            error_msg = f"Error importing assets batch: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def inspect_import_options(
        ctx: Context,
        source_file: str = "",
        extension: str = ""
    ) -> Dict[str, Any]:
        """Report import factories and supported extensions for a source file or extension."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
            return unreal.send_command("inspect_import_options", {
                "source_file": source_file,
                "extension": extension
            }) or {}
        except Exception as e:
            error_msg = f"Error inspecting import options: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def spawn_mesh_actor_from_asset(
        ctx: Context,
        asset_path: str,
        actor_name: str = "",
        location: List[float] = [0.0, 0.0, 0.0],
        rotation: List[float] = [0.0, 0.0, 0.0],
        scale: List[float] = [1.0, 1.0, 1.0]
    ) -> Dict[str, Any]:
        """Spawn a StaticMeshActor or SkeletalMeshActor from a mesh asset."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}

            response = unreal.send_command("spawn_mesh_actor_from_asset", {
                "asset_path": asset_path,
                "actor_name": actor_name,
                "location": location,
                "rotation": rotation,
                "scale": scale
            })
            return response or {}

        except Exception as e:
            error_msg = f"Error spawning mesh actor from asset: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def set_actor_mesh_asset(
        ctx: Context,
        actor_name: str,
        asset_path: str,
        component_name: str = ""
    ) -> Dict[str, Any]:
        """Set a StaticMesh or SkeletalMesh asset on an actor mesh component."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}

            response = unreal.send_command("set_actor_mesh_asset", {
                "actor_name": actor_name,
                "asset_path": asset_path,
                "component_name": component_name
            })
            return response or {}

        except Exception as e:
            error_msg = f"Error setting actor mesh asset: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def set_actor_component_property(
        ctx: Context,
        actor_name: str,
        component_name: str,
        property_name: str,
        property_value
    ) -> Dict[str, Any]:
        """Set a property on an actor component instance."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}

            response = unreal.send_command("set_actor_component_property", {
                "actor_name": actor_name,
                "component_name": component_name,
                "property_name": property_name,
                "property_value": property_value
            })
            return response or {}

        except Exception as e:
            error_msg = f"Error setting actor component property: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def set_actor_component_material(
        ctx: Context,
        actor_name: str,
        material_path: str,
        component_name: str = "",
        material_index: int = 0
    ) -> Dict[str, Any]:
        """Set a material slot on an actor mesh component."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}

            response = unreal.send_command("set_actor_component_material", {
                "actor_name": actor_name,
                "component_name": component_name,
                "material_path": material_path,
                "material_index": material_index
            })
            return response or {}

        except Exception as e:
            error_msg = f"Error setting actor component material: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def create_material(ctx: Context, asset_path: str, save: bool = True) -> Dict[str, Any]:
        """Create a Material asset."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
            return unreal.send_command("create_material", {
                "asset_path": asset_path,
                "save": save
            }) or {}
        except Exception as e:
            error_msg = f"Error creating material: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def create_material_instance(
        ctx: Context,
        asset_path: str,
        parent_material_path: str,
        save: bool = True
    ) -> Dict[str, Any]:
        """Create a MaterialInstanceConstant from a parent material."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
            return unreal.send_command("create_material_instance", {
                "asset_path": asset_path,
                "parent_material_path": parent_material_path,
                "save": save
            }) or {}
        except Exception as e:
            error_msg = f"Error creating material instance: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def inspect_material_parameters(ctx: Context, material_path: str) -> Dict[str, Any]:
        """List scalar, vector, texture, and static switch material parameters."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
            return unreal.send_command("inspect_material_parameters", {
                "material_path": material_path
            }) or {}
        except Exception as e:
            error_msg = f"Error inspecting material parameters: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def set_material_parameter(
        ctx: Context,
        material_path: str,
        parameter_name: str,
        value,
        parameter_type: str = "scalar"
    ) -> Dict[str, Any]:
        """Set a scalar or vector parameter on a Material or MaterialInstanceConstant asset."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}

            response = unreal.send_command("set_material_parameter", {
                "material_path": material_path,
                "parameter_name": parameter_name,
                "parameter_type": parameter_type,
                "value": value
            })
            return response or {}

        except Exception as e:
            error_msg = f"Error setting material parameter: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def set_material_texture_parameter(
        ctx: Context,
        material_path: str,
        parameter_name: str,
        texture_path: str
    ) -> Dict[str, Any]:
        """Set a texture parameter on a Material or MaterialInstanceConstant asset."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
            return unreal.send_command("set_material_texture_parameter", {
                "material_path": material_path,
                "parameter_name": parameter_name,
                "texture_path": texture_path
            }) or {}
        except Exception as e:
            error_msg = f"Error setting material texture parameter: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    @mcp.tool()
    def set_material_static_switch_parameter(
        ctx: Context,
        material_path: str,
        parameter_name: str,
        value: bool
    ) -> Dict[str, Any]:
        """Set a static switch parameter on a Material or MaterialInstance asset."""
        from unreal_mcp_server import get_unreal_connection

        try:
            unreal = get_unreal_connection()
            if not unreal:
                return {"success": False, "message": "Failed to connect to Unreal Engine"}
            return unreal.send_command("set_material_static_switch_parameter", {
                "material_path": material_path,
                "parameter_name": parameter_name,
                "value": value
            }) or {}
        except Exception as e:
            error_msg = f"Error setting material static switch parameter: {e}"
            logger.error(error_msg)
            return {"success": False, "message": error_msg}

    logger.info("Editor tools registered successfully")
