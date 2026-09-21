// Tests for the GPARSER.ZIL port (src/parser/gparser.*)
// Source: zil/gparser.zil:1-1407
#include "parser/gparser.h"
#include "core/globals.h"
#include "core/io.h"
#include "core/object.h"
#include "parser/gsyntax.h"
#include "verbs/verbs.h"
#include "world/objects.h"
#include "world/rooms.h"
#include "world/world.h"
#include <cassert>
#include <iostream>
#include <print>
#include <sstream>
#include <vector>

namespace {

class OutputCapture {
public:
  OutputCapture() : old_(std::cout.rdbuf(buf_.rdbuf())) {}
  ~OutputCapture() { std::cout.rdbuf(old_); }
  std::string str() const { return buf_.str(); }

private:
  std::stringstream buf_;
  std::streambuf *old_;
};

void setupWorld() {
  auto &g = Globals::instance();
  g.reset();
  initializeWorld();
  GParser::invalidateDictionary();
  GParser::resetState();
  g.winner = g.player;
  g.here = g.getObject(RoomIds::WEST_OF_HOUSE);
  if (g.player && g.here) g.player->moveTo(g.here);
  g.lit = true;
}

// ---------------------------------------------------------------------------
// B1: dictionary and READ
// ---------------------------------------------------------------------------

void testZcharTruncation() {
  std::println("Testing six z-character dictionary truncation...");
  assert(GParser::zkey("leaflet") == "leafle");
  assert(GParser::zkey("northeast") == "northe");
  assert(GParser::zkey("mailbox") == "mailbo");
  assert(GParser::zkey("take") == "take");
  // A2 characters cost two z-characters
  assert(GParser::zkey("FCD#3") == "fcd#");
  assert(GParser::zkey("air-pump") == "air-p");
  assert(GParser::zkey("#command") == "#comm");
  assert(GParser::zkey("#record") == "#reco");
  // ZSCII escape costs four
  assert(GParser::zkey("$verify") == "$ve");
  assert(GParser::zkey(".") == ".");
  assert(GParser::zkey("\"") == "\"");
  std::println("✓ zkey");
}

void testDictionaryPartsOfSpeech() {
  std::println("Testing dictionary parts of speech...");
  setupWorld();
  const auto *take = GParser::lookupWord("take");
  assert(take && (take->ps & GParser::PS_VERB) && take->verb == "take");
  const auto *get = GParser::lookupWord("get");
  assert(get && (get->ps & GParser::PS_VERB) && get->verb == "take");
  const auto *ask = GParser::lookupWord("ask");
  assert(ask && ask->verb == "tell");
  const auto *go = GParser::lookupWord("go");
  assert(go && go->verb == "walk");
  // "light" is a verb and an object word (LAMP synonym)
  const auto *light = GParser::lookupWord("light");
  assert(light && (light->ps & GParser::PS_VERB) && (light->ps & GParser::PS_OBJECT));
  // "in" is a preposition and a direction
  const auto *in = GParser::lookupWord("in");
  assert(in && (in->ps & GParser::PS_PREPOSITION) && (in->ps & GParser::PS_DIRECTION));
  assert(in->prep != 0 && in->dir == Direction::IN);
  const auto *inside = GParser::lookupWord("inside");
  assert(inside && inside->prep == in->prep && !(inside->ps & GParser::PS_DIRECTION));
  // NORTHEAST truncates to the NORTHE synonym
  const auto *ne = GParser::lookupWord("northeast");
  assert(ne && (ne->ps & GParser::PS_DIRECTION) && ne->dir == Direction::NE);
  assert(GParser::lookupWord("ne")->dir == Direction::NE && GParser::lookupWord("northe")->dir == Direction::NE);
  const auto *land = GParser::lookupWord("land");
  assert(land && land->dir == Direction::LAND);
  // Buzzwords
  for (const char *b : {"the", "a", "an", "all", "one", "but", "except", "then", "and", ".", ",", "\"", "again", "g", "oops"}) {
    const auto *w = GParser::lookupWord(b);
    assert(w && (w->ps & GParser::PS_BUZZ_WORD));
  }
  // Object and adjective words
  const auto *mailbox = GParser::lookupWord("mailbox");
  assert(mailbox && (mailbox->ps & GParser::PS_OBJECT));
  const auto *small = GParser::lookupWord("small");
  assert(small && (small->ps & GParser::PS_ADJECTIVE));
  // Debug verbs keep their prefixes
  const auto *verify = GParser::lookupWord("$verify");
  assert(verify && (verify->ps & GParser::PS_VERB));
  assert(GParser::lookupWord("verify") == nullptr);
  // R119 does not know "x"
  assert(GParser::lookupWord("x") == nullptr);
  assert(GParser::lookupWord("frobnicate") == nullptr);
  // W?xxx lookups
  assert(GParser::W("intnum") != nullptr);
  assert(GParser::W("it") != nullptr && (GParser::W("it")->ps & GParser::PS_OBJECT));
  assert(GParser::W("me") != nullptr);
  std::println("✓ dictionary");
}

void testVerbsTableOrder() {
  std::println("Testing VERBS table and PREP-FIND...");
  setupWorld();
  auto put = GParser::verbSyntaxes("put");
  assert(!put.empty());
  // Scanned last-defined first: the last record is PUT OBJECT IN OBJECT
  const auto &last = put.back();
  assert(last.nobj == 2 && last.prep1 == 0);
  assert(GParser::prepFind(last.prep2)->key == "in");
  assert(last.action == V_PUT);
  // TAKE OBJECT: FIND TAKEBIT, ON-GROUND IN-ROOM MANY
  auto take = GParser::verbSyntaxes("take");
  const auto &takeObj = take.back();
  assert(takeObj.nobj == 1 && takeObj.prep1 == 0);
  assert(takeObj.fwim1 == static_cast<uint64_t>(ObjectFlag::TAKEBIT));
  assert((takeObj.loc1 & GParser::SOG) && (takeObj.loc1 & GParser::SIR) && (takeObj.loc1 & GParser::SMANY));
  assert(!(takeObj.loc1 & GParser::SH));
  // An OBJECT with no search specifiers searches everywhere
  auto examine = GParser::verbSyntaxes("examine");
  bool sawAll = false;
  for (const auto &s : examine) {
    if (s.nobj == 1 && s.prep1 == 0) {
      assert((s.loc1 & (GParser::SH | GParser::SC | GParser::SIR | GParser::SOG)) ==
             (GParser::SH | GParser::SC | GParser::SIR | GParser::SOG));
      sawAll = true;
    }
  }
  assert(sawAll);
  assert(GParser::verbSyntaxes("look").size() >= 12);
  assert(GParser::verbSyntaxes("nosuchverb").empty());
  std::println("✓ VERBS");
}

void testRead() {
  std::println("Testing READ tokenisation...");
  setupWorld();
  auto &s = GParser::state();
  GParser::read("Open Mailbox.");
  assert(s.lexv.count == 3);
  assert(s.lexv.e[0].text == "open" && s.lexv.e[0].w && s.lexv.e[0].w->verb == "open");
  assert(s.lexv.e[1].text == "mailbox" && s.lexv.e[1].w == GParser::lookupWord("mailbox"));
  assert(s.lexv.e[2].text == "." && s.lexv.e[2].w == GParser::W("."));
  assert(s.inbuf == "open mailbox.");

  GParser::read("say \"hello sailor\", then n");
  assert(s.lexv.count == 8);
  assert(s.lexv.e[1].w == GParser::W("\""));
  assert(s.lexv.e[4].w == GParser::W("\""));
  assert(s.lexv.e[5].w == GParser::W(","));
  assert(s.lexv.e[6].w == GParser::W("then"));
  assert(s.lexv.e[7].w->dir == Direction::NORTH);

  // Unknown words keep their typed text
  GParser::read("frobnicate   the leaflet");
  assert(s.lexv.count == 3);
  assert(s.lexv.e[0].w == nullptr && s.lexv.e[0].text == "frobnicate");
  assert(s.lexv.e[2].w == GParser::lookupWord("leaflet"));

  // 59-word LEXV limit
  std::string many;
  for (int i = 0; i < 70; ++i) many += "n ";
  GParser::read(many);
  assert(s.lexv.count == GParser::P_LEXV_SIZE);

  // 120-byte P-INBUF limit
  std::string longLine(200, 'a');
  GParser::read(longLine);
  assert(s.inbuf.size() == 120);
  assert(s.lexv.count == 1 && s.lexv.e[0].text.size() == 120);
  std::println("✓ READ");
}

void testStuffAndInbuf() {
  std::println("Testing STUFF / INBUF-STUFF / INBUF-ADD...");
  setupWorld();
  auto &s = GParser::state();
  std::string many;
  for (int i = 0; i < 40; ++i) many += "n ";
  GParser::read(many);
  GParser::LexTable copy;
  GParser::stuff(s.lexv, copy);
  assert(copy.count == 40);
  assert(copy.e[28].w != nullptr);
  assert(copy.e[29].w == nullptr); // only 29 entries are copied

  std::string dest;
  GParser::inbufStuff("take lamp", dest);
  assert(dest == "take lamp");

  GParser::read("frob lamp");
  GParser::stuff(s.lexv, s.againLexv);
  GParser::inbufStuff(s.inbuf, s.oopsInbuf);
  GParser::inbufAdd("take", 0);
  assert(s.againLexv.e[0].text == "take");
  assert(s.oops.end);
  std::println("✓ STUFF");
}

void testUnknownAndCantUse() {
  std::println("Testing UNKNOWN-WORD / CANT-USE / WORD-PRINT...");
  setupWorld();
  auto &g = Globals::instance();
  auto &s = GParser::state();
  g.prsa = 0;
  GParser::read("Frobnicate leaflet");
  {
    OutputCapture cap;
    GParser::unknownWord(0);
    assert(cap.str() == "I don't know the word \"frobnicate\".\n");
  }
  assert(s.oops.ptr == 0);
  assert(!g.quoteFlag && !g.pOflag);
  GParser::read("take of");
  {
    OutputCapture cap;
    GParser::cantUse(1);
    assert(cap.str() == "You used the word \"of\" in a way that I don't understand.\n");
  }
  // After SAY both print "Nothing happens."
  g.prsa = V_SAY;
  {
    OutputCapture cap;
    GParser::unknownWord(0);
    assert(cap.str() == "Nothing happens.\n");
  }
  g.prsa = 0;
  std::println("✓ UNKNOWN-WORD");
}

void testBufferPrint() {
  std::println("Testing BUFFER-PRINT...");
  setupWorld();
  auto &g = Globals::instance();
  GParser::read("small mailbox , leaflet");
  {
    OutputCapture cap;
    GParser::bufferPrint(GParser::Ptr::lex(0), GParser::Ptr::lex(4), true);
    assert(cap.str() == "the small mailbox, leaflet");
  }
  {
    OutputCapture cap;
    GParser::bufferPrint(GParser::Ptr::lex(0), GParser::Ptr::lex(2), false);
    assert(cap.str() == "small mailbox");
  }
  // ME prints the ME object's DESC; IT prints P-IT-OBJECT when accessible
  GParser::read("me");
  {
    OutputCapture cap;
    GParser::bufferPrint(GParser::Ptr::lex(0), GParser::Ptr::lex(1), true);
    assert(cap.str() == "you");
  }
  g.it = g.getObject(ObjectIds::MAILBOX);
  GParser::read("it");
  {
    OutputCapture cap;
    GParser::bufferPrint(GParser::Ptr::lex(0), GParser::Ptr::lex(1), true);
    assert(cap.str() == "the small mailbox");
  }
  // With P-OFLAG the dictionary word (truncated) is printed
  g.pOflag = true;
  GParser::read("leaflet");
  {
    OutputCapture cap;
    GParser::bufferPrint(GParser::Ptr::lex(0), GParser::Ptr::lex(1), false);
    assert(cap.str() == "leafle");
  }
  g.pOflag = false;
  std::println("✓ BUFFER-PRINT");
}

void testSearchAndLit() {
  std::println("Testing SEARCH-LIST / DO-SL / LIT? / ACCESSIBLE? / META-LOC...");
  setupWorld();
  auto &g = Globals::instance();
  auto &s = GParser::state();
  ZObject *mailbox = g.getObject(ObjectIds::MAILBOX);
  ZObject *leaflet = g.getObject(ObjectIds::ADVERTISEMENT);
  assert(mailbox && leaflet);
  // Closed mailbox: the leaflet is not found in the room
  s.nam = nullptr;
  s.adj = nullptr;
  g.pGwimbit = 0;
  std::vector<ZObject *> found;
  s.table = &found;
  g.pSlocbits = -1;
  GParser::doSl(g.here, GParser::SOG, GParser::SIR);
  assert(GParser::zmemq(mailbox, found));
  assert(!GParser::zmemq(leaflet, found));
  // Open it: found at P-SRCTOP inside
  mailbox->setFlag(ObjectFlag::OPENBIT);
  found.clear();
  GParser::doSl(g.here, GParser::SOG, GParser::SIR);
  assert(GParser::zmemq(leaflet, found));
  // Name filter
  s.nam = GParser::lookupWord("leaflet");
  found.clear();
  GParser::doSl(g.here, GParser::SOG, GParser::SIR);
  assert(found.size() == 1 && found[0] == leaflet);
  s.nam = nullptr;

  assert(GParser::metaLoc(leaflet) == g.here);
  assert(GParser::isAccessible(mailbox));
  assert(GParser::isAccessible(leaflet));
  mailbox->clearFlag(ObjectFlag::OPENBIT);
  assert(!GParser::isAccessible(leaflet));
  assert(GParser::isLit(g.here));
  std::println("✓ search");
}

// ---------------------------------------------------------------------------
// B2: PARSER lexical stage and CLAUSE
// ---------------------------------------------------------------------------

// Runs PARSER on one line with output captured; returns P-WON and the text.
std::pair<bool, std::string> runParser(const std::string &line) {
  GParser::setNextInput(line);
  OutputCapture cap;
  bool won = GParser::parser();
  return {won, cap.str()};
}

void testParserDirections() {
  std::println("Testing PARSER direction short-cuts...");
  setupWorld();
  auto &g = Globals::instance();
  auto &s = GParser::state();
  auto [won, out] = runParser("n");
  assert(won);
  assert(out == "\n>");
  assert(g.prsa == V_WALK && g.pWalkDir == Direction::NORTH);
  assert(s.againDir == Direction::NORTH);
  assert(!g.pOflag);

  auto r2 = runParser("go northeast");
  assert(r2.first && g.pWalkDir == Direction::NE);

  // "walk to the north" is not a direction command (LEN > 2)
  g.pWalkDir.reset();
  auto r3 = runParser("walk north now");
  (void)r3;
  assert(!g.pWalkDir); // fails later (unknown word "now")

  // "n then s": the first parse walks north and leaves P-CONT at "s"
  auto r4 = runParser("n then s");
  assert(r4.first && g.pWalkDir == Direction::NORTH);
  assert(g.pCont == 2);
  assert(s.lexv.count == 1);
  // The THEN with no verb became a quote and the ITBL verb TELL (gparser.zil:261-266)
  assert(g.quoteFlag);
  auto r5 = runParser("");
  assert(r5.first && g.pWalkDir == Direction::SOUTH);
  assert(g.pCont == 0);
  assert(r5.second == "\n"); // P-CONT continuation: a CRLF but no prompt
  std::println("✓ directions");
}

void testParserClauses() {
  std::println("Testing PARSER noun clauses...");
  setupWorld();
  auto &g = Globals::instance();
  auto &s = GParser::state();
  // SYNTAX-CHECK is not ported yet, so the parse stops after the tables
  // are filled; inspect P-ITBL.
  runParser("open the small mailbox");
  assert(s.itbl.verb == "open");
  assert(s.itbl.verbn && s.vtbl.word == GParser::lookupWord("open") && s.vtbl.text == "open");
  assert(s.ncn == 1);
  assert(s.itbl.nc1 == GParser::Ptr::lex(2)); // THE skipped
  assert(s.itbl.nc1l == GParser::Ptr::lex(4));
  assert(s.itbl.prep1 == 0 && s.itbl.nc2.isNull());

  runParser("put leaflet in mailbox");
  assert(s.itbl.verb == "put" && s.ncn == 2);
  assert(s.itbl.nc1 == GParser::Ptr::lex(1) && s.itbl.nc1l == GParser::Ptr::lex(2));
  assert(s.itbl.prep2 == GParser::lookupWord("in")->prep);
  assert(s.itbl.prep2n == GParser::lookupWord("in"));
  assert(s.itbl.nc2 == GParser::Ptr::lex(3) && s.itbl.nc2l == GParser::Ptr::lex(4));

  // Trailing preposition: P-END-ON-PREP and PREP1
  runParser("look up");
  assert(s.itbl.verb == "look" && s.ncn == 0);
  assert(s.endOnPrep && s.itbl.prep1 == GParser::lookupWord("up")->prep);

  runParser("take all except mailbox");
  assert(s.ncn == 1);
  assert(s.itbl.nc1 == GParser::Ptr::lex(1) && s.itbl.nc1l == GParser::Ptr::lex(4));

  runParser("take lamp and sword");
  assert(s.ncn == 1);
  assert(s.itbl.nc1 == GParser::Ptr::lex(1) && s.itbl.nc1l == GParser::Ptr::lex(4));

  // "take lamp and go north": AND before a verb becomes THEN
  runParser("take lamp and go north");
  assert(s.ncn == 1);
  assert(s.lexv.e[2].w == GParser::W("then"));
  assert(g.pCont == 3);
  g.pCont = 0;

  // TO after TELL is a quote
  runParser("tell troll to go north");
  assert(s.itbl.verb == "tell" && s.ncn == 1);
  assert(g.quoteFlag);
  assert(g.pCont == 3);
  g.pCont = 0;
  g.quoteFlag = false;

  // Period ends the sentence
  runParser("open mailbox. read leaflet");
  assert(s.itbl.verb == "open" && s.ncn == 1);
  assert(s.itbl.nc1l == GParser::Ptr::lex(2));
  assert(g.pCont == 3);
  g.pCont = 0;

  // Adjective-only clauses are accepted here (GET-OBJECT decides later)
  runParser("take small");
  assert(s.ncn == 1);
  std::println("✓ clauses");
}

void testParserMessages() {
  std::println("Testing PARSER messages...");
  setupWorld();
  auto &g = Globals::instance();
  auto r = runParser("");
  assert(!r.first && r.second == "\n>I beg your pardon?\n");
  r = runParser("frobnicate the mailbox");
  assert(!r.first && r.second == "\n>I don't know the word \"frobnicate\".\n");
  r = runParser("x mailbox");
  assert(!r.first && r.second == "\n>I don't know the word \"x\".\n");
  r = runParser("open mailbox.");
  assert(g.pCont == 0);
  r = runParser("take of");
  assert(!r.first && r.second == "\n>You used the word \"of\" in a way that I don't understand.\n");
  r = runParser("put lamp in mailbox on table");
  assert(!r.first && r.second == "\n>There were too many nouns in that sentence.\n");
  r = runParser("tell troll attack");
  assert(!r.first && r.second.contains("Please consult your manual for the correct way to talk to other people or creatures."));
  // An unknown word records OOPS-TABLE O-PTR
  runParser("open frob");
  assert(GParser::state().oops.ptr == 1);
  // SUPER-BRIEF suppresses the blank line before the prompt
  g.superbriefMode = true;
  r = runParser("");
  assert(r.second == ">I beg your pardon?\n");
  g.superbriefMode = false;
  std::println("✓ messages");
}

// ---------------------------------------------------------------------------
// B3: OOPS
// ---------------------------------------------------------------------------

void testOops() {
  std::println("Testing OOPS...");
  setupWorld();
  auto &g = Globals::instance();
  auto &s = GParser::state();
  // Nothing to correct yet
  auto r = runParser("oops open");
  assert(!r.first && r.second == "\n>There was no word to replace!\n");
  // An unknown word, then a correction that re-parses the sentence
  r = runParser("frobnicate the mailbox");
  assert(!r.first && s.oops.ptr == 0);
  r = runParser("oops open");
  assert(r.second == "\n>"); // no complaint; the corrected sentence parsed
  assert(s.itbl.verb == "open" && s.ncn == 1);
  assert(s.lexv.e[0].text == "open" && s.lexv.e[0].w == GParser::lookupWord("open"));
  assert(s.itbl.nc1 == GParser::Ptr::lex(2)); // "the" skipped
  assert(s.inbuf.starts_with("frobnicate the mailboxopen"));
  assert(s.oops.ptr == -1);
  // A bare OOPS
  r = runParser("frobnicate mailbox");
  r = runParser("oops");
  assert(!r.first && r.second == "\n>I can't help your clumsiness.\n");
  // A comma after OOPS is skipped; extra words only warn
  r = runParser("frobnicate mailbox");
  r = runParser("oops, open the box");
  assert(r.second == "\n>Warning: only the first word after OOPS is used.\n");
  assert(s.itbl.verb == "open");
  // Unknown word in the middle of the sentence
  r = runParser("open frob");
  assert(s.oops.ptr == 1);
  r = runParser("oops mailbox");
  assert(s.itbl.verb == "open" && s.itbl.nc1 == GParser::Ptr::lex(1));
  assert(s.lexv.e[1].text == "mailbox" && s.lexv.e[1].w == GParser::lookupWord("mailbox"));
  // Corrections inside quotes are refused
  r = runParser("open frob");
  r = runParser("oops \"mailbox\"");
  assert(!r.first && r.second == "\n>Sorry, you can't correct mistakes in quoted text.\n");
  // Correcting to another unknown word complains about the new word
  r = runParser("open frob");
  r = runParser("oops blorp");
  assert(!r.first && r.second == "\n>I don't know the word \"blorp\".\n");
  (void)g;
  std::println("✓ OOPS");
}

// ---------------------------------------------------------------------------
// B4: AGAIN / G
// ---------------------------------------------------------------------------

void testAgain() {
  std::println("Testing AGAIN...");
  setupWorld();
  auto &g = Globals::instance();
  auto &s = GParser::state();
  // Nothing typed yet
  auto r = runParser("again");
  assert(!r.first && r.second == "\n>Beg pardon?\n");
  // Repeat a direction
  r = runParser("n");
  g.pWon = r.first;
  g.pWalkDir.reset();
  r = runParser("again");
  assert(r.first && g.pWalkDir == Direction::NORTH);
  g.pWon = true;
  g.pWalkDir.reset();
  r = runParser("g");
  assert(r.first && g.pWalkDir == Direction::NORTH);
  // Repeating a mistake
  r = runParser("frobnicate");
  g.pWon = r.first;
  r = runParser("again");
  assert(!r.first && r.second == "\n>That would just repeat a mistake.\n");
  // Repeating a fragment (orphaned sentence)
  r = runParser("n");
  g.pWon = true;
  g.pOflag = true;
  r = runParser("again");
  assert(!r.first && r.second == "\n>It's difficult to repeat fragments.\n");
  g.pOflag = false;
  // AGAIN followed by something other than a separator
  r = runParser("again now");
  assert(!r.first && r.second == "\n>I couldn't understand that sentence.\n");
  // "again then s": the remainder is parked in RESERVE-LEXV
  r = runParser("n");
  g.pWon = true;
  g.pWalkDir.reset();
  r = runParser("again then s");
  assert(r.first && g.pWalkDir == Direction::NORTH);
  assert(s.reservePtr == 2 && s.reserveLexv.count == 1);
  r = runParser("");
  assert(r.first && g.pWalkDir == Direction::SOUTH);
  assert(r.second == "\n"); // RESERVE path: CRLF, no prompt
  assert(s.reservePtr == -1);
  // AGAIN after a sentence restores P-ITBL from P-OTBL and re-runs the
  // syntax stage on the restored P-LEXV
  runParser("open mailbox");
  g.pWon = true;
  GParser::read("take"); // clobber P-LEXV; AGAIN must restore it
  r = runParser("again");
  assert(s.itbl.verb == "open" && s.itbl.nc1 == GParser::Ptr::lex(1));
  assert(s.lexv.e[1].text == "mailbox");
  std::println("✓ AGAIN");
}

// ---------------------------------------------------------------------------
// B5: SYNTAX-CHECK, GWIM, ORPHAN, ORPHAN-MERGE
// ---------------------------------------------------------------------------

void testSyntaxCheckAndOrphan() {
  std::println("Testing SYNTAX-CHECK / ORPHAN / ORPHAN-MERGE...");
  setupWorld();
  auto &g = Globals::instance();
  auto &s = GParser::state();
  // GET-OBJECT is not ported yet, so GWIM finds nothing and every missing
  // object orphans.
  auto r = runParser("take");
  assert(!r.first && r.second == "\n>What do you want to take?\n");
  assert(g.pOflag && s.otbl.nc1 == GParser::Ptr::one() && s.otbl.prep1 == 0);
  // The answer is merged into the orphaned sentence
  r = runParser("mailbox");
  assert(!g.pOflag && g.pMerged);
  assert(s.itbl.verb == "take" && s.ncn == 1);
  assert(s.itbl.nc1 == GParser::Ptr::lex(0) && s.itbl.nc1l == GParser::Ptr::lex(1));
  assert(g.prsa == V_TAKE);
  assert(s.syntax && s.syntax->nobj == 1);

  // Missing indirect object: the direct clause is echoed with "the"
  r = runParser("put leaflet");
  assert(!r.first && r.second == "\n>What do you want to put the leaflet in?\n");
  assert(s.otbl.nc2 == GParser::Ptr::one());
  assert(GParser::prepFind(s.otbl.prep2)->key == "in");
  r = runParser("mailbox");
  assert(g.pMerged && s.ncn == 2 && g.prsa == V_PUT);
  assert(s.itbl.nc2 == GParser::Ptr::lex(0));
  // The direct clause now lives in P-OCLAUSE
  assert(s.itbl.nc1.kind == GParser::Ptr::Ocl);
  assert(GParser::wordAt(s.itbl.nc1) == GParser::lookupWord("leaflet"));

  // A new verb abandons the orphan
  r = runParser("take");
  r = runParser("open mailbox");
  assert(!g.pOflag && g.prsa == V_OPEN);

  // FIND RMUNGBIT: GWIM yields ROOMS without asking
  r = runParser("look up");
  assert(r.first && g.prsa == V_LOOK);
  assert(s.prso.size() == 1 && s.prso[0] == g.getObject(ObjectIds::ROOMS));
  r = runParser("climb up");
  assert(r.first && g.prsa == V_CLIMB_UP && s.prso[0] == g.getObject(ObjectIds::ROOMS));
  r = runParser("stand up");
  assert(r.first && g.prsa == V_STAND);

  // No FIND flag: an ordinary orphan
  r = runParser("walk around");
  assert(!r.first && r.second == "\n>What do you want to walk around?\n");
  r = runParser("find");
  assert(!r.first && r.second == "\n>That question can't be answered.\n");
  r = runParser("kick mailbox to");
  assert(!r.first && r.second == "\n>That sentence isn't one I recognize.\n");
  r = runParser("put leaflet in");
  assert(!r.first && r.second == "\n>That sentence isn't one I recognize.\n");
  r = runParser("look");
  assert(r.first && g.prsa == V_LOOK && s.syntax->nobj == 0);
  r = runParser("look at mailbox");
  assert(r.first && g.prsa == V_EXAMINE);
  r = runParser("look at leaflet with lamp");
  assert(r.first && g.prsa == V_READ);
  r = runParser("shut door");
  assert(g.prsa == V_TURN || !r.first); // SHUT is a TURN synonym: TURN OBJECT (FIND TURNBIT) WITH ...
  r = runParser("pour water in bottle");
  assert(r.first && g.prsa == V_DROP);

  // The typed verb word is echoed; a synonym stays as typed
  r = runParser("get");
  assert(r.second == "\n>What do you want to get?\n");
  r = runParser("mailbox");
  assert(g.prsa == V_TAKE);

  // Orphaning is refused for a non-player WINNER
  // (only reachable on a P-CONT continuation, since a fresh READ resets
  // WINNER to the player)
  ZObject *troll = g.getObject(ObjectIds::TROLL);
  GParser::read("tell troll \"take");
  g.pCont = 3;
  s.lexv.count = 1; // the first parse stored the remaining word count
  g.winner = troll;
  g.quoteFlag = true;
  {
    OutputCapture cap;
    bool won = GParser::parser();
    assert(!won && cap.str() == "\"I don't understand! What are you referring to?\"\n");
  }
  g.winner = g.player;
  g.quoteFlag = false;
  std::println("✓ SYNTAX-CHECK");
}

// ---------------------------------------------------------------------------
// B6: SNARF-OBJECTS, SNARFEM, BUT-MERGE, GET-OBJECT
// ---------------------------------------------------------------------------

void testGetObject() {
  std::println("Testing SNARFEM / GET-OBJECT / BUT-MERGE...");
  setupWorld();
  auto &g = Globals::instance();
  auto &s = GParser::state();
  ZObject *mailbox = g.getObject(ObjectIds::MAILBOX);
  ZObject *leaflet = g.getObject(ObjectIds::ADVERTISEMENT);
  ZObject *notHere = g.getObject(ObjectIds::NOT_HERE_OBJECT);
  assert(mailbox && leaflet && notHere);

  auto r = runParser("open mailbox");
  assert(r.first && g.prsa == V_OPEN);
  assert(s.prso.size() == 1 && s.prso[0] == mailbox && s.prsi.empty());
  assert(g.pGetFlags == 0);

  // Not visible (inside the closed mailbox): NOT-HERE-OBJECT stands in
  r = runParser("take leaflet");
  assert(r.first);
  assert(s.prso.size() == 1 && s.prso[0] == notHere);
  assert(g.pXnam == "leafle" && g.pXadjn.empty());

  // Adjective + noun through an open container
  mailbox->setFlag(ObjectFlag::OPENBIT);
  r = runParser("take the leaflet");
  assert(r.first && s.prso.size() == 1 && s.prso[0] == leaflet);
  r = runParser("examine small mailbox");
  assert(r.first && s.prso.size() == 1 && s.prso[0] == mailbox);
  r = runParser("examine small");
  // adjective-only clause: the adjective word is not an object word
  assert(r.first && s.prso.size() == 1 && s.prso[0] == mailbox);

  // ALL and EXCEPT
  r = runParser("take all");
  assert(r.first && g.pGetFlags == GParser::P_ALL);
  assert(GParser::zmemq(mailbox, s.prso) && GParser::zmemq(leaflet, s.prso));
  r = runParser("take all except leaflet");
  assert(r.first);
  assert(GParser::zmemq(mailbox, s.prso) && !GParser::zmemq(leaflet, s.prso));
  assert(s.buts.size() == 1 && s.buts[0] == leaflet);
  r = runParser("take all except mailbox and leaflet");
  assert(r.first && s.prso.empty());
  assert(s.buts.size() == 2);

  // AND lists and unfound nouns
  r = runParser("take lamp and sword");
  assert(r.first && s.prso.size() == 2 && s.prso[0] == notHere && s.prso[1] == notHere);
  assert(s.pAnd);
  r = runParser("take leaflet, mailbox");
  assert(r.first && s.prso.size() == 2 && s.prso[0] == leaflet && s.prso[1] == mailbox);

  // "take the": the buzzword is skipped and GWIM finds the leaflet
  r = runParser("take the");
  assert(r.first && r.second == "\n>(leaflet)\n" && s.prso.size() == 1 && s.prso[0] == leaflet);
  // A missing noun
  r = runParser("put leaflet in the");
  assert(!r.first && r.second == "\n>There seems to be a noun missing in that sentence!\n");

  // Darkness
  g.lit = false;
  ZObject *here = g.here;
  here->clearFlag(ObjectFlag::ONBIT);
  r = runParser("take leaflet");
  // PARSER recomputes LIT from the room; West of House is ONBIT, so force it
  g.lit = false;
  s.nam = GParser::lookupWord("leaflet");
  s.adj = nullptr;
  g.pGetFlags = 0;
  g.pSlocbits = 0;
  std::vector<ZObject *> tbl;
  {
    OutputCapture cap;
    bool ok = GParser::getObject(tbl);
    assert(!ok && cap.str() == "It's too dark to see!\n");
  }
  here->setFlag(ObjectFlag::ONBIT);
  g.lit = true;

  // Two candidates: the second pass narrows by the syntax's scope bits,
  // then WHICH-PRINT (B7) and an orphan
  auto other = std::make_unique<ZObject>(9999, "other leaflet");
  other->addSynonym("leaflet");
  other->setFlag(ObjectFlag::TAKEBIT);
  ZObject *otherPtr = other.get();
  g.registerObject(9999, std::move(other));
  otherPtr->moveTo(here);
  r = runParser("take leaflet");
  assert(!r.first && g.pOflag);
  assert(s.aclause == GParser::P_NC1 && s.anam == GParser::lookupWord("leaflet"));
  g.pOflag = false;
  // "take one leaflet" picks one at random and says so ("a" is a buzzword
  // the PARSER loop skips before the clause starts, so only ONE sets P-ONE)
  r = runParser("take one leaflet");
  assert(r.first && s.prso.size() == 1);
  assert(r.second.starts_with("\n>(How about the "));
  // The held one is preferred once the leaflet is carried and the syntax
  // searches only the room
  leaflet->moveTo(g.player);
  r = runParser("take leaflet");
  assert(r.first && s.prso.size() == 1 && s.prso[0] == otherPtr);
  r = runParser("drop leaflet");
  assert(r.first && s.prso.size() == 1 && s.prso[0] == leaflet);
  std::println("✓ GET-OBJECT");
}

// ---------------------------------------------------------------------------
// B7: GLOBAL-CHECK (room globals, pseudo objects, GLOBAL-OBJECTS) and
//     WHICH-PRINT
// ---------------------------------------------------------------------------

void testGlobalCheckAndWhichPrint() {
  std::println("Testing GLOBAL-CHECK / WHICH-PRINT...");
  setupWorld();
  auto &g = Globals::instance();
  auto &s = GParser::state();
  ZObject *pseudo = g.getObject(ObjectIds::PSEUDO_OBJECT);
  ZObject *grue = g.getObject(ObjectIds::GRUE);
  assert(pseudo && grue);

  // GLOBAL-OBJECTS are found anywhere
  auto r = runParser("examine grue");
  assert(r.first && s.prso.size() == 1 && s.prso[0] == grue);

  // Pseudo objects resolve to PSEUDO-OBJECT in their room only
  g.here = g.getObject(RoomIds::LIVING_ROOM);
  g.player->moveTo(g.here);
  r = runParser("examine nails");
  assert(r.first && s.prso.size() == 1 && s.prso[0] == pseudo);
  assert(pseudo->getDesc() == "nails" && pseudo->hasAction());
  r = runParser("examine nail");
  assert(r.first && s.prso[0] == pseudo && pseudo->getDesc() == "nail");
  g.here = g.getObject(RoomIds::WEST_OF_HOUSE);
  g.player->moveTo(g.here);
  r = runParser("examine nails");
  assert(r.first && s.prso.size() == 1 && s.prso[0] == g.getObject(ObjectIds::NOT_HERE_OBJECT));
  // A pseudo object is never ACCESSIBLE? (LAST-PSEUDO-LOC is commented out)
  assert(!GParser::isAccessible(pseudo));

  // WHICH-PRINT: two candidates
  ZObject *leaflet = g.getObject(ObjectIds::ADVERTISEMENT);
  g.getObject(ObjectIds::MAILBOX)->setFlag(ObjectFlag::OPENBIT);
  auto other = std::make_unique<ZObject>(9998, "other leaflet");
  other->addSynonym("leaflet");
  other->addAdjective("other");
  other->setFlag(ObjectFlag::TAKEBIT);
  ZObject *otherPtr = other.get();
  g.registerObject(9998, std::move(other));
  otherPtr->moveTo(g.here);
  r = runParser("take leaflet");
  assert(!r.first);
  assert(r.second == "\n>Which leaflet do you mean, the leaflet or the other leaflet?\n" ||
         r.second == "\n>Which leaflet do you mean, the other leaflet or the leaflet?\n");
  assert(g.pOflag);
  // Answer with the distinguishing adjective: ACLAUSE-WIN merges it
  r = runParser("other");
  assert(r.first && s.prso.size() == 1 && s.prso[0] == otherPtr);
  assert(g.pMerged);
  // Three candidates: an Oxford comma
  auto third = std::make_unique<ZObject>(9997, "third leaflet");
  third->addSynonym("leaflet");
  third->addAdjective("third");
  third->setFlag(ObjectFlag::TAKEBIT);
  ZObject *thirdPtr = third.get();
  g.registerObject(9997, std::move(third));
  thirdPtr->moveTo(g.here);
  r = runParser("take leaflet");
  assert(!r.first && r.second.starts_with("\n>Which leaflet do you mean, the "));
  assert(r.second.contains(", or the ") && r.second.ends_with("?\n"));
  r = runParser("third");
  assert(r.first && s.prso.size() == 1 && s.prso[0] == thirdPtr);
  // With P-OFLAG the noun is printed from the dictionary (truncated)
  (void)leaflet;
  std::println("✓ GLOBAL-CHECK");
}

// ---------------------------------------------------------------------------
// B8: MANY-CHECK, TAKE-CHECK, ITAKE-CHECK
// ---------------------------------------------------------------------------

void testManyAndTakeCheck() {
  std::println("Testing MANY-CHECK / TAKE-CHECK / ITAKE-CHECK...");
  setupWorld();
  auto &g = Globals::instance();
  auto &s = GParser::state();
  ZObject *mailbox = g.getObject(ObjectIds::MAILBOX);
  ZObject *leaflet = g.getObject(ObjectIds::ADVERTISEMENT);
  ZObject *itObj = g.getObject(ObjectIds::IT);
  mailbox->setFlag(ObjectFlag::OPENBIT);

  // MANY is allowed for TAKE and EXAMINE, not for OPEN
  auto r = runParser("take leaflet and mailbox");
  assert(r.first && s.prso.size() == 2);
  r = runParser("open leaflet and mailbox");
  assert(!r.first && r.second == "\n>You can't use multiple direct objects with \"open\".\n");
  r = runParser("put leaflet in mailbox and mailbox");
  assert(!r.first && r.second == "\n>You can't use multiple indirect objects with \"put\".\n");

  // TAKE bit: READ takes the leaflet first and says so
  assert(leaflet->getLocation() == mailbox);
  r = runParser("read leaflet");
  assert(r.first && r.second == "\n>(Taken)\n");
  assert(leaflet->getLocation() == g.player);
  // Already held: nothing to do
  r = runParser("read leaflet");
  assert(r.first && r.second == "\n>");

  // HAVE bit without TAKE: DROP demands possession
  leaflet->moveTo(mailbox);
  r = runParser("drop leaflet");
  assert(!r.first && r.second == "\n>You don't have the leaflet.\n");
  r = runParser("drop lamp");
  assert(!r.first && r.second == "\n>You don't have that!\n");
  // TRYTAKEBIT objects are never taken implicitly
  r = runParser("drop mailbox");
  assert(!r.first && r.second == "\n>You don't have the small mailbox.\n");

  // IT: inaccessible P-IT-OBJECT
  g.it = nullptr;
  r = runParser("drop it");
  assert(!r.first && r.second == "\n>I don't see what you're referring to.\n");
  leaflet->moveTo(g.player);
  g.it = leaflet;
  r = runParser("drop it");
  assert(r.first && s.prso.size() == 1 && s.prso[0] == itObj);

  // A non-player WINNER never takes and never complains
  ZObject *troll = g.getObject(ObjectIds::TROLL);
  leaflet->moveTo(mailbox);
  GParser::read("tell troll \"read leaflet");
  g.pCont = 3;
  s.lexv.count = 2;
  g.winner = troll;
  g.quoteFlag = true;
  {
    OutputCapture cap;
    bool won = GParser::parser();
    assert(won && cap.str().empty());
    assert(leaflet->getLocation() == mailbox);
  }
  g.winner = g.player;
  g.quoteFlag = false;
  std::println("✓ TAKE-CHECK");
}

// ---------------------------------------------------------------------------
// B9: NUMBER?
// ---------------------------------------------------------------------------

void testNumber() {
  std::println("Testing NUMBER?...");
  setupWorld();
  auto &g = Globals::instance();
  auto &s = GParser::state();
  ZObject *intnum = g.getObject(ObjectIds::INTNUM);
  assert(intnum);
  auto r = runParser("take 1000");
  assert(r.first && s.prso.size() == 1 && s.prso[0] == intnum);
  assert(g.pNumber == 1000);
  r = runParser("take 0");
  assert(r.first && g.pNumber == 0);
  // Times: hh:mm, hours below 8 are afternoon
  r = runParser("take 12:30");
  assert(r.first && g.pNumber == 750);
  r = runParser("take 7:30");
  assert(r.first && g.pNumber == 1170);
  // Rejections read back as unknown words
  r = runParser("take 1001");
  assert(!r.first && r.second == "\n>I don't know the word \"1001\".\n");
  r = runParser("take 25:00");
  assert(!r.first && r.second == "\n>I don't know the word \"25:00\".\n");
  r = runParser("take 100000");
  assert(!r.first && r.second == "\n>I don't know the word \"100000\".\n");
  r = runParser("take 12a");
  assert(!r.first && r.second == "\n>I don't know the word \"12a\".\n");
  // P-NUMBER is reset by any new sentence that is not AGAIN
  r = runParser("take 5");
  assert(g.pNumber == 5);
  r = runParser("look");
  assert(g.pNumber == 0);
  // The number prints through BUFFER-PRINT as its value
  r = runParser("take 42");
  {
    OutputCapture cap;
    GParser::thingPrint(true, true);
    assert(cap.str() == "42");
  }
  std::println("✓ NUMBER?");
}

// ---------------------------------------------------------------------------
// B10: LIT?, THIS-IT?, ACCESSIBLE?, META-LOC, ALWAYS-LIT
// ---------------------------------------------------------------------------

void testLitThisItAccessible() {
  std::println("Testing LIT? / THIS-IT? / ACCESSIBLE? / META-LOC...");
  setupWorld();
  auto &g = Globals::instance();
  auto &s = GParser::state();
  ZObject *cellar = g.getObject(RoomIds::CELLAR);
  ZObject *lamp = g.getObject(ObjectIds::LAMP);
  ZObject *mailbox = g.getObject(ObjectIds::MAILBOX);
  ZObject *grue = g.getObject(ObjectIds::GRUE);
  assert(cellar && lamp && mailbox && grue);
  cellar->clearFlag(ObjectFlag::ONBIT);
  g.here = cellar;
  g.player->moveTo(cellar);
  lamp->clearFlag(ObjectFlag::ONBIT);
  s.nam = nullptr;
  s.adj = nullptr;

  // LIT?
  lamp->moveTo(g.player);
  assert(!GParser::isLit(cellar));
  lamp->setFlag(ObjectFlag::ONBIT);
  assert(GParser::isLit(cellar));
  // inside an open container the player holds
  mailbox->moveTo(g.player);
  mailbox->setFlag(ObjectFlag::OPENBIT);
  lamp->moveTo(mailbox);
  assert(GParser::isLit(cellar));
  mailbox->clearFlag(ObjectFlag::OPENBIT);
  assert(!GParser::isLit(cellar));
  mailbox->setFlag(ObjectFlag::OPENBIT);
  // in the room itself
  lamp->moveTo(cellar);
  assert(GParser::isLit(cellar));
  // a lamp the player holds does not light a room other than HERE
  lamp->moveTo(g.player);
  ZObject *trollRoom = g.getObject(RoomIds::TROLL_ROOM);
  trollRoom->clearFlag(ObjectFlag::ONBIT);
  assert(!GParser::isLit(trollRoom));
  assert(g.here == cellar); // HERE restored
  // RMBIT: the room's own ONBIT
  cellar->setFlag(ObjectFlag::ONBIT);
  lamp->clearFlag(ObjectFlag::ONBIT);
  assert(GParser::isLit(cellar));
  assert(!GParser::isLit(cellar, false));
  cellar->clearFlag(ObjectFlag::ONBIT);
  // ALWAYS-LIT
  s.alwaysLit = true;
  assert(GParser::isLit(cellar));
  s.alwaysLit = false;
  assert(!GParser::isLit(cellar));
  // LIT? leaves P-GWIMBIT clear
  assert(g.pGwimbit == 0);

  // THIS-IT?
  s.nam = GParser::lookupWord("lantern");
  assert(GParser::thisIt(lamp));
  s.nam = GParser::lookupWord("mailbox");
  assert(!GParser::thisIt(lamp));
  s.nam = GParser::lookupWord("lamp");
  s.adj = GParser::lookupWord("brass");
  assert(GParser::thisIt(lamp));
  s.adj = GParser::lookupWord("small");
  assert(!GParser::thisIt(lamp));
  s.adj = nullptr;
  g.pGwimbit = static_cast<uint64_t>(ObjectFlag::ONBIT);
  assert(!GParser::thisIt(lamp));
  lamp->setFlag(ObjectFlag::ONBIT);
  assert(GParser::thisIt(lamp));
  g.pGwimbit = 0;
  lamp->setFlag(ObjectFlag::INVISIBLE);
  assert(!GParser::thisIt(lamp));
  lamp->clearFlag(ObjectFlag::INVISIBLE);
  s.nam = nullptr;

  // META-LOC and ACCESSIBLE?
  assert(GParser::metaLoc(lamp) == cellar);
  lamp->moveTo(mailbox); // mailbox is held and open
  assert(GParser::metaLoc(lamp) == cellar);
  assert(GParser::isAccessible(lamp));
  mailbox->clearFlag(ObjectFlag::OPENBIT);
  assert(!GParser::isAccessible(lamp));
  assert(GParser::metaLoc(grue) == g.getObject(ObjectIds::GLOBAL_OBJECTS));
  assert(GParser::isAccessible(grue));
  // LOCAL-GLOBALS are accessible only where the room lists them
  ZObject *stairs = g.getObject(ObjectIds::STAIRS);
  assert(stairs && stairs->getLocation() == g.getObject(ObjectIds::LOCAL_GLOBALS));
  auto *cellarRoom = dynamic_cast<ZRoom *>(cellar);
  bool listed = cellarRoom->hasGlobal(ObjectIds::STAIRS);
  if (!listed) cellarRoom->addGlobal(ObjectIds::STAIRS);
  assert(GParser::isAccessible(stairs));
  g.here = trollRoom;
  g.player->moveTo(trollRoom);
  if (!dynamic_cast<ZRoom *>(trollRoom)->hasGlobal(ObjectIds::STAIRS)) {
    assert(!GParser::isAccessible(stairs));
  }
  // an object in another room
  lamp->moveTo(cellar);
  assert(!GParser::isAccessible(lamp));
  assert(GParser::metaLoc(nullptr) == nullptr);
  std::println("✓ LIT?");
}

// ---------------------------------------------------------------------------
// B12: debug syntaxes keep their $ and # prefixes
// ---------------------------------------------------------------------------

void testDebugSyntaxes() {
  std::println("Testing $VERIFY / #RANDOM / #COMMAND / #RECORD / #UNRECORD...");
  setupWorld();
  auto &g = Globals::instance();
  auto &s = GParser::state();
  auto r = runParser("$verify");
  assert(r.first && g.prsa == V_VERIFY);
  r = runParser("verify");
  assert(!r.first && r.second == "\n>I don't know the word \"verify\".\n");
  // NUMBER? rejects values above 1000 (gparser.zil:528), so seeds are small
  r = runParser("#random 999");
  assert(r.first && g.prsa == V_RANDOM);
  assert(s.prso.size() == 1 && s.prso[0] == g.getObject(ObjectIds::INTNUM));
  assert(g.pNumber == 999);
  r = runParser("#random 1984");
  assert(!r.first && r.second == "\n>I don't know the word \"1984\".\n");
  r = runParser("#random");
  // the orphan question echoes the typed verb word
  assert(!r.first && r.second == "\n>What do you want to #random?\n");
  r = runParser("#command");
  assert(r.first && g.prsa == V_COMMAND_FILE);
  r = runParser("#record");
  assert(r.first && g.prsa == V_RECORD);
  r = runParser("#unrecord");
  assert(r.first && g.prsa == V_UNRECORD);
  r = runParser("random 5");
  assert(!r.first);
  std::println("✓ debug syntaxes");
}

} // namespace

int main() {
  setScreenWidth(0);
  testZcharTruncation();
  testDictionaryPartsOfSpeech();
  testVerbsTableOrder();
  testRead();
  testStuffAndInbuf();
  testUnknownAndCantUse();
  testBufferPrint();
  testSearchAndLit();
  testParserDirections();
  testParserClauses();
  testParserMessages();
  testOops();
  testAgain();
  testSyntaxCheckAndOrphan();
  testGetObject();
  testGlobalCheckAndWhichPrint();
  testManyAndTakeCheck();
  testNumber();
  testLitThisItAccessible();
  testDebugSyntaxes();
  std::println("All gparser tests passed.");
  return 0;
}
