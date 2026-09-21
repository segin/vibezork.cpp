#!/usr/bin/env python3
"""verify_tables.py - brute-force decode every word-aligned address of zork1.z3's
high memory and check that every string in zil_tables.json and every exit
message in zil_world.json occurs verbatim in the story file."""
import json, struct, sys
sys.argv = ['z3dump', '/home/segin/vibezork.cpp/zil/COMPILED/zork1.z3']
import z3dump                                      # reuses its decoder (runs its dump too)
d = z3dump.d
hi = struct.unpack('>H', d[4:6])[0]
found = set()
for a in range(hi & ~1, len(d) - 1, 2):
    try:
        s, _ = z3dump.decode(a)
    except Exception:
        continue
    if len(s) >= 6 and all(32 <= ord(c) < 127 or c in '\n\t' for c in s):
        found.add(s)
def walk(t, out):
    for e in t['elements']:
        if 'str' in e: out.append(e['str'])
        if 'table' in e: walk(e['table'], out)
tabs = json.load(open('zil_tables.json'))['tables']
want = []
for name, t in tabs.items(): 
    lst = []; walk(t, lst); want += [(name, s) for s in lst]
world = json.load(open('zil_world.json'))
want += [(o['name'] + '/' + e['dir'], e['text']) for o in world['objects'] for e in o['exits'] if e['text']]
want += [('pseudo-word', w) for o in world['objects'] for w, _ in o['pseudo']]
missing = [(n, s) for n, s in want if s not in found and len(s) >= 6]
short = [(n, s) for n, s in want if len(s) < 6]
print(f"{len(found)} strings decoded from high memory; {len(want)} table/exit strings checked; "
      f"{len(short)} too short to check {short}; missing: {missing or 'none'}")
