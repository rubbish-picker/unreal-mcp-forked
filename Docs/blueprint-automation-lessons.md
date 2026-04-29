# Unreal MCP Blueprint Automation Lessons

This note records practical lessons from debugging a proximity door Blueprint through Unreal MCP. It is intentionally concrete: these are the checks to run before trusting a Blueprint automation result.

## 1. Confirm the real target first

Before editing a Blueprint, identify all three of these:

- Current map asset, for example `/Game/Project101/Maps/Lv1`
- Actor instance in the level, for example `BP_Door_C_1`
- Blueprint asset path behind that actor, for example `/Game/Project101/BluePrint/BP_Door`

Do not assume an actor name maps to the expected asset. A project can contain multiple similar doors, such as a training-map door and a level-specific door.

## 2. Compile is not save

`CompileBlueprint` only verifies and refreshes the Blueprint. It does not guarantee that the `.uasset` is written to disk.

For MCP commands that mutate Blueprints, the safe sequence is:

1. Modify the Blueprint.
2. Mark the Blueprint dirty.
3. Compile the Blueprint.
4. Save the Blueprint asset.
5. Reopen or reload once when persistence matters.

For UnrealMCP itself, `compile_blueprint` should save the loaded Blueprint asset after a successful compile. Otherwise edits can appear to work during the current editor session and vanish after restart.

## 3. Avoid multiple editor/game instances while saving assets

Unreal can fail to save a Blueprint if another editor or game process has the `.uasset` locked. A typical symptom is an error like:

```text
MoveFile was unable to move ... BP_Door.uasset ... Error Code 32
Error saving ... BP_Door.uasset
```

When saving fails, check for extra `UnrealEditor` processes and close standalone game instances before retrying the save.

## 4. Prefer component-bound overlap events for sensor components

For a self-contained proximity door, bind the sensor component directly:

- `AutoSensor.OnComponentBeginOverlap -> OpenDoor`
- `AutoSensor.OnComponentEndOverlap -> CloseDoor`

This is more reliable than relying on actor-level `ReceiveActorBeginOverlap`, especially when the sensor is a dedicated `BoxComponent` added to the door Blueprint.

## 5. Collision settings must be verified, not assumed

Adding a `BoxComponent` is not enough. For overlap-driven behavior, verify the component has:

- A useful extent, for example `BoxExtent = [500, 240, 220]`
- Collision enabled for query overlap, for example `QueryOnly`
- A trigger-like profile or compatible collision responses
- `bGenerateOverlapEvents = true`

Also remember that overlap requires both sides to be compatible. The player capsule must generate overlap/block events in a way the sensor can detect.

## 6. Runtime testing is separate from asset verification

Asset verification can prove that nodes and components exist in the `.uasset`, but it cannot prove that gameplay overlap fired.

Good runtime checks:

- Temporarily add `PrintString` with `bPrintToLog = true` on overlap events.
- Launch the target map directly and inspect the newest log file.
- Search logs for the exact debug string, such as `Door BeginOverlap`.

Remove or ignore temporary debug nodes once the behavior is confirmed.

## 7. Desktop key injection is weak evidence

Sending keys to the Unreal window from PowerShell or another desktop automation layer is fragile. Focus, input mode, window title, PIE vs standalone, and enhanced input can all make the injected keys meaningless.

Treat manual play testing or an Unreal-native automation path as stronger evidence than `SendKeys`-style input injection.

## 8. A small checklist for Blueprint MCP edits

Use this checklist before reporting success:

1. `get_actors_in_level` confirms the current map and target actor.
2. The Blueprint asset path is explicit, not inferred from a short name.
3. Mutating MCP calls return success.
4. The Blueprint compiles successfully.
5. The Blueprint asset saves successfully.
6. A restart or asset reload still shows the component/nodes.
7. Runtime logs confirm the event fired, when runtime behavior is the goal.

