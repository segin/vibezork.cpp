"""Shared helpers for emitting C++23 source from the extracted JSON."""
import re

HEADER_BANNER = """// GENERATED FILE - DO NOT EDIT.
// Produced by tools/worldgen/{tool} from {sources}.
// Strings are verbatim ZIL text after ZILCH conversion (| -> newline, line
// break -> space, line break after | dropped); vocabulary words as written.
"""


def cstr(s):
    """C++ string literal for s (ASCII expected; anything else is emitted as \\xNN with literal splitting)."""
    if s is None:
        return '{}'
    out = ['"']
    for ch in s:
        o = ord(ch)
        if ch == '"':
            out.append('\\"')
        elif ch == '\\':
            out.append('\\\\')
        elif ch == '\n':
            out.append('\\n')
        elif ch == '\t':
            out.append('\\t')
        elif 32 <= o < 127:
            out.append(ch)
        else:
            out.append('\\%03o' % o if o < 256 else '\\u%04x' % o)
    out.append('"')
    return ''.join(out)


def ident(name, prefix='k'):
    """ZIL atom -> C++ identifier: F-BUSY? -> kF_BUSY_Q, WHITE-HOUSE -> kWHITE_HOUSE."""
    s = name.replace('-', '_').replace('?', '_Q').replace('!', '_X').replace('#', '_H').replace('.', '_')
    s = re.sub(r'[^A-Za-z0-9_]', '_', s)
    if s[0].isdigit():
        s = '_' + s
    return prefix + s


def sv_list(words):
    return ', '.join(cstr(w) for w in words)


def banner(tool, sources):
    return HEADER_BANNER.format(tool=tool, sources=', '.join(sources))
