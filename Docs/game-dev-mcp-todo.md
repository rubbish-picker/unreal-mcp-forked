# Game Development MCP TODO

This checklist tracks low-level MCP capabilities needed to build real Unreal gameplay without baking in high-level gameplay templates.

## Rules

- Prefer atomic editor operations over packaged gameplay templates.
- Every new command needs a small smoke test.
- Keep `list_mcp_commands` updated so lost chat context can recover the tool catalog.
- Sync changes to both the real test project plugin and the repo template plugin.

## Batch 1: Asset Management

- [x] `create_content_folder`: create a Content Browser folder.
- [x] `duplicate_asset`: duplicate an asset.
- [x] `rename_asset`: rename or move an asset.
- [x] `delete_asset`: delete an asset.
- [x] `save_asset`: save one loaded asset.
- [x] `fixup_redirectors`: fix redirectors under a path.
- [ ] Smoke tests:
- [x] Create `/Game/MCP_Test`.
- [x] Duplicate `/Engine/BasicShapes/Cube` into it as `/Game/MCP_Test/MCP_Test_Cube`.
- [x] Rename/move the duplicate to `/Game/MCP_Test/MCP_Test_Cube_Renamed`.
- [x] Save it.
- [x] Delete it.
- [x] Fix redirectors under `/Game/MCP_Test`.

## Batch 2: Asset Import

- [x] `import_asset`: import FBX/PNG/WAV/etc. from local disk.
- [x] `import_assets_batch`: import multiple files.
- [ ] `inspect_import_options`: report supported import options.
- [ ] Smoke tests:
- [x] Import a tiny generated texture.
- [x] Verify asset exists.
- [x] Delete imported test asset.

## Batch 3: Materials

- [ ] `create_material`: create a Material asset.
- [ ] `create_material_instance`: create a MaterialInstanceConstant from a parent material.
- [ ] `inspect_material_parameters`: list scalar/vector/texture/static switch parameters.
- [ ] `set_material_texture_parameter`: set texture parameter.
- [ ] `set_material_static_switch_parameter`: set static switch parameter.
- [ ] Optional later: material graph node creation and connection.
- [ ] Smoke tests:
- [ ] Create a material instance from a known engine material.
- [ ] Set scalar/vector parameters when present.
- [ ] Save/delete test material instance.

## Batch 4: Enhanced Input

- [ ] `create_input_action_asset`: create an Enhanced InputAction asset.
- [ ] `create_input_mapping_context`: create an InputMappingContext asset.
- [ ] `add_enhanced_input_mapping`: add key mapping to a context.
- [ ] `inspect_enhanced_input_mapping_context`: read mappings.
- [ ] Smoke tests:
- [ ] Create test InputAction and MappingContext.
- [ ] Add E and MouseX mappings.
- [ ] Read them back.
- [ ] Delete test assets.

## Batch 5: World and Gameplay Framework

- [ ] `inspect_world_settings`: read current level WorldSettings.
- [ ] `set_world_game_mode`: set override GameMode on current level.
- [ ] `set_game_mode_defaults`: set DefaultPawnClass, PlayerControllerClass, HUDClass.
- [ ] `inspect_gameplay_framework`: report GameMode/GameState/Pawn/Controller/HUD classes.
- [ ] Smoke tests:
- [ ] Read current world settings.
- [ ] Set and restore a harmless override if available.

## Batch 6: Blueprint Graph Primitives

- [ ] `add_blueprint_branch_node`.
- [ ] `add_blueprint_sequence_node`.
- [ ] `add_blueprint_delay_node`.
- [ ] `add_blueprint_timer_node`.
- [ ] `add_blueprint_custom_event_node`.
- [ ] `add_blueprint_function_graph`.
- [ ] `add_blueprint_variable_get_node`.
- [ ] `add_blueprint_variable_set_node`.
- [ ] Smoke tests:
- [ ] Create temporary Blueprint.
- [ ] Add nodes and connect a simple execution chain.
- [ ] Compile.
- [ ] Delete test Blueprint.

## Batch 7: C++ Scaffolding

- [ ] `create_cpp_class`: create Actor/Component/Object class files.
- [ ] `update_build_cs_dependency`: add module dependency.
- [ ] `compile_project`: run project build through UBT.
- [ ] Smoke tests:
- [ ] Create temporary ActorComponent class.
- [ ] Compile.
- [ ] Delete test files or keep only if explicitly requested.

## Batch 8: Animation and Sockets

- [ ] `inspect_skeletal_mesh_sockets`.
- [ ] `set_animation_blueprint`.
- [ ] `inspect_anim_montage`.
- [ ] `play_montage_in_editor_preview` if editor-safe.
- [ ] `attach_actor_to_socket`.
- [ ] Smoke tests:
- [ ] List sockets on a skeletal mesh if available.
- [ ] Attach a harmless actor to a socket in a temp setup.

## Batch 9: Lighting and Post Process

- [ ] `spawn_light_actor_from_type`: support common light actors with detailed params.
- [ ] `spawn_post_process_volume`.
- [ ] `set_post_process_settings`: exposure, bloom, color grading basics.
- [ ] `spawn_sky_light`.
- [ ] `spawn_exponential_height_fog`.
- [ ] Smoke tests:
- [ ] Spawn and delete a test PointLight.
- [ ] Spawn and delete a test PostProcessVolume.

## Batch 10: Landscape

- [ ] `create_landscape`.
- [ ] `import_landscape_heightmap`.
- [ ] `set_landscape_material`.
- [ ] `inspect_landscape_components`.
- [ ] Smoke tests:
- [ ] Read existing landscapes if any.
- [ ] Defer destructive creation unless in a test map.

## Batch 11: Sequencer

- [ ] `create_level_sequence`.
- [ ] `add_actor_to_sequence`.
- [ ] `add_camera_cut_track`.
- [ ] `add_transform_keys`.
- [ ] `spawn_level_sequence_actor`.
- [ ] Smoke tests:
- [ ] Create temporary sequence.
- [ ] Add a cube/camera track.
- [ ] Save/delete test assets and actors.

## Batch 12: VFX and Audio

- [ ] `list_niagara_systems`.
- [ ] `spawn_niagara_actor`.
- [ ] `attach_niagara_to_component`.
- [ ] `spawn_sound_actor`.
- [ ] `set_sound_attenuation`.
- [ ] Smoke tests:
- [ ] List assets by class.
- [ ] Spawn/delete a test system or sound if asset exists.
