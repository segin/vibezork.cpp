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
  std::println("All gparser tests passed.");
  return 0;
}
