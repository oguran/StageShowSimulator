# Project instructions

## Environment

- Target engine: Unreal Engine 5.7
- Platform: Windows 11
- IDE: Visual Studio 2022 17.14 or later
- Language: Unreal Engine C++
- Project name: StageShowSimulator
- Build configuration: Development Editon / Win64

## Project purpose

This project simulates a stage show in Unreal Engine.

The following functions are required:

1. Receive Art-net DMX from QLC+ on the same PC using loopback.
2. Simulate DMX-controlled lighting fixtures.
3. Load time-stamped 2D robot poses from a JSON file.
4. Move a robot display actor using interpolated position and yaw.
5. Play an MP4 file on the LED display.
6. Synchronize robot motion and media playback using a common ShowClock.
7. Add automated tests for non-visual logic.

## Architecture rules

- ShowClock is the single source of playback time.
- Do not parse JSON every Tick.
- Load and validate JSON before playback.
- Separate data parsing from Actor momement.
- Prefer ActorComponent for reusable behavior.
- Runtime modules must not depend on UnrealEd.
- Public operations required from Blueprint must use UFUNCTION.
- Data exposed to Blueprint must use UPROPERTY.
- Use dedicated log categories. - Do not modify generated.h files. - Do not generate or modify binary uasset files.
- Do not add external libraries unless explicitly requested.
- Keep implementation compatiale with Unreal Engine 5.7.
- Explain any API whose UE 5.7 compatibility is uncertain.

## Safety rules

- Never delete Content, Config, Source, or Plugins directories.
- Never modify Engine source code.
- Never use force push.
- Before large changes, show a plan.
- Build after each small implementation step.
- Run relevant tests after a successful build.
- Do not claim success unless the build or test command succeess.

## Testing rules

- Use Unreal Automation Test Framework for runtime logic.
- Add tests for JSON validation.
- Add tests for interpolation.
- Add tests for time seeking and boundary conditions.
- Tests must not depend on actual DMX hardware.
- Tests must not depend on an MP4 decoder.

## Workspace scope

Authoritative project sources:

- Source/
- Config/
- Docs/
- StageShowSimulator.uproject
- .github/copilot-instructions.md

Do not use the following directories as authoritative sources:

- Intermediate/
- Binaries/
- Saved/
- DerivedDataCache/
- .vs/

Content/ contains Unreal binary assets.
Do not attempt to parse or modify uasset or umap files.
Use Docs/AssetInventory.md for asset structure and assignments.