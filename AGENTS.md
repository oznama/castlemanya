# AGENTS.md

# PSX Adventure Game — Project Context & Development Guidelines

## 1. Project Overview

This project is an original 2D action-adventure game for the Sony PlayStation (PSX / PS1), developed primarily in C using PSn00bSDK.

The game takes inspiration from the exploration, combat, progression, atmosphere, and interconnected level design of Castlevania: Symphony of the Night.

The goal is to create a technically feasible, visually appealing, and playable experience that respects the hardware limitations of the original PlayStation.

### Core Design Principles

- 2D side-scrolling action-adventure gameplay.
- Interconnected environments with multiple exploration paths.
- Combat focused on timing, positioning, and enemy patterns.
- Character progression through equipment, abilities, and exploration.
- Unlockable areas and environmental obstacles.
- Atmospheric environments, original music, and sound effects.
- Retro visual presentation inspired by the PS1 generation.
- Original characters, story, artwork, music, and game assets.

Do not reproduce copyrighted assets, characters, maps, dialogue, music, or proprietary code from existing games.

Use Castlevania: Symphony of the Night as a gameplay and design reference only.

---

## 2. Target Platform

The target platform is the original Sony PlayStation console.

### Hardware Constraints

All implementation decisions must consider the following hardware limitations:

- Main CPU: MIPS R3000A, approximately 33.8688 MHz.
- Main RAM: 2 MB.
- Video RAM: 1 MB.
- Audio RAM: 512 KB.
- CD-ROM transfer speed: approximately 2x.
- Native rendering resolution varies depending on the selected video mode.
- The GPU primarily renders textured polygons and does not provide a conventional hardware sprite system.
- The original console has no dedicated hardware floating-point unit.

The game must prioritize predictable performance, efficient memory usage, and minimal loading interruptions.

Avoid solutions that assume modern hardware capabilities.

---

## 3. Technology Stack

### Required Technologies

- Programming language: C.
- Development SDK: PSn00bSDK.
- Compiler: PSn00bSDK MIPS GCC toolchain.
- Target architecture: MIPS little-endian.
- Build system: CMake.
- Asset packaging: PSn00bSDK-compatible asset conversion and packaging tools.
- Testing: PS1-compatible emulators and original hardware when available.

### Official Resources

PSn00bSDK repository:

https://github.com/Lameguy64/PSn00bSDK

PSn00bSDK documentation:

https://github.com/Lameguy64/PSn00bSDK/tree/master/doc

PSn00bSDK examples:

https://github.com/Lameguy64/PSn00bSDK/tree/master/examples

Always consult the installed SDK version and its examples before using unfamiliar APIs.

Do not assume that APIs from PsyQ, modern PlayStation development libraries, or other SDKs are available in PSn00bSDK.

---

## 4. Programming Language Guidelines

### C Language

- Use C as the primary implementation language.
- Follow the C language standard supported by the installed PSn00bSDK toolchain.
- Prefer explicit, portable C code.
- Avoid unnecessary compiler-specific extensions.
- Use fixed-width integer types from `<stdint.h>` when appropriate.
- Avoid dynamic allocation during gameplay unless strictly necessary.
- Prefer statically allocated memory and explicit memory ownership.
- Avoid recursion in performance-critical systems.
- Avoid large stack allocations.
- Keep functions small, focused, and easy to understand.

### Naming Conventions

Use the following conventions:

- Types: `PascalCase`
- Functions: `snake_case`
- Variables: `snake_case`
- Constants: `UPPER_SNAKE_CASE`
- Enumerations: `PascalCase`
- Enumeration values: `UPPER_SNAKE_CASE`
- Files: `snake_case.c` and `snake_case.h`

Examples:

```c
typedef struct {
    int32_t x;
    int32_t y;
    int16_t health;
} Player;

void player_update(Player* player);
void player_render(const Player* player);

#define PLAYER_MAX_HEALTH 100
```

Avoid global variables unless they represent shared engine state that genuinely requires global access.

---

