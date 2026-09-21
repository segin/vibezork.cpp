#!/usr/bin/env python3
"""zilgen.py - extract the Zork I world model from zil/1dungeon.zil + zil/gglobals.zil.

Usage: zilgen.py [--zil DIR] [-o zil_world.json]

Output JSON (all strings already converted with mdl.zstring; vocabulary words kept
exactly as written in the source, plus their v3 dictionary key):

{
  "source":      {file: sha256, ...},
  "directions":  ["NORTH", ...],
  "globals":     [{name, value, file, line}],           # scalar GLOBALs (flags, SCORE-MAX ...)
  "tables":      [{name, kind, pure, elements, file, line}],  # HOUSE-AROUND, FOREST-AROUND, ...
  "constants":   [{name, value, file, line}],
  "propdefs":    {SIZE: 5, ...}                          # from zork1.zil
  "routines":    [{name, file, line}],                   # routines defined inside the two data files
  "objects":     [ObjectRec, ...]                        # ROOMs and OBJECTs in ZIL definition order
  "vocab":       {WORD-as-written: dictionary-key, ...}
  "unhandled":   [str, ...]                              # anything the extractor did not understand
}

ObjectRec:
  def_index   0-based position in definition order (1DUNGEON.ZIL then GGLOBALS.ZIL,
              the INSERT-FILE order of zork1.zil).  NOTE: the story file's object
              *numbers* are NOT in this order (ZILCH numbers by OBLIST hash order);
              what definition order determines is the child-chain order inside each
              parent: zork1.z3 links children in REVERSE definition order (the last
              defined child is FIRST?), verified by verify.py for every multi-child
              parent including ROOMS, GLOBAL-OBJECTS and LOCAL-GLOBALS.
  kind        "ROOM" | "OBJECT"
  role        "game" | "compiler-artifact" | "parser"   (see ROLE below)
  name, file, line
  in          parent object name or null            ("IN" clause; rooms are IN ROOMS)
  desc, ldesc, fdesc, text   converted strings or null when the property is absent
  flags       [ZIL flag names]                       (order as written)
  synonyms, adjectives      [words as written]
  size, capacity, value, tvalue, strength, vtype      int or null when absent
              (zork1.zil PROPDEF defaults: SIZE 5, CAPACITY 0, VALUE 0, TVALUE 0;
               GETP of an absent property returns the default)
  action, descfcn, contfcn, advfcn   routine name, null when absent, 0 when written as 0
  globals     [object names]                        (room GLOBAL clause)
  pseudo      [[word, routine], ...]
  exits       [{dir, kind, to, text, flag, door, routine, line}]
              kind: UEXIT (DIR TO room) | NEXIT (DIR "text") | CEXIT (DIR TO room IF flag [ELSE "text"])
                    DEXIT (DIR TO room IF door IS OPEN [ELSE "text"]) | FEXIT (DIR PER routine)
  extra       {KEY: raw clause} for any property this script does not know
"""
import argparse, hashlib, json, os, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mdl
from mdl import Atom, Str, Form, List, is_form, zstring, zchar_key

DEFAULT_ZIL = '/home/segin/vibezork.cpp/zil'

STRING_PROPS = ('DESC', 'LDESC', 'FDESC', 'TEXT')
INT_PROPS = ('SIZE', 'CAPACITY', 'VALUE', 'TVALUE', 'STRENGTH', 'VTYPE')
FCN_PROPS = ('ACTION', 'DESCFCN', 'CONTFCN', 'ADVFCN')
WORD_PROPS = ('SYNONYM', 'ADJECTIVE')

# ROLE: objects that exist only to make the ZIL compiler / parser work.
ROLE = {
    'GLOBAL-OBJECTS': 'compiler-artifact',   # holds every FLAG so the compiler allocates them; parent of globals
    'LOCAL-GLOBALS': 'compiler-artifact',    # parent of per-room GLOBAL objects; its properties exist only to allocate P?FDESC etc.
    'ROOMS': 'compiler-artifact',            # parent of all rooms; "(IN TO ROOMS)" only forces the IN direction property to exist
    'INTNUM': 'parser', 'PSEUDO-OBJECT': 'parser', 'IT': 'parser', 'NOT-HERE-OBJECT': 'parser',
}


def sha256(path):
    return hashlib.sha256(open(path, 'rb').read()).hexdigest()


