#!/usr/bin/env python3
"""melee_tables.py - extract the pure data tables of zil/1actions.zil and zil/gverbs.zil.

Usage: melee_tables.py [--zil DIR] [-o zil_tables.json] [--cpp-dir DIR]

Extracts (from the whole files, parsed with mdl.py; the line ranges named in the
TODO - 1actions.zil 3236-3330 and 3606-3803 - are where the melee data lives):
  constants   F-BUSY? F-DEAD F-UNCONSCIOUS F-CONSCIOUS F-FIRST?, MISSED..SITTING-DUCK,
              STRENGTH-MAX STRENGTH-MIN CURE-WAIT, F-WEP F-DEF, V-VILLAIN..V-MSGS
  tables      DEF1 DEF2A DEF2B DEF3A DEF3B DEF3C, DEF1-RES DEF2-RES DEF3-RES (plus the
              <PUT ,DEFn-RES i <REST ,DEFx n>> fix-ups GO performs at start-up),
              HERO-MELEE CYCLOPS-MELEE TROLL-MELEE THIEF-MELEE, VILLAINS,
              BAT-DROPS DROWNINGS CYCLOMAD LAMP-TABLE CANDLE-TABLE BDIGS RIVER-SPEEDS
              RIVER-NEXT RIVER-LAUNCH LOUD-RUNS (1actions),
              JUMPLOSS WHEEEEE HO-HUM HELLOS YUKS DUMMY SWIMYUKS (gverbs)
and writes zil_tables.json plus zil_tables.h / zil_tables.cpp.
"""
import argparse, json, os, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mdl
from mdl import Atom, Str, Form, List, is_form
from zilgen import table, scalar, sha256, DEFAULT_ZIL
from cppgen import cstr, ident, banner

WANTED_1ACTIONS = ['DEF1', 'DEF2A', 'DEF2B', 'DEF3A', 'DEF3B', 'DEF3C', 'DEF1-RES', 'DEF2-RES', 'DEF3-RES',
                   'HERO-MELEE', 'CYCLOPS-MELEE', 'TROLL-MELEE', 'THIEF-MELEE', 'VILLAINS',
                   'BAT-DROPS', 'DROWNINGS', 'CYCLOMAD', 'LAMP-TABLE', 'CANDLE-TABLE', 'BDIGS',
                   'RIVER-SPEEDS', 'RIVER-NEXT', 'RIVER-LAUNCH', 'LOUD-RUNS']
WANTED_GVERBS = ['JUMPLOSS', 'WHEEEEE', 'HO-HUM', 'HELLOS', 'YUKS', 'DUMMY', 'SWIMYUKS']
WANTED_CONSTANTS = ['F-BUSY?', 'F-DEAD', 'F-UNCONSCIOUS', 'F-CONSCIOUS', 'F-FIRST?',
                    'MISSED', 'UNCONSCIOUS', 'KILLED', 'LIGHT-WOUND', 'SERIOUS-WOUND', 'STAGGER', 'LOSE-WEAPON',
                    'HESITATE', 'SITTING-DUCK', 'STRENGTH-MAX', 'STRENGTH-MIN', 'CURE-WAIT', 'F-WEP', 'F-DEF',
                    'V-VILLAIN', 'V-BEST', 'V-BEST-ADV', 'V-PROB', 'V-MSGS']
BLOW_RESULTS = ['MISSED', 'UNCONSCIOUS', 'KILLED', 'LIGHT-WOUND', 'SERIOUS-WOUND', 'STAGGER', 'LOSE-WEAPON',
                'HESITATE', 'SITTING-DUCK']


def collect(path, wanted_tables):
    src = open(path, encoding='latin-1').read()
    objs = mdl.eval_read_macros(mdl.parse(src), zork_number=1)
    consts, tables, other_tables, routines = {}, {}, [], {}
    for o in objs:
        if not is_form(o) or not o or not isinstance(o[0], Atom):
            continue
        if o[0] == 'CONSTANT':
            consts[str(o[1])] = dict(value=scalar(o[2]), line=o.line)
        elif o[0] == 'GLOBAL' and len(o) > 2 and is_form(o[2]) and o[2] and str(o[2][0]) in ('TABLE', 'LTABLE', 'PTABLE', 'PLTABLE'):
            name = str(o[1])
            if name in wanted_tables:
                tables[name] = dict(line=o.line, file=os.path.basename(path), **table(o[2]))
            else:
                other_tables.append(f"{os.path.basename(path)}:{o.line} {name}")
        elif o[0] == 'ROUTINE':
            routines[str(o[1])] = o
    return consts, tables, other_tables, routines


