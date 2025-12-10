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
