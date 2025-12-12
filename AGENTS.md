# vibezork.cpp Agent Directives

This file provides guidance for AI agents working on the vibezork.cpp codebase.

## Steering Documents

**Before making any changes, read all Markdown files in [`.kiro/steering/`](.kiro/steering/):**

- [`cpp-modern-features.md`](.kiro/steering/cpp-modern-features.md) — C++ coding standards and modern feature usage
- [`git-workflow.md`](.kiro/steering/git-workflow.md) — Git commit and workflow conventions

These steering documents are the authoritative source for project conventions and must be followed.

## Quick Reference

- **Build**: `cmake --build build -j$(nproc)`
- **Test**: `ctest --test-dir build`

## Additional Notes

For specifications and task definitions, see [`.kiro/specs/`](.kiro/specs/).

## User Directives

- **Never use a simpler approach**: Always choose the correct, hard, comprehensive solution, no matter how arduous. Never compromise architectural integrity for convenience or speed. Simpler approaches that sacrifice correctness or robustness are strictly forbidden.

- **NEVER batch mark checklist items**: When implementing items from a checklist (e.g., ZIL actions, tests, features), you MUST implement, test, and commit EACH ITEM INDIVIDUALLY before marking it as complete. Batch marking multiple items at once is STRICTLY FORBIDDEN. This includes:
  - Never mark an item as complete without implementing it first
  - Never mark multiple items in a single commit
  - Never skip testing to "catch up" on checklist marking
  - Each individual item requires: implement → test → verify tests pass → commit → push → mark checkbox
  - This rule applies even if it seems tedious or time-consuming

- **Checklist progress tracking workflow**: When working through checklists, use the following status markers:
  - `[ ]` = Not started
  - `[/]` = In progress (currently being worked on)
  - `[x]` = Complete (verified and committed)
  
  **Workflow order:**
  1. Mark next item as `[/]` BEFORE starting work on it
  2. Implement and test the item
  3. Verify tests pass
  4. Commit and push the changes
  5. Change `[/]` to `[x]` for completed item
  6. Update progress counters
  7. Repeat from step 1 for next item

## ZIL Action Verification Protocol

**Purpose**: Systematic verification of ZIL action implementations against original ZIL source code to ensure fidelity.

**Per-Action Workflow**:

1. **Mark as In-Progress** (`[/]` in `docs/zil_actions.md`)
   
2. **Review ZIL Source**:
   - Locate in `zil/1actions.zil` or `zil/gglobals.zil`
   - Document line numbers and exact behavior
   - Note all handled verbs and state changes

3. **Implement/Update C++ Code**:
   - Match ZIL behavior exactly
   - Include ZIL source reference in comments
   - Format: `// ZIL: <behavior>` and `// Source: <file>:<lines>`

4. **Create Dedicated Unit Tests**:
   - Create new test file: `tests/<action_name>_tests.cpp`
   - Test suite name: `<ActionName>Fcn`
   - Minimum tests:
     * Handled verbs return TRUE with correct behavior
     * Unhandled verbs return FALSE
     * State changes verified (TVALUE, LDESC, flags, etc.)
     * Edge cases (null object, wrong object)
   - Add test target to `CMakeLists.txt`

5. **Build and Run Tests**:
   ```bash
   cd build
   make -j8 <action_name>_tests
   ./<action_name>_tests
   ```
   - ALL tests must pass before proceeding

6. **Commit and Push**:
   ```bash
   git add -A
   git commit -m "Implement <ACTION-NAME> with comprehensive unit tests
   
   **Implementation**:
   - <verb> handler behavior
   - State changes per ZIL spec
   
   **Tests**:
   - Test descriptions
   
   **Build System**:
   - Added <action_name>_tests target
   
   Per systematic ZIL action verification protocol."
   git push
   ```

7. **Mark Complete** (`[x]` in `docs/zil_actions.md`)
   - Update progress counters

8. **Repeat** for next action

**Example**: PAINTING-FCN verification (commit f48c660):
- ZIL Source: `1actions.zil:2207-2214`
- Behavior: V_MUNG sets TVALUE=0, changes LDESC
- Tests: 7 comprehensive tests in `painting_tests.cpp`
- All tests passed, committed individually

**CRITICAL**: Never skip tests, never batch multiple actions in one commit.
