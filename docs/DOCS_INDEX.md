# Documentation Index (auto-merged)

This index lists consolidated documentation created and the original files marked as deprecated.

Consolidated (new canonical docs):

- `docs/HUD.md` - HUD: merged HUD_SYSTEM, HUD_IMPLEMENTATION_SUMMARY, HUD_README, HUD_VISUAL_LAYOUT, HUD_SCREENSHOT_REFERENCE
- `docs/AUDIO.md` - Audio: merged AUDIO_SYSTEM, AUDIO_IMPLEMENTATION_SUMMARY, AUDIO_QUICK_REFERENCE
- `docs/ELEVATOR.md` - Elevator: merged ELEVATOR_SYSTEM, ELEVATOR_IMPLEMENTATION_SUMMARY
- `docs/FACILITIES.md` - Facilities: merged FACILITIES and ADVANCED_FACILITIES
- `docs/PERSONS.md` - Persons & Movement: merged PERSON_MOVEMENT_SYSTEM and PERSON_IMPLEMENTATION_SUMMARY
- `docs/SAVE_LOAD.md` - Save/Load: merged SAVE_LOAD_SYSTEM, SAVE_LOAD_INTEGRATION, README_SAVE_LOAD
- `docs/IMPLEMENTATION.md` - Implementation overview and changelog: merged IMPLEMENTATION_SUMMARY and IMPLEMENTATION_NOTES

Original files marked DEPRECATED (left in place with deprecation header):

- HUD_SYSTEM.md, HUD_IMPLEMENTATION_SUMMARY.md, HUD_README.md, HUD_VISUAL_LAYOUT.md, HUD_SCREENSHOT_REFERENCE.md
- AUDIO_SYSTEM.md, AUDIO_IMPLEMENTATION_SUMMARY.md, AUDIO_QUICK_REFERENCE.md
- ELEVATOR_SYSTEM.md, ELEVATOR_IMPLEMENTATION_SUMMARY.md
- FACILITIES.md, ADVANCED_FACILITIES.md
- PERSON_MOVEMENT_SYSTEM.md, PERSON_IMPLEMENTATION_SUMMARY.md
- SAVE_LOAD_SYSTEM.md, SAVE_LOAD_INTEGRATION.md, README_SAVE_LOAD.md
- IMPLEMENTATION_SUMMARY.md, IMPLEMENTATION_NOTES.md

Other docs left unchanged (examples / remaining subsystems):

- `docs/BUILD_INSTRUCTIONS.md`
- `docs/CI_WORKFLOW.md`
- `docs/CAMERA_SYSTEM.md`
- `docs/COMMAND_PATTERN_HISTORY.md` - **NEW**: Command Pattern and History System
- `docs/DYNAMIC_FLOOR_SYSTEM.md`
- `docs/FIXES_QUICKREF.md`
- `docs/GAME_CLASS_ARCHITECTURE.md`
- `docs/GENERAL_SETTINGS_MENU_IMPLEMENTATION.md`
- `docs/MODDING_API.md`
- `docs/PAUSE_MENU_IMPLEMENTATION.md`
- `docs/PLACEMENT_SYSTEM.md` - **UPDATED**: Enhanced with Command Pattern documentation
- `docs/README_ACHIEVEMENTS.md`
- `docs/RESEARCH_TREE_SYSTEM.md`
- `docs/STAR_RATING_IMPLEMENTATION.md`
- `docs/TITLE_SCREEN_IMPLEMENTATION.md`
- `docs/TUTORIAL_IMPLEMENTATION.md`
- `docs/UI_REFACTORING_SUMMARY.md`
- `docs/WINDOW_FIXES_SUMMARY.md`

Notes on validation performed:
- After creating consolidated files, I ran the project's documentation checker for errors.
- Found invalid JSON placeholder tokens in `SAVE_LOAD_SYSTEM.md` and `README_SAVE_LOAD.md` (the `{"...": { ... }}` placeholders were not valid JSON). I replaced those with small valid JSON examples. The docs now pass the checker for those files.

Recommended next steps (optional):
- Replace references in code/comments to point to the merged docs (e.g., update README.md links if present).
- Consider consolidating additional subsystem docs (Research, Achievements, Tutorial) if you want fewer files.
- Run a spellcheck or manual pass to ensure the merged docs read smoothly with one another.

If you'd like, I can:
- Update `README.md` to point to the new consolidated docs
- Merge other subsystem docs into single-topic files (Research, Achievements, Tutorial)
- Generate a single `docs/TOC.md` and add navigation anchors

Tell me which of those you'd like me to do next, or I can proceed to update `README.md` automatically to reference the new canonical docs.