class Extractor:
    def __init__(self):
        self.model = dict(source={}, directions=[], globals=[], tables=[], constants=[], propdefs={},
                          routines=[], objects=[], vocab={}, unhandled=[])
        self.def_index = 0

    # ------------------------------------------------------------------ files
    def load(self, path, only=None):
        fname = os.path.basename(path)
        self.model['source'][fname] = sha256(path)
        objs = mdl.eval_read_macros(mdl.parse(open(path, encoding='latin-1').read()), zork_number=1)
        for o in objs:
            if isinstance(o, Str):
                continue                                   # "SUBTITLE ..." banners
            if isinstance(o, Atom):
                if o == '/^L':
                    continue                               # gglobals.zil:88 - a form feed mangled to "/^L" in this copy of the source
                if o.strip():
                    self.model['unhandled'].append(f"{fname}: stray atom {o!r}")
                continue
            if not is_form(o) or not o or not isinstance(o[0], Atom):
                self.model['unhandled'].append(f"{fname}:{getattr(o, 'line', '?')}: {type(o).__name__} {str(o)[:60]!r}")
                continue
            head = o[0]
            if only and head not in only:
                continue
            handler = getattr(self, 'h_' + head.replace('-', '_').replace('?', '_'), None)
            if handler:
                handler(o, fname)
            else:
                self.model['unhandled'].append(f"{fname}:{o.line}: <{head} ...>")

    # --------------------------------------------------------------- top forms
    def h_DIRECTIONS(self, f, fname):
        self.model['directions'] = [str(a) for a in f[1:]]

    def h_GLOBAL(self, f, fname):
        name = str(f[1]); val = f[2] if len(f) > 2 else None
        if is_form(val, 'LTABLE') or is_form(val, 'TABLE') or is_form(val, 'PLTABLE') or is_form(val, 'PTABLE'):
            self.model['tables'].append(dict(name=name, file=fname, line=f.line, **table(val)))
        else:
            self.model['globals'].append(dict(name=name, value=scalar(val), file=fname, line=f.line))

    def h_CONSTANT(self, f, fname):
        self.model['constants'].append(dict(name=str(f[1]), value=scalar(f[2]), file=fname, line=f.line))

    def h_PROPDEF(self, f, fname):
        self.model['propdefs'][str(f[1])] = scalar(f[2])

    def h_ROUTINE(self, f, fname):
        self.model['routines'].append(dict(name=str(f[1]), file=fname, line=f.line))

    def h_GDECL(self, f, fname):
        pass

    def h_ROOM(self, f, fname):
        self.h_OBJECT(f, fname, kind='ROOM')

    def h_OBJECT(self, f, fname, kind='OBJECT'):
        name = str(f[1])
        rec = dict(def_index=self.def_index, kind=kind, role=ROLE.get(name, 'game'), name=name, file=fname, line=f.line,
                   **{'in': None}, desc=None, ldesc=None, fdesc=None, text=None, flags=[], synonyms=[], adjectives=[],
                   size=None, capacity=None, value=None, tvalue=None, strength=None, vtype=None,
                   action=None, descfcn=None, contfcn=None, advfcn=None, globals=[], pseudo=[], exits=[], extra={})
        self.def_index += 1
        dirs = set(self.model['directions'])
        for clause in f[2:]:
            if not isinstance(clause, List) or not clause or not isinstance(clause[0], Atom):
                self.model['unhandled'].append(f"{fname}:{f.line}: {name}: odd clause {clause!r}")
                continue
            key = str(clause[0]); rest = clause[1:]
            if key in dirs and not (key == 'IN' and rest and isinstance(rest[0], Atom) and rest[0] != 'TO' and rest[0] != 'PER'):
                ex = self.exit(clause, rest, name, fname)
                if ex:
                    rec['exits'].append(ex)
                continue
            if key == 'IN':
                rec['in'] = str(rest[0])
            elif key in STRING_PROPS:
                rec[key.lower()] = zstring(rest[0])
            elif key in INT_PROPS:
                rec[key.lower()] = rest[0]
            elif key in FCN_PROPS:
                rec[key.lower()] = 0 if rest[0] == 0 else str(rest[0])
            elif key == 'FLAGS':
                rec['flags'] = [str(a) for a in rest]
            elif key == 'SYNONYM':
                rec['synonyms'] = [self.word(a) for a in rest]
            elif key == 'ADJECTIVE':
                rec['adjectives'] = [self.word(a) for a in rest]
            elif key == 'GLOBAL':
                rec['globals'] = [str(a) for a in rest]
            elif key == 'PSEUDO':
                rec['pseudo'] = [[zstring(rest[i]), str(rest[i + 1])] for i in range(0, len(rest), 2)]
            else:
                rec['extra'][key] = [scalar(x) for x in rest]
                self.model['unhandled'].append(f"{fname}:{clause.line}: {name}: unknown property {key}")
        self.model['objects'].append(rec)

    def word(self, a):
        w = str(a)
        self.model['vocab'][w] = zchar_key(w)
        return w

    def exit(self, clause, rest, obj, fname):
        d = str(clause[0]); line = clause.line
        ex = dict(dir=d, kind=None, to=None, text=None, flag=None, door=None, routine=None, line=line)
        try:
            if isinstance(rest[0], Str):
                ex['kind'] = 'NEXIT'; ex['text'] = zstring(rest[0]); assert len(rest) == 1
            elif rest[0] == 'PER':
                ex['kind'] = 'FEXIT'; ex['routine'] = str(rest[1]); assert len(rest) == 2
            elif rest[0] == 'TO':
                ex['to'] = str(rest[1])
                if len(rest) == 2:
                    ex['kind'] = 'UEXIT'
                else:
                    assert rest[2] == 'IF'
                    if len(rest) > 4 and rest[4] == 'IS':
                        assert rest[5] == 'OPEN'
                        ex['kind'] = 'DEXIT'; ex['door'] = str(rest[3]); tail = rest[6:]
                    else:
                        ex['kind'] = 'CEXIT'; ex['flag'] = str(rest[3]); tail = rest[4:]
                    if tail:
                        assert tail[0] == 'ELSE' and len(tail) == 2 and isinstance(tail[1], Str)
                        ex['text'] = zstring(tail[1])
            else:
                raise AssertionError('unknown exit shape')
        except (AssertionError, IndexError) as e:
            self.model['unhandled'].append(f"{fname}:{line}: {obj}: exit clause {clause!r} ({e})")
            return None
        return ex