def go_fixups(go_form):
    """<PUT ,DEFn-RES idx <REST ,DEFx nbytes>> inside GO -> [{table, index, source, offset_words}]."""
    out = []

    def walk(f):
        if is_form(f, 'PUT') and len(f) == 4 and is_form(f[3], 'REST'):
            out.append(dict(table=str(f[1]).lstrip(','), index=f[2], source=str(f[3][1]).lstrip(','), offset_words=f[3][2] // 2))
        if isinstance(f, list):
            for x in f:
                walk(x)
    walk(go_form)
    return out


# ------------------------------------------------------------------ C++ emission
def melee_cpp(name, t, out_h, out_cpp, fdef_consts):
    """TABLE(PURE) of LTABLE(PURE) of LTABLE(PURE) of (string | F-WEP | F-DEF)."""
    base = ident(name)
    result_names = []
    for ri, res in enumerate(t['elements']):
        assert 'table' in res, (name, res)
        msg_names = []
        for mi, msg in enumerate(res['table']['elements']):
            assert 'table' in msg, (name, ri, msg)
            parts = []
            for el in msg['table']['elements']:
                if 'str' in el:
                    parts.append(f'{{{cstr(el["str"])}, MeleePart::Text}}')
                elif 'atom' in el and el['atom'] == 'F-WEP':
                    parts.append('{{}, MeleePart::Weapon}')
                elif 'atom' in el and el['atom'] == 'F-DEF':
                    parts.append('{{}, MeleePart::Defender}')
                else:
                    raise ValueError(f"{name}: unexpected melee element {el}")
            n = f'{base}_{ri}_{mi}'
            out_cpp.append(f'constexpr MeleePart {n}[] = {{{", ".join(parts)}}};')
            msg_names.append(n)
        n = f'{base}_{ri}'
        out_cpp.append(f'constexpr MeleeMsg {n}[] = {{{", ".join(msg_names)}}};')
        result_names.append(n)
    out_cpp.append(f'constexpr MeleeResult {base}_results[] = {{{", ".join(result_names)}}};')
    out_cpp.append(f'}}  // namespace\nconst std::span<const MeleeResult> {name.replace("-", "_")} = {base}_results;\nnamespace {{')
    out_h.append(f'extern const std::span<const MeleeResult> {name.replace("-", "_")};  // {t["file"]}:{t["line"]}, {len(result_names)} result rows')


def strings_of(t):
    return [e['str'] for e in t['elements'] if 'str' in e]


def emit_cpp(model, cpp_dir):
    tabs = model['tables']; consts = model['constants']
    h = [banner('melee_tables.py', ['zil/1actions.zil', 'zil/gverbs.zil', 'zil/1dungeon.zil (GO fix-ups)']),
         '#pragma once', '#include <cstdint>', '#include <span>', '#include <string_view>', '',
         'namespace zork::zil {', '',
         '// ---- fight-state slot indexes (F-*) and other constants, values as in 1actions.zil']
    for name in ['F-BUSY?', 'F-DEAD', 'F-UNCONSCIOUS', 'F-CONSCIOUS', 'F-FIRST?', 'STRENGTH-MAX', 'STRENGTH-MIN', 'CURE-WAIT',
                 'F-WEP', 'F-DEF', 'V-VILLAIN', 'V-BEST', 'V-BEST-ADV', 'V-PROB', 'V-MSGS']:
        h.append(f'inline constexpr int {ident(name)} = {consts[name]["value"]};')
    h += ['', '// ---- blow results (1actions.zil "blow results")', 'enum class BlowResult : std::uint8_t {']
    for name in BLOW_RESULTS:
        h.append(f'    {name.replace("-", "_")} = {consts[name]["value"]},')
    h += ['};', '',
          '// ---- DEFn tables: TABLE (PURE) of blow results, indexed by the melee dice roll.']
    for name in ['DEF1', 'DEF2A', 'DEF2B', 'DEF3A', 'DEF3B', 'DEF3C']:
        t = tabs[name]
        vals = ', '.join('BlowResult::' + e['atom'].replace('-', '_') for e in t['elements'])
        h.append(f'inline constexpr BlowResult {ident(name)}[] = {{{vals}}};  // {t["file"]}:{t["line"]}')
    h += ['',
          '// ---- DEFn-RES: rows of table slices selected by defender strength.  The ZIL source',
          '// holds 0 placeholders that GO (1dungeon.zil) patches with <REST ,DEFx n>; the',
          '// resolved form is given here: {table name, first element index}.  An empty name',
          '// is a slot GO never fills (stays 0 in the story).',
          'struct DefSlice { std::string_view table; int offset; };']
    fix = {(f['table'], f['index']): f for f in model['go_fixups']}
    for name in ['DEF1-RES', 'DEF2-RES', 'DEF3-RES']:
        t = tabs[name]; cells = []
        for i, e in enumerate(t['elements']):
            if 'atom' in e:
                cells.append(f'{{"{e["atom"]}", 0}}')
            elif (name, i) in fix:
                f = fix[(name, i)]; cells.append(f'{{"{f["source"]}", {f["offset_words"]}}}')
            else:
                cells.append('{{}, 0}')
        h.append(f'inline constexpr DefSlice {ident(name)}[] = {{{", ".join(cells)}}};  // {t["file"]}:{t["line"]}')
    h += ['',
          '// ---- melee message tables: TABLE (PURE) of result rows (element index i = ZIL',
          '// <GET tbl i>, i.e. blow result - 1); each row is an LTABLE of messages; each',
          '// message is an LTABLE of parts printed in order: text, F-WEP (the weapon name),',
          '// or F-DEF (the defender name).',
          'struct MeleePart { enum Kind : std::uint8_t { Text, Weapon, Defender }; std::string_view text; Kind kind; };',
          'using MeleeMsg = std::span<const MeleePart>;',
          'using MeleeResult = std::span<const MeleeMsg>;']
    c = [banner('melee_tables.py', ['zil/1actions.zil', 'zil/gverbs.zil']), '#include "zil_tables.h"', '',
         'namespace zork::zil {', 'namespace {']
    for name in ['HERO-MELEE', 'CYCLOPS-MELEE', 'TROLL-MELEE', 'THIEF-MELEE']:
        melee_cpp(name, tabs[name], h, c, consts)
    c += ['}  // namespace']
    # VILLAINS
    h += ['', '// ---- VILLAINS: LTABLE of <TABLE villain best-weapon best-adv prob msgs> (1actions.zil)',
          'struct VillainDef { std::string_view villain; std::string_view best_weapon; int best_adv; int prob; std::string_view msgs; };']
    rows = []
    for e in tabs['VILLAINS']['elements']:
        v = e['table']['elements']
        rows.append(f'{{"{v[0]["atom"]}", {cstr(v[1]["atom"]) if v[1]["atom"] else "{}"}, {v[2]["int"]}, {v[3]["int"]}, "{v[4]["atom"]}"}}')
    h.append(f'inline constexpr VillainDef {ident("VILLAINS")}[] = {{{", ".join(rows)}}};  // 1actions.zil:{tabs["VILLAINS"]["line"]}')
    # simple string tables
    h += ['', '// ---- message tables.  For <LTABLE 0 ...> (PICK-ONE tables) the leading 0 cursor',
          '// slot is omitted; only the messages are listed, in ZIL order.']
    for name in ['DROWNINGS', 'CYCLOMAD', 'BDIGS', 'JUMPLOSS', 'WHEEEEE', 'HO-HUM', 'HELLOS', 'YUKS', 'DUMMY', 'SWIMYUKS']:
        t = tabs[name]
        pick = t['kind'] == 'LTABLE' and t['elements'] and t['elements'][0] == {'int': 0}
        h.append(f'inline constexpr std::string_view {ident(name)}[] = {{{", ".join(cstr(s) for s in strings_of(t))}}};'
                 f'  // {t["file"]}:{t["line"]} {t["kind"]}{" (PICK-ONE)" if pick else ""}')
    h += ['', '// ---- room lists (object names)']
    for name in ['BAT-DROPS', 'LOUD-RUNS', 'RIVER-NEXT']:
        t = tabs[name]
        pick = t['kind'] == 'LTABLE' and t['elements'] and t['elements'][0] == {'int': 0}
        atoms = [e['atom'] for e in t['elements'] if 'atom' in e]
        h.append(f'inline constexpr std::string_view {ident(name)}[] = {{{", ".join(cstr(a) for a in atoms)}}};'
                 f'  // {t["file"]}:{t["line"]} {t["kind"]}{" (leading 0 slot omitted)" if pick else ""}')
    h += ['', '// ---- light-source timers: (turns-remaining, message) steps, 0 terminated as in ZIL',
          'struct TimerStep { int turns; std::string_view text; };']
    for name in ['LAMP-TABLE', 'CANDLE-TABLE']:
        t = tabs[name]; els = t['elements']; cells = []
        i = 0
        while i < len(els):
            n = els[i]['int']
            if n == 0:
                cells.append('{0, {}}'); i += 1
            else:
                cells.append(f'{{{n}, {cstr(els[i + 1]["str"])}}}'); i += 2
        h.append(f'inline constexpr TimerStep {ident(name)}[] = {{{", ".join(cells)}}};  // {t["file"]}:{t["line"]}')
    h += ['', '// ---- river tables', 'struct RoomInt { std::string_view room; int value; };', 'struct RoomPair { std::string_view from; std::string_view to; };']
    t = tabs['RIVER-SPEEDS']; els = t['elements']
    h.append(f'inline constexpr RoomInt {ident("RIVER-SPEEDS")}[] = {{' +
             ', '.join(f'{{"{els[i]["atom"]}", {els[i + 1]["int"]}}}' for i in range(0, len(els), 2)) + f'}};  // {t["file"]}:{t["line"]}')
    t = tabs['RIVER-LAUNCH']; els = t['elements']
    h.append(f'inline constexpr RoomPair {ident("RIVER-LAUNCH")}[] = {{' +
             ', '.join(f'{{"{els[i]["atom"]}", "{els[i + 1]["atom"]}"}}' for i in range(0, len(els), 2)) + f'}};  // {t["file"]}:{t["line"]}')
    h += ['', '}  // namespace zork::zil', '']
    c += ['}  // namespace zork::zil', '']
    open(os.path.join(cpp_dir, 'zil_tables.h'), 'w').write('\n'.join(h))
    open(os.path.join(cpp_dir, 'zil_tables.cpp'), 'w').write('\n'.join(c))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--zil', default=DEFAULT_ZIL)
    ap.add_argument('-o', '--output', default='zil_tables.json')
    ap.add_argument('--cpp-dir', default='.')
    a = ap.parse_args()
    c1, t1, other1, r1 = collect(os.path.join(a.zil, '1actions.zil'), set(WANTED_1ACTIONS))
    c2, t2, other2, _ = collect(os.path.join(a.zil, 'gverbs.zil'), set(WANTED_GVERBS))
    _, _, other3, r3 = collect(os.path.join(a.zil, '1dungeon.zil'), set())
    consts = {k: v for k, v in c1.items() if k in WANTED_CONSTANTS}
    missing = [n for n in WANTED_CONSTANTS if n not in consts] + [n for n in WANTED_1ACTIONS if n not in t1] + [n for n in WANTED_GVERBS if n not in t2]
    model = dict(source={f: sha256(os.path.join(a.zil, f)) for f in ('1actions.zil', 'gverbs.zil', '1dungeon.zil')},
                 constants=consts, tables={**t1, **t2}, go_fixups=go_fixups(r3['GO']),
                 other_tables_not_extracted=other1 + other2, missing=missing)
    json.dump(model, open(a.output, 'w'), indent=1)
    emit_cpp(model, a.cpp_dir)
    print(f"{a.output}: {len(consts)} constants, {len(model['tables'])} tables, {len(model['go_fixups'])} GO fix-ups; missing: {missing or 'none'}")
    print("other GLOBAL tables present but not extracted:", *model['other_tables_not_extracted'], sep='\n  ')


if __name__ == '__main__':
    main()
