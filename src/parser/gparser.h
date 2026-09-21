#pragma once
#include "core/flags.h"
#include "core/globals.h"
#include "core/object.h"
#include "core/types.h"
#include "parser/gsyntax.h"
#include "world/rooms.h"
#include <array>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

/**
 * @file gparser.h
 * @brief Verbatim port of GPARSER.ZIL (the Zork trilogy parser)
 *
 * Source: zil/gparser.zil:1-1407
 *
 * The data model mirrors the ZIL tables byte for byte where the game's
 * behaviour depends on them:
 *  - the dictionary (Z-machine v3 words truncated to six z-characters),
 *  - P-LEXV / AGAIN-LEXV / RESERVE-LEXV (59-entry LEXV tables),
 *  - P-INBUF / OOPS-INBUF (120-byte READ buffers),
 *  - P-ITBL / P-OTBL (the input and orphan tables), P-VTBL / P-OVTBL,
 *  - P-PRSO / P-PRSI / P-BUTS / P-MERGE (50-entry match tables),
 *  - P-OCLAUSE (the orphan clause buffer), OOPS-TABLE, P-CCTBL.
 *
 * Every routine keeps its ZIL name (camel-cased) and a Source: reference.
 */

namespace GParser {

// ============================================================================
// Constants (gparser.zil:73-99, 693-705, 960-976, 1032-1038, 1212-1214)
// ============================================================================

// Parts of speech (ZILCH PS?/P1? bit values)
constexpr int PS_OBJECT = 128;
constexpr int PS_VERB = 64;
constexpr int PS_ADJECTIVE = 32;
constexpr int PS_DIRECTION = 16;
constexpr int PS_PREPOSITION = 8;
constexpr int PS_BUZZ_WORD = 4;

// Table sizes
constexpr int P_LEXV_SIZE = 59;     // <ITABLE 59 (LEXV) ...> (gparser.zil:34-40)
constexpr int P_INBUF_SIZE = 120;   // <ITABLE 120 (BYTE LENGTH) 0> (gparser.zil:44-47)
constexpr int STUFF_MAX = 29;       // STUFF "OPTIONAL" (MAX 29) (gparser.zil:387)
constexpr int P_ITBLLEN = 9;        // (gparser.zil:82)
constexpr int P_MATCH_SIZE = 50;    // <ITABLE NONE 50> (gparser.zil:964-967)
constexpr int P_OCLAUSE_SIZE = 100; // <ITABLE NONE 100> (gparser.zil:968)

// P-ITBL slot numbers (gparser.zil:90-99)
constexpr int P_VERB = 0;
constexpr int P_VERBN = 1;
constexpr int P_PREP1 = 2;
constexpr int P_PREP1N = 3;
constexpr int P_PREP2 = 4;
constexpr int P_PREP2N = 5;
constexpr int P_NC1 = 6;
constexpr int P_NC1L = 7;
constexpr int P_NC2 = 8;
constexpr int P_NC2L = 9;

// Syntax record layout (gparser.zil:695-705)
constexpr int P_SONUMS = 3;

// Search location bits (gparser.zil:1032-1038)
constexpr int SH = 128;
constexpr int SC = 64;
constexpr int SIR = 32;
constexpr int SOG = 16;
constexpr int STAKE = 8;
constexpr int SMANY = 4;
constexpr int SHAVE = 2;

// Search levels (gparser.zil:1212-1214)
constexpr int P_SRCBOT = 2;
constexpr int P_SRCTOP = 0;
constexpr int P_SRCALL = 1;

// P-GETFLAGS values (gparser.zil:971-973)
constexpr int P_ALL = 1;
constexpr int P_ONE = 2;
constexpr int P_INHIBIT = 4;

// CLAUSE-COPY pointer table offsets (gparser.zil:24-27)
constexpr int CC_SBPTR = 0;
constexpr int CC_SEPTR = 1;
constexpr int CC_DBPTR = 2;
constexpr int CC_DEPTR = 3;

// ============================================================================
// Dictionary (Z-machine v3 vocabulary as ZILCH built it from GSYNTAX and
// the OBJECT definitions)
// ============================================================================

struct DictWord {
  std::string key;                 // the encoded (truncated) word; PRINTB prints this
  int ps = 0;                      // parts-of-speech bits (PS_*)
  std::string verb;                // canonical verb word (ACT?xxx) when PS_VERB
  int prep = 0;                    // preposition number (PR?xxx) when PS_PREPOSITION
  std::optional<Direction> dir;    // direction (P?xxx) when PS_DIRECTION
};

// Z-machine v3 dictionary truncation: the word encoded to six z-characters,
// given back as the prefix of characters that fit (A0 letters cost one
// z-character, digits and punctuation two, anything else four).
std::string zkey(std::string_view word);

// (Re)build the dictionary from GSyntax and every object's SYNONYM and
// ADJECTIVE words. Cheap when nothing changed.
void buildDictionary();
// Force a rebuild on the next use (objects were added).
void invalidateDictionary();

// Dictionary lookup of a typed word (truncated like READ does).
const DictWord *lookupWord(std::string_view typed);
// W?xxx: dictionary word by its exact key; nullptr if absent.
const DictWord *W(std::string_view key);

// Syntax record (one SYNTAX line as the VERBS table stores it,
// gparser.zil:693-705)
struct Syntax {
  int nobj = 0;          // P-SBITS & P-SONUMS
  int prep1 = 0;         // P-SPREP1
  int prep2 = 0;         // P-SPREP2
  uint64_t fwim1 = 0;    // P-SFWIM1 (FIND bit)
  uint64_t fwim2 = 0;    // P-SFWIM2
  int loc1 = 0;          // P-SLOC1 (SH SC SIR SOG STAKE SMANY SHAVE)
  int loc2 = 0;          // P-SLOC2
  VerbId action = 0;     // P-SACTION
  const GSyntax::ZilSyntax *def = nullptr;
};

// <GET ,VERBS <- 255 .VERB>>: the syntax records for a verb word, in the
// order SYNTAX-CHECK scans them.
std::span<const Syntax> verbSyntaxes(std::string_view canonicalVerb);
// PREP-FIND: the dictionary word printed for a preposition number.
const DictWord *prepFind(int prep);

// ============================================================================
// Tables
// ============================================================================

// One LEXV entry: the dictionary word (nullptr when unknown) and the typed
// characters (what WORD-PRINT echoes).
struct LexEntry {
  const DictWord *w = nullptr;
  std::string text;
};

// P-LEXV, AGAIN-LEXV, RESERVE-LEXV: byte 1 is the word count, entries
// follow. Indices here are entry indices (ZIL word pointer PTR maps to
// entry (PTR-1)/2).
struct LexTable {
  int count = 0;
  std::array<LexEntry, P_LEXV_SIZE + 1> e{};
};

// A pointer into P-LEXV or P-OCLAUSE, or one of the two special values the
// P-ITBL noun-clause slots take: 0 (none) and 1 (orphan marker).
struct Ptr {
  enum Kind : int8_t { Null = 0, One = 1, Lex = 2, Ocl = 3 };
  Kind kind = Null;
  int idx = 0;
  static Ptr lex(int i) { return Ptr{Lex, i}; }
  static Ptr ocl(int i) { return Ptr{Ocl, i}; }
  static Ptr one() { return Ptr{One, 0}; }
  bool isNull() const { return kind == Null; }
  bool operator==(const Ptr &) const = default;
};

// P-VTBL / P-OVTBL: the verb word and (when byte 2 is non-zero) the typed
// text WORD-PRINT echoes (gparser.zil:86-87).
struct Vtbl {
  const DictWord *word = nullptr;
  std::string text;
  bool haveText = false; // GETB 2 non-zero
};

// P-ITBL / P-OTBL (gparser.zil:82-99)
struct ITbl {
  std::string verb;               // P-VERB: the ACT? value (canonical verb word; empty = 0)
  bool verbn = false;             // P-VERBN (0 or ,P-VTBL)
  int prep1 = 0;                  // P-PREP1
  const DictWord *prep1n = nullptr;
  int prep2 = 0;                  // P-PREP2
  const DictWord *prep2n = nullptr;
  Ptr nc1, nc1l, nc2, nc2l;       // P-NC1 .. P-NC2L
  Ptr &slot(int n);
  const Ptr &slot(int n) const;
  void clear() { *this = ITbl{}; }
};

// OOPS-TABLE (gparser.zil:48-54)
struct OopsTable {
  int ptr = -1;      // O-PTR: entry index of the unknown word, -1 = <>
  int start = 0;     // O-START: entry index of the sentence start
  int length = 0;    // O-LENGTH
  bool end = false;  // O-END (non-<> once a correction was appended)
};

struct State {
  // Input buffers
  LexTable lexv;         // P-LEXV
  LexTable againLexv;    // AGAIN-LEXV
  LexTable reserveLexv;  // RESERVE-LEXV
  int reservePtr = -1;   // RESERVE-PTR (-1 = <>)
  std::string inbuf;     // P-INBUF (the typed line)
  std::string oopsInbuf; // OOPS-INBUF
  OopsTable oops;        // OOPS-TABLE
  int len = 0;           // P-LEN