def scalar(v):
    if v is None:
        return None
    if isinstance(v, Form) and len(v) == 0:
        return False                                   # <> = FALSE
    if isinstance(v, Str):
        return zstring(v)
    if isinstance(v, (Atom, int)):
        return v if isinstance(v, int) else str(v)
    if isinstance(v, Form):
        return {'form': [scalar(x) for x in v]}
    if isinstance(v, List):
        return {'list': [scalar(x) for x in v]}
    return str(v)


def table(f):
    """<TABLE/LTABLE [(PURE ...)] elem...> -> dict(kind, pure, elements). Nested tables recurse."""
    kind = str(f[0]); items = f[1:]; pure = False; flags = []
    if items and isinstance(items[0], List):
        flags = [str(x) for x in items[0]]; pure = 'PURE' in flags; items = items[1:]
    els = []
    for x in items:
        if isinstance(x, Str):
            els.append({'str': zstring(x)})
        elif isinstance(x, int):
            els.append({'int': x})
        elif isinstance(x, Atom):
            els.append({'atom': str(x)})
        elif is_form(x) and len(x) == 0:
            els.append({'atom': None})                # <> inside a table (VILLAINS: no best weapon)
        elif is_form(x) and str(x[0]) in ('TABLE', 'LTABLE', 'PTABLE', 'PLTABLE'):
            els.append({'table': table(x)})
        else:
            els.append({'raw': repr(x)})
    return dict(kind=kind, pure=pure, table_flags=flags, elements=els)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--zil', default=DEFAULT_ZIL)
    ap.add_argument('-o', '--output', default='zil_world.json')
    a = ap.parse_args()
    ex = Extractor()
    ex.h_PROPDEF(Form([Atom('PROPDEF'), Atom('SIZE'), 5]), 'zork1.zil')      # from zork1.zil, between the two INSERT-FILEs
    for p in ('CAPACITY', 'VALUE', 'TVALUE'):
        ex.h_PROPDEF(Form([Atom('PROPDEF'), Atom(p), 0]), 'zork1.zil')
    ex.load(os.path.join(a.zil, '1dungeon.zil'))
    ex.load(os.path.join(a.zil, 'gglobals.zil'), only={'OBJECT', 'ROOM', 'GLOBAL', 'CONSTANT', 'ROUTINE'})
    m = ex.model
    rooms = [o for o in m['objects'] if o['kind'] == 'ROOM']
    objs = [o for o in m['objects'] if o['kind'] == 'OBJECT']
    json.dump(m, open(a.output, 'w'), indent=1)
    print(f"{a.output}: {len(rooms)} rooms, {len(objs)} objects "
          f"({sum(o['file'] == '1dungeon.zil' for o in objs)} in 1dungeon, {sum(o['file'] == 'gglobals.zil' for o in objs)} in gglobals), "
          f"{sum(len(o['exits']) for o in rooms)} exits, {len(m['vocab'])} vocabulary words, "
          f"{len(m['globals'])} globals, {len(m['tables'])} tables, {len(m['unhandled'])} unhandled")
    for u in m['unhandled']:
        print("  unhandled:", u)


if __name__ == '__main__':
    main()
