#include "parser/gparser.h"
#include "core/globals.h"
#include "core/io.h"
#include "core/object.h"
#include "verbs/verbs.h"
#include "world/objects.h"
#include "world/rooms.h"
#include <algorithm>
#include <cctype>
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

void invalidateDictionary() { dict().built = false; }

void buildDictionary() {
  auto &g = Globals::instance();
  auto &d = dict();
  if (d.built && d.objectCount == g.getAllObjects().size()) return;
  d.words.clear();
  d.verbs.clear();
  d.prepositions.clear();
  d.prepNumbers.clear();
  d.prepositions.push_back(nullptr); // prep numbers start at 1

  const auto &syntaxes = GSyntax::getAllSyntaxes();

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

  // Object SYNONYMs and ADJECTIVEs.
  for (const auto &[id, obj] : g.getAllObjects()) {
    for (const auto &syn : obj->getSynonyms()) addWord(syn, PS_OBJECT);
    for (const auto &adj : obj->getAdjectives()) addWord(adj, PS_ADJECTIVE);
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

std::optional<int> clause(int, int, const DictWord *) { return std::nullopt; }
const DictWord *numberQ(int) { return nullptr; }
bool orphanMerge() { return false; }
bool aclauseWin(const DictWord *) { return true; }
bool nclauseWin() { return true; }
bool syntaxCheck() { return false; }
bool cantOrphan() {
  printLine("\"I don't understand! What are you referring to?\"");
  return false;
}
void orphan(const Syntax *, const Syntax *) {}
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
void clauseCopy(ITbl &, ITbl &, const DictWord *) {}
void clauseAdd(const DictWord *wrd) { state().oclause.push_back(wrd); }
void syntaxFound(const Syntax *syn) {
  state().syntax = syn;
  Globals::instance().prsa = syn ? syn->action : 0;
}
ZObject *gwim(uint64_t, int, int) { return nullptr; }
bool snarfObjects() { return true; }
void butMerge(std::vector<ZObject *> &) {}
bool snarfem(Ptr, Ptr, std::vector<ZObject *> &) { return false; }
bool getObject(std::vector<ZObject *> &, bool) { return false; }
void whichPrint(int, int, const std::vector<ZObject *> &) {}
void globalCheck(std::vector<ZObject *> &) {}
bool takeCheck() { return true; }
bool itakeCheck(std::vector<ZObject *> &, int) { return true; }
bool manyCheck() { return true; }
bool parser() { return false; }

} // namespace GParser
