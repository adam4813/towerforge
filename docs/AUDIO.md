# Audio (Consolidated)

This document is the canonical, consolidated reference for the TowerForge audio system. It merges and replaces the split docs:
- `docs/AUDIO_SYSTEM.md`
- `docs/AUDIO_IMPLEMENTATION_SUMMARY.md`
- `docs/AUDIO_QUICK_REFERENCE.md`

Use this file for architecture, implementation notes, quick usage, API examples, asset layout, and future improvements.

---

## Quick start

```cpp
#include "audio/audio_manager.h"

auto& audio = towerforge::audio::AudioManager::GetInstance();
audio.Initialize();
// In the game loop:
audio.Update(delta_time);
// Play music and SFX
audio.PlayMusic(towerforge::audio::AudioCue::MainTheme, true, 1.0f);
audio.PlaySFX(towerforge::audio::AudioCue::MenuClick);
// Adjust volumes
audio.SetMasterVolume(0.8f);
```

---

## Summary / Key features

- Centralized `AudioManager` singleton (headers: `include/audio/audio_manager.h`, implementation: `src/audio/audio_manager.cpp`).
- Supports Music, SFX, and Ambient channels.
- Crossfading, fade-in/out, and independent volumes (Master, Music, SFX, Ambient).
- Resource caching with single-load policy and failure tracking (graceful degradation when assets are missing).
- Real-time synchronization with UI (`AudioSettingsMenu`).
- Audio updates via `audio_manager.Update(delta_time)` called from game loops.

---

## Architecture & design

The audio system follows a singleton pattern centered on `AudioManager`:

- Responsibilities:
  - Initialize and shutdown the audio device
  - Load and cache music and sound assets
  - Play/stop music, SFX, and ambient tracks
  - Support fade transitions and crossfades
  - Expose volume controls and query methods
  - Track failed loads to avoid repeated attempts

- Implementation notes:
  - Uses raylib's audio API (InitAudioDevice, LoadMusicStream, LoadSound, UpdateMusicStream, etc.)
  - Manual volume tracking is performed to support smooth fades (raylib limitation)
  - All public calls guard against uninitialized audio device
  - Update() should be called each frame to advance streams and handle fades

## Architecture compliance (summary)

The implementation follows the intended ASCII architecture with `AudioManager` as the central singleton coordinating Music, SFX and Ambience channels:

```
AUDIO SYSTEM OVERVIEW
======================
┌─────────────────────────────┐
│  AudioManager (singleton)   │
├─────────────────────────────┤
│  - PlayMusic(track)         │ ✓ Implemented
│  - PlaySFX(cue)             │ ✓ Implemented
│  - PlayAmbient(track)       │ ✓ Implemented
│  - SetVolume(type, value)   │ ✓ Implemented
│  - FadeIn/Out(track)        │ ✓ Implemented
└─────────────────────────────┘
    |         |         |
    V         V         V
[Music]   [SFX]     [Ambience]
```


## Acceptance criteria verification

The implementation meets the project's acceptance criteria:

- ✅ All game states/events play appropriate audio cues (title screen, gameplay, menus, achievements).
- ✅ Audio does not block gameplay or crash when assets are missing (graceful degradation and one-time warnings).
- ✅ Centralized manager supports real-time changes (volume updates apply immediately; settings menu controls AudioManager directly).
- ✅ Matches the envisioned architecture and event map used across `src/main.cpp` and UI integrations.


## API reference (high level)

Key methods (see `include/audio/audio_manager.h` for full signatures):
- `AudioManager::GetInstance()` — access singleton
- `Initialize()` / `Shutdown()` — lifecycle
- `Update(float dt)` — per-frame updates
- `PlayMusic(AudioCue cue, bool loop, float fade_seconds = 0.0f)`
- `StopMusic(float fade_seconds = 0.0f)`
- `PlaySFX(AudioCue cue, float volume = 1.0f)`
- `PlayAmbient(AudioCue cue, bool loop, float fade_seconds = 0.0f)`
- `SetMasterVolume(float)` / `GetMasterVolume()`
- `SetVolume(AudioType, float)` / `GetVolume(AudioType)`
- `IsInitialized()` — check initialization state


## Audio cues (catalog)

Common cues used across the project (defined in `AudioCue` enum):

Menu Sounds:
- `MenuClick`, `MenuConfirm`, `MenuError`, `MenuOpen`, `MenuClose`

Gameplay / Event Sounds:
- `FacilityPlace`, `FacilityDemolish`, `ElevatorDing`, `ElevatorChime`, `Construction`, `ConstructionComplete`

Notifications / Achievements:
- `Achievement`, `Milestone`, `Toast`, `Alert`

Errors:
- `InvalidPlacement`, `ErrorBuzz`

Music Tracks:
- `MainTheme`, `GameplayLoop`, `VictoryTheme`, `Fanfare`

(Refer to `include/audio/audio_manager.h` for the full enum listing.)

---

## Quick reference examples

Play music (looping with fade-in):

```cpp
audio.PlayMusic(towerforge::audio::AudioCue::MainTheme, true, 1.0f); // fade-in 1s
```

Stop music with fade-out:

```cpp
audio.StopMusic(1.0f); // fade-out 1s
```

Play SFX:

```cpp
audio.PlaySFX(towerforge::audio::AudioCue::MenuClick);
```

Adjust volumes:

```cpp
audio.SetMasterVolume(0.9f);
audio.SetVolume(towerforge::audio::AudioType::Music, 0.6f);
```

Check initialization:

```cpp
if (!audio.IsInitialized()) {
    // fallback behavior
}
```

---

## Asset organization

Recommended layout under the repo root:

```
assets/audio/
├── music/
│   ├── main_theme.ogg
│   ├── gameplay_loop.ogg
│   └── ...
└── sfx/
    ├── menu_click.wav
    ├── facility_place.wav
    └── ...
```

Supported formats (current): `.ogg` for music, `.wav` for SFX — follow the project's conventions in `AudioManager::GetAudioFilePath()`.

The system tolerates missing assets and logs a single warning per absent file.

---

## Integration points

- `main.cpp` and all game loops must call `audio_manager.Update(dt)`.
- `AudioSettingsMenu` (UI) reads/writes volumes and test-plays SFX when sliders are adjusted.
- Event mappings (examples in `src/main.cpp`): menu navigation sounds, building placement/demolish, achievements, and music transitions between title and gameplay.

Example event map (non-exhaustive):
- Title Screen → `MainTheme`
- Start Game → crossfade to `GameplayLoop`
- Menu Navigation → `MenuClick` / `MenuConfirm`
- Building Placed → `FacilityPlace`
- Achievement → `Achievement` SFX

---

## Implementation notes & file layout

Files of interest:
- `include/audio/audio_manager.h` — public API and enums
- `src/audio/audio_manager.cpp` — implementation (loading, caching, Update loop)
- `include/ui/audio_settings_menu.h` / `src/ui/audio_settings_menu.cpp` — settings integration
- `CMakeLists.txt` — audio manager added to builds (e.g., `screenshot_app`)

Key implementation decisions:
- Singleton for global access
- Manual volume tracking and fade logic
- Load-once with failure-tracking to avoid log spam
- Separate ambient channel for environmental audio

## Build system & files changed

Notable build/integration changes required to add the audio manager:

- `CMakeLists.txt` updated to include `src/audio/audio_manager.cpp` in builds (for `towerforge` and `screenshot_app`).
- New/modified source and header files:
  - `include/audio/audio_manager.h` (public API, `AudioCue` enum)
  - `src/audio/audio_manager.cpp` (implementation)
  - `include/ui/audio_settings_menu.h` / `src/ui/audio_settings_menu.cpp` (settings integration)
  - Small additions in `src/main.cpp` to initialize AudioManager and call `Update()` in loops

File changes summary (approx):
- New: `include/audio/audio_manager.h`, `src/audio/audio_manager.cpp`
- Modified: `include/ui/audio_settings_menu.h`, `src/ui/audio_settings_menu.cpp`, `src/main.cpp`, `CMakeLists.txt`


## Testing & runtime behavior

- System initializes even when assets are missing; missing files produce one-time warnings but do not crash the game.
- With no assets, volume controls and calls are safe and do not throw.
- Update() must be called each frame to keep music streams advancing.

Test notes:
- `screenshot_app` and `towerforge` builds include the audio manager.
- To test headless screenshot app behavior, use an environment with no audio device or a virtual audio device; audio code is resilient.

Recommended tests:
- Save/Load tests should ensure audio manager state does not persist unintended side-effects.
- Play a set of SFX concurrently to check mixing/limits.
- Verify crossfade timings and fade-ins/outs under rapid state changes.

---

## Known limitations & future improvements

Current limitations:
- Implementation is synchronous; large assets may pause the main thread on first load.
- No advanced mixer or ducking implemented yet.
- No asset pack is included by default.

Suggested improvements:
1. Async preloading of common cues
2. Playlist and adaptive music support
3. Spatial (3D) audio for positional sounds
4. Mixer/ducking to prioritize SFX over music
5. LRU caching with memory limits
6. Include a small CC0 audio pack for CI and demos

---

## Troubleshooting

- If audio doesn't play: ensure `audio_manager.Initialize()` is called and `IsInitialized()` returns true.
- Check `assets/audio/` paths and filenames.
- If a file fails to load, the manager logs a single warning for that file; inspect logs to find missing files.

---

## References

- Implementation summary: `docs/AUDIO_IMPLEMENTATION_SUMMARY.md` (kept only for history)
- System docs: `docs/AUDIO_SYSTEM.md` (kept only for history)
- Quick reference: `docs/AUDIO_QUICK_REFERENCE.md` (kept only for history)
- API: `include/audio/audio_manager.h`

---

This file now serves as the authoritative audio reference. The older split documents are preserved in git history and may be removed from the working tree to avoid duplication.

## Conclusion

The audio system implementation is complete and production-ready as a core feature. The consolidated `docs/AUDIO.md` now contains architectural rationale, API guidance, usage examples, integration points, testing notes, and the acceptance criteria summary. Older split docs exist only for history and can be archived or removed from the working tree once you've verified links and references.
