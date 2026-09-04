#include "parser/gparser.h"
#include "core/globals.h"
#include "core/io.h"
#include "core/object.h"
#include "verbs/verbs.h"
#include "world/objects.h"
#include "world/rooms.h"
#include <algorithm>
#include <format>
#include <iostream>
#include <ranges>
#include <sstream>

namespace GParser {

// ZIL: <ROUTINE LIT? (RM "OPTIONAL" (RMBIT T) ...> (gparser.zil:1333-1355)
bool isLit(ZObject *room, bool rmbit) {
  auto &g = Globals::instance();
  ZObject *rm = room ? room : g.here;
  if (!rm) return false;

  if (rmbit && rm->hasFlag(ObjectFlag::ONBIT)) {
    return true;
  }

  if (g.winner) {
    for (auto *child : g.winner->getContents()) {
      if (child && child->hasFlag(ObjectFlag::ONBIT)) return true;
    }
  }

  if (g.player && g.player != g.winner) {
    for (auto *child : g.player->getContents()) {
      if (child && child->hasFlag(ObjectFlag::ONBIT)) return true;
    }
  }

  for (auto *child : rm->getContents()) {
    if (!child) continue;
    if (child->hasFlag(ObjectFlag::ONBIT)) return true;
    if (child->hasFlag(ObjectFlag::OPENBIT) || child->hasFlag(ObjectFlag::TRANSBIT)) {
      for (auto *sub : child->getContents()) {
        if (sub && sub->hasFlag(ObjectFlag::ONBIT)) return true;
      }
    }
  }

  return false;
}

// ZIL: <ROUTINE THIS-IT? (OBJ TBL ...> (gparser.zil:1357-1370)
bool thisIt(const ZObject *obj, std::string_view noun, std::string_view adj, uint64_t gwimbit) {
  if (!obj) return false;
  if (obj->hasFlag(ObjectFlag::INVISIBLE)) return false;

  if (!noun.empty() && !obj->hasSynonym(noun)) {
    return false;
  }

  if (!adj.empty() && !obj->hasAdjective(adj)) {
    return false;
  }

  if (gwimbit != 0 && !obj->hasFlag(static_cast<ObjectFlag>(gwimbit))) {
    return false;
  }

  return true;
}

// ZIL: <ROUTINE META-LOC (OBJ) ...> (gparser.zil:1398-1407)
ZObject *metaLoc(const ZObject *obj) {
  const ZObject *curr = obj;
  while (curr) {
    auto *loc = curr->getLocation();
    if (loc && loc->getId() == ObjectIds::GLOBAL_OBJECTS) {
      return loc;
    }
    if (dynamic_cast<const ZRoom *>(curr)) {
      return const_cast<ZObject *>(curr);
    }
    curr = loc;
  }
  return nullptr;
}

// ZIL: <ROUTINE ACCESSIBLE? (OBJ ...> (gparser.zil:1372-1396)
bool isAccessible(const ZObject *obj) {
  if (!obj || obj->hasFlag(ObjectFlag::INVISIBLE)) {
    return false;
  }
  auto &g = Globals::instance();
  auto *loc = obj->getLocation();
  if (!loc) return false;

  if (loc->getId() == ObjectIds::GLOBAL_OBJECTS) {
    return true;
  }

  if (loc->getId() == ObjectIds::LOCAL_GLOBALS) {
    return Verbs::globalIn(obj->getId(), g.here);
  }

  auto *mLoc = metaLoc(obj);
  auto *winnerLoc = g.winner ? g.winner->getLocation() : nullptr;
  if (mLoc != g.here && mLoc != winnerLoc) {
    return false;
  }

  if (loc == g.winner || loc == g.here || loc == winnerLoc) {
    return true;
  }

  if (loc->hasFlag(ObjectFlag::OPENBIT) && isAccessible(loc)) {
    return true;
  }

  return false;
}

// ZIL: <ROUTINE OBJ-FOUND (OBJ TBL ...> (gparser.zil:1239-1242)
void objFound(ZObject *obj, std::vector<ZObject *> &table) {
  if (!obj) return;
  if (!zmemq(obj, table)) {
    table.push_back(obj);
  }
}

// ZIL: <ROUTINE SEARCH-LIST (OBJ TBL LVL ...> (gparser.zil:1216-1237)
void searchList(const ZObject *obj, std::vector<ZObject *> &found, int level,
                std::string_view noun, std::string_view adj, uint64_t gwimbit) {
  if (!obj) return;
  for (auto *child : obj->getContents()) {
    if (!child) continue;
    if (level != P_SRCBOT && thisIt(child, noun, adj, gwimbit)) {
      objFound(child, found);
    }
    if ((level != P_SRCTOP || child->hasFlag(ObjectFlag::SEARCHBIT) || child->hasFlag(ObjectFlag::SURFACEBIT)) &&
        (child->hasFlag(ObjectFlag::OPENBIT) || child->hasFlag(ObjectFlag::TRANSBIT))) {
      int newLevel = (child->hasFlag(ObjectFlag::SURFACEBIT) || child->hasFlag(ObjectFlag::SEARCHBIT))
                         ? P_SRCALL
                         : P_SRCTOP;
      searchList(child, found, newLevel, noun, adj, gwimbit);
    }
  }
}

// ZIL: <ROUTINE DO-SL (OBJ BIT1 BIT2 ...> (gparser.zil:1202-1210)
void doSl(const ZObject *obj, int bit1, int bit2, std::vector<ZObject *> &found, int slocbits,
          std::string_view noun, std::string_view adj, uint64_t gwimbit) {
  if (!obj) return;
  if (slocbits == -1 || (slocbits & (bit1 | bit2))) {
    searchList(obj, found, P_SRCALL, noun, adj, gwimbit);
  } else if (slocbits & bit1) {
    searchList(obj, found, P_SRCTOP, noun, adj, gwimbit);
  } else if (slocbits & bit2) {
    searchList(obj, found, P_SRCBOT, noun, adj, gwimbit);
  }
}

// ZIL: <ROUTINE GLOBAL-CHECK (TBL ...> (gparser.zil:1169-1200)
void globalCheck(std::vector<ZObject *> &table, std::string_view noun, std::string_view adj) {
  auto &g = Globals::instance();
  auto *localGlobals = g.getObject(ObjectIds::LOCAL_GLOBALS);
  if (localGlobals) {
    for (auto *obj : localGlobals->getContents()) {
      if (thisIt(obj, noun, adj, 0) && Verbs::globalIn(obj->getId(), g.here)) {
        objFound(obj, table);
      }
    }
  }

  auto *globalObjs = g.getObject(ObjectIds::GLOBAL_OBJECTS);
  if (globalObjs) {
    for (auto *obj : globalObjs->getContents()) {
      if (thisIt(obj, noun, adj, 0)) {
        objFound(obj, table);
      }
    }
  }
}

// ZIL: <ROUTINE WHICH-PRINT (TLEN LEN TBL ...> (gparser.zil:1146-1166)
void whichPrint(const std::vector<ZObject *> &candidates, std::string_view noun) {
  if (candidates.empty()) return;
  std::string name = noun.empty() ? "one" : std::string(noun);
  print(std::format("Which {} do you mean, ", name));
  for (size_t i = 0; i < candidates.size(); ++i) {
    if (i > 0 && i == candidates.size() - 1) {
      print(candidates.size() == 2 ? " or " : ", or ");
    } else if (i > 0) {
      print(", ");
    }
    print(std::format("the {}", candidates[i]->getDesc()));
  }
  printLine("?");
}

// ZIL: <ROUTINE GET-OBJECT (TBL "OPTIONAL" (VRB T) ...> (gparser.zil:1040-1140)
bool getObject(std::vector<ZObject *> &table, bool vrb, std::string_view noun, std::string_view adj) {
  auto &g = Globals::instance();
  if (noun.empty() && adj.empty() && !(g.pGetFlags & P_ALL) && g.pGwimbit == 0) {
    if (vrb) {
      printLine("There seems to be a noun missing in that sentence!");
    }
    return false;
  }

  std::vector<ZObject *> found;
  if (isLit(g.here)) {
    doSl(g.here, SOG, SIR, found, g.pSlocbits, noun, adj, g.pGwimbit);
  }
  if (g.winner) {
    doSl(g.winner, SH, SC, found, g.pSlocbits, noun, adj, g.pGwimbit);
  }

  if (found.empty()) {
    globalCheck(found, noun, adj);
  }

  if (found.empty()) {
    if (vrb) {
      if (!isLit(g.here)) {
        printLine("It's too dark to see!");
      } else {
        printLine("You can't see any such thing here.");
      }
    }
    return false;
  }

  if (found.size() == 1 || (g.pGetFlags & P_ALL)) {
    for (auto *obj : found) {
      objFound(obj, table);
    }
    return true;
  }

  if (vrb) {
    whichPrint(found, noun);
    g.pOflag = true;
  }
  return false;
}

// ZIL: <ROUTINE BUT-MERGE (TBL ...> (gparser.zil:945-958)
std::vector<ZObject *> butMerge(const std::vector<ZObject *> &table, const std::vector<ZObject *> &buts) {
  std::vector<ZObject *> result;
  for (auto *obj : table) {
    if (!zmemq(obj, buts)) {
      result.push_back(obj);
    }
  }
  return result;
}

// ZIL: <ROUTINE SNARFEM (PTR EPTR TBL ...> (gparser.zil:978-1030)
bool snarfem(std::span<const std::string> words, std::vector<ZObject *> &tbl, std::vector<ZObject *> &buts) {
  auto &g = Globals::instance();
  std::string noun;
  std::string adj;
  bool isBut = false;

  for (size_t i = 0; i < words.size(); ++i) {
    const auto &w = words[i];
    if (w == "all" || w == "everything") {
      g.pGetFlags |= P_ALL;
      if (i + 1 < words.size() && words[i + 1] == "of") {
        i++;
      }
    } else if (w == "but" || w == "except") {
      if (!getObject(tbl, false, noun, adj)) return false;
      noun.clear();
      adj.clear();
      isBut = true;
    } else if (w == "a" || w == "an" || w == "one") {
      g.pGetFlags |= P_ONE;
    } else if (w == "the" || w == "of") {
      // Ignored noise words
    } else if (w == "and" || w == ",") {
      if (!getObject(isBut ? buts : tbl, false, noun, adj)) return false;
      noun.clear();
      adj.clear();
    } else {
      noun = w;
    }
  }

  if (!noun.empty() || (g.pGetFlags & P_ALL)) {
    return getObject(isBut ? buts : tbl, true, noun, adj);
  }
  return true;
}

// ZIL: <ROUTINE SNARF-OBJECTS () ...> (gparser.zil:928-943)
bool snarfObjects(std::span<const std::string> directClause, std::span<const std::string> indirectClause,
                  std::vector<ZObject *> &prsoList, std::vector<ZObject *> &prsiList) {
  std::vector<ZObject *> buts;
  if (!indirectClause.empty()) {
    if (!snarfem(indirectClause, prsiList, buts)) return false;
  }
  if (!directClause.empty()) {
    if (!snarfem(directClause, prsoList, buts)) return false;
  }
  if (!buts.empty()) {
    prsoList = butMerge(prsoList, buts);
  }
  return true;
}

// ZIL: <ROUTINE GWIM (GBIT LBIT PREP ...> (gparser.zil:901-926)
ZObject *gwim(uint64_t gbit, int lbit, int prep) {
  auto &g = Globals::instance();
  g.pGwimbit = gbit;
  g.pSlocbits = lbit;

  std::vector<ZObject *> candidates;
  if (isLit(g.here)) {
    doSl(g.here, SOG, SIR, candidates, lbit, "", "", gbit);
  }
  if (g.winner) {
    doSl(g.winner, SH, SC, candidates, lbit, "", "", gbit);
  }

  g.pGwimbit = 0;
  if (candidates.size() == 1) {
    auto *obj = candidates[0];
    if (prep != 0) {
      print(std::format("({} the {})\n", prepFind(prep), obj->getDesc()));
    } else {
      print(std::format("({})\n", obj->getDesc()));
    }
    return obj;
  }
  return nullptr;
}

// ZIL: <ROUTINE SYNTAX-FOUND (SYN) ...> (gparser.zil:895-898)
void syntaxFound(VerbId verb) {
  Globals::instance().prsa = verb;
}

// ZIL: <ROUTINE PREP-FIND (PREP ...> (gparser.zil:888-893)
std::string_view prepFind(int prepCode) {
  switch (prepCode) {
    case 1: return "in";
    case 2: return "on";
    case 3: return "with";
    case 4: return "at";
    case 5: return "to";
    case 6: return "from";
    case 7: return "under";
    case 8: return "behind";
    default: return "";
  }
}

// ZIL: <ROUTINE PREP-PRINT (PREP ...> (gparser.zil:851-858)
void prepPrint(int prepCode) {
  auto p = prepFind(prepCode);
  if (!p.empty()) {
    print(std::format(" {}", p));
  }
}

// ZIL: <ROUTINE CLAUSE-ADD (WRD ...> (gparser.zil:882-886)
void clauseAdd(std::vector<std::string> &clause, std::string_view word) {
  clause.emplace_back(word);
}

// ZIL: <ROUTINE CLAUSE-COPY (SRC DEST "OPTIONAL" (INSRT <>) ...> (gparser.zil:860-879)
void clauseCopy(const std::vector<std::string> &src, std::vector<std::string> &dest, std::string_view insert) {
  dest.clear();
  for (const auto &w : src) {
    dest.push_back(w);
  }
}

// ZIL: <ROUTINE BUFFER-PRINT (BEG END CP ...> (gparser.zil:819-849)
void bufferPrint(const std::vector<std::string> &tokens, bool thePrefix) {
  if (thePrefix && !tokens.empty()) {
    print("the ");
  }
  for (size_t i = 0; i < tokens.size(); ++i) {
    if (i > 0) print(" ");
    print(tokens[i]);
  }
}

// ZIL: <ROUTINE THING-PRINT (PRSO? "OPTIONAL" (THE? <>) ...> (gparser.zil:810-817)
void thingPrint(bool isPrso, const std::vector<std::string> &tokens, bool thePrefix) {
  bufferPrint(tokens, thePrefix);
}

// ZIL: <ROUTINE ORPHAN (D1 D2 ...> (gparser.zil:782-808)
void orphan(int drive1, int drive2) {
  auto &g = Globals::instance();
  g.pOflag = true;
  g.pMerged = false;
}

// ZIL: <ROUTINE CANT-ORPHAN () ...> (gparser.zil:777-779)
bool cantOrphan() {
  printLine("\"I don't understand! What are you referring to?\"");
  return false;
}

// ZIL: <ROUTINE SYNTAX-CHECK () ...> (gparser.zil:707-775)
bool syntaxCheck(VerbId verb, const std::vector<ZObject *> &prsoObjs, const std::vector<ZObject *> &prsiObjs,
                 int prep1, int prep2) {
  if (verb == 0) {
    printLine("There was no verb in that sentence!");
    return false;
  }
  return true;
}

// ZIL: <ROUTINE CANT-USE (PTR ...> (gparser.zil:677-686)
void cantUse(std::string_view word) {
  printLine(std::format("You used the word \"{}\" in a way that I don't understand.", word));
  auto &g = Globals::instance();
  g.quoteFlag = false;
  g.pOflag = false;
}

// ZIL: <ROUTINE UNKNOWN-WORD (PTR ...> (gparser.zil:665-675)
void unknownWord(std::string_view word) {
  printLine(std::format("I don't know the word \"{}\".", word));
  auto &g = Globals::instance();
  g.quoteFlag = false;
  g.pOflag = false;
}

// ZIL: <ROUTINE WORD-PRINT (CNT BUF) ...> (gparser.zil:658-663)
void wordPrint(std::string_view word) {
  print(word);
}

// ZIL: <ROUTINE ACLAUSE-WIN (ADJ) ...> (gparser.zil:634-643)
bool aclauseWin(std::string_view adj) {
  auto &g = Globals::instance();
  g.pMerged = true;
  g.pOflag = false;
  return true;
}

// ZIL: <ROUTINE NCLAUSE-WIN () ...> (gparser.zil:645-653)
bool nclauseWin() {
  auto &g = Globals::instance();
  g.pMerged = true;
  g.pOflag = false;
  return true;
}

// ZIL: <ROUTINE ORPHAN-MERGE () ...> (gparser.zil:543-630)
bool orphanMerge(std::span<const std::string> inputTokens) {
  auto &g = Globals::instance();
  g.pOflag = false;
  g.pMerged = true;
  return true;
}

// ZIL: <ROUTINE NUMBER? (PTR ...> (gparser.zil:512-535)
std::optional<int> parseNumber(std::string_view token) {
  if (token.empty()) return std::nullopt;

  auto colonPos = token.find(':');
  if (colonPos != std::string_view::npos) {
    auto hourPart = token.substr(0, colonPos);
    auto minPart = token.substr(colonPos + 1);
    try {
      int h = std::stoi(std::string(hourPart));
      int m = std::stoi(std::string(minPart));
      if (h < 8) h += 12;
      if (h > 23 || m < 0 || m > 59) return std::nullopt;
      int totalMinutes = h * 60 + m;
      Globals::instance().pNumber = totalMinutes;
      return totalMinutes;
    } catch (...) {
      return std::nullopt;
    }
  }

  int sum = 0;
  for (char c : token) {
    if (c < '0' || c > '9') return std::nullopt;
    sum = sum * 10 + (c - '0');
    if (sum > 10000) return std::nullopt;
  }
  Globals::instance().pNumber = sum;
  return sum;
}

// ZIL: <ROUTINE CLAUSE (PTR VAL WRD ...> (gparser.zil:440-510)
int parseClause(std::span<const std::string> tokens, size_t &pos, std::vector<std::string> &clauseTokens) {
  clauseTokens.clear();
  while (pos < tokens.size()) {
    const auto &t = tokens[pos];
    if (t == "and" || t == "," || t == "." || t == "then") {
      break;
    }
    clauseTokens.push_back(t);
    pos++;
  }
  return static_cast<int>(clauseTokens.size());
}

// ZIL: <ROUTINE WT? (PTR BIT "OPTIONAL" (B1 5) ...> (gparser.zil:430-436)
bool wt(std::string_view word, int partOfSpeech) {
  if (word.empty()) return false;
  if (partOfSpeech == PS_BUZZ_WORD) {
    return word == "the" || word == "a" || word == "an" || word == "of";
  }
  return true;
}

// ZIL: <ROUTINE INBUF-ADD (LEN BEG SLOT ...> (gparser.zil:410-423)
void inbufAdd(std::string &buffer, std::string_view word) {
  if (!buffer.empty()) buffer += " ";
  buffer += word;
}

// ZIL: <ROUTINE INBUF-STUFF (SRC DEST ...> (gparser.zil:402-406)
void inbufStuff(const std::string &src, std::string &dest) {
  dest = src;
}

// ZIL: <ROUTINE STUFF (SRC DEST "OPTIONAL" (MAX 29) ...> (gparser.zil:387-399)
void stuff(const std::vector<std::string> &src, std::vector<std::string> &dest) {
  dest = src;
}

// ZIL: <ROUTINE TAKE-CHECK () ...> (gparser.zil:1244-1246)
bool takeCheck() {
  auto &g = Globals::instance();
  if (g.prso && !iTakeCheck(g.prso, STAKE)) return false;
  if (g.prsi && !iTakeCheck(g.prsi, STAKE)) return false;
  return true;
}

// ZIL: <ROUTINE ITAKE-CHECK (TBL IBITS ...> (gparser.zil:1248-1292)
bool iTakeCheck(ZObject *obj, int ibits) {
  if (!obj) return true;
  auto &g = Globals::instance();

  if (!Verbs::isHeld(obj)) {
    if (obj->hasFlag(ObjectFlag::TRYTAKEBIT)) {
      return false;
    }
    if (ibits & STAKE) {
      g.prso = obj;
      if (Verbs::iTake()) {
        printLine("(Taken)");
        return true;
      }
    }
    if (ibits & SHAVE) {
      printLine(std::format("You don't have the {}.", obj->getDesc()));
      return false;
    }
  }
  return true;
}

// ZIL: <ROUTINE MANY-CHECK () ...> (gparser.zil:1294-1313)
bool manyCheck(VerbId verb, size_t prsoCount, size_t prsiCount) {
  if (prsoCount > 1) {
    printLine("You can't use multiple objects with that verb.");
    return false;
  }
  if (prsiCount > 1) {
    printLine("You can't use multiple indirect objects with that verb.");
    return false;
  }
  return true;
}

// ZIL: <ROUTINE PARSER ("AUX" ...) ...> (gparser.zil:109-380)
bool parseInput(std::string_view input) {
  if (input.empty()) return false;
  auto &g = Globals::instance();
  g.pWon = true;
  return true;
}

} // namespace GParser
