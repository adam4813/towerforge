# Copilot Instructions for TowerForge

**👉 READ THIS FIRST, THEN GO TO `AGENTS.md` FOR COMPLETE INSTRUCTIONS 👈**

## ⚠️ MANDATORY FIRST STEPS - DO THESE IMMEDIATELY ⚠️

### Step 1: Install System Dependencies (Linux ONLY)

**IF YOU ARE ON LINUX**, run this FIRST before vcpkg, CMake, or any build steps:

```bash
sudo apt-get update && sudo apt-get install -y build-essential cmake pkg-config \
  libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev \
  libgl1-mesa-dev libglu1-mesa-dev xvfb
```

**Why**: Raylib requires X11 libraries. Without these, vcpkg CANNOT build raylib and the project WILL FAIL.

**On Windows/macOS**: Skip this step.

### Step 2: Install and Bootstrap vcpkg

```bash
# Clone vcpkg if not present
git clone https://github.com/microsoft/vcpkg.git

# Bootstrap vcpkg
./vcpkg/bootstrap-vcpkg.sh      # Linux/macOS
vcpkg\bootstrap-vcpkg.bat       # Windows (cmd.exe)

# Set environment variable
export VCPKG_ROOT=/path/to/vcpkg    # Linux/macOS
set VCPKG_ROOT=D:\path\to\vcpkg     # Windows (cmd.exe)
```

### Step 3: Configure CMake with CLI Preset

```bash
cmake --preset cli-native
```

**This uses `build-cli/cli-native/` directory (isolated from IDE builds in `build/native/`).**

---

## Quick Project Summary

**TowerForge** - SimTower-inspired skyscraper simulation game

**Tech Stack**:
- C++20 (strict requirement)
- ECS: flecs
- Rendering: Raylib
- Build: CMake 3.20+ with vcpkg

**Key Points**:
- Use `cli-native` preset for CLI/Copilot builds (NOT `native` - that's for IDE)
- Use `cli-test` preset for tests (NOT `test` - that's for IDE)
- Always build and verify before completing work: `cmake --build --preset cli-native-debug`

---

## 📖 For Complete Instructions

**All detailed guidelines are in `AGENTS.md`** including:
- Execution protocol and workflow
- Build verification checklist
- Coding standards (C++20, RAII, smart pointers, small functions)
- Modern C++ best practices with examples
- File organization
- Testing guidelines
- Documentation policy

**Go read `AGENTS.md` now** - it contains everything you need to work on this project.
