#!/usr/bin/env python3
"""Small MDL/ZIL reader shared by zilgen.py and melee_tables.py.

Produces a plain tree:
  Form   -> list subclass for <...>
  List   -> list subclass for (...)
  Atom   -> str subclass (atom name, backslash quoting resolved)
  Str    -> str subclass (RAW string contents, only \\x escapes resolved)
  int    -> decimal number
  Char   -> str subclass for !\\x character literals

Comments: `;OBJECT` drops the following object (string, atom, form, list),
exactly like MDL's READ.  `'` (QUOTE) and `%` (read-time evaluation) prefixes
are dropped and the object kept; callers that need read-time COND
evaluation (ZORK-NUMBER branches) use eval_read_macros().

String conversion to the ZILCH/Z-machine form is done by zstring():
  - newline inside a literal        -> single space
  - newline immediately after '|'   -> dropped (verified against zork1.z3:
                                       "...ZORK!|\\n|\\nZORK is" prints with no
                                       leading space on the following line)
  - '|'                             -> newline
  - '\\"' -> '"', '\\\\' -> '\\' (resolved while tokenising)
"""
import re


class Atom(str):
    __slots__ = ()

    def __repr__(self):
        return f"Atom({str.__repr__(self)})"


class Str(str):
    __slots__ = ()

    def __repr__(self):
        return f"Str({str.__repr__(self)})"


class Char(str):
    __slots__ = ()

    def __repr__(self):
        return f"Char({str.__repr__(self)})"


class Form(list):
    __slots__ = ("line",)

    def __repr__(self):
        return "<" + " ".join(map(repr, self)) + ">"


class List(list):
    __slots__ = ("line",)

    def __repr__(self):
        return "(" + " ".join(map(repr, self)) + ")"


class Comment:
    """Marker pushed when ';' is seen: the next object goes into the bin."""
    __slots__ = ()


_DELIMS = set('<>()[]";\x0c')
_WS = set(' \t\r\n\x0c')
_NUM = re.compile(r'^[-+]?\d+$')


def tokenize(src):
    """Yield (kind, value, line) with kind in {'<','>','(',')','[',']',';','STR','ATOM','NUM','CHAR'}."""
    i, n, line = 0, len(src), 1
    while i < n:
        c = src[i]
        if c == '\n':
            line += 1; i += 1; continue
        if c in _WS:
            i += 1; continue
        if c == '"':
            j = i + 1; buf = []; start = line
            while j < n:
                ch = src[j]
                if ch == '\\':
                    buf.append(src[j + 1]); line += src[j + 1] == '\n'; j += 2; continue
                if ch == '"':
                    break
                if ch == '\n':
                    line += 1
                buf.append(ch); j += 1
            yield ('STR', ''.join(buf), start); i = j + 1; continue
        if c in '<>()[]':
            yield (c, c, line); i += 1; continue
        if c == ';':
            yield (';', ';', line); i += 1; continue
        if c in "'%":
            i += 1; continue                      # QUOTE / read-macro prefix: transparent
        if c == '!':
            if i + 2 < n and src[i + 1] == '\\':  # !\x character literal
                yield ('CHAR', src[i + 2], line); i += 3; continue
            i += 1; continue                      # !, !. segment prefixes
        if c == '\\' and i + 1 < n and src[i + 1] == '\n':
            line += 1; i += 2; continue           # lone "\" at end of line (page-break atom)
        j = i; buf = []
        while j < n and src[j] not in _WS and src[j] not in _DELIMS:
            if src[j] == '\\' and j + 1 < n:
                buf.append(src[j + 1]); j += 2; continue
            buf.append(src[j]); j += 1
        tok = ''.join(buf)
        if _NUM.match(tok):
            yield ('NUM', int(tok), line)
        else:
            yield ('ATOM', tok, line)
        i = j


