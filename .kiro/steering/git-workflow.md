---
inclusion: always
---

# Git Workflow and Commit Protocol

## Core Principle

**All completed work must be committed and pushed immediately to ensure version control and backup.**

## Commit Requirements

After completing any of the following:
- Feature implementation
- Bug fix
- Code refactoring
- Documentation update
- Configuration change
- Test addition

You must:
1. Verify the build succeeds
2. Commit with a descriptive message
3. Push to remote repository

## Standard Workflow

```bash
# 1. Verify build succeeds
make

# 2. Stage changes
git add .

# 3. Commit with descriptive message
git commit -m "Clear description of changes"

# 4. Push to remote
git push
```

## Critical Build Verification

### Before Every Commit

**ALWAYS run `make` to verify successful compilation and linking.**

If `make` fails:
- **DO NOT commit**
- Fix the build errors first
- Retry `make` until successful

### Common Build Failure: Executable In Use

If you see "Permission denied" or linking fails:
- The executable is currently running
- **STOP** - Do not attempt to commit
- **WAIT** for the user to close the running program
- Retry `make` once the program is closed
- Only commit after successful build

**Never commit broken builds or incomplete linking.**

## Commit Message Guidelines

### Format
```
<type>: <short summary>

[optional detailed explanation]
```

### Types
- `feat`: New feature
- `fix`: Bug fix
- `refactor`: Code restructuring without behavior change
- `docs`: Documentation changes
- `test`: Test additions or modifications
- `build`: Build system or dependency changes
- `chore`: Maintenance tasks

### Writing Good Messages

**DO:**
- Use present tense: "Add feature" not "Added feature"
- Be specific: "Fix mailbox action handler" not "Fix bug"
- Explain why when helpful: "Refactor parser to use std::optional for clearer null handling"
- Keep first line under 72 characters
- Add details in body if needed

**DON'T:**
- Be vague: "Update code" or "Fix stuff"
- Use past tense: "Fixed the parser"
- Omit context: "Changes" or "WIP"

### Examples

**Good:**
```
feat: Add TAKE verb with object validation

Implements the TAKE verb handler with proper checks for:
- Object TAKEBIT flag
- Object location (must be accessible)
- Inventory weight limits
- Container capacity

Includes unit tests for edge cases.
```

**Good (simple):**
```
fix: Correct room exit direction for North of House
```

**Bad:**
```
updated files
```

## When to Commit

### Commit Frequently
- After implementing a complete feature
- After fixing a bug (one commit per fix)
- After refactoring a module
- Before switching tasks
- At the end of each work session
- When tests pass

### Atomic Commits
Each commit should represent **one logical change**:
- ✓ "Add mailbox object with action handler"
- ✓ "Fix parser tokenization for multi-word commands"
- ✗ "Add mailbox, fix parser, update docs, refactor verbs" (too much)

### Work-in-Progress
If you must save incomplete work:
```bash
git add .
git commit -m "WIP: Parser enhancement (incomplete)"
git push
```

Then amend or squash before merging to main branch.

## Multi-File Changes

When changes span multiple files:

```bash
# Stage related files together
git add src/parser/parser.cpp src/parser/parser.h tests/parser_tests.cpp
git commit -m "feat: Add object recognition to parser"

# Or stage everything if it's all related
git add .
git commit -m "refactor: Reorganize world building code"
```

## Handling Mistakes

### Wrong Commit Message
```bash
git commit --amend -m "Corrected message"
git push --force  # Only if not pushed yet, or on feature branch
```

### Forgot to Add Files
```bash
git add forgotten_file.cpp
git commit --amend --no-edit
```

### Need to Undo Last Commit (Not Pushed)
```bash
git reset --soft HEAD~1  # Keeps changes staged
# or
git reset HEAD~1         # Keeps changes unstaged
```

## Branch Strategy

### Main Branch
- Always keep `main` or `master` in working state
- All commits should build successfully
- All tests should pass

### Feature Branches (Optional)
For larger features:
```bash
git checkout -b feature/parser-enhancement
# ... make changes ...
git commit -m "feat: Add syntax pattern matching"
git push -u origin feature/parser-enhancement
```

Then merge via pull request after review.

## Integration with Development

### After Each Task Completion

1. **Build**: `make` (verify success)
2. **Test**: Run relevant tests if available
3. **Review**: Quickly review changes with `git diff`
4. **Stage**: `git add .` or `git add <specific files>`
5. **Commit**: `git commit -m "descriptive message"`
6. **Push**: `git push`

### Before Starting New Task

```bash
git status  # Ensure clean working directory
git pull    # Get latest changes
```

## Commit Frequency Guidelines

**Too Infrequent:**
- ✗ One commit per day with "Daily progress"
- ✗ Waiting until feature is "perfect"
- ✗ Accumulating many unrelated changes

**Good Frequency:**
- ✓ Commit after each logical unit of work
- ✓ 3-10 commits per work session
- ✓ Each commit represents a complete thought

**Too Frequent:**
- ✗ Commit after every line change
- ✗ "Save point" commits every few minutes
- ✗ Commits with no meaningful change

## Summary

The git workflow ensures:
1. **No lost work** - Everything is backed up
2. **Clear history** - Easy to understand what changed and why
3. **Revertible changes** - Can undo mistakes
4. **Collaboration ready** - Others can see progress
5. **Professional practice** - Industry-standard version control

**Remember: Build first, commit second, push third. Always.**
