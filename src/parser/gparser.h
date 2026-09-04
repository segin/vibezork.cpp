#pragma once
#include "core/flags.h"
#include "core/globals.h"
#include "core/object.h"
#include "core/types.h"
#include "world/rooms.h"
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

/**
 * @file gparser.h
 * @brief Canonical parser routines and constants ported from GPARSER.ZIL
 *
 * Source: zil/gparser.zil:1-1407
 *
 * Implements:
 * - Table operations: ZMEMQ, ZMEMQB, BUT-MERGE, STUFF, INBUF-STUFF, INBUF-ADD
 * - Word & input inspection: NUMBER?, WT?, CLAUSE, BUFFER-PRINT, WORD-PRINT, THING-PRINT, UNKNOWN-WORD, CANT-USE
 * - Object accessibility & visibility: ACCESSIBLE?, META-LOC, LIT?, THIS-IT?
 * - Object searches: DO-SL, SEARCH-LIST, OBJ-FOUND, GLOBAL-CHECK, GWIM, GET-OBJECT, WHICH-PRINT
 * - Syntax & phrase parsing: SNARFEM, SNARF-OBJECTS, SYNTAX-CHECK, SYNTAX-FOUND, PREP-FIND, PREP-PRINT
 * - Orphaning & resumption: ORPHAN, ORPHAN-MERGE, ACLAUSE-WIN, NCLAUSE-WIN, CANT-ORPHAN, CLAUSE-COPY, CLAUSE-ADD
 * - Execution checks: TAKE-CHECK, ITAKE-CHECK, MANY-CHECK
 * - Top-level loop: PARSER
 */

