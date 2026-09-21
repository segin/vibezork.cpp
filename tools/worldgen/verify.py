#!/usr/bin/env python3
"""verify.py - cross-check zil_world.json against (a) the earlier scratchpad
zil_world.json (list form, from zil_extract.py) and (b) zork1.z3 ground truth
dumped by z3dump.py (strings, dictionary, object tree)."""
import json, sys
from collections import Counter

new = json.load(open('zil_world.json'))
old = json.load(open('../zil_world.json'))
z3 = json.load(open('z3_truth.json'))
objs = new['objects']; by = {o['name']: o for o in objs}
problems = []

# ------------------------------------------------------------- summary stats
rooms = [o for o in objs if o['kind'] == 'ROOM']; items = [o for o in objs if o['kind'] == 'OBJECT']
print(f"counts: {len(rooms)} rooms, {len(items)} objects ({Counter(o['file'] for o in items)}), total {len(objs)}")
print("exit kinds:", dict(Counter(e['kind'] for o in objs for e in o['exits'])))
print("roles:", dict(Counter(o['role'] for o in objs)))
print("extra (unknown) properties:", {o['name']: o['extra'] for o in objs if o['extra']} or 'none')
print("flags used:", len({f for o in objs for f in o['flags']}), sorted({f for o in objs for f in o['flags']}))
print("objects without DESC:", [o['name'] for o in objs if o['desc'] is None])
print("objects without IN:", [o['name'] for o in objs if o['in'] is None])
print("strings with tab:", [(o['name'], k) for o in objs for k in ('desc', 'ldesc', 'fdesc', 'text') if o[k] and '\t' in o[k]])
print("non-ASCII/control chars:", [(o['name'], k) for o in objs for k in ('desc', 'ldesc', 'fdesc', 'text')
                                    if o[k] and any(ord(c) > 126 or (ord(c) < 32 and c not in '\n\t') for c in o[k])] or 'none')
print("vocabulary keys differing from the word:", {w: k for w, k in new['vocab'].items() if k != w.lower()})
print("unhandled:", new['unhandled'] or 'none')

# ------------------------------------------------------------- (a) vs old extractor
def old_text(s):
    return s.replace('\n', ' ').replace('|', '\n') if s is not None else None

oldby = {r['name']: r for r in old}
assert set(oldby) == set(by), (set(oldby) ^ set(by))
diff = Counter()
for n, o in by.items():
    r = oldby[n]; p = r['props']
    if o['desc'] != p.get('DESC'): diff['DESC'] += 1; problems.append(f"{n}: DESC {o['desc']!r} vs old {p.get('DESC')!r}")
    for k in ('LDESC', 'FDESC', 'TEXT'):
        a = o[k.lower()]; b = old_text(p.get(k))
        if a != b:
            # the only allowed difference: old kept the space after a '|'-newline
            if b is not None and a == b.replace('\n ', '\n'):
                diff[k + ' (|-newline space only)'] += 1
            else:
                diff[k] += 1; problems.append(f"{n}: {k} differs\n  new {a!r}\n  old {b!r}")
    if o['flags'] != r['flags']: problems.append(f"{n}: flags {o['flags']} vs {r['flags']}")
    if o['synonyms'] != r['synonym']: problems.append(f"{n}: synonyms {o['synonyms']} vs {r['synonym']}")
    if o['adjectives'] != r['adjective']: problems.append(f"{n}: adjectives {o['adjectives']} vs {r['adjective']}")
    if o['globals'] != r['global']: problems.append(f"{n}: globals differ")
    if [x for pr in o['pseudo'] for x in pr] != r['pseudo']: problems.append(f"{n}: pseudo differ")
    if o['in'] != p.get('IN') and not (n == 'ROOMS'): problems.append(f"{n}: IN {o['in']} vs {p.get('IN')}")
    for k in ('SIZE', 'CAPACITY', 'VALUE', 'TVALUE', 'STRENGTH', 'VTYPE'):
        if str(o[k.lower()]) != str(p.get(k)): problems.append(f"{n}: {k} {o[k.lower()]} vs {p.get(k)}")  # old JSON keeps numbers as strings
    for k in ('ACTION', 'DESCFCN', 'CONTFCN', 'ADVFCN'):
        if str(o[k.lower()]) != str(p.get(k)): problems.append(f"{n}: {k} {o[k.lower()]} vs {p.get(k)}")
    ne = [(e['dir'], e['kind'], e['to'], e['text'], e['flag'], e['door'], e['routine']) for e in o['exits']]
    oe = [(e['dir'], e['type'], e.get('target'), e.get('msg'), e.get('flag'), e.get('door'), e.get('routine')) for e in r['exits']]
    if ne != oe: problems.append(f"{n}: exits differ\n  new {ne}\n  old {oe}")
