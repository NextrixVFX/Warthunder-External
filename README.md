# Warthunder-External

External ESP overlay for War Thunder. The usermode application reads game memory, caches world state on background threads, and renders an DirectX 11 + ImGui overlay on top of the game window.

> **Important:** The kernel-mode / hypervisor driver is **intentionally excluded** from this repository (see `.gitignore`). The solution file references a `hypervisor/` project, but that directory is not shipped. If you want kernel-level memory access, you must supply and integrate your own driver. This repo only includes the usermode client and a fallback usermode memory backend.

---

## Table of Contents

- [Requirements](#requirements)
- [Building](#building)
- [Running](#running)
- [Memory Backends](#memory-backends)
- [Architecture](#architecture)
- [Project Layout](#project-layout)
- [Offsets](#offsets)
- [Configuration](#configuration)
- [Controls](#controls)
- [Extending the Project](#extending-the-project)
- [License](#license)

---

## Requirements

| Component | Version / Notes |
|-----------|-----------------|
| OS | Windows 10/11 x64 |
| IDE | Visual Studio 2022 (toolset **v145**) |
| Windows SDK | 10.0 |
| Platform | **x64** (Win32 configs exist but the project is intended for x64) |
| Privileges | Administrator (UAC elevation required) |
| Target game | War Thunder (`aces.exe`) |

Third-party libraries bundled under `usermode/impl/include/`:

- [Dear ImGui](https://github.com/ocornut/imgui) (DX11 + Win32 backends)
- [DirectXTK](https://github.com/microsoft/DirectXTK) (prebuilt `.lib` in `usermode/impl/lib/`)
- [RapidJSON](https://github.com/Tencent/rapidjson)
- [skCrypter](https://github.com/skadro-official/skCrypter) (compile-time string encryption)

---

## Building

1. Clone the repository.
2. Open `Warthunder-External.slnx` in Visual Studio 2022.
3. Select the **usermode** project and set the configuration to **Release | x64**.
4. Build the solution.

Output is written to:

```
build/usermode/Release/Edge.exe
```

The executable is named `Edge.exe` by design (streamer-mode window title spoofing). Copy `usermode/assets/offsets.txt` (and `usermode/assets/Poppins.ttf` if present) next to the built binary, or run from a working directory where `assets\offsets.txt` resolves correctly.

> The solution lists a `hypervisor` project that is not included in the repo. You can ignore build errors related to the missing hypervisor project, or remove that project reference from the solution and build **usermode** directly.

---

## Running

1. Launch War Thunder.
2. Run `Edge.exe` as Administrator.
3. The console waits for `aces.exe`, loads offsets from `assets\offsets.txt`, and attaches to the process.
4. When prompted, press **OK** once you are loaded into the lobby.
5. Use **Right Shift** to toggle the menu. Press **F10** to shut down cleanly (saves config, stops caches, tears down the overlay).

Config is loaded from and saved to:

```
%APPDATA%\novision-warthunder\config.json
```

---

## Memory Backends

Memory access is abstracted through `External::Runtime::c_memory` in `usermode/impl/memory/memory.hxx`.

### Usermode (included, default)

In `usermode/impl/includes.hxx`, the macro `is_usermode_mem` is defined:

```cpp
#define is_usermode_mem
```

With this enabled, the client uses standard Win32 APIs:

- Process enumeration via `CreateToolhelp32Snapshot`
- `OpenProcess` with `PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION`
- `ReadProcessMemory` / `WriteProcessMemory`
- Base address resolution through `NtQueryInformationProcess` + PEB read

This backend is fully functional for development and testing but is subject to standard usermode limitations and anti-cheat visibility.

### Kernel / hypervisor (not included)

To use a kernel driver or hypervisor for physical/virtual memory reads:

1. **Remove** `#define is_usermode_mem` from `usermode/impl/includes.hxx`.
2. Implement your driver communication inside `c_memory` — populate `m_application.m_handle`, `m_base_address`, and wire `read()` / `write()` to your IOCTL or shared-memory interface.
3. Load your own driver before starting the usermode client.

The original kernel/hypervisor implementation lives outside this repository and is **not provided**. No driver binary, loader, or communication protocol is included. You are responsible for driver development, signing, loading, and compliance with applicable laws and terms of service.

---

## Architecture

High-level data flow:

```
aces.exe  ──read──▶  c_memory  ──▶  Runtime caches (background threads)
                                           │
                                           ▼
                              Feature modules (ESP, etc.)
                                           │
                                           ▼
                         DX11 overlay (ImGui render thread)
```

### Runtime caches

Background worker threads poll game memory and publish double-buffered snapshots. Registered in `usermode/workspace/core/runtime/caches/`:

| Cache | ID | Purpose |
|-------|----|---------|
| `c_engine_cache` | `engine` | Game context, local player, unit list, camera controller |
| `c_camera_cache` | `camera` | View matrix and camera position |
| `c_entities_cache` | `entities` | Per-unit data: position, bbox, team, type, screen projection |

Caches self-register via `c_cache_registrar<T>` and are started from `Runtime::register_caches()` / `Runtime::start_caches()` in `main.cxx`.

Each cache implements `tick()` and must sleep at the end of its loop. Tick rates scale with `Config::g_using_efficiency_mode`.

### Features

Features implement `c_feature` with `on_update()` (main thread) and `on_render()` (render thread). They consume cache data rather than reading memory directly during draw calls.

Currently registered:

| Feature | ID | Description |
|---------|----|-------------|
| `c_esp` | `visual_esp` | 3D bounding boxes, snaplines, usernames, distance |

### Rendering

- Transparent topmost overlay window aligned to the game client area
- DirectX 11 swap chain + ImGui immediate-mode UI
- Drawing helpers in `workspace/core/gui/deps/shapes.hxx`
- Menu in `workspace/core/gui/menu.hxx`
- Mouse passthrough when the menu is not capturing input (`WS_EX_TRANSPARENT`)

### Engine layer

- `workspace/engine/offsets.hxx` — offset loader and named references
- `workspace/engine/structs.hxx` — game structure layouts
- `workspace/engine/sdk.hxx` — helpers (string reads, player name parsing)
- `workspace/engine/enums.hxx` — unit type / state enums

---

## Project Layout

```
Warthunder-External/
├── Warthunder-External.slnx    # Solution (references excluded hypervisor project)
├── usermode/
│   ├── main.cxx                # Entry point, init/shutdown lifecycle
│   ├── usermode.vcxproj
│   ├── assets/
│   │   └── offsets.txt         # Game version offsets (required at runtime)
│   ├── impl/
│   │   ├── includes.hxx        # Master include; memory backend toggle
│   │   ├── memory/
│   │   │   ├── memory.hxx      # c_memory abstraction
│   │   │   └── usermode/       # Default ReadProcessMemory backend
│   │   ├── include/            # Third-party headers (ImGui, RapidJSON, DirectXTK, …)
│   │   └── lib/                # Prebuilt DirectXTK libraries
│   └── workspace/
│       ├── engine/             # Offsets, structs, SDK helpers
│       ├── utility/            # Config, logging, math primitives, assets
│       ├── core/
│       │   ├── runtime/caches/ # Background data collectors
│       │   ├── features/       # ESP and future gameplay/visual modules
│       │   └── gui/            # Overlay window, render loop, menu
│       └── resources/          # Icon, RC file
├── build/                      # Build output (gitignored)
└── hypervisor/                 # EXCLUDED — not in repo; bring your own driver
```

---

## Offsets

Offsets are loaded at startup from `assets/offsets.txt`. Format:

```
-- site: https://api.monkrel.cc/api/v1/offsets
-- version: 2.57.1.48

global.g_game_context 7115338
global.g_local_player 70EE780
game.g_unit_list_3 340
...
```

- Lines starting with `--` are comments (version metadata).
- Each data line: `namespace.key <hex_value>`.
- Values are parsed as hexadecimal and mapped in `offsets.hxx` to typed references (e.g. `offsets::global::g_game_context`).

**Game updates break offsets.** When War Thunder patches, update `offsets.txt` to match the new client version before running.

---

## Configuration

Settings live in `%APPDATA%\novision-warthunder\config.json` and are managed by `Config::Json` in `workspace/utility/config.hxx`.

| Setting | Description |
|---------|-------------|
| `g_lines_enabled` | Draw snaplines to enemies |
| `g_boxes_enabled` | Draw 3D bounding boxes |
| `g_usernames_enabled` | Show player names |
| `g_distance_enabled` | Show distance labels |
| `g_*_color` | Team / tank / plane colors (RGBA arrays) |
| `g_using_efficiency_mode` | Lower cache poll rates |
| `g_using_vsync` | VSync for overlay present |
| `g_using_console` | Show/hide debug console |
| `g_is_streamer_mode` | Spoof window title as "Edge" |
| `g_is_rendering` | Master render toggle |

Config is loaded after memory attach and saved on **F10** exit.

---

## Controls

| Input | Action |
|-------|--------|
| **Right Shift** | Toggle overlay menu |
| **F10** | Exit (save config, cleanup) |

---

## Extending the Project

### Add a cache

1. Create a class inheriting `c_cache` in `workspace/core/runtime/caches/`.
2. Assign a unique `CACHE_ID` in `registry.hxx`.
3. Add a static `c_cache_registrar<YourCache>` member to auto-register.
4. Implement `tick()` — read memory, write to a `c_double_buffer`, sleep at the end.

### Add a feature

1. Create a class inheriting `c_feature` in `workspace/core/features/`.
2. Assign a unique `FEATURE_ID` in `registry.hxx`.
3. Add a static `c_feature_registrar<YourFeature>` member.
4. Implement `on_update()` and `on_render()`. Pull data from caches via `CacheType::get()`.

### Switch memory backend

See [Memory Backends](#memory-backends). The usermode path requires no extra setup. Kernel mode requires your own driver and edits to `memory.hxx` outside the `#ifdef is_usermode_mem` blocks.

---

## License

MIT License — see [LICENSE](LICENSE).