## 5. Architecture

The game must be organized into independent modules.

Recommended architecture:

```text
src/
├── main.c
├── engine/
│   ├── game.c
│   ├── game.h
│   ├── game_state.c
│   ├── game_state.h
│   ├── input.c
│   ├── input.h
│   ├── collision.c
│   ├── collision.h
│   ├── camera.c
│   └── camera.h
├── graphics/
│   ├── renderer.c
│   ├── renderer.h
│   ├── sprite.c
│   ├── sprite.h
│   ├── animation.c
│   └── animation.h
├── gameplay/
│   ├── player.c
│   ├── player.h
│   ├── enemy.c
│   ├── enemy.h
│   ├── combat.c
│   └── combat.h
├── world/
│   ├── room.c
│   ├── room.h
│   ├── level.c
│   ├── level.h
│   ├── tilemap.c
│   └── tilemap.h
├── audio/
│   ├── audio.c
│   └── audio.h
├── resources/
│   ├── resource.c
│   └── resource.h
└── ui/
    ├── hud.c
    └── hud.h
```

This structure is a starting point, not a mandatory fixed layout.

Create additional modules only when they improve separation of responsibilities.

### Module Responsibilities

- `engine`: game loop, timing, state management, and shared engine functionality.
- `graphics`: rendering, sprites, animation, and GPU resource management.
- `gameplay`: player control, combat, enemies, and game mechanics.
- `world`: rooms, maps, collision geometry, and transitions.
- `audio`: music and sound effect playback.
- `resources`: loading, unloading, and management of game assets.
- `ui`: HUD, menus, inventory, and dialogue interfaces.

Avoid circular dependencies between modules.

Use header files to expose public interfaces and keep implementation details private.

---

## 6. Game Loop

The game loop must be deterministic and suitable for the original PlayStation.

The preferred update sequence is:

1. Synchronize with the video frame.
2. Read controller input.
3. Update the current game state.
4. Update player movement.
5. Update enemies and AI.
6. Resolve collisions.
7. Update combat and interactions.
8. Update camera position.
9. Update animations.
10. Render the world.
11. Render the HUD.
12. Submit the GPU command list.

The game must avoid uncontrolled frame-rate-dependent movement.

Prefer integer or fixed-point arithmetic for gameplay calculations.

Do not introduce floating-point operations into frequently executed gameplay code without a strong technical reason.

---

## 7. Graphics and Rendering

Rendering must use the GPU capabilities exposed by PSn00bSDK.

### Rendering Requirements

- Use textured polygons and GPU primitives for sprites and backgrounds.
- Prefer pre-rendered or carefully authored 2D assets.
- Use texture atlases when they provide a measurable benefit.
- Minimize texture page changes and GPU state changes.
- Avoid excessive GPU primitive counts.
- Minimize unnecessary transparency and overdraw.
- Reuse rendering structures when possible.
- Use double buffering or the rendering approach demonstrated by the installed SDK.
- Keep the display and drawing environments consistent with the selected video mode.

### Sprite System

Sprites should support:

- Position.
- Dimensions.
- Texture coordinates.
- Animation frame.
- Rendering attributes.
- Transparency where required.
- Horizontal flipping.
- Optional vertical flipping.

Avoid allocating GPU primitives dynamically for every frame.

Prefer reusable rendering structures and predictable allocation.

### PS1 Texture Considerations

The PlayStation GPU supports several texture formats, including 4-bit, 8-bit, and 15-bit textures.

Texture memory is limited and must be managed explicitly.

Do not assume modern GPU features such as shaders, texture filtering, or unlimited texture memory.

---

## 8. Memory Management

Memory management is a critical requirement.

### Rules

- Avoid unrestricted `malloc()` and `free()` calls during gameplay.
- Prefer fixed-size pools, static arrays, or explicitly managed arenas.
- Define maximum counts for enemies, projectiles, particles, and entities.
- Track memory usage for large systems.
- Avoid memory fragmentation.
- Keep temporary buffers small.
- Avoid copying large structures unnecessarily.
- Release resources when leaving a room or game state if they are no longer needed.

