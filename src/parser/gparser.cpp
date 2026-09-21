#include "parser/gparser.h"
#include "core/globals.h"
#include "core/io.h"
#include "core/object.h"
#include "verbs/verbs.h"
#include "world/objects.h"
#include "world/rooms.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <map>
#include <unordered_map>

namespace GParser {

// ============================================================================
// Dictionary
// ============================================================================

namespace {

// Z-machine alphabet A2 (the characters that cost two z-characters).
constexpr std::string_view kA2 = "0123456789.,!?_#'\"/\\-:()";

int zcharCost(char c) {
  if (c >= 'a' && c <= 'z') return 1;
  if (kA2.find(c) != std::string_view::npos) return 2;
  return 4; // ZSCII escape: 5, 6, hi, lo
}

std::string toLower(std::string_view sv) {
  std::string s(sv);
  for (auto &c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  return s;
}

struct Dictionary {
  std::map<std::string, DictWord> words;      // by key
  std::map<std::string, std::vector<Syntax>> verbs; // VERBS table by canonical verb
  std::vector<const DictWord *> prepositions;  // PREPOSITIONS: index = prep number
  std::unordered_map<std::string, int> prepNumbers;
  size_t objectCount = 0;
  bool built = false;
};

Dictionary &dict() {
  static Dictionary d;
  return d;
}

DictWord &addWord(std::string_view text, int ps) {
  auto key = zkey(text);
  auto &w = dict().words[key];
  w.key = key;
  w.ps |= ps;
  return w;
}

int locBits(const GSyntax::SyntaxElement &e) {
  int bits = 0;
  if (e.scopeFlags & GSyntax::SH_HELD) bits |= SH;
  if (e.scopeFlags & GSyntax::SH_CARRIED) bits |= SC;
  if (e.scopeFlags & GSyntax::SH_IN_ROOM) bits |= SIR;
  if (e.scopeFlags & GSyntax::SH_ON_GROUND) bits |= SOG;
  // ZILCH: an OBJECT without search specifiers searches everywhere.
  if (bits == 0) bits = SH | SC | SIR | SOG;
  if (e.take) bits |= STAKE;
  if (e.many) bits |= SMANY;
  if (e.have) bits |= SHAVE;
  return bits;
}

const std::vector<std::pair<std::string, Direction>> &directionNames() {
  // ZIL: <DIRECTIONS NORTH EAST WEST SOUTH NE NW SE SW UP DOWN IN OUT LAND>
  // (1dungeon.zil:5)
  static const std::vector<std::pair<std::string, Direction>> names = {
      {"north", Direction::NORTH}, {"east", Direction::EAST},
      {"west", Direction::WEST},   {"south", Direction::SOUTH},
      {"ne", Direction::NE},       {"nw", Direction::NW},
      {"se", Direction::SE},       {"sw", Direction::SW},
      {"up", Direction::UP},       {"down", Direction::DOWN},
      {"in", Direction::IN},       {"out", Direction::OUT},
      {"land", Direction::LAND}};
  return names;
}

} // namespace

std::string zkey(std::string_view word) {
  std::string lower = toLower(word);
  std::string key;
  int cost = 0;
  for (char c : lower) {
    int cc = zcharCost(c);
    if (cost + cc > 6) break;
    cost += cc;
    key.push_back(c);
  }
  return key;
}

void invalidateDictionary() {
  dict().built = false;
  dict().objectCount = 0;
}

void buildDictionary() {
  auto &g = Globals::instance();
  auto &d = dict();
  if (d.built && d.objectCount == g.getAllObjects().size()) return;
  // Dictionary words are never removed (pointers into the map stay valid
  // across rebuilds); a rebuild only adds the words of new objects.
  if (!d.built) {
    d.words.clear();
    d.verbs.clear();
    d.prepositions.clear();
    d.prepNumbers.clear();
    d.prepositions.push_back(nullptr); // prep numbers start at 1
  }

  const auto &syntaxes = GSyntax::getAllSyntaxes();
  if (d.built) {
    for (const auto &[id, obj] : g.getAllObjects()) {
      for (const auto &syn : obj->getSynonyms()) addWord(syn, PS_OBJECT);
      for (const auto &adj : obj->getAdjectives()) addWord(adj, PS_ADJECTIVE);
      if (auto *room = dynamic_cast<const ZRoom *>(obj.get())) {
        for (const auto &entry : room->getPseudos()) addWord(entry.word, PS_OBJECT);
      }
    }
    d.objectCount = g.getAllObjects().size();
    return;
  }

  // Prepositions: numbered in order of first appearance (PREPOSITIONS table).
  for (const auto &s : syntaxes) {
    for (const auto &e : s.elements) {
      if (e.type != GSyntax::SyntaxElement::Type::PREPOSITION) continue;
      std::string canon = toLower(e.value);
      if (!d.prepNumbers.contains(canon)) {
        int num = static_cast<int>(d.prepositions.size());
        d.prepNumbers[canon] = num;
        auto &w = addWord(canon, PS_PREPOSITION);
        w.prep = num;
        d.prepositions.push_back(&w);
      }
      int num = d.prepNumbers[canon];
      for (const auto &syn : e.synonyms) {
        auto &w = addWord(syn, PS_PREPOSITION);
        w.prep = num;
      }
    }
  }
  // Fix up the PREPOSITIONS pointers (map nodes are stable, but be explicit).
  for (auto &[canon, num] : d.prepNumbers) {
    d.prepositions[num] = &d.words[zkey(canon)];
  }

  // Verbs and their SYNONYMs; the VERBS table entry for each verb word.
  for (const auto &s : syntaxes) {
    std::string canon = toLower(s.verb);
    auto &w = addWord(canon, PS_VERB);
    w.verb = canon;
    for (const auto &syn : GSyntax::getVerbSynonyms(s.verbId)) {
      if (GSyntax::canonicalVerb(syn) != canon) continue;
      auto &sw = addWord(syn, PS_VERB);
      sw.verb = canon;
    }
    Syntax rec;
    rec.def = &s;
    rec.action = s.actionId;
    int objIndex = 0;
    int pendingPrep = 0;
    for (const auto &e : s.elements) {
      if (e.type == GSyntax::SyntaxElement::Type::PREPOSITION) {
        pendingPrep = d.prepNumbers[toLower(e.value)];
      } else if (e.type == GSyntax::SyntaxElement::Type::OBJECT) {
        ++objIndex;
        uint64_t fwim = e.findFlag ? static_cast<uint64_t>(*e.findFlag) : 0;
        if (objIndex == 1) {
          rec.prep1 = pendingPrep;
          rec.fwim1 = fwim;
          rec.loc1 = locBits(e);
        } else if (objIndex == 2) {
          rec.prep2 = pendingPrep;
          rec.fwim2 = fwim;
          rec.loc2 = locBits(e);
        }
        pendingPrep = 0;
      }
    }
    if (pendingPrep != 0 && objIndex == 0) rec.prep1 = pendingPrep;
    rec.nobj = objIndex;
    d.verbs[canon].push_back(rec);
  }
  // ZILCH stores a verb's syntax lines in reverse definition order, so
  // SYNTAX-CHECK scans them last-defined first (the DRIVE1/DRIVE2 orphan
  // choice depends on it: "put leaflet" asks "...in?").
  for (auto &[canon, list] : d.verbs) {
    std::reverse(list.begin(), list.end());
  }

  // Directions and their synonyms.
  for (const auto &[name, dir] : directionNames()) {
    auto &w = addWord(name, PS_DIRECTION);
    w.dir = dir;
    for (const auto &syn : GSyntax::getDirectionSynonyms(name)) {
      auto &sw = addWord(syn, PS_DIRECTION);
      sw.dir = dir;
    }
  }

  // Buzzwords (gsyntax.zil:9-11).
  for (const auto &b : GSyntax::getBuzzWords()) {
    addWord(b, PS_BUZZ_WORD);
  }

  // Object SYNONYMs and ADJECTIVEs, and the rooms' PSEUDO words (ZILCH
  // enters those in the vocabulary as object words).
  for (const auto &[id, obj] : g.getAllObjects()) {
    for (const auto &syn : obj->getSynonyms()) addWord(syn, PS_OBJECT);
    for (const auto &adj : obj->getAdjectives()) addWord(adj, PS_ADJECTIVE);
    if (auto *room = dynamic_cast<const ZRoom *>(obj.get())) {
      for (const auto &entry : room->getPseudos()) addWord(entry.word, PS_OBJECT);
    }
  }

  d.objectCount = g.getAllObjects().size();
  d.built = true;
}

const DictWord *lookupWord(std::string_view typed) {
  buildDictionary();
  auto &d = dict();
  auto it = d.words.find(zkey(typed));
  return it == d.words.end() ? nullptr : &it->second;
}

const DictWord *W(std::string_view key) {
  buildDictionary();
  auto &d = dict();
  auto it = d.words.find(std::string(key));
  return it == d.words.end() ? nullptr : &it->second;
}

std::span<const Syntax> verbSyntaxes(std::string_view canonicalVerb) {
  buildDictionary();
  auto &d = dict();
  auto it = d.verbs.find(std::string(canonicalVerb));
  if (it == d.verbs.end()) return {};
  return it->second;
}

// ZIL: <ROUTINE PREP-FIND (PREP ...> (gparser.zil:888-893)
const DictWord *prepFind(int prep) {
  buildDictionary();
  auto &d = dict();
  if (prep <= 0 || prep >= static_cast<int>(d.prepositions.size())) return nullptr;
  return d.prepositions[prep];
}

// ============================================================================
// State
// ============================================================================

Ptr &ITbl::slot(int n) {
  switch (n) {
  case P_NC1: return nc1;
  case P_NC1L: return nc1l;
  case P_NC2: return nc2;
  default: return nc2l;
  }
}

const Ptr &ITbl::slot(int n) const {
  switch (n) {
  case P_NC1: return nc1;
  case P_NC1L: return nc1l;
  case P_NC2: return nc2;
  default: return nc2l;
  }
}

State &state() {
  static State s;
  return s;
}

void resetState() {
  state() = State{};
}

const DictWord *wordAt(const Ptr &p) {
  auto &s = state();
  if (p.kind == Ptr::Lex) {
    if (p.idx < 0 || p.idx >= static_cast<int>(s.lexv.e.size())) return nullptr;
    return s.lexv.e[p.idx].w;
  }
  if (p.kind == Ptr::Ocl) {
    if (p.idx < 0 || p.idx >= static_cast<int>(s.oclause.size())) return nullptr;
    return s.oclause[p.idx];
  }
  return nullptr;
}

std::string_view textAt(const Ptr &p) {
  auto &s = state();
  if (p.kind == Ptr::Lex && p.idx >= 0 && p.idx < static_cast<int>(s.lexv.e.size())) {
    return s.lexv.e[p.idx].text;
  }
  // P-OCLAUSE entries carry dictionary words only; ZIL would print garbage
  // here, but every caller guards this path with P-OFLAG / P-MERGED.
  auto *w = wordAt(p);
  return w ? std::string_view(w->key) : std::string_view();
}

// ============================================================================
// READ and the LEXV buffers
// ============================================================================

namespace {
std::optional<std::string> g_nextInput;
}

void setNextInput(std::string line) { g_nextInput = std::move(line); }

// The Z-machine READ opcode: the line is lowercased and truncated to the
// buffer size; words are split at spaces and at the word separators in
// SIBREAKS (".,\"" gparser.zil:12), which become words of their own; at
// most P_LEXV_SIZE words are recorded.
void read(std::string_view line) {
  buildDictionary();
  auto &s = state();
  std::string text = toLower(line);
  if (text.size() > static_cast<size_t>(P_INBUF_SIZE)) text.resize(P_INBUF_SIZE);
  s.inbuf = text;
  s.lexv.count = 0;
  size_t i = 0;
  while (i < text.size() && s.lexv.count < P_LEXV_SIZE) {
    char c = text[i];
    if (c == ' ') {
      ++i;
      continue;
    }
    std::string tok;
    if (c == '.' || c == ',' || c == '"') {
      tok.push_back(c);
      ++i;
    } else {
      while (i < text.size() && text[i] != ' ' && text[i] != '.' && text[i] != ',' && text[i] != '"') {
        tok.push_back(text[i]);
        ++i;
      }
    }
    auto &e = s.lexv.e[s.lexv.count];
    e.text = tok;
    e.w = lookupWord(tok);
    ++s.lexv.count;
  }
}

// ZIL: <ROUTINE STUFF (SRC DEST "OPTIONAL" (MAX 29) ...> (gparser.zil:387-399)
// Copies the header and the first MAX entries only.
void stuff(const LexTable &src, LexTable &dest, int max) {
  dest.count = src.count;
  for (int i = 0; i < max && i < static_cast<int>(src.e.size()); ++i) {
    dest.e[i] = src.e[i];
  }
}

// ZIL: <ROUTINE INBUF-STUFF (SRC DEST ...> (gparser.zil:402-406)
void inbufStuff(const std::string &src, std::string &dest) { dest = src; }

// ZIL: <ROUTINE INBUF-ADD (LEN BEG SLOT ...> (gparser.zil:410-423)
// Appends the corrected word's characters to OOPS-INBUF and points the
// AGAIN-LEXV entry at them.
void inbufAdd(std::string_view text, int slot) {
  auto &s = state();
  s.oopsInbuf.append(text);
  s.oops.end = true;
  if (slot >= 0 && slot < static_cast<int>(s.againLexv.e.size())) {
    s.againLexv.e[slot].text = std::string(text);
  }
}

// ZIL: <ROUTINE WT? (PTR BIT "OPTIONAL" (B1 5) ...> (gparser.zil:430-436)
bool wt(const DictWord *w, int bit) { return w && (w->ps & bit); }

// ZIL: <ROUTINE WORD-PRINT (CNT BUF) ...> (gparser.zil:658-663)
void wordPrint(std::string_view text) { print(text); }

// ZIL: <ROUTINE UNKNOWN-WORD (PTR ...> (gparser.zil:665-675)
void unknownWord(int ptr) {
  auto &g = Globals::instance();
  auto &s = state();
  s.oops.ptr = ptr;
  if (g.prsa == V_SAY) {
    printLine("Nothing happens.");
    return;
  }
  print("I don't know the word \"");
  wordPrint(s.lexv.e[ptr].text);
  printLine("\".");
  g.quoteFlag = false;
  g.pOflag = false;
}

// ZIL: <ROUTINE CANT-USE (PTR ...> (gparser.zil:677-686)
void cantUse(int ptr) {
  auto &g = Globals::instance();
  auto &s = state();
  if (g.prsa == V_SAY) {
    printLine("Nothing happens.");
    return;
  }
  print("You used the word \"");
  wordPrint(s.lexv.e[ptr].text);
  printLine("\" in a way that I don't understand.");
  g.quoteFlag = false;
  g.pOflag = false;
}

// ZIL: <ROUTINE BUFFER-PRINT (BEG END CP ...> (gparser.zil:819-849)
void bufferPrint(Ptr beg, Ptr end, bool cp) {
  auto &g = Globals::instance();
  bool nosp = true;
  bool first = true;
  bool pn = false;
  const DictWord *comma = W(",");
  const DictWord *period = W(".");
  const DictWord *me = W("me");
  const DictWord *intnum = W("intnum");
  const DictWord *it = W("it");
  while (!(beg == end)) {
    const DictWord *wrd = wordAt(beg);
    if (wrd && wrd == comma) {
      print(", ");
    } else if (nosp) {
      nosp = false;
    } else {
      print(" ");
    }
    if (wrd && (wrd == period || wrd == comma)) {
      nosp = true;
    } else if (wrd && wrd == me) {
      if (auto *meObj = g.getObject(ObjectIds::ME)) printDesc(meObj);
      pn = true;
    } else if (wrd && wrd == intnum) {
      print(std::to_string(g.pNumber));
      pn = true;
    } else {
      if (first && !pn && cp) print("the ");
      if (g.pOflag || g.pMerged) {
        if (wrd) print(wrd->key);
      } else if (wrd && wrd == it && isAccessible(g.it)) {
        printDesc(g.it);
      } else {
        wordPrint(textAt(beg));
      }
      first = false;
    }
    beg.idx += 1; // <REST .BEG ,P-WORDLEN>
  }
}

// ============================================================================
// Object search (used by LIT? and, from B6 on, GET-OBJECT)
// ============================================================================

// ZIL: <ROUTINE OBJ-FOUND (OBJ TBL ...> (gparser.zil:1239-1242)
void objFound(ZObject *obj, std::vector<ZObject *> &tbl) { tbl.push_back(obj); }

// ZIL: <ROUTINE THIS-IT? (OBJ TBL ...> (gparser.zil:1357-1370)
bool thisIt(ZObject *obj) {
  auto &g = Globals::instance();
  auto &s = state();
  if (!obj) return false;
  if (obj->hasFlag(ObjectFlag::INVISIBLE)) return false;
  if (s.nam) {
    bool found = false;
    for (const auto &syn : obj->getSynonyms()) {
      if (zkey(syn) == s.nam->key) {
        found = true;
        break;
      }
    }
    if (!found) return false;
  }
  if (s.adj) {
    bool found = false;
    for (const auto &adj : obj->getAdjectives()) {
      if (zkey(adj) == s.adj->key) {
        found = true;
        break;
      }
    }
    if (!found) return false;
  }
  if (g.pGwimbit != 0 && !obj->hasFlag(static_cast<ObjectFlag>(g.pGwimbit))) return false;
  return true;
}

// ZIL: <ROUTINE SEARCH-LIST (OBJ TBL LVL ...> (gparser.zil:1216-1237)
void searchList(ZObject *obj, std::vector<ZObject *> &tbl, int lvl) {
  if (!obj) return;
  // Iterate over a copy: OBJ-FOUND never moves objects, but callers of the
  // parser may run with actions that do.
  const auto contents = obj->getContents();
  for (auto *child : contents) {
    if (!child) continue;
    if (lvl != P_SRCBOT && !child->getSynonyms().empty() && thisIt(child)) {
      objFound(child, tbl);
    }
    if ((lvl != P_SRCTOP || child->hasFlag(ObjectFlag::SEARCHBIT) || child->hasFlag(ObjectFlag::SURFACEBIT)) &&
        !child->getContents().empty() &&
        (child->hasFlag(ObjectFlag::OPENBIT) || child->hasFlag(ObjectFlag::TRANSBIT))) {
      int sub = child->hasFlag(ObjectFlag::SURFACEBIT) ? P_SRCALL
                : child->hasFlag(ObjectFlag::SEARCHBIT) ? P_SRCALL
                                                          : P_SRCTOP;
      searchList(child, tbl, sub);
    }
  }
}

// ZIL: <ROUTINE DO-SL (OBJ BIT1 BIT2 ...> (gparser.zil:1202-1210)
void doSl(ZObject *obj, int bit1, int bit2) {
  auto &g = Globals::instance();
  auto &s = state();
  if (!s.table) return;
  int bits = g.pSlocbits;
  if ((bits & (bit1 + bit2)) == (bit1 + bit2)) {
    searchList(obj, *s.table, P_SRCALL);
  } else if (bits & bit1) {
    searchList(obj, *s.table, P_SRCTOP);
  } else if (bits & bit2) {
    searchList(obj, *s.table, P_SRCBOT);
  }
}

// ZIL: <ROUTINE META-LOC (OBJ) ...> (gparser.zil:1398-1407)
ZObject *metaLoc(const ZObject *obj) {
  auto &g = Globals::instance();
  ZObject *globals = g.getObject(ObjectIds::GLOBAL_OBJECTS);
  const ZObject *curr = obj;
  while (true) {
    if (!curr) return nullptr;
    if (curr->getLocation() && curr->getLocation() == globals) return globals;
    if (dynamic_cast<const ZRoom *>(curr)) return const_cast<ZObject *>(curr);
    curr = curr->getLocation();
  }
}

// ZIL: <ROUTINE ACCESSIBLE? (OBJ ...> (gparser.zil:1372-1396)
bool isAccessible(const ZObject *obj) {
  auto &g = Globals::instance();
  if (!obj) return false;
  ZObject *l = obj->getLocation();
  if (obj->hasFlag(ObjectFlag::INVISIBLE)) return false;
  if (!l) return false;
  if (l->getId() == ObjectIds::GLOBAL_OBJECTS) return true;
  if (l->getId() == ObjectIds::LOCAL_GLOBALS && Verbs::globalIn(obj->getId(), g.here)) return true;
  ZObject *winnerLoc = g.winner ? g.winner->getLocation() : nullptr;
  ZObject *ml = metaLoc(obj);
  if (!(ml == g.here || ml == winnerLoc)) return false;
  if (l == g.winner || l == g.here || l == winnerLoc) return true;
  if (l->hasFlag(ObjectFlag::OPENBIT) && isAccessible(l)) return true;
  return false;
}

// ZIL: <ROUTINE LIT? (RM "OPTIONAL" (RMBIT T) ...> (gparser.zil:1333-1355)
bool isLit(ZObject *rm, bool rmbit) {
  auto &g = Globals::instance();
  auto &s = state();
  if (!rm) rm = g.here;
  if (s.alwaysLit && g.winner == g.player) return true;
  g.pGwimbit = static_cast<uint64_t>(ObjectFlag::ONBIT);
  ZObject *ohere = g.here;
  g.here = rm;
  bool lit = false;
  if (rmbit && rm && rm->hasFlag(ObjectFlag::ONBIT)) {
    lit = true;
  } else {
    s.merge.clear();
    s.table = &s.merge;
    g.pSlocbits = -1;
    if (ohere == rm) {
      doSl(g.winner, 1, 1);
      if (g.winner != g.player && g.player && g.player->getLocation() == rm) {
        doSl(g.player, 1, 1);
      }
    }
    doSl(rm, 1, 1);
    if (!s.table->empty()) lit = true;
  }
  g.here = ohere;
  g.pGwimbit = 0;
  return lit;
}

// ============================================================================
// Routines ported in later items (B2-B10)
// ============================================================================

const DictWord *numberQ(int) { return nullptr; }
bool cantOrphan() {
  printLine("\"I don't understand! What are you referring to?\"");
  return false;
}
void thingPrint(bool prso, bool the) {
  auto &s = state();
  if (prso) {
    bufferPrint(s.itbl.nc1, s.itbl.nc1l, the);
  } else {
    bufferPrint(s.itbl.nc2, s.itbl.nc2l, the);
  }
}
void prepPrint(int prep) {
  if (prep != 0) {
    print(" ");
    if (auto *w = prepFind(prep)) print(w->key);
  }
}
void clauseAdd(const DictWord *wrd) { state().oclause.push_back(wrd); }
void syntaxFound(const Syntax *syn) {
  state().syntax = syn;
  Globals::instance().prsa = syn ? syn->action : 0;
}


// ============================================================================
// PARSER and CLAUSE (gparser.zil:109-380, 440-510)
// ============================================================================

namespace {

bool isThenPeriod(const DictWord *w) { return w && (w == W("then") || w == W(".")); }
bool isCommaAnd(const DictWord *w) { return w && (w == W(",") || w == W("and")); }

void setPrepSlot(ITbl &t, int num, int val, const DictWord *wrd) {
  if (num == P_PREP1) {
    t.prep1 = val;
    t.prep1n = wrd;
  } else {
    t.prep2 = val;
    t.prep2n = wrd;
  }
}

} // namespace

// ZIL: <ROUTINE CLAUSE (PTR VAL WRD ...> (gparser.zil:440-510)
// PTR is a P-LEXV entry index. Returns the entry index of the clause's
// last word, -1 when the input is exhausted, nullopt on RFALSE.
std::optional<int> clause(int ptr, int val, const DictWord *wrd) {
  auto &g = Globals::instance();
  auto &s = state();
  int off = (s.ncn - 1) * 2;
  int num;
  bool andflg = false;
  bool first = true;
  const DictWord *nw = nullptr;
  const DictWord *lw = nullptr;
  if (val != 0) {
    num = P_PREP1 + off;
    setPrepSlot(s.itbl, num, val, wrd);
    ptr += 1;
  } else {
    s.len += 1;
  }
  if (s.len == 0) {
    s.ncn -= 1;
    return -1;
  }
  num = P_NC1 + off;
  s.itbl.slot(num) = Ptr::lex(ptr);
  {
    const DictWord *w = s.lexv.e[ptr].w;
    if (w && (w == W("the") || w == W("a") || w == W("an"))) {
      s.itbl.slot(num) = Ptr::lex(ptr + 1);
    }
  }
  while (true) {
    if (--s.len < 0) {
      s.itbl.slot(num + 1) = Ptr::lex(ptr);
      return -1;
    }
    wrd = s.lexv.e[ptr].w;
    if (!wrd) wrd = numberQ(ptr);
    if (wrd) {
      nw = (s.len == 0) ? nullptr : s.lexv.e[ptr + 1].w;
      if (isCommaAnd(wrd)) {
        andflg = true;
      } else if (wrd == W("all") || wrd == W("one")) {
        if (nw && nw == W("of")) {
          s.len -= 1;
          ptr += 1;
        }
      } else if (isThenPeriod(wrd) ||
                 (wt(wrd, PS_PREPOSITION) && !s.itbl.verb.empty() && !first)) {
        // "ADDED 4/27 FOR TURTLE,UP"
        s.len += 1;
        s.itbl.slot(num + 1) = Ptr::lex(ptr);
        return ptr - 1;
      } else if (wt(wrd, PS_OBJECT)) {
        if (s.len > 0 && nw && nw == W("of") && !(wrd == W("all") || wrd == W("one"))) {
          // adjective-like noun followed by OF: keep scanning
        } else if (wt(wrd, PS_ADJECTIVE) && nw != nullptr && wt(nw, PS_OBJECT)) {
          // adjective use of an object word
        } else if (!andflg && !(nw && (nw == W("but") || nw == W("except"))) && !isCommaAnd(nw)) {
          s.itbl.slot(num + 1) = Ptr::lex(ptr + 1);
          return ptr;
        } else {
          andflg = false;
        }
      } else if ((g.pMerged || g.pOflag || !s.itbl.verb.empty()) &&
                 (wt(wrd, PS_ADJECTIVE) || wt(wrd, PS_BUZZ_WORD))) {
        // adjectives and buzzwords inside the clause
      } else if (andflg && (wt(wrd, PS_DIRECTION) || wt(wrd, PS_VERB))) {
        // "take lamp and go north": the AND becomes THEN
        ptr -= 2;
        s.lexv.e[ptr + 1].w = W("then");
        s.len += 2;
      } else if (wt(wrd, PS_PREPOSITION)) {
        // a preposition as the first word of the clause
      } else {
        cantUse(ptr);
        return std::nullopt;
      }
    } else {
      unknownWord(ptr);
      return std::nullopt;
    }
    lw = wrd;
    (void)lw;
    first = false;
    ptr += 1;
  }
}

// ZIL: <ROUTINE PARSER ("AUX" ...) ...> (gparser.zil:109-380)
bool parser() {
  auto &g = Globals::instance();
  auto &s = state();
  buildDictionary();
  int ptr = 0;               // P-LEXSTART as an entry index
  const DictWord *wrd = nullptr;
  int val = 0;
  std::string verb;          // VERB (an ACT? value; empty = <>)
  bool ofFlag = false;
  ZObject *owinner = nullptr;
  bool omerged = false;
  int len = 0;
  std::optional<Direction> dir;
  const DictWord *nw = nullptr;
  const DictWord *lw = nullptr;

  // <REPEAT ... <COND (<NOT ,P-OFLAG> <PUT ,P-OTBL .CNT <GET ,P-ITBL .CNT>>)>
  //             <PUT ,P-ITBL .CNT 0>>
  if (!g.pOflag) s.otbl = s.itbl;
  s.itbl.clear();
  owinner = g.winner;
  omerged = g.pMerged;
  // P-ADVERB is dead in Zork I
  g.pMerged = false;
  s.endOnPrep = false;
  s.prso.clear();
  s.prsi.clear();
  s.buts.clear();
  if (!g.quoteFlag && g.winner != g.player) {
    g.winner = g.player;
    g.here = metaLoc(g.player);
    g.lit = isLit(g.here);
  }
  if (s.reservePtr >= 0) {
    ptr = s.reservePtr;
    stuff(s.reserveLexv, s.lexv);
    if (!g.superbriefMode && g.player == g.winner) crlf();
    s.reservePtr = -1;
    g.pCont = 0;
  } else if (g.pCont) {
    ptr = g.pCont;
    if (!g.superbriefMode && g.player == g.winner && g.prsa != V_SAY) crlf();
    g.pCont = 0;
  } else {
    g.winner = g.player;
    g.quoteFlag = false;
    if (g.winner && !(g.winner->getLocation() && g.winner->getLocation()->hasFlag(ObjectFlag::VEHBIT))) {
      g.here = g.winner->getLocation();
    }
    g.lit = isLit(g.here);
    if (!g.superbriefMode) crlf();
    print(">");
    if (g_nextInput) {
      std::string line = std::move(*g_nextInput);
      g_nextInput.reset();
      read(line);
    } else {
      read(readLine());
    }
  }
  s.len = s.lexv.count;
  if (s.len == 0) {
    printLine("I beg your pardon?");
    return false;
  }
  wrd = s.lexv.e[ptr].w;
  if (wrd && wrd == W("oops")) {
    // <COND (<EQUAL? <GET ,P-LEXV <+ .PTR ,P-LEXELEN>> ,W?PERIOD ,W?COMMA> ...>
    {
      const DictWord *next = s.lexv.e[ptr + 1].w;
      if (next && (next == W(".") || next == W(","))) {
        ptr += 1;
        s.len -= 1;
      }
    }
    if (!(s.len > 1)) {
      printLine("I can't help your clumsiness.");
      return false;
    } else if (s.oops.ptr >= 0) {
      const DictWord *next = s.lexv.e[ptr + 1].w;
      if (s.len > 2 && next && next == W("\"")) {
        printLine("Sorry, you can't correct mistakes in quoted text.");
        return false;
      } else if (s.len > 2) {
        printLine("Warning: only the first word after OOPS is used.");
      }
      // <PUT ,AGAIN-LEXV <GET ,OOPS-TABLE ,O-PTR> <GET ,P-LEXV <+ .PTR ,P-LEXELEN>>>
      s.againLexv.e[s.oops.ptr].w = next;
      g.winner = owinner; // "maybe fix oops vs. chars.?"
      inbufAdd(s.lexv.e[ptr + 1].text, s.oops.ptr);
      stuff(s.againLexv, s.lexv);
      s.len = s.lexv.count;
      ptr = s.oops.start;
      inbufStuff(s.oopsInbuf, s.inbuf);
    } else {
      s.oops.end = false;
      printLine("There was no word to replace!");
      return false;
    }
  } else {
    if (!(wrd && (wrd == W("again") || wrd == W("g")))) g.pNumber = 0;
    s.oops.end = false;
  }
  wrd = s.lexv.e[ptr].w;
  if (wrd && (wrd == W("again") || wrd == W("g"))) {
    // <COND (<ZERO? <GETB ,OOPS-INBUF 1>> ...)>: nothing was ever typed
    if (s.oopsInbuf.empty()) {
      printLine("Beg pardon?");
      return false;
    } else if (g.pOflag) {
      printLine("It's difficult to repeat fragments.");
      return false;
    } else if (!g.pWon) {
      printLine("That would just repeat a mistake.");
      return false;
    } else if (s.len > 1) {
      const DictWord *next = s.lexv.e[ptr + 1].w;
      if (next && (next == W(".") || next == W(",") || next == W("then") || next == W("and"))) {
        ptr += 2;
        s.lexv.count -= 2;
      } else {
        printLine("I couldn't understand that sentence.");
        return false;
      }
    } else {
      ptr += 1;
      s.lexv.count -= 1;
    }
    if (s.lexv.count > 0) {
      stuff(s.lexv, s.reserveLexv);
      s.reservePtr = ptr;
    } else {
      s.reservePtr = -1;
    }
    g.winner = owinner;
    g.pMerged = omerged;
    inbufStuff(s.oopsInbuf, s.inbuf);
    stuff(s.againLexv, s.lexv);
    dir = s.againDir;
    s.itbl = s.otbl;
  } else {
    stuff(s.lexv, s.againLexv);
    inbufStuff(s.inbuf, s.oopsInbuf);
    s.oops.start = ptr;
    s.oops.length = 4 * s.len;
    s.oops.end = true; // O-END: the byte after the last typed word
    s.reservePtr = -1;
    len = s.len;
    // P-DIR is never read
    s.ncn = 0;
    g.pGetFlags = 0;
    while (true) {
      if (--s.len < 0) {
        g.quoteFlag = false;
        break;
      }
      wrd = s.lexv.e[ptr].w;
      if (!wrd) wrd = numberQ(ptr);
      if (wrd) {
        nw = (s.len == 0) ? nullptr : s.lexv.e[ptr + 1].w;
        if (wrd == W("to") && verb == "tell") {
          wrd = W("\"");
        } else if (wrd == W("then") && s.len > 0 && verb.empty() && !g.quoteFlag) {
          // "Last NOT added 7/3"
          if (lw == nullptr || lw == W(".")) {
            wrd = W("the");
          } else {
            s.itbl.verb = "tell";
            s.itbl.verbn = false;
            wrd = W("\"");
          }
        }
        if (wrd == W("then") || wrd == W(".") || wrd == W("\"")) {
          if (wrd == W("\"")) g.quoteFlag = !g.quoteFlag;
          if (s.len != 0) g.pCont = ptr + 1;
          s.lexv.count = s.len;
          break;
        } else if (wt(wrd, PS_DIRECTION) && (verb.empty() || verb == "walk") &&
                   (len == 1 || (len == 2 && verb == "walk") ||
                    ((isThenPeriod(nw) || (nw && nw == W("\""))) && !(len < 2)) ||
                    (g.quoteFlag && len == 2 && nw && nw == W("\"")) ||
                    (len > 2 && isCommaAnd(nw)))) {
          dir = wrd->dir;
          if (isCommaAnd(nw)) s.lexv.e[ptr + 1].w = W("then");
          if (!(len > 2)) {
            g.quoteFlag = false;
            break;
          }
        } else if (wt(wrd, PS_VERB) && verb.empty()) {
          verb = wrd->verb;
          s.itbl.verb = verb;
          s.itbl.verbn = true;
          s.vtbl.word = wrd;
          s.vtbl.text = s.lexv.e[ptr].text;
          s.vtbl.haveText = true;
        } else if ((val = wt(wrd, PS_PREPOSITION) ? wrd->prep : 0, val != 0) ||
                   wrd == W("all") || wrd == W("one") ||
                   wt(wrd, PS_ADJECTIVE) || wt(wrd, PS_OBJECT)) {
          if (s.len > 1 && nw && nw == W("of") && val == 0 &&
              !(wrd == W("all") || wrd == W("one") || wrd == W("a"))) {
            ofFlag = true;
          } else if (val != 0 && (s.len == 0 || isThenPeriod(nw))) {
            s.endOnPrep = true;
            if (s.ncn < 2) {
              s.itbl.prep1 = val;
              s.itbl.prep1n = wrd;
            }
          } else if (s.ncn == 2) {
            printLine("There were too many nouns in that sentence.");
            return false;
          } else {
            s.ncn += 1;
            s.act = verb;
            auto r = clause(ptr, val, wrd);
            if (!r) return false;
            ptr = *r;
            if (ptr < 0) {
              g.quoteFlag = false;
              break;
            }
          }
        } else if (wrd == W("of")) {
          if (!ofFlag || isThenPeriod(nw)) {
            cantUse(ptr);
            return false;
          }
          ofFlag = false;
        } else if (wt(wrd, PS_BUZZ_WORD)) {
          // buzzwords are skipped
        } else if (verb == "tell" && wt(wrd, PS_VERB) && g.winner == g.player) {
          printLine("Please consult your manual for the correct way to talk to other people or creatures.");
          return false;
        } else {
          cantUse(ptr);
          return false;
        }
      } else {
        unknownWord(ptr);
        return false;
      }
      lw = wrd;
      ptr += 1;
    }
  }
  s.oops.ptr = -1;
  if (dir) {
    g.prsa = V_WALK;
    g.prso = nullptr; // PRSO holds the direction property in ZIL
    g.pOflag = false;
    g.pWalkDir = dir;
    s.againDir = dir;
    return true;
  }
  if (g.pOflag) orphanMerge();
  g.pWalkDir.reset();
  s.againDir.reset();
  (void)owinner;
  (void)omerged;
  return syntaxCheck() && snarfObjects() && manyCheck() && takeCheck();
}


// ============================================================================
// Syntax matching and orphaning (gparser.zil:543-655, 707-926)
// ============================================================================

// ZIL: <ROUTINE CLAUSE-COPY (SRC DEST "OPTIONAL" (INSRT <>) ...> (gparser.zil:860-879)
void clauseCopy(ITbl &src, ITbl &dest, const DictWord *insrt) {
  auto &s = state();
  Ptr beg = src.slot(s.cctbl[CC_SBPTR]);
  Ptr end = src.slot(s.cctbl[CC_SEPTR]);
  dest.slot(s.cctbl[CC_DBPTR]) = Ptr::ocl(static_cast<int>(s.oclause.size()));
  while (true) {
    if (beg == end) {
      dest.slot(s.cctbl[CC_DEPTR]) = Ptr::ocl(static_cast<int>(s.oclause.size()));
      return;
    }
    const DictWord *w = wordAt(beg);
    if (insrt && s.anam && s.anam == w) clauseAdd(insrt);
    clauseAdd(w);
    beg.idx += 1;
  }
}

// ZIL: <ROUTINE ORPHAN (D1 D2 ...> (gparser.zil:782-808)
void orphan(const Syntax *d1, const Syntax *d2) {
  auto &g = Globals::instance();
  auto &s = state();
  if (!g.pMerged) s.oclause.clear();
  s.ovtbl = s.vtbl;
  s.otbl = s.itbl;
  if (s.ncn == 2) {
    s.cctbl = {P_NC2, P_NC2L, P_NC2, P_NC2L};
    clauseCopy(s.itbl, s.otbl);
  }
  if (s.ncn >= 1) {
    s.cctbl = {P_NC1, P_NC1L, P_NC1, P_NC1L};
    clauseCopy(s.itbl, s.otbl);
  }
  if (d1) {
    s.otbl.prep1 = d1->prep1;
    s.otbl.nc1 = Ptr::one();
  } else if (d2) {
    s.otbl.prep2 = d2->prep2;
    s.otbl.nc2 = Ptr::one();
  }
}

// ZIL: <ROUTINE ACLAUSE-WIN (ADJ) ...> (gparser.zil:634-643)
bool aclauseWin(const DictWord *adj) {
  auto &s = state();
  s.itbl.verb = s.otbl.verb;
  s.cctbl = {s.aclause, s.aclause + 1, s.aclause, s.aclause + 1};
  clauseCopy(s.otbl, s.otbl, adj);
  if (!s.otbl.nc2.isNull()) s.ncn = 2;
  s.aclause = 0;
  return true;
}

// ZIL: <ROUTINE NCLAUSE-WIN () ...> (gparser.zil:645-653)
bool nclauseWin() {
  auto &s = state();
  s.cctbl = {P_NC1, P_NC1L, s.aclause, s.aclause + 1};
  clauseCopy(s.itbl, s.otbl);
  if (!s.otbl.nc2.isNull()) s.ncn = 2;
  s.aclause = 0;
  return true;
}

// ZIL: <ROUTINE ORPHAN-MERGE ...> (gparser.zil:543-630)
// "New ORPHAN-MERGE for TRAP Retrofix 6/21/84"
bool orphanMerge() {
  auto &g = Globals::instance();
  auto &s = state();
  bool adjFlag = false;         // ADJ set to T
  const DictWord *adj = nullptr; // ADJ set to a word in the P-ACLAUSE loop
  g.pOflag = false;
  // <SET WRD <GET <GET ,P-ITBL ,P-VERBN> 0>>: with no verb this reads the
  // story header in ZIL, whose bits make both tests fail.
  const DictWord *wrd = s.itbl.verbn ? s.vtbl.word : nullptr;
  std::string wrdVerb = (wrd && wt(wrd, PS_VERB)) ? wrd->verb : std::string();
  if (wrdVerb == s.otbl.verb || (wrd && wt(wrd, PS_ADJECTIVE))) {
    adjFlag = true;
  } else if (wrd && wt(wrd, PS_OBJECT) && s.ncn == 0) {
    s.itbl.verb.clear();
    s.itbl.verbn = false;
    s.itbl.nc1 = Ptr::lex(0);
    s.itbl.nc1l = Ptr::lex(1);
    s.ncn = 1;
  }
  std::string verb = s.itbl.verb;
  if (!verb.empty() && !adjFlag && verb != s.otbl.verb) {
    return false;
  } else if (s.ncn == 2) {
    return false;
  } else if (s.otbl.nc1 == Ptr::one()) {
    int temp = s.itbl.prep1;
    if (temp == s.otbl.prep1 || temp == 0) {
      if (adjFlag) {
        s.otbl.nc1 = Ptr::lex(0);
        if (s.itbl.nc1l.isNull()) s.itbl.nc1l = Ptr::lex(1);
        if (s.ncn == 0) s.ncn = 1;
      } else {
        s.otbl.nc1 = s.itbl.nc1;
      }
      s.otbl.nc1l = s.itbl.nc1l;
    } else {
      return false;
    }
  } else if (s.otbl.nc2 == Ptr::one()) {
    int temp = s.itbl.prep1;
    if (temp == s.otbl.prep2 || temp == 0) {
      if (adjFlag) {
        s.itbl.nc1 = Ptr::lex(0);
        if (s.itbl.nc1l.isNull()) s.itbl.nc1l = Ptr::lex(1);
      }
      s.otbl.nc2 = s.itbl.nc1;
      s.otbl.nc2l = s.itbl.nc1l;
      s.ncn = 2;
    } else {
      return false;
    }
  } else if (s.aclause != 0) {
    if (s.ncn != 1 && !adjFlag) {
      s.aclause = 0;
      return false;
    }
    Ptr beg = s.itbl.nc1;
    if (adjFlag) {
      beg = Ptr::lex(0);
      adjFlag = false;
    }
    Ptr end = s.itbl.nc1l;
    while (true) {
      wrd = wordAt(beg);
      if (beg == end) {
        if (adj) {
          aclauseWin(adj);
          break;
        }
        s.aclause = 0;
        return false;
      } else if (!adj && wrd && (wt(wrd, PS_ADJECTIVE) || wrd == W("all") || wrd == W("one"))) {
        adj = wrd;
      } else if (wrd && wrd == W("one")) {
        aclauseWin(adj);
        break;
      } else if (wrd && wt(wrd, PS_OBJECT)) {
        if (wrd == s.anam) {
          aclauseWin(adj);
        } else {
          nclauseWin();
        }
        break;
      }
      beg.idx += 1;
      if (end.isNull()) {
        end = beg;
        s.ncn = 1;
        s.itbl.nc1 = Ptr{beg.kind, beg.idx - 1};
        s.itbl.nc1l = beg;
      }
    }
  }
  // <PUT ,P-VTBL 0 <GET ,P-OVTBL 0>> <PUTB ,P-VTBL 2 ...> <PUTB ,P-VTBL 3 ...>
  s.vtbl = s.ovtbl;
  s.otbl.verbn = true; // <PUT ,P-OTBL ,P-VERBN ,P-VTBL>
  s.vtbl.haveText = false; // <PUTB ,P-VTBL 2 0>
  s.itbl = s.otbl;
  g.pMerged = true;
  return true;
}

// ZIL: <ROUTINE GWIM (GBIT LBIT PREP ...> (gparser.zil:901-926)
ZObject *gwim(uint64_t gbit, int lbit, int prep) {
  auto &g = Globals::instance();
  auto &s = state();
  if (gbit == static_cast<uint64_t>(ObjectFlag::RMUNGBIT)) {
    return g.getObject(ObjectIds::ROOMS);
  }
  g.pGwimbit = gbit;
  g.pSlocbits = lbit;
  s.merge.clear();
  if (getObject(s.merge, false)) {
    g.pGwimbit = 0;
    if (s.merge.size() == 1) {
      ZObject *obj = s.merge[0];
      print("(");
      if (prep != 0 && !s.endOnPrep) {
        const DictWord *pw = prepFind(prep);
        if (pw) print(pw->key);
        if (pw && pw == W("out")) print(" of");
        print(" ");
        if (obj->getId() == ObjectIds::HANDS) {
          print("your hands");
        } else {
          print("the ");
          printDesc(obj);
        }
        printLine(")");
      } else {
        printDesc(obj);
        printLine(")");
      }
      return obj;
    }
    return nullptr;
  }
  g.pGwimbit = 0;
  return nullptr;
}

// ZIL: <ROUTINE SYNTAX-CHECK () ...> (gparser.zil:707-775)
bool syntaxCheck() {
  auto &g = Globals::instance();
  auto &s = state();
  const std::string verb = s.itbl.verb;
  if (verb.empty()) {
    printLine("There was no verb in that sentence!");
    return false;
  }
  auto syns = verbSyntaxes(verb);
  int len = static_cast<int>(syns.size());
  const Syntax *drive1 = nullptr;
  const Syntax *drive2 = nullptr;
  size_t i = 0;
  while (true) {
    const Syntax &syn = syns[i];
    int num = syn.nobj;
    if (s.ncn > num) {
      // this syntax takes fewer objects than were typed
    } else if (num >= 1 && s.ncn == 0 &&
               (s.itbl.prep1 == 0 || s.itbl.prep1 == syn.prep1)) {
      drive1 = &syn;
    } else if (syn.prep1 == s.itbl.prep1) {
      if (num == 2 && s.ncn == 1) {
        drive2 = &syn;
      } else if (syn.prep2 == s.itbl.prep2) {
        syntaxFound(&syn);
        return true;
      }
    }
    if (--len < 1) {
      if (drive1 || drive2) break;
      printLine("That sentence isn't one I recognize.");
      return false;
    }
    ++i;
  }
  ZObject *obj = nullptr;
  if (drive1 && (obj = gwim(drive1->fwim1, drive1->loc1, drive1->prep1))) {
    s.prso.assign(1, obj);
    syntaxFound(drive1);
    return true;
  } else if (drive2 && (obj = gwim(drive2->fwim2, drive2->loc2, drive2->prep2))) {
    s.prsi.assign(1, obj);
    syntaxFound(drive2);
    return true;
  } else if (verb == "find") {
    printLine("That question can't be answered.");
    return false;
  } else if (g.winner != g.player) {
    return cantOrphan();
  }
  orphan(drive1, drive2);
  print("What do you want to ");
  if (!s.otbl.verbn) {
    print("tell");
  } else if (!s.vtbl.haveText) {
    if (s.vtbl.word) print(s.vtbl.word->key);
  } else {
    wordPrint(s.vtbl.text);
    s.vtbl.haveText = false;
  }
  if (drive2) {
    print(" ");
    thingPrint(true, true);
  }
  g.pOflag = true;
  prepPrint(drive1 ? drive1->prep1 : drive2->prep2);
  printLine("?");
  return false;
}


// ============================================================================
// Noun-clause resolution (gparser.zil:928-1140)
// ============================================================================

// ZIL: <ROUTINE SNARF-OBJECTS () ...> (gparser.zil:928-943)
bool snarfObjects() {
  auto &g = Globals::instance();
  auto &s = state();
  s.buts.clear();
  Ptr iptr = s.itbl.nc2;
  Ptr optr = s.itbl.nc1;
  if (!iptr.isNull()) {
    g.pSlocbits = s.syntax ? s.syntax->loc2 : 0;
    if (!snarfem(iptr, s.itbl.nc2l, s.prsi)) return false;
  }
  if (!optr.isNull()) {
    g.pSlocbits = s.syntax ? s.syntax->loc1 : 0;
    if (!snarfem(optr, s.itbl.nc1l, s.prso)) return false;
  }
  if (!s.buts.empty()) {
    int l = static_cast<int>(s.prso.size());
    if (!optr.isNull()) butMerge(s.prso);
    if (!iptr.isNull() && (optr.isNull() || l == static_cast<int>(s.prso.size()))) {
      butMerge(s.prsi);
    }
  }
  return true;
}

// ZIL: <ROUTINE BUT-MERGE (TBL ...> (gparser.zil:945-958)
// Leaves the merged list in TBL and the previous contents in P-MERGE (the
// ZIL swaps the two tables).
void butMerge(std::vector<ZObject *> &tbl) {
  auto &s = state();
  s.merge.clear();
  for (auto *obj : tbl) {
    if (!zmemq(obj, s.buts)) s.merge.push_back(obj);
  }
  std::swap(tbl, s.merge);
}

// ZIL: <ROUTINE SNARFEM (PTR EPTR TBL ...> (gparser.zil:978-1030)
bool snarfem(Ptr ptr, Ptr eptr, std::vector<ZObject *> &tbl) {
  auto &g = Globals::instance();
  auto &s = state();
  std::vector<ZObject *> *but = nullptr;
  bool wasAll = false;
  s.pAnd = false;
  if (g.pGetFlags == P_ALL) wasAll = true;
  g.pGetFlags = 0;
  tbl.clear();
  const DictWord *wrd = wordAt(ptr);
  const DictWord *nw = nullptr;
  while (true) {
    if (ptr == eptr) {
      bool wv = getObject(but ? *but : tbl);
      if (wasAll) g.pGetFlags = P_ALL;
      return wv;
    }
    Ptr next{ptr.kind, ptr.idx + 1};
    nw = (next == eptr) ? nullptr : wordAt(next);
    if (wrd && wrd == W("all")) {
      g.pGetFlags = P_ALL;
      if (nw && nw == W("of")) ptr.idx += 1;
    } else if (wrd && (wrd == W("but") || wrd == W("except"))) {
      if (!getObject(but ? *but : tbl)) return false;
      but = &s.buts;
      s.buts.clear();
    } else if (wrd && (wrd == W("a") || wrd == W("one"))) {
      if (!s.adj) {
        g.pGetFlags = P_ONE;
        if (nw && nw == W("of")) ptr.idx += 1;
      } else {
        s.nam = s.oneobj;
        if (!getObject(but ? *but : tbl)) return false;
        if (!nw) return true;
      }
    } else if (isCommaAnd(wrd) && !isCommaAnd(nw)) {
      s.pAnd = true;
      if (!getObject(but ? *but : tbl)) return false;
    } else if (wt(wrd, PS_BUZZ_WORD)) {
      // buzzwords (including a doubled AND / comma) are skipped
    } else if (isCommaAnd(wrd)) {
      // unreachable: AND and comma are buzzwords
    } else if (wrd && wrd == W("of")) {
      if (g.pGetFlags == 0) g.pGetFlags = P_INHIBIT;
    } else if (wt(wrd, PS_ADJECTIVE) && !s.adj) {
      s.adj = wrd;
      s.adjn = wrd;
    } else if (wt(wrd, PS_OBJECT)) {
      s.nam = wrd;
      s.oneobj = wrd;
    }
    if (!(ptr == eptr)) {
      ptr.idx += 1;
      wrd = nw;
    }
  }
}

// ZIL: <ROUTINE GET-OBJECT (TBL "OPTIONAL" (VRB T) ...> (gparser.zil:1040-1140)
bool getObject(std::vector<ZObject *> &tbl, bool vrb) {
  auto &g = Globals::instance();
  auto &s = state();
  int xbits = g.pSlocbits;
  int tlen = static_cast<int>(tbl.size());
  bool gcheck = false;
  int olen = 0;
  int len = 0;
  if (g.pGetFlags & P_INHIBIT) return true;
  if (!s.nam && s.adj) {
    if (wt(s.adjn, PS_OBJECT)) {
      s.nam = s.adjn;
      s.adj = nullptr;
    }
    // (Zork III's direction-as-object case is compiled out: <NULL-F>)
  }
  if (!s.nam && !s.adj && g.pGetFlags != P_ALL && g.pGwimbit == 0) {
    if (vrb) printLine("There seems to be a noun missing in that sentence!");
    return false;
  }
  if (g.pGetFlags != P_ALL || g.pSlocbits == 0) g.pSlocbits = -1;
  s.table = &tbl;
  while (true) {
    if (gcheck) {
      globalCheck(tbl);
    } else {
      if (g.lit) {
        if (g.player) g.player->clearFlag(ObjectFlag::TRANSBIT);
        doSl(g.here, SOG, SIR);
        if (g.player) g.player->setFlag(ObjectFlag::TRANSBIT);
      }
      doSl(g.player, SH, SC);
    }
    len = static_cast<int>(tbl.size()) - tlen;
    if (g.pGetFlags & P_ALL) {
      // every match is kept
    } else if ((g.pGetFlags & P_ONE) && len != 0) {
      if (len != 1) {
        int pick = (std::rand() % len) + 1; // <RANDOM .LEN>
        tbl[0] = tbl[pick - 1];              // <PUT .TBL 1 <GET .TBL <RANDOM .LEN>>>
        print("(How about the ");
        printDesc(tbl[0]);
        printLine("?)");
      }
      tbl.resize(1); // <PUT .TBL ,P-MATCHLEN 1>
    } else if (len > 1 || (len == 0 && g.pSlocbits != -1)) {
      if (g.pSlocbits == -1) {
        g.pSlocbits = xbits;
        olen = len;
        tbl.resize(tbl.size() - len);
        continue; // <AGAIN>
      }
      if (len == 0) len = olen;
      if (g.winner != g.player) {
        cantOrphan();
        return false;
      } else if (vrb && s.nam) {
        whichPrint(tlen, len, tbl);
        s.aclause = (&tbl == &s.prso) ? P_NC1 : P_NC2;
        s.aadj = s.adj;
        s.anam = s.nam;
        orphan(nullptr, nullptr);
        g.pOflag = true;
      } else if (vrb) {
        printLine("There seems to be a noun missing in that sentence!");
      }
      s.nam = nullptr;
      s.adj = nullptr;
      return false;
    }
    if (len == 0 && gcheck) {
      if (vrb) {
        // "next added 1/2/85 by JW"
        g.pSlocbits = xbits;
        if (g.lit || g.prsa == V_TELL) {
          // "Changed 6/10/83 - MARC"
          objFound(g.getObject(ObjectIds::NOT_HERE_OBJECT), tbl);
          g.pXnam = s.nam ? s.nam->key : std::string();
          g.pXadjn = (s.adj && s.adjn) ? s.adjn->key : std::string();
          s.nam = nullptr;
          s.adj = nullptr;
          s.adjn = nullptr;
          return true;
        }
        printLine("It's too dark to see!");
      }
      s.nam = nullptr;
      s.adj = nullptr;
      return false;
    } else if (len == 0) {
      gcheck = true;
      continue; // <AGAIN>
    }
    g.pSlocbits = xbits;
    s.nam = nullptr;
    s.adj = nullptr;
    return true;
  }
}


// ============================================================================
// WHICH-PRINT and GLOBAL-CHECK (gparser.zil:1146-1200)
// ============================================================================

// ZIL: <ROUTINE WHICH-PRINT (TLEN LEN TBL ...> (gparser.zil:1146-1166)
void whichPrint(int tlen, int len, const std::vector<ZObject *> &tbl) {
  auto &g = Globals::instance();
  auto &s = state();
  int rlen = len;
  print("Which ");
  if (g.pOflag || g.pMerged || s.pAnd) {
    const DictWord *w = s.nam ? s.nam : (s.adj ? s.adjn : W("one"));
    if (w) print(w->key);
  } else {
    thingPrint(&tbl == &s.prso);
  }
  print(" do you mean, ");
  while (true) {
    tlen += 1;
    ZObject *obj = (tlen - 1 < static_cast<int>(tbl.size())) ? tbl[tlen - 1] : nullptr;
    print("the ");
    if (obj) printDesc(obj);
    if (len == 2) {
      if (rlen != 2) print(",");
      print(" or ");
    } else if (len > 2) {
      print(", ");
    }
    if (--len < 1) {
      printLine("?");
      return;
    }
  }
}

// ZIL: <ROUTINE GLOBAL-CHECK (TBL ...> (gparser.zil:1169-1200)
void globalCheck(std::vector<ZObject *> &tbl) {
  auto &g = Globals::instance();
  auto &s = state();
  int len = static_cast<int>(tbl.size());
  int obits = g.pSlocbits;
  auto *room = dynamic_cast<ZRoom *>(g.here);
  if (room) {
    // <GETPT ,HERE ,P?GLOBAL>: the room's GLOBAL list
    for (ObjectId id : room->getGlobals()) {
      ZObject *obj = g.getObject(id);
      if (obj && thisIt(obj)) objFound(obj, tbl);
    }
    // <GETPT ,HERE ,P?PSEUDO>: PSEUDO-OBJECT takes the matched routine
    // and the noun's name
    if (!room->getPseudos().empty()) {
      for (const auto &entry : room->getPseudos()) {
        if (s.nam && zkey(entry.word) == s.nam->key) {
          ZObject *pseudo = g.getObject(ObjectIds::PSEUDO_OBJECT);
          if (pseudo) {
            pseudo->setAction(entry.action);
            pseudo->setDesc(s.nam->key);
            objFound(pseudo, tbl);
          }
          break;
        }
      }
    }
  }
  if (static_cast<int>(tbl.size()) == len) {
    g.pSlocbits = -1;
    s.table = &tbl;
    doSl(g.getObject(ObjectIds::GLOBAL_OBJECTS), 1, 1);
    g.pSlocbits = obits;
    if (tbl.empty() && (g.prsa == V_LOOK_INSIDE || g.prsa == V_SEARCH || g.prsa == V_EXAMINE)) {
      doSl(g.getObject(ObjectIds::ROOMS), 1, 1);
    }
  }
}


// ============================================================================
// TAKE-CHECK, ITAKE-CHECK, MANY-CHECK (gparser.zil:1244-1313)
// ============================================================================

// ZIL: <ROUTINE TAKE-CHECK () ...> (gparser.zil:1244-1246)
bool takeCheck() {
  auto &s = state();
  int loc1 = s.syntax ? s.syntax->loc1 : 0;
  int loc2 = s.syntax ? s.syntax->loc2 : 0;
  return itakeCheck(s.prso, loc1) && itakeCheck(s.prsi, loc2);
}

// ZIL: <ROUTINE ITAKE-CHECK (TBL IBITS ...> (gparser.zil:1248-1292)
bool itakeCheck(std::vector<ZObject *> &tbl, int ibits) {
  auto &g = Globals::instance();
  int ptr = static_cast<int>(tbl.size());
  if (ptr != 0 && ((ibits & SHAVE) || (ibits & STAKE))) {
    ZObject *itObj = g.getObject(ObjectIds::IT);
    ZObject *adventurer = g.getObject(ObjectIds::ADVENTURER);
    while (true) {
      if (--ptr < 0) return true;
      ZObject *obj = tbl[ptr];
      if (obj && obj == itObj) {
        if (!isAccessible(g.it)) {
          printLine("I don't see what you're referring to.");
          return false;
        }
        obj = g.it;
      }
      if (obj && !Verbs::isHeld(obj) &&
          !(obj->getId() == ObjectIds::HANDS || obj->getId() == ObjectIds::ME)) {
        g.prso = obj;
        bool taken;
        if (obj->hasFlag(ObjectFlag::TRYTAKEBIT)) {
          taken = true;
        } else if (g.winner != adventurer) {
          taken = false;
        } else if ((ibits & STAKE) && static_cast<int>(Verbs::iTake(false)) == 1) {
          // <EQUAL? <ITAKE <>> T>: only a plain true counts as taken
          taken = false;
        } else {
          taken = true;
        }
        if (taken && (ibits & SHAVE) && g.winner == adventurer) {
          if (obj->getId() == ObjectIds::NOT_HERE_OBJECT) {
            printLine("You don't have that!");
            return false;
          }
          print("You don't have the ");
          printDesc(obj);
          printLine(".");
          return false;
        } else if (!taken && g.winner == adventurer) {
          printLine("(Taken)");
        }
      }
    }
  }
  return true;
}

// ZIL: <ROUTINE MANY-CHECK () ...> (gparser.zil:1294-1313)
bool manyCheck() {
  auto &g = Globals::instance();
  auto &s = state();
  int loss = 0;
  int loc1 = s.syntax ? s.syntax->loc1 : 0;
  int loc2 = s.syntax ? s.syntax->loc2 : 0;
  if (s.prso.size() > 1 && !(loc1 & SMANY)) {
    loss = 1;
  } else if (s.prsi.size() > 1 && !(loc2 & SMANY)) {
    loss = 2;
  }
  if (loss) {
    print("You can't use multiple ");
    if (loss == 2) print("in");
    print("direct objects with \"");
    if (!s.itbl.verbn) {
      print("tell");
    } else if (g.pOflag || g.pMerged) {
      if (s.vtbl.word) print(s.vtbl.word->key);
    } else {
      wordPrint(s.vtbl.text);
    }
    printLine("\".");
    return false;
  }
  return true;
}

} // namespace GParser
