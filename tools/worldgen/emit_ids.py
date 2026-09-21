#!/usr/bin/env python3
"""emit_ids.py - emit the ZIL-name -> ObjectId table.

The port addresses rooms and objects by the numeric constants in
src/world/rooms.h (RoomIds::) and src/world/objects.h (ObjectIds::).  Every
ZIL name maps onto one of those constants by replacing '-' with '_' and
dropping '#'.  This script emits that mapping so the loader can turn the
generated data (which is keyed by ZIL name) into the port's ids, and fails
loudly if a name has no constant - a new ZIL object needs a new constant.
"""
import argparse
import json
import os
import re

import cppgen


def constants(path):
    """constexpr ObjectId NAME = 123; -> {NAME: 123}, first definition wins."""
    out = {}
    for line in open(path, encoding='utf-8'):
        m = re.match(r'\s*constexpr ObjectId (\w+)\s*=\s*(\d+);', line)
        if m and m.group(1) not in out:
            out[m.group(1)] = int(m.group(2))
    return out


def cname(zil_name):
    return zil_name.replace('-', '_').replace('#', '')


def emit(model, src_dir, cpp_dir):
    rooms = constants(os.path.join(src_dir, 'world', 'rooms.h'))
    objects = constants(os.path.join(src_dir, 'world', 'objects.h'))
    rows, missing = [], []
    for o in model['objects']:
        n = cname(o['name'])
        if n in rooms:
            rows.append((o['name'], f'RoomIds::{n}', rooms[n]))
        elif n in objects:
            rows.append((o['name'], f'ObjectIds::{n}', objects[n]))
        else:
            missing.append(o['name'])
    if missing:
        raise SystemExit('no ObjectId constant for: ' + ', '.join(missing))

    seen = {}
    for zil, ref, val in rows:
        seen.setdefault(val, []).append(zil)
    dupes = {v: n for v, n in seen.items() if len(n) > 1}
    if dupes:
        raise SystemExit('ObjectId collisions: ' + repr(dupes))

    sources = ['zil/1dungeon.zil', 'zil/gglobals.zil', 'src/world/rooms.h', 'src/world/objects.h']
    h = [cppgen.banner('emit_ids.py', sources), '#pragma once',
         '#include "core/types.h"', '#include <span>', '#include <string_view>', '',
         'namespace zork::zil {', '',
         '/// One ZIL name and the port ObjectId constant it denotes.',
         'struct NameId { std::string_view name; ObjectId id; };', '',
         '/// Every ROOM and OBJECT of 1dungeon.zil and gglobals.zil, definition order.',
         'extern const std::span<const NameId> kNameIds;', '',
         '}  // namespace zork::zil', '']
    c = [cppgen.banner('emit_ids.py', sources), '#include "zil_ids.h"',
         '#include "world/objects.h"', '#include "world/rooms.h"', '',
         'namespace zork::zil {', 'namespace {', '',
         'constexpr NameId kNameIdsArr[] = {']
    for zil, ref, _ in rows:
        c.append(f'    {{{cppgen.cstr(zil)}, {ref}}},')
    c += ['};', '', '}  // namespace', '',
          'const std::span<const NameId> kNameIds = kNameIdsArr;', '',
          f'static_assert(std::size(kNameIdsArr) == {len(rows)});', '',
          '}  // namespace zork::zil', '']
    open(os.path.join(cpp_dir, 'zil_ids.h'), 'w').write('\n'.join(h))
    open(os.path.join(cpp_dir, 'zil_ids.cpp'), 'w').write('\n'.join(c))
    return len(rows)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('-i', '--input', default='zil_world.json')
    ap.add_argument('--src-dir', default='src')
    ap.add_argument('--cpp-dir', default='.')
    a = ap.parse_args()
    n = emit(json.load(open(a.input)), a.src_dir, a.cpp_dir)
    print(f'zil_ids.h/.cpp: {n} names')


if __name__ == '__main__':
    main()
