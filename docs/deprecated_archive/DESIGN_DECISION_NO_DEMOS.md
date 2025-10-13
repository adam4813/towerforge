# Design Decision: No Standalone Demo Applications

## Decision

We will not create standalone demo applications separate from the main application for testing or showcasing features.

## Rationale

1. **Single Source of Truth**: The main application (`towerforge`) serves as the primary demonstration of all features. Having multiple demo applications can lead to:
   - Code duplication
   - Maintenance burden (keeping demos in sync with main application)
   - Confusion about which application to use

2. **Integration Focus**: Features should be demonstrated in the context of the full application where they will actually be used. This ensures:
   - Features are tested in realistic conditions
   - Integration issues are discovered early
   - Users see features working together, not in isolation

3. **Reduced Build Complexity**: Each additional executable:
   - Increases build time
   - Adds to binary distribution size
   - Complicates the build configuration

4. **Documentation Over Demos**: Feature documentation with code examples is more maintainable and useful than standalone demo applications:
   - Documentation can be updated independently of builds
   - Code examples show integration patterns
   - Users can copy examples directly into their code

## Implementation

- The main `towerforge` application includes all features and serves as the demonstration
- Feature documentation includes:
  - Usage examples
  - Integration patterns
  - Visual references (ASCII art, screenshots when available)
  - API documentation

## Exceptions

The only exception to this rule is the `screenshot_app`, which serves a specific purpose:
- Generates documentation screenshots in headless environments
- Not intended for user interaction
- Used only during documentation generation
- Clearly documented as a build tool, not a demo

## Applied To

This decision was applied to the HUD system implementation, where a standalone `hud_demo` application was initially created but subsequently removed in favor of integrating the HUD demonstration into the main application.

## Related Documentation

- See `docs/HUD_SYSTEM.md` for HUD usage examples
- See `src/main.cpp` for HUD integration in the main application
- See `README.md` for running the main application with HUD features
