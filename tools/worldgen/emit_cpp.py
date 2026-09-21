#!/usr/bin/env python3
"""emit_cpp.py - turn zil_world.json (from zilgen.py) into world_data.h / world_data.cpp.

Usage: emit_cpp.py [-i zil_world.json] [--cpp-dir DIR]

The generated code depends only on <cstdint>, <optional>, <span>, <string_view>.
Routine references (ACTION, DESCFCN, CONTFCN, FEXIT PER, PSEUDO handlers) are
plain ZIL names; the loader resolves them through a registry.  Flag names are
the ZIL flag names.  Tables are in ZIL definition order.
"""
import argparse, json, os, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from cppgen import cstr, ident, banner

HEADER = '''#pragma once
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

namespace zork::zil {

// One clause of a ROOM's exit list.  Field use per kind:
//   UEXIT  (DIR TO room)                                 to
//   NEXIT  (DIR "text")                                  text
//   CEXIT  (DIR TO room IF flag [ELSE "text"])           to, flag, text
//   DEXIT  (DIR TO room IF door IS OPEN [ELSE "text"])   to, door, text
//   FEXIT  (DIR PER routine)                             routine
enum class ExitKind : std::uint8_t { UEXIT, NEXIT, CEXIT, DEXIT, FEXIT };

struct ExitDef {
    std::string_view dir;       // DIRECTIONS name (NORTH ... LAND)
    ExitKind kind;
    std::string_view to{};      // target room
    std::string_view text{};    // NEXIT message or CEXIT/DEXIT ELSE message; empty = none
    std::string_view flag{};    // CEXIT global flag name
    std::string_view door{};    // DEXIT door object name
    std::string_view routine{}; // FEXIT routine name
    std::uint16_t line{};       // source line in 1dungeon.zil
};

// A vocabulary word: `text` exactly as written in the ZIL SYNONYM/ADJECTIVE
// clause, `key` the Z-machine v3 dictionary form (lower case, cut at six
// z-characters; A2 characters such as '#', '-' and digits cost two).  Register
// both: the key is what player input matches after the same truncation.
struct Word { std::string_view text; std::string_view key; };

struct PseudoDef { std::string_view word; std::string_view routine; };

// Game: a real room/object.  CompilerArtifact: GLOBAL-OBJECTS, LOCAL-GLOBALS,
// ROOMS - exist to allocate flags/properties and to parent globals; never
// visible in play.  Parser: IT, INTNUM, NOT-HERE-OBJECT, PSEUDO-OBJECT.
enum class ObjRole : std::uint8_t { Game, CompilerArtifact, Parser };

using OptInt = std::optional<std::int16_t>;   // nullopt = property absent in the source

struct RoomDef {
    std::uint16_t def_index;                    // position in ZIL definition order (shared with ObjectDef)
    std::string_view name;
    std::string_view desc{};
    std::string_view ldesc{};
    std::span<const std::string_view> flags{};
    OptInt value{};
    std::string_view action{};
    std::span<const std::string_view> globals{};
    std::span<const PseudoDef> pseudo{};
    std::span<const ExitDef> exits{};
    std::uint16_t line{};
};

struct ObjectDef {
    std::uint16_t def_index;
    std::string_view name;
    ObjRole role;
    std::string_view in{};                      // parent object name; empty = no IN clause
    std::span<const Word> synonyms{};
    std::span<const Word> adjectives{};
    std::string_view desc{};
    std::string_view fdesc{};
    std::string_view ldesc{};
    std::string_view text{};
    std::span<const std::string_view> flags{};
    OptInt size{};                              // absent -> PROPDEF default (kPropDefaults)
    OptInt capacity{};
    OptInt value{};
    OptInt tvalue{};
    OptInt strength{};
    std::string_view vtype{};                   // vehicle flag name, e.g. NONLANDBIT (LOCAL-GLOBALS writes the literal 1)
    std::string_view action{};                  // empty for absent and for "(ACTION 0)"
    std::string_view descfcn{};
    std::string_view contfcn{};                 // (ADVFCN is omitted: it occurs once, as 0, on LOCAL-GLOBALS)
    std::span<const std::string_view> globals{};    // only LOCAL-GLOBALS uses it
    std::span<const PseudoDef> pseudo{};            // only LOCAL-GLOBALS uses it
    std::span<const ExitDef> exits{};               // only ROOMS "(IN TO ROOMS)" uses it
    std::string_view file{};
    std::uint16_t line{};
};

struct GlobalDef { std::string_view name; std::int16_t value; };          // <> is stored as 0
struct WalkTable { std::string_view name; std::span<const std::string_view> rooms; };
struct PropDefault { std::string_view prop; std::int16_t value; };

extern const std::span<const std::string_view> kDirections;    // <DIRECTIONS ...> order
extern const std::span<const RoomDef> kRooms;                  // definition order
extern const std::span<const ObjectDef> kObjects;              // definition order, 1dungeon.zil then gglobals.zil
// Object tree order: zork1.z3 links each parent's children in REVERSE definition
// order (verified for every multi-child parent, ROOMS/GLOBAL-OBJECTS/LOCAL-GLOBALS
// included).  To reproduce FIRST?/NEXT? walks ("take all", thief, room
// descriptions) insert each definition at the FRONT of its parent's chain while
// walking kRooms/kObjects merged by def_index.
extern const std::span<const GlobalDef> kGlobals;              // scalar GLOBALs of 1dungeon.zil (flags, SCORE-MAX)
extern const std::span<const WalkTable> kWalkTables;           // HOUSE-AROUND, FOREST-AROUND, IN-HOUSE-AROUND, ABOVE-GROUND
extern const std::span<const PropDefault> kPropDefaults;       // zork1.zil PROPDEFs
extern const std::span<const Word> kVocabulary;                // every distinct SYNONYM/ADJECTIVE word
extern const std::span<const std::string_view> kFlagNames;     // every flag name used, first-use order
extern const std::span<const std::string_view> kRoutineNames;  // every routine name referenced by the data

inline constexpr std::size_t kRoomCount = {rooms};
inline constexpr std::size_t kObjectCount = {objects};
inline constexpr std::size_t kDefinitionCount = kRoomCount + kObjectCount;

}  // namespace zork::zil
'''