  // Sentence tables
  ITbl itbl;             // P-ITBL
  ITbl otbl;             // P-OTBL
  Vtbl vtbl;             // P-VTBL
  Vtbl ovtbl;            // P-OVTBL
  int ncn = 0;           // P-NCN
  bool endOnPrep = false;// P-END-ON-PREP
  std::string act;       // P-ACT
  std::optional<Direction> againDir; // AGAIN-DIR
  int aclause = 0;       // P-ACLAUSE (0, P_NC1 or P_NC2)
  const DictWord *anam = nullptr; // P-ANAM
  const DictWord *aadj = nullptr; // P-AADJ
  std::array<int, 4> cctbl{};     // P-CCTBL (P-ITBL slot numbers)

  // Object matching
  const DictWord *nam = nullptr;  // P-NAM
  const DictWord *adj = nullptr;  // P-ADJ (adjective value)
  const DictWord *adjn = nullptr; // P-ADJN
  const DictWord *oneobj = nullptr; // P-ONEOBJ
  bool pAnd = false;              // P-AND
  std::vector<ZObject *> prso;    // P-PRSO
  std::vector<ZObject *> prsi;    // P-PRSI
  std::vector<ZObject *> buts;    // P-BUTS
  std::vector<ZObject *> merge;   // P-MERGE
  std::vector<ZObject *> *table = nullptr; // P-TABLE
  std::vector<const DictWord *> oclause;   // P-OCLAUSE
  const Syntax *syntax = nullptr; // P-SYNTAX