def parse(src):
    """Parse a whole file; returns the list of top-level objects (comments removed)."""
    root = List(); root.line = 1
    stack = [root]
    for kind, val, line in tokenize(src):
        top = stack[-1]
        if kind in '<([':
            node = Form() if kind == '<' else List()
            node.line = line
            stack.append(node); continue
        if kind in '>)]':
            node = stack.pop()
            if not stack:
                raise SyntaxError(f"unbalanced close at line {line}")
            _append(stack[-1], node); continue
        if kind == ';':
            top.append(Comment()); continue
        if kind == 'STR':
            _append(top, Str(val))
        elif kind == 'NUM':
            _append(top, val)
        elif kind == 'CHAR':
            _append(top, Char(val))
        else:
            _append(top, Atom(val))
    if len(stack) != 1:
        raise SyntaxError("unbalanced open form at end of file")
    return root


def _append(container, item):
    if container and isinstance(container[-1], Comment):
        container.pop()          # the commented-out object is discarded
        return
    container.append(item)


def zstring(raw):
    """Convert a raw MDL string literal body to the text ZILCH puts in the story."""
    out = []
    prev = ''
    for ch in raw:
        if ch == '\n':
            if prev != '|':
                out.append(' ')
        elif ch == '|':
            out.append('\n')
        else:
            out.append(ch)
        prev = ch
    return ''.join(out)


def is_form(x, head=None):
    return isinstance(x, Form) and (head is None or (x and isinstance(x[0], Atom) and x[0] == head))


def eval_read_macros(objs, zork_number=1):
    """Resolve top-level <COND (<==? ,ZORK-NUMBER n> ...) (T ...)> forms into their live branch."""
    out = []
    for o in objs:
        if is_form(o, 'COND'):
            for clause in o[1:]:
                if not isinstance(clause, List) or not clause:
                    continue
                if _cond_true(clause[0], zork_number):
                    out.extend(eval_read_macros(clause[1:], zork_number))
                    break
        else:
            out.append(o)
    return out


def _cond_true(test, zork_number):
    if isinstance(test, Atom):
        return test in ('T', 'ELSE')
    if is_form(test) and len(test) == 3 and isinstance(test[1], Atom) and test[1] == ',ZORK-NUMBER':
        if test[0] == '==?':
            return test[2] == zork_number
        if test[0] == 'N==?':
            return test[2] != zork_number
    raise ValueError(f"cannot evaluate read-time test {test!r}")


# ---------------------------------------------------------------- Z-machine v3 dictionary key
_A2 = ' \n0123456789.,!?_#\'"/\\-:()'


def zchar_key(word):
    """Dictionary key a v3 ZILCH build produces for WORD: lowercase, encoded to
    z-characters and cut at 6 z-chars (A2 characters such as '#', '-', digits
    cost 2 z-chars each), then decoded again.  Verified against zork1.z3:
    FCD#3 -> 'fcd#', AIR-PUMP -> 'air-p', #COMMAND -> '#comm', NORTHEAST -> 'northe'."""
    zc = []
    for ch in word.lower():
        if 'a' <= ch <= 'z':
            zc.append(('L', ch))
        elif ch in _A2 and ch not in ' \n':
            zc.append(('S', None)); zc.append(('A2', ch))
        else:                                   # 10-bit ZSCII escape: shift, 6, hi, lo
            zc.append(('S', None)); zc.append(('ESC', None)); zc.append(('HI', ch)); zc.append(('LO', ch))
    zc = zc[:6]
    out = []
    i = 0
    while i < len(zc):
        k, v = zc[i]
        if k == 'L':
            out.append(v); i += 1
        elif k == 'S':
            if i + 1 < len(zc):
                nk, nv = zc[i + 1]
                if nk == 'A2':
                    out.append(nv); i += 2
                elif nk == 'ESC' and i + 3 < len(zc):
                    out.append(zc[i + 2][1]); i += 4
                else:
                    i = len(zc)                 # truncated escape: nothing decodable
            else:
                i += 1                          # dangling shift at position 6
        else:
            i += 1
    return ''.join(out)