def sv_array(name, ty, items, out):
    if not items:
        return None
    out.append(f'constexpr {ty} {name}[] = {{{", ".join(items)}}};')
    return name


def exit_cell(e):
    parts = [f'.dir = {cstr(e["dir"])}', f'.kind = ExitKind::{e["kind"]}']
    for k in ('to', 'text', 'flag', 'door', 'routine'):
        if e[k] is not None:
            parts.append(f'.{k} = {cstr(e[k])}')
    parts.append(f'.line = {e["line"]}')
    return '{' + ', '.join(parts) + '}'


def emit(model, cpp_dir):
    objs = model['objects']; vocab = model['vocab']
    rooms = [o for o in objs if o['kind'] == 'ROOM']; items = [o for o in objs if o['kind'] == 'OBJECT']
    sources = ['zil/1dungeon.zil', 'zil/gglobals.zil']
    c = [banner('emit_cpp.py', sources), '#include "world_data.h"', '', 'namespace zork::zil {', 'namespace {', '']
    flag_names = []; routine_names = []

    def note_routine(r):
        if r and r != 0 and r not in routine_names:
            routine_names.append(r)

    def sub_arrays(o):
        base = ident(o['name'], '')
        refs = {}
        for f in o['flags']:
            if f not in flag_names:
                flag_names.append(f)
        refs['flags'] = sv_array(f'kFl_{base}', 'std::string_view', [cstr(f) for f in o['flags']], c)
        refs['synonyms'] = sv_array(f'kSyn_{base}', 'Word', [f'{{{cstr(w)}, {cstr(vocab[w])}}}' for w in o['synonyms']], c)
        refs['adjectives'] = sv_array(f'kAdj_{base}', 'Word', [f'{{{cstr(w)}, {cstr(vocab[w])}}}' for w in o['adjectives']], c)
        refs['globals'] = sv_array(f'kGl_{base}', 'std::string_view', [cstr(g) for g in o['globals']], c)
        refs['pseudo'] = sv_array(f'kPs_{base}', 'PseudoDef', [f'{{{cstr(w)}, {cstr(r)}}}' for w, r in o['pseudo']], c)
        for _, r in o['pseudo']:
            note_routine(r)
        for e in o['exits']:
            note_routine(e['routine'])
        refs['exits'] = sv_array(f'kEx_{base}', 'ExitDef', [exit_cell(e) for e in o['exits']], c)
        return refs

    room_rows = []
    for o in rooms:
        refs = sub_arrays(o)
        for k in ('action',):
            note_routine(o[k])
        row = [f'.def_index = {o["def_index"]}', f'.name = {cstr(o["name"])}']
        if o['desc'] is not None: row.append(f'.desc = {cstr(o["desc"])}')
        if o['ldesc'] is not None: row.append(f'.ldesc = {cstr(o["ldesc"])}')
        if refs['flags']: row.append(f'.flags = {refs["flags"]}')
        if o['value'] is not None: row.append(f'.value = {o["value"]}')
        if o['action']: row.append(f'.action = {cstr(o["action"])}')
        if refs['globals']: row.append(f'.globals = {refs["globals"]}')
        if refs['pseudo']: row.append(f'.pseudo = {refs["pseudo"]}')
        if refs['exits']: row.append(f'.exits = {refs["exits"]}')
        row.append(f'.line = {o["line"]}')
        room_rows.append('    {' + ', '.join(row) + '},')
        for k in ('in',):
            assert o[k] == 'ROOMS', o
        assert not o['synonyms'] and not o['adjectives'] and o['text'] is None and o['fdesc'] is None, o['name']
    c += ['', f'constexpr RoomDef kRoomsArr[] = {{'] + room_rows + ['};', '']

    role = {'game': 'ObjRole::Game', 'compiler-artifact': 'ObjRole::CompilerArtifact', 'parser': 'ObjRole::Parser'}
    obj_rows = []
    for o in items:
        refs = sub_arrays(o)
        for k in ('action', 'descfcn', 'contfcn'):
            note_routine(o[k])
        row = [f'.def_index = {o["def_index"]}', f'.name = {cstr(o["name"])}', f'.role = {role[o["role"]]}']
        if o['in'] is not None: row.append(f'.in = {cstr(o["in"])}')
        if refs['synonyms']: row.append(f'.synonyms = {refs["synonyms"]}')
        if refs['adjectives']: row.append(f'.adjectives = {refs["adjectives"]}')
        for k in ('desc', 'fdesc', 'ldesc', 'text'):
            if o[k] is not None: row.append(f'.{k} = {cstr(o[k])}')
        if refs['flags']: row.append(f'.flags = {refs["flags"]}')
        for k in ('size', 'capacity', 'value', 'tvalue', 'strength'):
            if o[k] is not None: row.append(f'.{k} = {o[k]}')
        if o['vtype'] is not None: row.append(f'.vtype = {cstr(str(o["vtype"]))}')
        for k in ('action', 'descfcn', 'contfcn'):
            if o[k]: row.append(f'.{k} = {cstr(o[k])}')
        if refs['globals']: row.append(f'.globals = {refs["globals"]}')
        if refs['pseudo']: row.append(f'.pseudo = {refs["pseudo"]}')
        if refs['exits']: row.append(f'.exits = {refs["exits"]}')
        row.append(f'.file = {cstr(o["file"])}'); row.append(f'.line = {o["line"]}')
        obj_rows.append('    {' + ', '.join(row) + '},')
    c += ['', f'constexpr ObjectDef kObjectsArr[] = {{'] + obj_rows + ['};', '']

    c.append(f'constexpr std::string_view kDirectionsArr[] = {{{", ".join(cstr(d) for d in model["directions"])}}};')
    gl = []
    for g in model['globals']:
        v = g['value']
        gl.append(f'{{{cstr(g["name"])}, {0 if v is False else (v if isinstance(v, int) else 0)}}}')
    c.append(f'constexpr GlobalDef kGlobalsArr[] = {{{", ".join(gl)}}};')
    for t in model['tables']:
        c.append(f'constexpr std::string_view {ident(t["name"], "kTab_")}[] = {{{", ".join(cstr(e["atom"]) for e in t["elements"])}}};')
    c.append('constexpr WalkTable kWalkTablesArr[] = {' + ', '.join(f'{{{cstr(t["name"])}, {ident(t["name"], "kTab_")}}}' for t in model['tables']) + '};')
    c.append('constexpr PropDefault kPropDefaultsArr[] = {' + ', '.join(f'{{{cstr(k)}, {v}}}' for k, v in model['propdefs'].items()) + '};')
    c.append('constexpr Word kVocabularyArr[] = {' + ', '.join(f'{{{cstr(w)}, {cstr(k)}}}' for w, k in vocab.items()) + '};')
    c.append(f'constexpr std::string_view kFlagNamesArr[] = {{{", ".join(cstr(f) for f in flag_names)}}};')
    c.append(f'constexpr std::string_view kRoutineNamesArr[] = {{{", ".join(cstr(r) for r in routine_names)}}};')
    c += ['', '}  // namespace', '',
          'const std::span<const std::string_view> kDirections = kDirectionsArr;',
          'const std::span<const RoomDef> kRooms = kRoomsArr;',
          'const std::span<const ObjectDef> kObjects = kObjectsArr;',
          'const std::span<const GlobalDef> kGlobals = kGlobalsArr;',
          'const std::span<const WalkTable> kWalkTables = kWalkTablesArr;',
          'const std::span<const PropDefault> kPropDefaults = kPropDefaultsArr;',
          'const std::span<const Word> kVocabulary = kVocabularyArr;',
          'const std::span<const std::string_view> kFlagNames = kFlagNamesArr;',
          'const std::span<const std::string_view> kRoutineNames = kRoutineNamesArr;',
          '', f'static_assert(std::size(kRoomsArr) == kRoomCount);', f'static_assert(std::size(kObjectsArr) == kObjectCount);',
          '', '}  // namespace zork::zil', '']
    h = banner('emit_cpp.py', sources) + HEADER.replace('{rooms}', str(len(rooms))).replace('{objects}', str(len(items)))
    open(os.path.join(cpp_dir, 'world_data.h'), 'w').write(h)
    open(os.path.join(cpp_dir, 'world_data.cpp'), 'w').write('\n'.join(c))
    return len(rooms), len(items), len(flag_names), len(routine_names)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('-i', '--input', default='zil_world.json')
    ap.add_argument('--cpp-dir', default='.')
    a = ap.parse_args()
    r, o, f, rt = emit(json.load(open(a.input)), a.cpp_dir)
    print(f"world_data.h/.cpp: {r} rooms, {o} objects, {f} flag names, {rt} routine names")


if __name__ == '__main__':
    main()
