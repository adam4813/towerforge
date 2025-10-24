# AI Agent Instructions for TowerForge

**Purpose**: Internal process guidelines for AI agents working on TowerForge  
**Audience**: AI assistants only (not user documentation)  
**Status**: Active

---

## ⚠️ MANDATORY FIRST STEPS - READ THIS BEFORE ANY ACTION ⚠️

### Step 1: Install System Dependencies (Linux ONLY)

**IF YOU ARE ON LINUX**, you MUST run this command FIRST before vcpkg, CMake, or any build steps:

```bash
sudo apt-get update && sudo apt-get install -y build-essential cmake pkg-config \
  libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev \
  libgl1-mesa-dev libglu1-mesa-dev xvfb
```

**Critical**: Without these X11 libraries, vcpkg CANNOT build raylib and the project WILL FAIL.

**On Windows/macOS**: Skip this step.

### Step 2: Bootstrap vcpkg

```bash
# Clone vcpkg if not present
git clone https://github.com/microsoft/vcpkg.git

# Bootstrap
./vcpkg/bootstrap-vcpkg.sh      # Linux/macOS
vcpkg\bootstrap-vcpkg.bat       # Windows

# Set environment
export VCPKG_ROOT=/path/to/vcpkg    # Linux/macOS
set VCPKG_ROOT=D:\path\to\vcpkg     # Windows
```

### Step 3: Configure CMake

```bash
cmake --preset cli-native
```

**These steps are NOT optional. Complete them before proceeding with any task.**

---

## Core Directives

### 1. Required Reading Before Any Work

**Read these pattern/API guides immediately when working in their domains**:
- **UI work**: Read `UI_DEVELOPMENT_BIBLE.md` - declarative, reactive, event-driven patterns
- **Testing work**: Read `TESTING.md` - test structure, priorities, and best practices
- **Future**: API documentation describing engine usage patterns (not yet created)

These are **reference documentation** that define correct implementation patterns. Don't improvise - follow the documented patterns.

### 2. Planning is Internal Only
- **Never create plan documentation files** unless explicitly requested
- Plans, strategies, and implementation approaches are working memory only
- Execute directly without documenting the plan
- Exception: AGENTS.md (this file) is allowed as it improves AI process

### 3. Temporary Files Must Be Cleaned Up
- **Never leave temporary test programs, scripts, or exploration code** in the repository
- If you create temporary files for investigation (e.g., testing a hypothesis, exploring an API):
  - Delete them immediately after use
  - Use `.gitignore`d locations if they must persist briefly
- Clean up at the end of your task before reporting completion
- Examples of temporary files to avoid: test programs, debug scripts, scratch files

### 4. Documentation Policy
**NEVER create summary documents, reports, or reviews** of work completed. These are working memory only.

**Do NOT create** unless explicitly requested:
- HOWTOs, tutorials, guides
- Implementation summaries, work summaries, review summaries
- Design notes or architecture docs
- Supplementary documentation of any kind
- Status reports, completion reports, or any "summary" documents

**Only create documentation if**:
- User explicitly requests it (e.g., "create a document explaining X")
- It documents a human workflow/process users must follow (e.g., build instructions, setup steps)
- It directly improves automated tooling or AI code generation

**Rule of thumb**: If you're just going to tell the user about it anyway, don't create a file for it.

### 5. No Standalone Applications
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
1. Configure (first time): `cmake --preset cli-native`
2. Build: `cmake --build --preset cli-native-debug --parallel $(nproc)`
   - Windows: Use `%NUMBER_OF_PROCESSORS%` instead of `$(nproc)`
3. Verify: No errors or warnings
4. If errors: Fix and rebuild
5. Do not mark complete until build succeeds

**Note**: Use `cli-native` presets (not `native`) to avoid conflicts with IDE builds.
Build directory: `build-cli/cli-native/` (isolated from IDE's `build/native/`)

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

#### Modern C++20 Requirements
- **C++20 features**: Use ranges, concepts, modules (when stable), coroutines (when appropriate)
- **RAII**: All resource management (files, memory, locks) via RAII types
- **Smart pointers**: 
  - `std::unique_ptr` for exclusive ownership
  - `std::shared_ptr` only when truly shared ownership needed
  - Raw pointers for non-owning references
  - Never use `new`/`delete` directly
- **Containers**: Prefer standard library containers over manual memory management
- **Algorithms**: Use `<algorithm>` and `<ranges>` over raw loops when clearer

#### Modern C++ Patterns
```cpp
// CORRECT: Use ranges instead of raw loops
auto filtered = data 
    | std::views::filter([](const auto& item) { return item.active; })
    | std::views::transform([](const auto& item) { return item.value; });

// CORRECT: Smart pointers for ownership
auto component = std::make_unique<Component>();
panel->AddChild(std::move(component));  // Transfer ownership

// CORRECT: RAII for resources
class ResourceManager {
    std::unique_ptr<Resource> resource_;  // Automatically cleaned up
};

// CORRECT: Small, focused functions
auto FindEntity(int id) const -> std::optional<Entity> {
    auto it = std::ranges::find_if(entities_, 
        [id](const auto& e) { return e.id == id; });
    return it != entities_.end() ? std::optional{*it} : std::nullopt;
}
```

#### Composition Over Inheritance
- **Prefer composition** for building functionality
- **Use inheritance** only for polymorphism (Gang of Four patterns like Strategy, Observer, Composite)
- Keep inheritance hierarchies shallow (max 2-3 levels)

```cpp
// CORRECT: Composition for building features
class BuildingManager {
    TowerGrid grid_;                    // Has-a
    FacilityManager facility_mgr_;      // Has-a
    EconomySystem economy_;             // Has-a
};

// CORRECT: Inheritance for polymorphism (Composite pattern)
class UIElement {
public:
    virtual void Render() const = 0;
    virtual bool ProcessEvent(const Event& e) = 0;
};

class Button : public UIElement { /* ... */ };
class Panel : public UIElement { /* ... */ };

// WRONG: Inheritance for code reuse
class StorageFacility : public Facility {  // Avoid - use composition instead
};
```

#### Gang of Four Patterns (Use When Appropriate)
- **Composite**: UI element trees (see `UI_DEVELOPMENT_BIBLE.md`)
- **Observer**: Event callbacks, reactive updates
- **Strategy**: Interchangeable algorithms
- **Factory**: Entity/component creation
- **Singleton**: Use sparingly, only for true single instances (settings, managers)

#### Code Organization
- **Small functions**: Max 20-30 lines, single responsibility
- **Comments**: Only for complex logic or non-obvious design decisions
- **Const correctness**: Use `const` everywhere possible
- **Type safety**: Prefer `enum class` over plain `enum` or integers

---

## File Organization

### Pattern/API Documentation
- `UI_DEVELOPMENT_BIBLE.md` - UI component patterns and Gang of Four design patterns
- `TESTING.md` - Test structure, priorities, best practices
- `HELP_SYSTEM_SUMMARY.md` - Help system implementation patterns
- Future: Additional API guides for engine subsystems

These define **how to use the engine**. Read the relevant guide before working in that domain.

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
- Only for complex logic explanation or non-obvious design decisions
- Don't comment on "what" changed - that's in version control
- Don't explain obvious code
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
- [ ] Clean up any temporary files created during investigation
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