namespace GParser {

// ============================================================================
// Constants from gparser.zil
// ============================================================================

// Parts of speech bit masks
constexpr int PS_DIRECTION = 1;
constexpr int PS_ADJECTIVE = 2;
constexpr int PS_VERB = 4;
constexpr int PS_OBJECT = 8;
constexpr int PS_PREPOSITION = 16;
constexpr int PS_BUZZ_WORD = 32;

// Search location bits (gparser.zil:1032-1038)
constexpr int SH = 128;   // Search held (in winner's inventory)
constexpr int SC = 64;    // Search carrier (in winner)
constexpr int SIR = 32;   // Search in room
constexpr int SOG = 16;   // Search on ground
constexpr int STAKE = 8;  // Implicit take allowed
constexpr int SMANY = 4;  // Multiple objects allowed
constexpr int SHAVE = 2;  // Must have (held)

// Search levels for SEARCH-LIST (gparser.zil:1212-1214)
constexpr int P_SRCBOT = 2;
constexpr int P_SRCTOP = 0;
constexpr int P_SRCALL = 1;

// GETFLAGS bits (gparser.zil:971-973)
constexpr int P_ALL = 1;
constexpr int P_ONE = 2;
constexpr int P_INHIBIT = 4;

// CLAUSE-COPY table offsets (gparser.zil:24-27)
constexpr int CC_SBPTR = 0;
constexpr int CC_SEPTR = 1;
constexpr int CC_DBPTR = 2;
constexpr int CC_DEPTR = 3;

// OOPS-TABLE offsets (gparser.zil:51-54)
constexpr int O_PTR = 0;
constexpr int O_START = 1;
constexpr int O_LENGTH = 2;
constexpr int O_END = 3;

// ============================================================================
// Routine Declarations (gparser.zil:109-1407)
// ============================================================================

// ZIL: <ROUTINE ZMEMQ (ITM TBL "OPTIONAL" (SIZE -1) ...> (gparser.zil:1315-1322)
template <typename T, typename U>
inline bool zmemq(const T &item, std::span<U> table) {
  for (const auto &elem : table) {
    if (elem == item) return true;
  }
  return false;
}

template <typename T, typename U>
inline bool zmemq(const T &item, const std::vector<U> &table) {
  for (const auto &elem : table) {
    if (elem == item) return true;
  }
  return false;
}

// ZIL: <ROUTINE ZMEMQB (ITM TBL SIZE ...> (gparser.zil:1324-1329)
inline bool zmemqb(uint8_t byteVal, std::span<const uint8_t> table) {
  for (uint8_t b : table) {
    if (b == byteVal) return true;
  }
  return false;
}

// ZIL: <ROUTINE LIT? (RM "OPTIONAL" (RMBIT T) ...> (gparser.zil:1333-1355)
bool isLit(ZObject *room = nullptr, bool rmbit = true);

// ZIL: <ROUTINE THIS-IT? (OBJ TBL ...> (gparser.zil:1357-1370)
bool thisIt(const ZObject *obj, std::string_view noun = "", std::string_view adj = "", uint64_t gwimbit = 0);

// ZIL: <ROUTINE ACCESSIBLE? (OBJ ...> (gparser.zil:1372-1396)
bool isAccessible(const ZObject *obj);

// ZIL: <ROUTINE META-LOC (OBJ) ...> (gparser.zil:1398-1407)
ZObject *metaLoc(const ZObject *obj);

// ZIL: <ROUTINE DO-SL (OBJ BIT1 BIT2 ...> (gparser.zil:1202-1210)
void doSl(const ZObject *obj, int bit1, int bit2, std::vector<ZObject *> &found, int slocbits = -1,
          std::string_view noun = "", std::string_view adj = "", uint64_t gwimbit = 0);

// ZIL: <ROUTINE SEARCH-LIST (OBJ TBL LVL ...> (gparser.zil:1216-1237)
void searchList(const ZObject *obj, std::vector<ZObject *> &found, int level,
                std::string_view noun = "", std::string_view adj = "", uint64_t gwimbit = 0);

// ZIL: <ROUTINE OBJ-FOUND (OBJ TBL ...> (gparser.zil:1239-1242)
void objFound(ZObject *obj, std::vector<ZObject *> &table);

// ZIL: <ROUTINE GLOBAL-CHECK (TBL ...> (gparser.zil:1169-1200)
void globalCheck(std::vector<ZObject *> &table, std::string_view noun, std::string_view adj = "");

// ZIL: <ROUTINE WHICH-PRINT (TLEN LEN TBL ...> (gparser.zil:1146-1166)
void whichPrint(const std::vector<ZObject *> &candidates, std::string_view noun);

// ZIL: <ROUTINE GET-OBJECT (TBL "OPTIONAL" (VRB T) ...> (gparser.zil:1040-1140)
bool getObject(std::vector<ZObject *> &table, bool vrb = true, std::string_view noun = "", std::string_view adj = "");

// ZIL: <ROUTINE BUT-MERGE (TBL ...> (gparser.zil:945-958)
std::vector<ZObject *> butMerge(const std::vector<ZObject *> &table, const std::vector<ZObject *> &buts);

// ZIL: <ROUTINE SNARFEM (PTR EPTR TBL ...> (gparser.zil:978-1030)
bool snarfem(std::span<const std::string> words, std::vector<ZObject *> &tbl, std::vector<ZObject *> &buts);

// ZIL: <ROUTINE SNARF-OBJECTS () ...> (gparser.zil:928-943)
bool snarfObjects(std::span<const std::string> directClause, std::span<const std::string> indirectClause,
                  std::vector<ZObject *> &prsoList, std::vector<ZObject *> &prsiList);

// ZIL: <ROUTINE GWIM (GBIT LBIT PREP ...> (gparser.zil:901-926)
ZObject *gwim(uint64_t gbit, int lbit, int prep = 0);

// ZIL: <ROUTINE SYNTAX-FOUND (SYN) ...> (gparser.zil:895-898)
void syntaxFound(VerbId verb);

// ZIL: <ROUTINE PREP-FIND (PREP ...> (gparser.zil:888-893)
std::string_view prepFind(int prepCode);

// ZIL: <ROUTINE PREP-PRINT (PREP ...> (gparser.zil:851-858)
void prepPrint(int prepCode);

// ZIL: <ROUTINE CLAUSE-ADD (WRD ...> (gparser.zil:882-886)
void clauseAdd(std::vector<std::string> &clause, std::string_view word);

// ZIL: <ROUTINE CLAUSE-COPY (SRC DEST "OPTIONAL" (INSRT <>) ...> (gparser.zil:860-879)
void clauseCopy(const std::vector<std::string> &src, std::vector<std::string> &dest, std::string_view insert = "");

// ZIL: <ROUTINE BUFFER-PRINT (BEG END CP ...> (gparser.zil:819-849)
void bufferPrint(const std::vector<std::string> &tokens, bool thePrefix = false);

// ZIL: <ROUTINE THING-PRINT (PRSO? "OPTIONAL" (THE? <>) ...> (gparser.zil:810-817)
void thingPrint(bool isPrso, const std::vector<std::string> &tokens, bool thePrefix = false);

// ZIL: <ROUTINE ORPHAN (D1 D2 ...> (gparser.zil:782-808)
void orphan(int drive1 = 0, int drive2 = 0);

// ZIL: <ROUTINE CANT-ORPHAN () ...> (gparser.zil:777-779)
bool cantOrphan();

// ZIL: <ROUTINE SYNTAX-CHECK () ...> (gparser.zil:707-775)
bool syntaxCheck(VerbId verb, const std::vector<ZObject *> &prsoObjs, const std::vector<ZObject *> &prsiObjs,
                 int prep1 = 0, int prep2 = 0);

// ZIL: <ROUTINE CANT-USE (PTR ...> (gparser.zil:677-686)
void cantUse(std::string_view word);

// ZIL: <ROUTINE UNKNOWN-WORD (PTR ...> (gparser.zil:665-675)
void unknownWord(std::string_view word);

// ZIL: <ROUTINE WORD-PRINT (CNT BUF) ...> (gparser.zil:658-663)
void wordPrint(std::string_view word);

// ZIL: <ROUTINE ACLAUSE-WIN (ADJ) ...> (gparser.zil:634-643)
bool aclauseWin(std::string_view adj);

// ZIL: <ROUTINE NCLAUSE-WIN () ...> (gparser.zil:645-653)
bool nclauseWin();

// ZIL: <ROUTINE ORPHAN-MERGE () ...> (gparser.zil:543-630)
bool orphanMerge(std::span<const std::string> inputTokens);

// ZIL: <ROUTINE NUMBER? (PTR ...> (gparser.zil:512-535)
std::optional<int> parseNumber(std::string_view token);

// ZIL: <ROUTINE CLAUSE (PTR VAL WRD ...> (gparser.zil:440-510)
int parseClause(std::span<const std::string> tokens, size_t &pos, std::vector<std::string> &clauseTokens);

// ZIL: <ROUTINE WT? (PTR BIT "OPTIONAL" (B1 5) ...> (gparser.zil:430-436)
bool wt(std::string_view word, int partOfSpeech);

// ZIL: <ROUTINE INBUF-ADD (LEN BEG SLOT ...> (gparser.zil:410-423)
void inbufAdd(std::string &buffer, std::string_view word);

// ZIL: <ROUTINE INBUF-STUFF (SRC DEST ...> (gparser.zil:402-406)
void inbufStuff(const std::string &src, std::string &dest);

// ZIL: <ROUTINE STUFF (SRC DEST "OPTIONAL" (MAX 29) ...> (gparser.zil:387-399)
void stuff(const std::vector<std::string> &src, std::vector<std::string> &dest);

// ZIL: <ROUTINE TAKE-CHECK () ...> (gparser.zil:1244-1246)
bool takeCheck();

// ZIL: <ROUTINE ITAKE-CHECK (TBL IBITS ...> (gparser.zil:1248-1292)
bool iTakeCheck(ZObject *obj, int ibits);

// ZIL: <ROUTINE MANY-CHECK () ...> (gparser.zil:1294-1313)
bool manyCheck(VerbId verb, size_t prsoCount, size_t prsiCount);

// ZIL: <ROUTINE PARSER ("AUX" ...) ...> (gparser.zil:109-380)
bool parseInput(std::string_view input);

} // namespace GParser
