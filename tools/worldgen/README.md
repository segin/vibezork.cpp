# worldgen

Extracts the game data out of the original ZIL sources in `zil/` and emits it
as C++ so the port's world is the ZIL's world rather than a transcription of
it.  The data is keyed by ZIL name; `src/world/zil_registry.cpp` maps those
names onto the port's `ObjectId` constants, flag bits and routines, and
`src/world/zil_loader.cpp` builds the rooms and objects from them.

## Regenerating

From the repository root:

```sh
python3 tools/worldgen/zilgen.py  --zil zil -o tools/worldgen/zil_world.json
python3 tools/worldgen/emit_cpp.py -i tools/worldgen/zil_world.json --cpp-dir src/world/generated
python3 tools/worldgen/emit_ids.py -i tools/worldgen/zil_world.json --src-dir src --cpp-dir src/world/generated
python3 tools/worldgen/melee_tables.py --zil zil -o tools/worldgen/zil_tables.json --cpp-dir src/systems/generated
```

`zil_world.json` is committed: it is the reference the data-level test diffs
the live world against, so a hand edit to the generated C++ fails the build.

## Files

| Script | Role |
|---|---|
| `mdl.py` | MDL/ZIL reader: tokenizer, `;` comment elision, read-time `<COND ZORK-NUMBER>`, ZILCH string conversion, v3 dictionary truncation |
| `zilgen.py` | `zil/1dungeon.zil` + the OBJECT forms of `zil/gglobals.zil` -> `zil_world.json` |
| `emit_cpp.py` | `zil_world.json` -> `src/world/generated/world_data.{h,cpp}` |
| `emit_ids.py` | ZIL name -> `RoomIds::`/`ObjectIds::` constant table |
| `melee_tables.py` | the pure tables of `zil/1actions.zil` and `zil/gverbs.zil` (DEF\*, the melee message tables, BAT-DROPS, LAMP-TABLE, ...) |
| `cppgen.py` | shared C++ literal and identifier helpers |
| `z3dump.py` | dumps the object tree, property strings and dictionary out of `zil/COMPILED/zork1.z3` |
| `verify.py`, `verify_tables.py` | cross-check the JSON against the story file |

## Conversions the reader implements

* A newline inside a ZIL string is a space; `|` is a hard newline; a newline
  directly after `|` is dropped; indentation after a newline is kept.
* Dictionary keys are cut at six *z-characters*, not six letters: A2
  characters (digits, `#`, `-`, ...) cost two, so `FCD#3` is `fcd#`,
  `AIR-PUMP` is `air-p` and `#COMMAND` is `#comm`.
* Absent `SIZE`/`CAPACITY`/`VALUE`/`TVALUE` take the `PROPDEF` defaults of
  `zork1.zil` (SIZE 5, the rest 0).
* `VTYPE` holds a flag name, not a number (`INFLATED-BOAT` has
  `(VTYPE NONLANDBIT)`).
* Object numbers in the story file are not definition order, but each
  parent's child chain is the *reverse* of definition order, which is what
  `FIRST?`/`NEXT?` walks such as "take all" follow.
* `GLOBAL-OBJECTS`, `LOCAL-GLOBALS` and `ROOMS` are compiler artifacts that
  exist to allocate flags and parent the globals; `IT`, `INTNUM`,
  `NOT-HERE-OBJECT` and `PSEUDO-OBJECT` belong to the parser.  Both are
  tagged in the JSON so the loader can skip them.
