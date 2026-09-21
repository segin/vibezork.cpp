#!/usr/bin/env python3
"""Minimal Z-machine v3 story dumper used only to verify zilgen output.
Prints: object tree in child-chain order, every string reachable from an object
property (abbreviations expanded), and the dictionary."""
import sys, struct, json
d = open(sys.argv[1], 'rb').read()
A0 = 'abcdefghijklmnopqrstuvwxyz'; A1 = A0.upper(); A2 = ' \n0123456789.,!?_#\'"/\\-:()'
ABBR = struct.unpack('>H', d[24:26])[0]
def zchars(addr):
    zc=[]; i=addr
    while True:
        w = struct.unpack('>H', d[i:i+2])[0]; i += 2
        zc += [(w>>10)&31, (w>>5)&31, w&31]
        if w & 0x8000: return zc, i
def decode(addr, allow_abbr=True):
    zc, end = zchars(addr)
    out=[]; shift=0; k=0
    while k < len(zc):
        c = zc[k]; k+=1
        if c == 0: out.append(' ')
        elif c in (1,2,3):
            if k >= len(zc): break
            idx = 32*(c-1) + zc[k]; k+=1
            a = struct.unpack('>H', d[ABBR+2*idx:ABBR+2*idx+2])[0]*2
            out.append(decode(a, False)[0])
        elif c == 4: shift=1; continue
        elif c == 5: shift=2; continue
        else:
            if shift==2 and c==6:
                if k+1 < len(zc): out.append(chr((zc[k]<<5)|zc[k+1]))
                k+=2
            else: out.append((A0,A1,A2)[shift][c-6])
        shift=0
    return ''.join(out), end
objtab = struct.unpack('>H', d[10:12])[0]
first = objtab + 31*2
objs = {}; minprop = 1<<30; addr = first; n = 1
while addr < minprop:
    attrs = struct.unpack('>I', d[addr:addr+4])[0]
    parent, sibling, child = d[addr+4], d[addr+5], d[addr+6]
    prop = struct.unpack('>H', d[addr+7:addr+9])[0]
    minprop = min(minprop, prop)
    tl = d[prop]; name = decode(prop+1)[0] if tl else ''
    p = prop + 1 + 2*tl; props = {}
    while d[p]:
        size = (d[p] >> 5) + 1; num = d[p] & 31; props[num] = d[p+1:p+1+size]; p += 1 + size
    objs[n] = dict(num=n, name=name, attrs=[i for i in range(32) if attrs & (1 << (31-i))], parent=parent, sibling=sibling, child=child, props=props)
    n += 1; addr += 9
strings = {}
for o in objs.values():
    for num, raw in o['props'].items():
        if len(raw) == 2:
            pa = struct.unpack('>H', raw)[0]*2
            if 0 < pa < len(d)-1:
                try:
                    s, _ = decode(pa)
                    if s and all(32 <= ord(c) < 127 or c in '\n\t' for c in s): strings.setdefault(s, []).append((o['num'], num))
                except Exception: pass
def chain(first):
    out=[]; c=first
    while c: out.append(c); c = objs[c]['sibling']
    return out
tree = {o['num']: chain(o['child']) for o in objs.values() if o['child']}
dic = struct.unpack('>H', d[8:10])[0]
nsep = d[dic]; p = dic+1+nsep; elen = d[p]; cnt = struct.unpack('>H', d[p+1:p+3])[0]; p += 3
words=[]
for i in range(cnt): words.append(decode(p)[0]); p += elen
json.dump(dict(objects={n: dict(name=o['name'], attrs=o['attrs'], parent=o['parent'], props={k: v.hex() for k,v in o['props'].items()}) for n,o in objs.items()},
               tree=tree, strings=sorted(strings), dictionary=words), open('z3_truth.json','w'), indent=0)
print("objects", len(objs), "strings", len(strings), "dict", len(words))
print("dict has fcd#:", 'fcd#' in words, "| fcd#3:", 'fcd#3' in words, "| h2o:", 'h2o' in words, "| northe:", 'northe' in words)
print("leaflet text:", [s for s in strings if s.startswith('"WELCOME')])
print("guide tail:", [s[-120:] for s in strings if 'Dam Lobby' in s])
print("boat label:", [s for s in strings if 'FROBOZZ' in s])
lr = [n for n,o in objs.items() if o['name']=='Living Room'][0]
print("Living Room children:", [(c, objs[c]['name']) for c in tree[lr]])
print("MAZE-5 skeleton:", [s for s in strings if 'skeleton' in s and 'maze' in s])