### Example

```c
#define MAX_ENEMIES 32

typedef struct {
    int32_t x;
    int32_t y;
    int16_t health;
    uint8_t active;
} Enemy;

static Enemy enemies[MAX_ENEMIES];
```

All entity limits must be documented and enforced.

---

## 9. Player and Gameplay Systems

The player controller should support:

- Horizontal movement.
- Acceleration and deceleration where appropriate.
- Jumping.
- Gravity.
- Ground detection.
- Collision with walls and platforms.
- Basic attacks.
- Damage and knockback.
- Health management.
- Interaction with objects.
- Room transitions.

Gameplay should be implemented through explicit state machines.

### Player States

Possible states include:

- IDLE
- WALKING
- RUNNING
- JUMPING
- FALLING
- ATTACKING
- DAMAGED
- DEAD

Avoid implementing the entire player controller as a large function with deeply nested conditionals.

Separate movement, combat, animation, and state transitions.

### Combat

Combat must support:

- Hit detection.
- Damage calculation.
- Attack cooldowns.
- Enemy invulnerability frames where appropriate.
- Knockback.
- Player damage.
- Enemy defeat.
- Basic enemy behavior patterns.

Collision detection must be efficient and deterministic.

---

## 10. World and Level Design

The world should consist of interconnected rooms or areas.

Each room should define:

- Room identifier.
- Dimensions.
- Tilemap or background data.
- Collision geometry.
- Enemies.
- Interactive objects.
- Entry and exit points.
- Camera boundaries.
- Optional music or ambient audio.

Use data-driven room definitions when possible.

Avoid hardcoding all room geometry and enemy placement directly into gameplay logic.

### Room Transitions

Room transitions should:

- Preserve player state when required.
- Validate destination room identifiers.
- Position the player at the correct entrance.
- Update camera boundaries.
- Load required assets.
- Release assets that are no longer needed.

Prefer predictable room loading and unloading.

Avoid loading large amounts of data synchronously during active gameplay.

---

## 11. Asset Pipeline

All game assets must be prepared for the PlayStation hardware.

### Graphics

- Use original artwork.
- Optimize textures for VRAM.
- Consider texture page boundaries.
- Use indexed color where appropriate.
- Avoid oversized images.
- Prefer compact formats compatible with the build pipeline.

### Audio

- Prepare music and sound effects using formats supported by the installed PSn00bSDK audio libraries.
- Consider ADPCM encoding and SPU memory limitations.
- Avoid loading unnecessary sound data.
- Reuse sound effects when possible.
- Reserve audio channels according to a documented policy.

### Asset Organization

```text
assets/
├── sprites/
│   ├── player/
│   ├── enemies/
│   └── objects/
├── backgrounds/
├── tilesets/
├── audio/
│   ├── music/
│   └── sfx/
├── fonts/
└── ui/
```

Generated assets must have reproducible conversion steps.

Do not commit temporary conversion files unless required by the project.

---

## 12. Build System

The project must use the CMake configuration provided by PSn00bSDK.

Do not replace the SDK toolchain configuration with a generic desktop GCC configuration.

The project must generate a PlayStation-compatible executable and, when required, a CD image.

Typical build commands:

```bash
cmake --preset default .
cmake --build ./build
```

The exact commands and presets must follow the project's current CMake configuration.

### Build Requirements

- Use the PSn00bSDK toolchain.
- Keep source files inside the configured source tree.
- Avoid hardcoded absolute paths.
- Use reproducible asset conversion scripts.
- Keep generated files inside the build directory.
- Do not commit generated build artifacts unless explicitly required.

Do not modify SDK installation files to solve project-specific problems.

---

## 13. Testing and Debugging

The game must be tested in a PS1-compatible emulator.

Recommended environments include:

- DuckStation.
- PCSX-Redux.
- Original PlayStation hardware when available.

### Testing Requirements

Validate:

- Correct startup.
- Rendering.
- Controller input.
- Collision detection.
- Player movement.
- Room transitions.
- Enemy behavior.
- Audio playback.
- Memory usage.
- Performance.
- Save functionality when implemented.

Do not assume that successful compilation means the game works correctly.

Test gameplay changes in an emulator whenever possible.

### Debugging

Use debug output and emulator debugging tools when available.

Avoid excessive logging during normal gameplay.

Debug features must be removable or disabled in release builds.

---

## 14. Performance Guidelines

Performance is a first-class requirement.

- Keep the main loop efficient.
- Avoid unnecessary calculations.
- Avoid excessive entity updates.
- Use spatial partitioning or room-based filtering when needed.
- Avoid expensive collision checks between unrelated entities.
- Use fixed-point arithmetic when appropriate.
- Avoid large memory copies.
- Avoid unnecessary GPU synchronization.
- Minimize CD-ROM access during gameplay.
- Measure performance before introducing complex optimizations.

Do not introduce premature optimization that significantly reduces code maintainability.

---

## 15. AI Agent Instructions

When implementing new functionality:

1. Inspect the existing project structure.
2. Read the relevant source files and headers.
3. Identify the module responsible for the requested behavior.
4. Follow existing coding conventions.
5. Check the PSn00bSDK documentation before using unfamiliar APIs.
6. Keep changes focused and minimal.
7. Avoid unnecessary architectural changes.
8. Ensure that the code is compatible with the PS1 toolchain.
9. Build the project when the development environment permits.
10. Report any untested behavior or unresolved issues.

### Important Restrictions

- Do not assume the project uses modern C++.
- Do not introduce C++ dependencies.
- Do not introduce desktop-only APIs.
- Do not use SDL, GLFW, OpenGL, DirectX, or Vulkan in the PS1 runtime.
- Do not assume POSIX APIs are available.
- Do not use dynamic memory allocation without considering its impact.
- Do not introduce modern GPU features unavailable on the PS1.
- Do not modify unrelated files.
- Do not overwrite existing assets without explicit justification.
- Do not invent SDK APIs or function signatures.

If an implementation depends on an API whose availability is uncertain, inspect the installed SDK headers or official examples before proceeding.

If a requested feature is not feasible on the original hardware, explain the limitation and propose a practical alternative.

---

## 16. Development Priorities

The project should be developed incrementally.

Recommended milestones:

### Milestone 1 — Technical Foundation

- Initialize PSn00bSDK project.
- Configure CMake.
- Create a minimal executable.
- Initialize graphics.
- Initialize controller input.
- Establish the main loop.

### Milestone 2 — Playable Prototype

- Implement player movement.
- Implement camera scrolling.
- Implement collision detection.
- Implement a basic room.
- Implement player animation.

### Milestone 3 — Gameplay

- Implement combat.
- Implement enemy AI.
- Implement damage and health.
- Implement interactive objects.
- Implement room transitions.

### Milestone 4 — Exploration

- Implement interconnected rooms.
- Implement doors and obstacles.
- Implement progression abilities.
- Implement inventory.
- Implement checkpoints.

### Milestone 5 — Presentation

- Implement HUD.
- Implement menus.
- Implement music and sound effects.
- Improve animation.
- Improve atmosphere and visual effects.

### Milestone 6 — Optimization

- Profile CPU and GPU usage.
- Optimize VRAM allocation.
- Optimize entity processing.
- Optimize asset loading.
- Validate the game on original hardware when available.

---

## 17. Definition of Done

A feature is considered complete when:

- Its behavior is implemented.
- It follows the project's architecture.
- It respects PS1 hardware constraints.
- It compiles with the PSn00bSDK toolchain.
- It does not introduce unrelated regressions.
- Its resource usage is predictable.
- Its behavior has been tested when possible.
- Any limitations or untested behavior are documented.

The final objective is to build a stable, performant, and maintainable PS1 adventure game with original content and a strong emphasis on exploration, combat, atmosphere, and progression.