print("vs old extractor: string differences by cause:", dict(diff))

# ------------------------------------------------------------- (b) vs zork1.z3
z3names = {o['name'] for o in z3['objects'].values()}
z3strings = set(z3['strings']) | z3names
miss = [(o['name'], k) for o in objs for k in ('desc', 'ldesc', 'fdesc', 'text')
        if o[k] and o[k] not in z3strings and o['role'] == 'game']
print("game strings not found verbatim in zork1.z3:", miss or 'none')
nongame = [(o['name'], k) for o in objs for k in ('desc', 'ldesc', 'fdesc', 'text') if o[k] and o[k] not in z3strings and o['role'] != 'game']
print("non-game strings not found in zork1.z3 (expected for artifact placeholders):", nongame or 'none')
exit_texts = {e['text'] for o in objs for e in o['exits'] if e['text']}
print("exit message strings not in z3 property strings (NEXIT/ELSE strings are not object properties, so 'all' is expected):",
      len([t for t in exit_texts if t not in z3strings]), "of", len(exit_texts))
dic = set(z3['dictionary'])
missing_keys = {w: k for w, k in new['vocab'].items() if k not in dic}
print("vocabulary keys missing from zork1.z3 dictionary:", missing_keys or 'none')
print("z3 object count:", len(z3['objects']), "== ours:", len(objs))

# child-chain order.  Map z3 objects to ZIL names through unique short names; the
# three DESC-less containers are found structurally (parent of 'sailor', 'pseudo', 'Living Room').
desc_count = Counter(o['desc'] for o in objs if o['desc'])
uniq = {o['desc']: o['name'] for o in objs if o['desc'] and desc_count[o['desc']] == 1}
z3obj = {int(k): v for k, v in z3['objects'].items()}
parents = {}
for pnum, chain in z3['tree'].items():
    pname = z3obj[int(pnum)]['name']
    if pname in uniq:
        parents[int(pnum)] = uniq[pname]
for marker, zname in (('sailor', 'GLOBAL-OBJECTS'), ('pseudo', 'LOCAL-GLOBALS'), ('Living Room', 'ROOMS')):
    parents[[o['parent'] for o in z3obj.values() if o['name'] == marker][0]] = zname
fwd_ok = rev_ok = single = 0; bad = []
for pnum, zname in parents.items():
    z3seq = [z3obj[c]['name'] for c in z3['tree'][str(pnum)]]
    ours = [o['desc'] or '' for o in objs if o['in'] == zname]
    if len(ours) < 2: single += 1
    elif z3seq == ours[::-1]: rev_ok += 1
    elif z3seq == ours: fwd_ok += 1
    else: bad.append((zname, z3seq, ours))
print(f"child-chain order vs definition order: {len(parents)} parents tested ({single} single-child), "
      f"{rev_ok} multi-child chains are REVERSE definition order, {fwd_ok} forward, {len(bad)} neither")
for b in bad[:5]:
    print("   mismatch:", b)
# attribute count vs flag count for uniquely named objects
attr_bad = []
for o in z3obj.values():
    if o['name'] in uniq:
        ours = by[uniq[o['name']]]
        if len(o['attrs']) != len(ours['flags']):
            attr_bad.append((ours['name'], len(o['attrs']), ours['flags']))
print("z3 attribute-count vs FLAGS-count mismatches (unique-desc objects):", attr_bad or 'none')
# object numbers vs definition order
nums = [(int(n), uniq[o['name']]) for n, o in z3obj.items() if o['name'] in uniq]
nums.sort()
print("z3 object numbers 1..8 (unique names only):", [(n, by[z]['def_index']) for n, z in nums[:8]], "-> not definition order")

print()
if problems:
    print(f"{len(problems)} DIFFERENCES vs old extractor:")
    for p in problems: print(" -", p)
else:
    print("no differences vs old extractor beyond the |-newline rule")
