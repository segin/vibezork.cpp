# PsyMP3 Agent Directives

This file provides guidance for AI agents working on the PsyMP3 codebase.

## Steering Documents

**Before making any changes, read all Markdown files in [`.kiro/steering/`](.kiro/steering/):**

- [`cpp-modern-features.md`](.kiro/steering/cpp-modern-features.md) — C++ coding standards and modern feature usage
- [`git-workflow.md`](.kiro/steering/git-workflow.md) — Git commit and workflow conventions

These steering documents are the authoritative source for project conventions and must be followed.

## Quick Reference

- **Build**: `make -j8`
- **Test**: `make check`
- **Reconfigure**: `autoreconf -fi && ./configure [options]`

## Additional Notes

For specifications and task definitions, see [`.kiro/specs/`](.kiro/specs/).