  bool alwaysLit = false;         // ALWAYS-LIT
};

State &state();
void resetState();

// Word at / typed text at a clause pointer (P-LEXV or P-OCLAUSE entry).
const DictWord *wordAt(const Ptr &p);
std::string_view textAt(const Ptr &p);

// ============================================================================
// Routines (gparser.zil:109-1407)
// ============================================================================

// ZIL: <ROUTINE PARSER ...> (gparser.zil:109-380). Prints the prompt, READs
// a line and fills PRSA/P-PRSO/P-PRSI. Returns P-WON.
bool parser();
// Makes the next READ inside PARSER return this line instead of reading
// stdin (tests and the oracle harness).
void setNextInput(std::string line);
// READ: fills P-INBUF and P-LEXV from a line the way the Z-machine does.
void read(std::string_view line);

// ZIL: <ROUTINE STUFF (SRC DEST "OPTIONAL" (MAX 29) ...> (gparser.zil:387-399)
void stuff(const LexTable &src, LexTable &dest, int max = STUFF_MAX);
// ZIL: <ROUTINE INBUF-STUFF (SRC DEST ...> (gparser.zil:402-406)
void inbufStuff(const std::string &src, std::string &dest);
// ZIL: <ROUTINE INBUF-ADD (LEN BEG SLOT ...> (gparser.zil:410-423)
void inbufAdd(std::string_view text, int slot);
// ZIL: <ROUTINE WT? (PTR BIT "OPTIONAL" (B1 5) ...> (gparser.zil:430-436)
bool wt(const DictWord *w, int bit);
// ZIL: <ROUTINE CLAUSE (PTR VAL WRD ...> (gparser.zil:440-510)
// Returns the entry index of the clause's last word, -1 when the sentence
// is finished, or nullopt for RFALSE.
std::optional<int> clause(int ptr, int val, const DictWord *wrd);
// ZIL: <ROUTINE NUMBER? (PTR ...> (gparser.zil:512-535)
const DictWord *numberQ(int ptr);
// ZIL: <ROUTINE ORPHAN-MERGE ...> (gparser.zil:543-630)
bool orphanMerge();
// ZIL: <ROUTINE ACLAUSE-WIN (ADJ) ...> (gparser.zil:634-643)
bool aclauseWin(const DictWord *adj);
// ZIL: <ROUTINE NCLAUSE-WIN () ...> (gparser.zil:645-653)
bool nclauseWin();
// ZIL: <ROUTINE WORD-PRINT (CNT BUF) ...> (gparser.zil:658-663)
void wordPrint(std::string_view text);
// ZIL: <ROUTINE UNKNOWN-WORD (PTR ...> (gparser.zil:665-675)
void unknownWord(int ptr);
// ZIL: <ROUTINE CANT-USE (PTR ...> (gparser.zil:677-686)
void cantUse(int ptr);
// ZIL: <ROUTINE SYNTAX-CHECK () ...> (gparser.zil:707-775)
bool syntaxCheck();
// ZIL: <ROUTINE CANT-ORPHAN () ...> (gparser.zil:777-779)
bool cantOrphan();
// ZIL: <ROUTINE ORPHAN (D1 D2 ...> (gparser.zil:782-808)
void orphan(const Syntax *d1, const Syntax *d2);
// ZIL: <ROUTINE THING-PRINT (PRSO? "OPTIONAL" (THE? <>) ...> (gparser.zil:810-817)
void thingPrint(bool prso, bool the = false);
// ZIL: <ROUTINE BUFFER-PRINT (BEG END CP ...> (gparser.zil:819-849)
void bufferPrint(Ptr beg, Ptr end, bool cp);
// ZIL: <ROUTINE PREP-PRINT (PREP ...> (gparser.zil:851-858)
void prepPrint(int prep);
// ZIL: <ROUTINE CLAUSE-COPY (SRC DEST "OPTIONAL" (INSRT <>) ...> (gparser.zil:860-879)
void clauseCopy(ITbl &src, ITbl &dest, const DictWord *insrt = nullptr);
// ZIL: <ROUTINE CLAUSE-ADD (WRD ...> (gparser.zil:882-886)
void clauseAdd(const DictWord *wrd);
// ZIL: <ROUTINE SYNTAX-FOUND (SYN) ...> (gparser.zil:895-898)
void syntaxFound(const Syntax *syn);
// ZIL: <ROUTINE GWIM (GBIT LBIT PREP ...> (gparser.zil:901-926)
ZObject *gwim(uint64_t gbit, int lbit, int prep);
// ZIL: <ROUTINE SNARF-OBJECTS () ...> (gparser.zil:928-943)
bool snarfObjects();
// ZIL: <ROUTINE BUT-MERGE (TBL ...> (gparser.zil:945-958)
void butMerge(std::vector<ZObject *> &tbl);
// ZIL: <ROUTINE SNARFEM (PTR EPTR TBL ...> (gparser.zil:978-1030)
bool snarfem(Ptr ptr, Ptr eptr, std::vector<ZObject *> &tbl);
// ZIL: <ROUTINE GET-OBJECT (TBL "OPTIONAL" (VRB T) ...> (gparser.zil:1040-1140)
bool getObject(std::vector<ZObject *> &tbl, bool vrb = true);
// ZIL: <ROUTINE WHICH-PRINT (TLEN LEN TBL ...> (gparser.zil:1146-1166)
void whichPrint(int tlen, int len, const std::vector<ZObject *> &tbl);
// ZIL: <ROUTINE GLOBAL-CHECK (TBL ...> (gparser.zil:1169-1200)
void globalCheck(std::vector<ZObject *> &tbl);
// ZIL: <ROUTINE DO-SL (OBJ BIT1 BIT2 ...> (gparser.zil:1202-1210)
void doSl(ZObject *obj, int bit1, int bit2);
// ZIL: <ROUTINE SEARCH-LIST (OBJ TBL LVL ...> (gparser.zil:1216-1237)
void searchList(ZObject *obj, std::vector<ZObject *> &tbl, int lvl);
// ZIL: <ROUTINE OBJ-FOUND (OBJ TBL ...> (gparser.zil:1239-1242)
void objFound(ZObject *obj, std::vector<ZObject *> &tbl);
// ZIL: <ROUTINE TAKE-CHECK () ...> (gparser.zil:1244-1246)
bool takeCheck();
// ZIL: <ROUTINE ITAKE-CHECK (TBL IBITS ...> (gparser.zil:1248-1292)
bool itakeCheck(std::vector<ZObject *> &tbl, int ibits);
// ZIL: <ROUTINE MANY-CHECK () ...> (gparser.zil:1294-1313)
bool manyCheck();
// ZIL: <ROUTINE ZMEMQ (ITM TBL "OPTIONAL" (SIZE -1) ...> (gparser.zil:1315-1322)
template <typename T, typename U>
inline bool zmemq(const T &item, const std::vector<U> &table) {
  for (const auto &elem : table) {
    if (elem == item) return true;
  }
  return false;
}
// ZIL: <ROUTINE LIT? (RM "OPTIONAL" (RMBIT T) ...> (gparser.zil:1333-1355)
bool isLit(ZObject *rm = nullptr, bool rmbit = true);
// ZIL: <ROUTINE THIS-IT? (OBJ TBL ...> (gparser.zil:1357-1370)
bool thisIt(ZObject *obj);
// ZIL: <ROUTINE ACCESSIBLE? (OBJ ...> (gparser.zil:1372-1396)
bool isAccessible(const ZObject *obj);
// ZIL: <ROUTINE META-LOC (OBJ) ...> (gparser.zil:1398-1407)
ZObject *metaLoc(const ZObject *obj);

} // namespace GParser
