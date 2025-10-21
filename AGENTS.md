# AI Agent Instructions for TowerForge

**Purpose**: Internal process guidelines for AI agents working on TowerForge  
**Audience**: AI assistants only (not user documentation)  
**Status**: Active

---

## Core Directives

### 1. Planning is Internal Only
- **Never create plan documentation files** unless explicitly requested
- Plans, strategies, and implementation approaches are working memory only
- Execute directly without documenting the plan
- Exception: AGENTS.md (this file) is allowed as it improves AI process

### 2. Documentation Policy
**Do NOT create** unless explicitly requested:
- HOWTOs, tutorials, guides
- Implementation summaries
- Design notes or architecture docs
- Supplementary documentation of any kind

**Only create documentation if**:
- User explicitly requests it
- It documents human workflow/process users must follow
- It directly improves automated tooling or AI code generation

### 3. No Standalone Applications
- Main `towerforge` binary is the only demo/showcase
- Do not create standalone test applications
- Exception: `screenshot_app` (already exists for CI)
- Test features by integrating into existing code (e.g., main menu)

---

## Execution Protocol

### Before Any Task
1. Read request carefully
2. Identify files to modify (surgical changes only)
3. Execute changes directly without creating plan documents
4. Build and test

### Making Changes
- **Minimal modifications only**: Change as few lines as possible
- Do not delete/remove/modify working code unless absolutely necessary
- Ignore unrelated bugs or broken tests
- Update documentation only if directly related to changes

### Build Verification (REQUIRED)
Before completing any work:
1. Build: `cmake --build --preset native-debug`
2. Verify: No errors or warnings
3. If errors: Fix and rebuild
4. Do not mark complete until build succeeds

---

## Project Constraints

### Technical Requirements
- **C++20**: Strict requirement
- **CMake 3.20+**: Required
- **vcpkg**: Use for dependencies
- **Raylib**: UI rendering
- **flecs**: ECS framework

### Platform Support
- Windows: MSVC or MinGW-w64 GCC 10+
- Linux: GCC 10+ or Clang 10+
- macOS: Xcode 12+ / Clang 10+

### Code Standards
- RAII for resource management
- Smart pointers over raw pointers
- Small, focused functions
- Standard library algorithms preferred
- Comment only complex logic

---

## File Organization

### Build Outputs
- `build/bin/towerforge` - Main executable
- `build/bin/screenshot_app` - CI tool only

### Source Structure
- `src/` - Implementation files
- `include/` - Headers
- `docs/` - User-facing documentation only
- `screenshots/` - Visual documentation
- `mods/` - Modding system

---

## When Testing Features

### Strategy
1. Integrate into existing code (e.g., main menu)
2. Use existing UI components
3. Test within the main application
4. Do not create separate test executables

### Example: Testing Batch Renderer
- ✅ Modify `src/ui/main_menu.cpp` to use batch renderer
- ❌ Create `tests/test_batch_renderer.cpp` standalone app

---

## Documentation Guidelines

### User Documentation (README.md)
- Focus on usage and gameplay
- User manual style
- Not implementation details

### Code Comments
- Minimal, only for complex logic
- Self-documenting code preferred

### Screenshots
- Add to `screenshots/` when adding features
- Visual documentation of user-facing changes

---

## Process Checklist

When given a task:
- [ ] Understand requirement (no plan doc needed)
- [ ] Identify minimal changes
- [ ] Execute changes directly
- [ ] Build and verify
- [ ] No standalone tests/demos created
- [ ] No unnecessary documentation created
- [ ] Changes are surgical and minimal

---

## Error Handling

### Build Errors
- Fix immediately
- Rebuild to verify
- Do not proceed until clean build

### Breaking Changes
- Avoid at all costs
- Opt-in features only
- Maintain backward compatibility

---

## Key Principles

1. **Execute, don't document the plan**
2. **Minimal changes, surgical precision**
3. **Test in production code, not standalone apps**
4. **Build verification is mandatory**
5. **No supplementary documentation**
6. **When in doubt, ask the user**

---

**Status**: This file is the only internal process documentation permitted. Follow these directives strictly.
