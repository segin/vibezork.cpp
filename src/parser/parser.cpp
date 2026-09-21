#include "parser.h"
#include "core/globals.h"
#include "core/io.h"
#include "parser/gparser.h"
#include "parser/gsyntax.h"
#include "verb_registry.h"
#include "verbs/verbs.h"
#include "world/objects.h"
#include "world/rooms.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

Parser::Parser() {
  static VerbRegistry defaultRegistry;
  verbRegistry_ = &defaultRegistry;
  initializeVerbsAndDirections();
}

void Parser::initializeVerbsAndDirections() {
  // Initialize verb synonyms (from gsyntax.zil)
  verbSynonyms_["take"] = V_TAKE;
  verbSynonyms_["get"] = V_TAKE;
  verbSynonyms_["drop"] = V_DROP;
  verbSynonyms_["put"] = V_PUT;
  verbSynonyms_["look"] = V_LOOK;
  verbSynonyms_["l"] = V_LOOK;
  verbSynonyms_["examine"] = V_EXAMINE;
  verbSynonyms_["read"] = V_READ;
  verbSynonyms_["open"] = V_OPEN;
  verbSynonyms_["close"] = V_CLOSE;
  verbSynonyms_["inventory"] = V_INVENTORY;
  verbSynonyms_["i"] = V_INVENTORY;
  verbSynonyms_["go"] = V_WALK;
  verbSynonyms_["attack"] = V_ATTACK;
  verbSynonyms_["kill"] = V_ATTACK; // ZIL: KILL ... = V-ATTACK (gsyntax.zil:264)
  verbSynonyms_["quit"] = V_QUIT;
  verbSynonyms_["q"] = V_QUIT;

  // Meta-game verbs
  verbSynonyms_["score"] = V_SCORE;
  verbSynonyms_["diagnose"] = V_DIAGNOSE;
  verbSynonyms_["verbose"] = V_VERBOSE;
  verbSynonyms_["brief"] = V_BRIEF;
  verbSynonyms_["superbrief"] = V_SUPERBRIEF;
  verbSynonyms_["super"] = V_SUPERBRIEF;

  // Game control verbs
  verbSynonyms_["save"] = V_SAVE;
  verbSynonyms_["restore"] = V_RESTORE;
  verbSynonyms_["restart"] = V_RESTART;
  verbSynonyms_["version"] = V_VERSION;

  // Easter eggs / special words
  verbSynonyms_["hello"] = V_HELLO;
  verbSynonyms_["hi"] = V_HELLO;
  verbSynonyms_["zork"] = V_ZORK;
  verbSynonyms_["plugh"] = V_PLUGH;
  verbSynonyms_["xyzzy"] = V_PLUGH;
  verbSynonyms_["frobozz"] = V_FROBOZZ;

  // Manipulation verbs
  verbSynonyms_["move"] = V_MOVE;
  verbSynonyms_["push"] = V_PUSH;
  verbSynonyms_["press"] = V_PUSH;
  verbSynonyms_["pull"] = V_PULL;
  verbSynonyms_["tug"] = V_PULL;
  verbSynonyms_["yank"] = V_PULL;
  verbSynonyms_["turn"] = V_TURN;
  verbSynonyms_["rotate"] = V_TURN;
  verbSynonyms_["twist"] = V_TURN;
  verbSynonyms_["wind"] = V_WIND;
  verbSynonyms_["raise"] = V_RAISE;
  verbSynonyms_["lift"] = V_RAISE;
  verbSynonyms_["lower"] = V_LOWER;

  // Movement verbs
  verbSynonyms_["enter"] = V_ENTER;
  verbSynonyms_["exit"] = V_EXIT;
  verbSynonyms_["leave"] = V_EXIT;
  verbSynonyms_["climb"] = V_CLIMB_UP;
  verbSynonyms_["board"] = V_BOARD;
  verbSynonyms_["disembark"] = V_DISEMBARK;

  // Interaction verbs
  verbSynonyms_["tie"] = V_TIE;
  verbSynonyms_["fasten"] = V_TIE;
  verbSynonyms_["untie"] = V_UNTIE;
  verbSynonyms_["unfasten"] = V_UNTIE;
  verbSynonyms_["listen"] = V_LISTEN;
  verbSynonyms_["smell"] = V_SMELL;
  verbSynonyms_["sniff"] = V_SMELL;
  verbSynonyms_["touch"] = V_TOUCH;
  verbSynonyms_["feel"] = V_RUB;
  verbSynonyms_["rub"] = V_RUB;
  verbSynonyms_["yell"] = V_YELL;
  verbSynonyms_["shout"] = V_YELL;
  verbSynonyms_["scream"] = V_YELL;

  // Consumption verbs
  verbSynonyms_["eat"] = V_EAT;
  verbSynonyms_["consume"] = V_EAT;
  verbSynonyms_["drink"] = V_DRINK;

  // Light verbs
  verbSynonyms_["light"] = V_LAMP_ON;
  verbSynonyms_["extinguish"] = V_LAMP_OFF;
  verbSynonyms_["douse"] = V_LAMP_OFF;

  // Special action verbs
  verbSynonyms_["inflate"] = V_INFLATE;
  verbSynonyms_["deflate"] = V_DEFLATE;
  verbSynonyms_["pray"] = V_PRAY;
  verbSynonyms_["wave"] = V_WAVE;
  verbSynonyms_["ring"] = V_RING;
  verbSynonyms_["dig"] = V_DIG;
  verbSynonyms_["burn"] = V_BURN;
  verbSynonyms_["fill"] = V_FILL;

  // Combat verbs
  verbSynonyms_["throw"] = V_THROW;
  verbSynonyms_["hurl"] = V_THROW;
  verbSynonyms_["toss"] = V_THROW;
  verbSynonyms_["swing"] = V_SWING;

  // Communication verbs
  verbSynonyms_["talk"] = V_TALK;
  verbSynonyms_["speak"] = V_TALK;
  verbSynonyms_["say"] = V_TALK;
  verbSynonyms_["ask"] = V_ASK;
  verbSynonyms_["tell"] = V_TELL;
  verbSynonyms_["odysseus"] = V_ODYSSEUS;
  verbSynonyms_["ulysses"] = V_ODYSSEUS;

  // Additional common verbs
  verbSynonyms_["wait"] = V_WAIT;
  verbSynonyms_["z"] = V_WAIT;
  verbSynonyms_["swim"] = V_SWIM;
  verbSynonyms_["bathe"] = V_SWIM;
  verbSynonyms_["wade"] = V_SWIM;
  verbSynonyms_["back"] = V_BACK;
  verbSynonyms_["jump"] = V_JUMP;
  verbSynonyms_["leap"] = V_JUMP;
  verbSynonyms_["curse"] = V_CURSE;
  verbSynonyms_["damn"] = V_CURSE;
  verbSynonyms_["shit"] = V_CURSE;
  verbSynonyms_["fuck"] = V_CURSE;

  // Phase 12 Batch 2: Missing Verbs
  verbSynonyms_["oil"] = V_OIL;
  verbSynonyms_["grease"] = V_OIL;
  verbSynonyms_["lubricate"] = V_OIL;
  verbSynonyms_["stab"] = V_STAB;
  verbSynonyms_["pierce"] = V_STAB;
  verbSynonyms_["thrust"] = V_STAB;
  verbSynonyms_["record"] = V_RECORD;
  verbSynonyms_["unrecord"] = V_UNRECORD;
  verbSynonyms_["verify"] = V_VERIFY;
  verbSynonyms_["random"] = V_RANDOM;

  // Directions
  directions_["north"] = Direction::NORTH;
  directions_["n"] = Direction::NORTH;
  directions_["south"] = Direction::SOUTH;
  directions_["s"] = Direction::SOUTH;
  directions_["east"] = Direction::EAST;
  directions_["e"] = Direction::EAST;
  directions_["west"] = Direction::WEST;
  directions_["w"] = Direction::WEST;
  directions_["ne"] = Direction::NE;
  directions_["northeast"] = Direction::NE;
  directions_["nw"] = Direction::NW;
  directions_["northwest"] = Direction::NW;
  directions_["se"] = Direction::SE;
  directions_["southeast"] = Direction::SE;
  directions_["sw"] = Direction::SW;
  directions_["southwest"] = Direction::SW;
  directions_["up"] = Direction::UP;
  directions_["u"] = Direction::UP;
  directions_["down"] = Direction::DOWN;
  directions_["d"] = Direction::DOWN;
  directions_["in"] = Direction::IN;
  directions_["inside"] = Direction::IN;
  directions_["out"] = Direction::OUT;
  directions_["outside"] = Direction::OUT;
  directions_["land"] = Direction::LAND;
  directions_["launch"] = Direction::LAUNCH;

  // Prepositions (from gsyntax.zil)
  // Core prepositions
  prepositions_.insert("with");
  prepositions_.insert("using");
  prepositions_.insert("through");
  prepositions_.insert("thru");

  prepositions_.insert("in");
  prepositions_.insert("inside");
  prepositions_.insert("into");

  prepositions_.insert("on");
  prepositions_.insert("onto");

  prepositions_.insert("under");
  prepositions_.insert("underneath");
  prepositions_.insert("beneath");
  prepositions_.insert("below");

  // Additional prepositions used in syntax patterns
  prepositions_.insert("to");
  prepositions_.insert("at");
  prepositions_.insert("from");
  prepositions_.insert("for");
  prepositions_.insert("about");
  prepositions_.insert("off");
  prepositions_.insert("out");
  prepositions_.insert("over");
  prepositions_.insert("across");
  prepositions_.insert("behind");
  prepositions_.insert("around");
  prepositions_.insert("down");
  prepositions_.insert("up");
}

Parser::Parser(VerbRegistry *registry) : verbRegistry_(registry) {
  initializeVerbsAndDirections();
}

void Parser::tokenize(const std::string &input,
                      std::vector<std::string> &tokens) {
  std::istringstream iss(input);
  std::string word;
  while (iss >> word) {
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
    // ZIL: "," is a word-break character that becomes its own word
    // (gparser.zil:12, SIBREAKS); keep the rest of the clause splitting for
    // Phase B.
    while (!word.empty() && word.back() == ',') {
      word.pop_back();
      if (!word.empty()) {
        tokens.push_back(word);
        word.clear();
      }
      tokens.push_back(",");
    }
    if (!word.empty()) {
      tokens.push_back(word);
    }
  }
}

VerbId Parser::findVerb(const std::string &word) const {
  auto it = verbSynonyms_.find(word);
  return it != verbSynonyms_.end() ? it->second : 0;
}

ZObject *Parser::findObject(const std::string &word) {
  // Simplified object lookup - would need full implementation
  return nullptr;
}

Direction *Parser::findDirection(const std::string &word) {
  auto it = directions_.find(word);
  return it != directions_.end() ? &it->second : nullptr;
}

bool Parser::matchesSynonym(ZObject *obj, const std::string &word) const {
  // Check if the word matches any of the object's synonyms
  return obj->hasSynonym(word);
}

bool Parser::matchesAdjectives(
    ZObject *obj, const std::vector<std::string> &adjectives) const {
  // Check if all provided adjectives match the object
  for (const auto &adj : adjectives) {
    if (!obj->hasAdjective(adj)) {
      return false;
    }
  }
  return true;
}

int Parser::getLocationPriority(ZObject *obj) const {
  auto &g = Globals::instance();

  // Priority 1: Objects in current room
  if (obj->getLocation() == g.here) {
    return 3;
  }

  // Priority 2: Objects in player inventory
  if (obj->getLocation() == g.winner) {
    return 2;
  }

  // Priority 3: Objects in open or transparent containers in room or inventory
  ZObject *loc = obj->getLocation();
  if (loc && loc->hasFlag(ObjectFlag::CONTBIT) &&
      (loc->hasFlag(ObjectFlag::OPENBIT) || loc->hasFlag(ObjectFlag::TRANSBIT))) {
    if (loc->getLocation() == g.here || loc->getLocation() == g.winner) {
      return 1;
    }
  }

  // Priority 0: Objects elsewhere (not visible)
  return 0;
}

bool Parser::isObjectVisible(ZObject *obj) const {
  if (!obj) {
    return false;
  }
  if (obj->hasFlag(ObjectFlag::INVISIBLE)) {
    return false;
  }

  auto &g = Globals::instance();

  // ZIL: GLOBAL-CHECK (gparser.zil:1169-1200) reaches the room's GLOBAL list
  // (objects in LOCAL-GLOBALS) and everything in GLOBAL-OBJECTS.
  if (ZObject *loc = obj->getLocation()) {
    if (loc->getId() == ObjectIds::GLOBAL_OBJECTS) {
      return true;
    }
    if (loc->getId() == ObjectIds::LOCAL_GLOBALS) {
      return Verbs::globalIn(obj->getId(), g.here);
    }
  }
  // Interim: objects placed directly in a room by the old data but listed as
  // that room's globals elsewhere (kitchen window) stay reachable.
  if (obj->getId() == ObjectIds::KITCHEN_WINDOW) {
    if (g.here && (g.here->getId() == RoomIds::BEHIND_HOUSE ||
                   g.here->getId() == RoomIds::KITCHEN)) {
      return true;
    }
  }

  // Objects with priority > 0 are visible
  return getLocationPriority(obj) > 0;
}

std::vector<ZObject *>
Parser::findObjects(const std::vector<std::string> &words, size_t startIdx) {
  std::vector<ZObject *> matches;

  if (startIdx >= words.size()) {
    return matches;
  }

  auto &g = Globals::instance();

  // Separate adjectives from nouns
  std::vector<std::string> adjectives;
  std::vector<std::string> nouns;

  for (size_t i = startIdx; i < words.size(); ++i) {
    const auto &word = words[i];

    // Skip articles and prepositions
    if (word == "the" || word == "a" || word == "an" ||
        prepositions_.find(word) != prepositions_.end()) {
      continue;
    }

    // For now, treat all words as potential nouns or adjectives
    // We'll check both during matching
    nouns.push_back(word);
  }

  if (nouns.empty()) {
    return matches;
  }

  // ZIL: the pronoun words are synonyms of the IT object in GLOBAL-OBJECTS
  // (gglobals.zil:42-46); it is always found and resolved later by
  // MAIN-LOOP-1 / PERFORM (gmain.zil:45-64, 194-203).
  if (nouns.size() == 1 && isPronoun(nouns[0])) {
    if (auto *itObj = g.getObject(ObjectIds::IT)) {
      matches.push_back(itObj);
    }
    return matches;
  }

  // Search through all objects
  for (const auto &[id, objPtr] : g.getAllObjects()) {
    ZObject *obj = objPtr.get();

    // Skip invisible objects
    if (!isObjectVisible(obj)) {
      continue;
    }

    // Try to match the object
    bool matched = false;

    // Strategy 1: Try matching last word as noun with earlier words as
    // adjectives
    if (nouns.size() > 1) {
      const std::string &noun = nouns.back();
      std::vector<std::string> potentialAdjectives(nouns.begin(),
                                                   nouns.end() - 1);

      if (matchesSynonym(obj, noun) &&
          matchesAdjectives(obj, potentialAdjectives)) {
        matched = true;
      }
    }

    // Strategy 2: Try matching any single word as a synonym
    if (!matched) {
      for (const auto &word : nouns) {
        if (matchesSynonym(obj, word)) {
          // If we have multiple words, check if the others are adjectives
          if (nouns.size() > 1) {
            std::vector<std::string> otherWords;
            for (const auto &w : nouns) {
              if (w != word) {
                otherWords.push_back(w);
              }
            }
            // Only match if other words are adjectives
            if (matchesAdjectives(obj, otherWords)) {
              matched = true;
              break;
            }
          } else {
            matched = true;
            break;
          }
        }
      }
    }

    if (matched) {
      matches.push_back(obj);
    }
  }

  // Sort by location priority (highest first)
  std::sort(matches.begin(), matches.end(), [this](ZObject *a, ZObject *b) {
    return getLocationPriority(a) > getLocationPriority(b);
  });

  return matches;
}

std::string Parser::formatObjectDescription(ZObject *obj) const {
  // Format object for disambiguation list
  // Show description and location for clarity
  std::string result = obj->getDesc();

  // Add location context if helpful
  auto &g = Globals::instance();
  ZObject *loc = obj->getLocation();

  if (loc == g.winner) {
    result += " (in your inventory)";
  } else if (loc && loc->hasFlag(ObjectFlag::CONTBIT)) {
    result += " (in the " + std::string(loc->getDesc()) + ")";
  } else if (loc == g.here) {
    result += " (here)";
  }

  return result;
}

ZObject *
Parser::parseDisambiguationResponse(const std::string &response,
                                    const std::vector<ZObject *> &candidates) {
  if (candidates.empty()) {
    return nullptr;
  }

  // Tokenize the response
  std::vector<std::string> tokens;
  tokenize(response, tokens);

  if (tokens.empty()) {
    return nullptr;
  }

  // Try to parse as a number (1-based index)
  try {
    int choice = std::stoi(tokens[0]);
    if (choice >= 1 && choice <= static_cast<int>(candidates.size())) {
      return candidates[choice - 1];
    }
  } catch (...) {
    // Not a number, try matching by name
  }

  // Try to match by object name/synonym
  // Look for objects that match the response words
  for (auto *candidate : candidates) {
    // Check if any word in response matches a synonym
    for (const auto &word : tokens) {
      if (matchesSynonym(candidate, word)) {
        return candidate;
      }
    }

    // Check if response matches adjectives + synonym
    if (tokens.size() > 1) {
      std::vector<std::string> adjectives(tokens.begin(), tokens.end() - 1);
      const std::string &noun = tokens.back();

      if (matchesSynonym(candidate, noun) &&
          matchesAdjectives(candidate, adjectives)) {
        return candidate;
      }
    }
  }

  return nullptr;
}

ZObject *Parser::disambiguate(const std::vector<ZObject *> &candidates,
                              const std::string &noun) {
  if (candidates.empty()) {
    return nullptr;
  }

  if (candidates.size() == 1) {
    return candidates[0];
  }

  // GWIMBIT: "Get What I Mean" - if exactly one candidate has this flag,
  // auto-select it as the preferred/obvious choice
  ZObject *gwimChoice = nullptr;
  int gwimCount = 0;
  for (auto *candidate : candidates) {
    if (candidate->hasFlag(ObjectFlag::GWIMBIT)) {
      gwimChoice = candidate;
      gwimCount++;
    }
  }
  if (gwimCount == 1 && gwimChoice) {
    return gwimChoice;
  }

  // Display disambiguation prompt
  print("Which " + noun + " do you mean?\n");

  // List the candidates with numbers
  for (size_t i = 0; i < candidates.size(); ++i) {
    print("  " + std::to_string(i + 1) + ". " +
          formatObjectDescription(candidates[i]) + "\n");
  }

  // Read player's choice
  print("> ");
  std::string response = readLine();

  // Parse the response
  ZObject *selected = parseDisambiguationResponse(response, candidates);

  if (!selected) {
    printLine("I don't understand that choice.");
  }

  return selected;
}

bool Parser::isPreposition(const std::string &word) const {
  return prepositions_.find(word) != prepositions_.end();
}

std::optional<size_t>
Parser::findPrepositionIndex(const std::vector<std::string> &tokens) const {
  for (size_t i = 1; i < tokens.size(); ++i) { // Start at 1 to skip verb
    if (isPreposition(tokens[i])) {
      return i;
    }
  }
  return std::nullopt;
}

bool Parser::validatePreposition(VerbId verb,
                                 const std::string &preposition) const {
  // If we have a verb registry, use it for validation
  if (verbRegistry_) {
    return verbRegistry_->isPrepositionValidForVerb(verb, preposition);
  }

  // Otherwise, just check if it's a known preposition
  return isPreposition(preposition);
}

// Special command support methods
void Parser::setLastCommand(const std::string &cmd) { lastCommand_ = cmd; }

std::string_view Parser::getLastCommand() const { return lastCommand_; }

void Parser::setLastUnknownWord(const std::string &word) {
  lastUnknownWord_ = word;
  hadUnknownWordLastTurn_ = true;
}

std::string_view Parser::getLastUnknownWord() const { return lastUnknownWord_; }

void Parser::clearLastUnknownWord() {
  lastUnknownWord_.clear();
  hadUnknownWordLastTurn_ = false;
}

bool Parser::isAllKeyword(const std::string &word) const {
  return word == "all" || word == "everything";
}

bool Parser::isExceptKeyword(const std::string &word) const {
  return word == "except" || word == "but";
}

bool Parser::isAgainCommand(const std::vector<std::string> &tokens) const {
  return !tokens.empty() && (tokens[0] == "again" || tokens[0] == "g");
}

bool Parser::isOopsCommand(const std::vector<std::string> &tokens) const {
  return !tokens.empty() && tokens[0] == "oops";
}

// ZIL: <OBJECT IT ... (SYNONYM IT THEM HER HIM)> (gglobals.zil:42-46)
bool Parser::isPronoun(const std::string &word) const {
  return word == "it" || word == "them" || word == "her" || word == "him";
}


// ---------------------------------------------------------------------------
// Interim stand-ins for SNARFEM / GET-OBJECT / DO-SL / SEARCH-LIST /
// GLOBAL-CHECK / BUT-MERGE (gparser.zil:928-1030, 1040-1140, 1169-1243).
// They feed the ZIL match tables consumed by MAIN-LOOP-1; Phase B replaces
// them with the verbatim parser.
// ---------------------------------------------------------------------------

// ZIL: THIS-IT? with P-NAM and P-ADJ empty and P-GWIMBIT zero
// (gparser.zil:1357-1370): everything that is not INVISIBLE matches.
static bool thisItAll(const ZObject *obj) {
  return obj && !obj->hasFlag(ObjectFlag::INVISIBLE);
}

// ZIL: <ROUTINE OBJ-FOUND (OBJ TBL ...> (gparser.zil:1239-1242)
static void objFoundTable(ZObject *obj, std::vector<ZObject *> &table) {
  table.push_back(obj);
}

// ZIL: <ROUTINE SEARCH-LIST (OBJ TBL LVL ...> (gparser.zil:1216-1237)
void Parser::searchList(ZObject *obj, std::vector<ZObject *> &table,
                        int level) const {
  if (!obj) {
    return;
  }
  // Iterate a copy: OBJ-FOUND never moves objects, but callers may.
  std::vector<ZObject *> children = obj->getContents();
  for (ZObject *child : children) {
    if (level != GParser::P_SRCBOT && !child->getSynonyms().empty() &&
        thisItAll(child)) {
      objFoundTable(child, table);
    }
    if ((level != GParser::P_SRCTOP || child->hasFlag(ObjectFlag::SEARCHBIT) ||
         child->hasFlag(ObjectFlag::SURFACEBIT)) &&
        !child->getContents().empty() &&
        (child->hasFlag(ObjectFlag::OPENBIT) ||
         child->hasFlag(ObjectFlag::TRANSBIT))) {
      int next = child->hasFlag(ObjectFlag::SURFACEBIT)   ? GParser::P_SRCALL
                 : child->hasFlag(ObjectFlag::SEARCHBIT) ? GParser::P_SRCALL
                                                          : GParser::P_SRCTOP;
      searchList(child, table, next);
    }
  }
}

// ZIL: <ROUTINE DO-SL (OBJ BIT1 BIT2 ...> (gparser.zil:1202-1210).
// BTST is (X & Y) == Y.
void Parser::doSl(ZObject *obj, int bit1, int bit2,
                  std::vector<ZObject *> &table) const {
  auto btst = [this](int bits) { return (pSlocbits_ & bits) == bits; };
  if (btst(bit1 + bit2)) {
    searchList(obj, table, GParser::P_SRCALL);
  } else if (btst(bit1)) {
    searchList(obj, table, GParser::P_SRCTOP);
  } else if (btst(bit2)) {
    searchList(obj, table, GParser::P_SRCBOT);
  }
}

// ZIL: <ROUTINE GLOBAL-CHECK (TBL ...> (gparser.zil:1169-1200) for the
// P-NAM-less (ALL) case: the PSEUDO clause can never match.
void Parser::globalCheck(const ParsedCommand &cmd,
                         std::vector<ZObject *> &table) const {
  auto &g = Globals::instance();
  size_t len = table.size();
  int obits = pSlocbits_;
  if (auto *room = dynamic_cast<ZRoom *>(g.here)) {
    for (ObjectId id : room->getGlobals()) {
      ZObject *obj = g.getObject(id);
      if (thisItAll(obj)) {
        objFoundTable(obj, table);
      }
    }
  }
  if (table.size() == len) {
    pSlocbits_ = -1;
    doSl(g.getObject(ObjectIds::GLOBAL_OBJECTS), 1, 1, table);
    pSlocbits_ = obits;
    if (table.empty() && (cmd.verb == V_LOOK_INSIDE || cmd.verb == V_SEARCH ||
                          cmd.verb == V_EXAMINE)) {
      // ZIL: <DO-SL ,ROOMS 1 1>. Rooms are not children of ROOMS in this
      // port yet (Phase D); scan them directly in registration order.
      for (const auto &[id, objPtr] : g.getAllObjects()) {
        auto *room = dynamic_cast<ZRoom *>(objPtr.get());
        if (room && !room->getSynonyms().empty() && thisItAll(room)) {
          objFoundTable(room, table);
        }
      }
    }
  }
}

// ZIL: the object element's location bits of the matched SYNTAX line
// (gsyntax.zil), read through the GSyntax transcription.
int Parser::syntaxScopeBits(const ParsedCommand &cmd, bool directSlot) const {
  if (cmd.words.empty()) {
    return 0;
  }
  std::vector<std::string> preps;
  if (auto prepIdx = findPrepositionIndex(cmd.words);
      prepIdx && *prepIdx + 1 < cmd.words.size()) {
    preps.push_back(cmd.words[*prepIdx]);
  }
  size_t objectCount = 1;
  if (!preps.empty()) {
    // "verb prep obj" has one object; "verb obj prep obj" has two.
    auto prepIdx = *findPrepositionIndex(cmd.words);
    objectCount = prepIdx > 1 ? 2 : 1;
  }
  std::string canonical = GSyntax::canonicalVerb(cmd.words[0]);
  const GSyntax::ZilSyntax *syn = GSyntax::matchSyntax(
      canonical.empty() ? cmd.words[0] : canonical, preps, objectCount);
  if (!syn) {
    return 0;
  }
  int seen = 0;
  for (const auto &elem : syn->elements) {
    if (elem.type != GSyntax::SyntaxElement::Type::OBJECT) {
      continue;
    }
    ++seen;
    if ((directSlot && seen == 1) || (!directSlot && seen == 2)) {
      int bits = 0;
      if (elem.scopeFlags & GSyntax::SH_HELD) bits |= GParser::SH;
      if (elem.scopeFlags & GSyntax::SH_CARRIED) bits |= GParser::SC;
      if (elem.scopeFlags & GSyntax::SH_ON_GROUND) bits |= GParser::SOG;
      if (elem.scopeFlags & GSyntax::SH_IN_ROOM) bits |= GParser::SIR;
      if (elem.many) bits |= GParser::SMANY;
      if (elem.have) bits |= GParser::SHAVE;
      if (elem.take) bits |= GParser::STAKE;
      return bits;
    }
  }
  return 0;
}

// ZIL: GET-OBJECT with P-GETFLAGS = P-ALL (gparser.zil:1040-1140)
bool Parser::collectAll(ParsedCommand &cmd, bool directSlot,
                        std::vector<ZObject *> &table) {
  auto &g = Globals::instance();
  int xbits = syntaxScopeBits(cmd, directSlot);
  pSlocbits_ = xbits;
  // ZIL: <COND (<OR <NOT <EQUAL? ,P-GETFLAGS ,P-ALL>> <ZERO? ,P-SLOCBITS>>
  //             <SETG P-SLOCBITS -1>)>
  if (pSlocbits_ == 0) {
    pSlocbits_ = -1;
  }
  size_t tlen = table.size();
  bool gcheck = false;
  while (true) {
    if (gcheck) {
      globalCheck(cmd, table);
    } else {
      if (g.lit) {
        // ZIL: <FCLEAR ,PLAYER ,TRANSBIT> <DO-SL ,HERE ,SOG ,SIR>
        //      <FSET ,PLAYER ,TRANSBIT>
        if (g.player) g.player->clearFlag(ObjectFlag::TRANSBIT);
        doSl(g.here, GParser::SOG, GParser::SIR, table);
        if (g.player) g.player->setFlag(ObjectFlag::TRANSBIT);
      }
      doSl(g.player, GParser::SH, GParser::SC, table);
    }
    size_t len = table.size() - tlen;
    if (len == 0 && gcheck) {
      pSlocbits_ = xbits;
      if (g.lit || cmd.verb == V_TELL) {
        objFoundTable(g.getObject(ObjectIds::NOT_HERE_OBJECT), table);
        g.pXnam.clear();
        g.pXadjn.clear();
        return true;
      }
      printLine("It's too dark to see!");
      return false;
    }
    if (len == 0) {
      gcheck = true;
      continue;
    }
    pSlocbits_ = xbits;
    return true;
  }
}

// One noun clause -> match table. Handles AND / "," lists, ALL, ALL EXCEPT
// (BUT) lists, pronouns, unknown words and NOT-HERE-OBJECT.
bool Parser::snarfPhrase(ParsedCommand &cmd,
                         const std::vector<std::string> &phrase,
                         bool directSlot, std::vector<ZObject *> &table) {
  auto &g = Globals::instance();
  std::vector<ZObject *> buts;
  bool inBut = false;

  // Split into sub-phrases on AND / ","; EXCEPT / BUT switches to the BUTS
  // table (gparser.zil:1001-1004, 1013-1016).
  std::vector<std::vector<std::string>> parts;
  std::vector<bool> partIsBut;
  parts.emplace_back();
  partIsBut.push_back(false);
  for (const auto &word : phrase) {
    if (word == "and" || word == ",") {
      if (!parts.back().empty()) {
        parts.emplace_back();
        partIsBut.push_back(inBut);
      }
      continue;
    }
    if (isExceptKeyword(word)) {
      inBut = true;
      if (!parts.back().empty()) {
        parts.emplace_back();
      } else {
        partIsBut.pop_back();
      }
      partIsBut.push_back(true);
      continue;
    }
    parts.back().push_back(word);
  }
  if (parts.back().empty()) {
    parts.pop_back();
    partIsBut.pop_back();
  }

  for (size_t p = 0; p < parts.size(); ++p) {
    const auto &part = parts[p];
    std::vector<ZObject *> &dest = partIsBut[p] ? buts : table;

    // ALL / ALL OF
    std::vector<std::string> content;
    for (const auto &w : part) {
      if (w == "the" || w == "a" || w == "an" || w == "of" ||
          isPreposition(w)) {
        continue;
      }
      content.push_back(w);
    }
    if (content.size() == 1 && isAllKeyword(content[0])) {
      cmd.getFlags = GParser::P_ALL;
      if (directSlot && !partIsBut[p] && p == 0) {
        cmd.nc1IsAll = true;
      }
      if (!collectAll(cmd, directSlot, dest)) {
        return false;
      }
      continue;
    }
    if (content.empty()) {
      continue;
    }

    auto matches = findObjects(part, 0);
    if (!matches.empty()) {
      ZObject *chosen = matches.size() == 1
                            ? matches[0]
                            : disambiguate(matches, content.back());
      if (!chosen) {
        return false;
      }
      objFoundTable(chosen, dest);
      continue;
    }

    // No visible object: an unknown word wins (gparser.zil:665-675)
    for (const auto &word : part) {
      if (word == "the" || word == "a" || word == "an" || isPreposition(word)) {
        continue;
      }
      if (!isKnownObjectWord(word)) {
        setLastUnknownWord(word);
        printLine("I don't know the word \"" + word + "\".");
        return false;
      }
    }
    // Known words, nothing here: NOT-HERE-OBJECT when lit or TELL
    // (gparser.zil:1122-1128), else "It's too dark to see!" (1129).
    if (g.lit || cmd.verb == V_TELL) {
      objFoundTable(g.getObject(ObjectIds::NOT_HERE_OBJECT), dest);
      // The clause words echoed by NOT-HERE-PRINT / BUFFER-PRINT; the
      // buzzwords THE / A / AN are not part of it ("take the lamp" ->
      // "You can't see any lamp here!").
      if (directSlot) {
        g.pNc1 = content;
      } else {
        g.pNc2 = content;
      }
      g.pXnam.clear();
      g.pXadjn.clear();
      continue;
    }
    printLine("It's too dark to see!");
    return false;
  }

  // ZIL: BUT-MERGE (gparser.zil:945-958)
  if (!buts.empty()) {
    std::vector<ZObject *> merged;
    for (ZObject *obj : table) {
      if (std::find(buts.begin(), buts.end(), obj) == buts.end()) {
        merged.push_back(obj);
      }
    }
    table = std::move(merged);
    if (directSlot && !buts.empty()) {
      cmd.exceptObject = buts.front();
    }
  }
  return true;
}

std::string Parser::replaceOopsWord(const std::string &original,
                                    const std::string &replacement) {
  if (lastUnknownWord_.empty()) {
    return original;
  }

  std::string result = original;
  size_t pos = result.find(lastUnknownWord_);
  if (pos != std::string::npos) {
    result.replace(pos, lastUnknownWord_.length(), replacement);
  }

  return result;
}

void Parser::setOrphanDirect(VerbId verb, const std::string &verbWord) {
  orphanFlag_ = true;
  orphanVerb_ = verb;
  orphanNeedsDirect_ = true;
  orphanNeedsIndirect_ = false;
  orphanDirectObj_ = nullptr;
  orphanPreposition_.clear();
}

void Parser::setOrphanIndirect(VerbId verb, ZObject *directObj,
                               const std::string &prep) {
  orphanFlag_ = true;
  orphanVerb_ = verb;
  orphanNeedsDirect_ = false;
  orphanNeedsIndirect_ = true;
  orphanDirectObj_ = directObj;
  orphanPreposition_ = prep;
}

void Parser::clearOrphan() {
  orphanFlag_ = false;
  orphanVerb_ = 0;
  orphanNeedsDirect_ = true;
  orphanNeedsIndirect_ = false;
  orphanDirectObj_ = nullptr;
  orphanPreposition_.clear();
}

// Helper to check if a verb requires a direct object
bool verbRequiresObject(VerbId verb) {
  // Verbs that require an object (based on ZIL gsyntax.zil)
  switch (verb) {
  case V_TAKE:
  case V_DROP:
  case V_PUT:
  case V_GIVE:
  case V_EXAMINE:
  case V_READ:
  case V_LOOK_INSIDE:
  case V_SEARCH:
  case V_OPEN:
  case V_CLOSE:
  case V_LOCK:
  case V_UNLOCK:
  case V_ATTACK:
  case V_THROW:
  case V_LAMP_ON:
  case V_LAMP_OFF:
  case V_TURN:
  case V_PUSH:
  case V_PULL:
  case V_MOVE:
  case V_TIE:
  case V_UNTIE:
  case V_EAT:
  case V_DRINK:
  case V_INFLATE:
  case V_DEFLATE:
  case V_WAVE:
  case V_RUB:
  case V_RING:
  case V_CLIMB_UP:
  case V_CLIMB_DOWN:
  case V_CLIMB_ON:
  case V_BOARD:
  case V_DISEMBARK:
    return true;
  default:
    return false;
  }
}

// Helper to get verb name for orphan prompt
std::string getVerbName(VerbId verb) {
  switch (verb) {
  case V_TAKE:
    return "take";
  case V_DROP:
    return "drop";
  case V_PUT:
    return "put";
  case V_GIVE:
    return "give";
  case V_EXAMINE:
    return "examine";
  case V_READ:
    return "read";
  case V_LOOK_INSIDE:
    return "look in";
  case V_SEARCH:
    return "search";
  case V_OPEN:
    return "open";
  case V_CLOSE:
    return "close";
  case V_LOCK:
    return "lock";
  case V_UNLOCK:
    return "unlock";
  case V_ATTACK:
    return "attack";
  case V_THROW:
    return "throw";
  case V_LAMP_ON:
    return "turn on";
  case V_LAMP_OFF:
    return "turn off";
  case V_TURN:
    return "turn";
  case V_PUSH:
    return "push";
  case V_PULL:
    return "pull";
  case V_MOVE:
    return "move";
  case V_TIE:
    return "tie";
  case V_UNTIE:
    return "untie";
  case V_EAT:
    return "eat";
  case V_DRINK:
    return "drink";
  case V_INFLATE:
    return "inflate";
  case V_DEFLATE:
    return "deflate";
  case V_WAVE:
    return "wave";
  case V_RUB:
    return "rub";
  case V_RING:
    return "ring";
  case V_CLIMB_UP:
    return "climb up";
  case V_CLIMB_DOWN:
    return "climb down";
  case V_CLIMB_ON:
    return "climb on";
  case V_BOARD:
    return "board";
  case V_DISEMBARK:
    return "disembark";
  default:
    return "do that to";
  }
}

// Check if a word is a known noun/adjective (exists in vocabulary)
bool Parser::isKnownObjectWord(const std::string &word) const {
  auto &g = Globals::instance();

  // ZIL: IT THEM HER HIM are dictionary words (synonyms of the IT object)
  if (isPronoun(word)) {
    return true;
  }

  // Check all objects for this word as a synonym or adjective
  for (const auto &[id, objPtr] : g.getAllObjects()) {
    if (objPtr->hasSynonym(word) || objPtr->hasAdjective(word)) {
      return true;
    }
  }
  return false;
}

// ZIL: SYNTAX-CHECK with P-NCN = 0 (gparser.zil:707-775). A syntax line
// taking no objects (optionally after the typed preposition) is accepted and
// P-SONUMS is 0; otherwise the ZIL would try GWIM and then orphan with
// "What do you want to <verb> [<prep>]?". GWIM arrives with Phase B; until
// then the orphan question is asked directly. Returns false when the parse
// must stop.
bool Parser::resolveObjectlessSyntax(ParsedCommand &cmd,
                                     const std::string &trailingPrep) {
  const std::string &verbWord = cmd.words[0];
  std::vector<std::string> preps;
  if (!trailingPrep.empty()) {
    preps.push_back(trailingPrep);
  }
  bool known = GSyntax::lookupVerb(verbWord).has_value();
  if (known) {
    // The syntax tables are keyed by the canonical verb word ("l" -> "look").
    std::string canonical = GSyntax::canonicalVerb(verbWord);
    if (GSyntax::matchSyntax(canonical.empty() ? verbWord : canonical, preps,
                             0)) {
      cmd.objectsExpected = 0;
      return true;
    }
  } else if (!verbRequiresObject(cmd.verb)) {
    cmd.objectsExpected = 0;
    return true;
  }
  // ZIL: <ORPHAN .DRIVE1 .DRIVE2> <TELL "What do you want to "> <WORD-PRINT
  //      verb> <PREP-PRINT prep> <TELL "?" CR> (gparser.zil:762-775)
  setOrphanDirect(cmd.verb, verbWord);
  std::string prompt = "What do you want to " + verbWord;
  if (!trailingPrep.empty()) {
    prompt += " " + trailingPrep;
  }
  printLine(prompt + "?");
  return false;
}

ParsedCommand Parser::parse(const std::string &input) {
  // ZIL: <SETG P-WALK-DIR <>> unless the input is a direction (gparser.zil:370-375)
  Globals::instance().pWalkDir.reset();
  ParsedCommand cmd;

  // Handle AGAIN command
  tokenize(input, cmd.words);
  if (isAgainCommand(cmd.words)) {
    if (lastCommand_.empty()) {
      printLine("You haven't entered a command yet.");
      return cmd;
    }
    // Re-parse the last command
    orphanFlag_ = false; // Clear orphan state for AGAIN
    return parse(lastCommand_);
  }

  // Handle OOPS command
  if (isOopsCommand(cmd.words)) {
    if (!hadUnknownWordLastTurn_ || lastUnknownWord_.empty()) {
      printLine("There was no word to correct.");
      return cmd;
    }

    if (cmd.words.size() < 2) {
      printLine("Oops what?");
      return cmd;
    }

    // Replace the unknown word and retry
    std::string corrected = replaceOopsWord(lastCommand_, cmd.words[1]);
    clearLastUnknownWord();
    return parse(corrected);
  }

  // Clear unknown word flag for normal commands
  if (!cmd.words.empty()) {
    hadUnknownWordLastTurn_ = false;
  }

  if (cmd.words.empty()) {
    return cmd;
  }

  // Check if we're continuing from an orphaned (incomplete) command
  if (orphanFlag_) {
    // Try to merge this input with the previous incomplete command
    // First, check if this is a new verb (user is starting fresh)
    VerbId newVerb = findVerb(cmd.words[0]);
    Direction *newDir = findDirection(cmd.words[0]);

    if (newVerb != 0 || newDir) {
      // User started a new command, abandon the orphan
      orphanFlag_ = false;
    } else {
      // Try to use this input as the missing object
      auto matches = findObjects(cmd.words, 0);
      if (!matches.empty()) {
        // Successfully found an object - complete the orphaned command
        cmd.verb = orphanVerb_;

        if (orphanNeedsDirect_) {
          cmd.directObj = matches.size() == 1
                              ? matches[0]
                              : disambiguate(matches, cmd.words.back());
        } else if (orphanNeedsIndirect_) {
          cmd.directObj = orphanDirectObj_;
          cmd.indirectObj = matches.size() == 1
                                ? matches[0]
                                : disambiguate(matches, cmd.words.back());
        }

        orphanFlag_ = false;
        // Save the completed command for AGAIN
        lastCommand_ = input; // This isn't quite right but works for now
        finishTables(cmd);
        return cmd;
      } else {
        // Couldn't find an object - check for unknown word
        for (const auto &word : cmd.words) {
          if (word != "the" && word != "a" && word != "an") {
            setLastUnknownWord(word);
            printLine("I don't know the word \"" + word + "\".");
            orphanFlag_ = false;
            return cmd;
          }
        }
        printLine("I don't see that here.");
        orphanFlag_ = false;
        return cmd;
      }
    }
  }

  // Save this command for AGAIN (but not if it's AGAIN itself)
  if (!isAgainCommand(cmd.words) && !isOopsCommand(cmd.words)) {
    lastCommand_ = input;
  }

  // Check if first word is a direction
  Direction *dir = findDirection(cmd.words[0]);
  if (dir) {
    cmd.isDirection = true;
    cmd.direction = *dir;
    cmd.verb = V_WALK;
    // ZIL: <SETG PRSA ,V?WALK> <SETG PRSO .DIR> <SETG P-WALK-DIR .DIR> (gparser.zil:370-374)
    Globals::instance().pWalkDir = *dir;
    orphanFlag_ = false;
    return cmd;
  }

  // Check for verb
  cmd.verb = findVerb(cmd.words[0]);
  if (cmd.verb == 0) {
    // Check for unknown words first - prevents OOPS failure when mixing
    // unknown/known words
    for (const auto &word : cmd.words) {
      if (word != "the" && word != "a" && word != "an" &&
          !isPreposition(word) && !isKnownObjectWord(word)) {

        // Unknown word - save for OOPS
        setLastUnknownWord(word);
        printLine("I don't know the word \"" + word + "\".");
        return cmd;
      }
    }

    // Check if it might be an object name (user typed just an object without a
    // verb)
    auto matches = findObjects(cmd.words, 0);
    if (!matches.empty()) {
      printLine("I don't understand that sentence.");
      return cmd;
    }

    // All words are known but syntax is invalid
    printLine("I don't understand that sentence.");
    return cmd;
  }

  // Special handling for "go <direction>" (e.g., "go in", "go out", "go north")
  // Per ZIL: GO IN = ENTER, GO OUT = EXIT
  if (cmd.verb == V_WALK && cmd.words.size() >= 2) {
    Direction *dir = findDirection(cmd.words[1]);
    if (dir) {
      cmd.isDirection = true;
      cmd.direction = *dir;
      Globals::instance().pWalkDir = *dir;
      orphanFlag_ = false;
      return cmd;
    }
  }

  // Find preposition and extract indirect object (PRSI)
  if (cmd.verb != 0 && !cmd.isDirection) {
    auto prepIdx = findPrepositionIndex(cmd.words);
    if (prepIdx.has_value() && prepIdx.value() + 1 < cmd.words.size()) {
      const std::string &preposition = cmd.words[prepIdx.value()];

      // Validate preposition for this verb
      if (!validatePreposition(cmd.verb, preposition)) {
        // Invalid preposition for this verb
        printLine("I don't understand that.");
        cmd.verb = 0; // Mark command as invalid
        return cmd;
      }

      // Update verb ID based on syntax (e.g., PUT + ON -> V_PUT_ON)
      if (verbRegistry_) {
        auto specificVerb =
            verbRegistry_->getVerbIdForSyntax(cmd.verb, preposition);
        if (specificVerb.has_value()) {
          cmd.verb = specificVerb.value();
        }
      }

      // Extract direct object (words between verb and preposition)
      std::vector<std::string> directObjWords(
          cmd.words.begin() + 1, cmd.words.begin() + prepIdx.value());

      // Extract indirect object (words after preposition)
      std::vector<std::string> indirectObjWords(
          cmd.words.begin() + prepIdx.value() + 1, cmd.words.end());

      // ZIL: SNARF-OBJECTS resolves NC2 before NC1 (gparser.zil:928-936)
      if (!directObjWords.empty()) {
        cmd.objectsExpected = indirectObjWords.empty() ? 1 : 2;
        if (!indirectObjWords.empty() &&
            !snarfPhrase(cmd, indirectObjWords, false, cmd.prsiTable)) {
          cmd.verb = 0;
          return cmd;
        }
        if (!snarfPhrase(cmd, directObjWords, true, cmd.prsoTable)) {
          cmd.verb = 0;
          return cmd;
        }
      } else {
        // Preposition immediately follows verb (e.g., "turn on lamp",
        // "look at sword"): the object after it is the direct object (PRSO)
        cmd.objectsExpected = 1;
        if (!indirectObjWords.empty() &&
            !snarfPhrase(cmd, indirectObjWords, true, cmd.prsoTable)) {
          cmd.verb = 0;
          return cmd;
        }
      }
    } else if (cmd.verb != 0) {
      // A trailing preposition with nothing after it ("turn on", "look at")
      std::string trailingPrep;
      if (prepIdx.has_value()) {
        trailingPrep = cmd.words[prepIdx.value()];
      }
      std::vector<std::string> objWords;
      if (cmd.words.size() > 1) {
        size_t last = prepIdx.has_value() ? prepIdx.value() : cmd.words.size();
        objWords.assign(cmd.words.begin() + 1, cmd.words.begin() + last);
      }
      if (!objWords.empty()) {
        // No preposition, just try to find direct object
        cmd.objectsExpected = 1;
        if (!snarfPhrase(cmd, objWords, true, cmd.prsoTable)) {
          cmd.verb = 0;
          return cmd;
        }
      } else if (!resolveObjectlessSyntax(cmd, trailingPrep)) {
        cmd.verb = 0;
        return cmd;
      }
    }
  }

  finishTables(cmd);
  return cmd;
}

// Fill the ZIL match tables (P-PRSO / P-PRSI) from the single-object
// results of the interim parser, and keep the legacy mirrors coherent.
void Parser::finishTables(ParsedCommand &cmd) {
  if (cmd.directObj && cmd.prsoTable.empty()) {
    cmd.prsoTable.push_back(cmd.directObj);
  }
  if (cmd.indirectObj && cmd.prsiTable.empty()) {
    cmd.prsiTable.push_back(cmd.indirectObj);
  }
  cmd.directObj = cmd.prsoTable.empty() ? nullptr : cmd.prsoTable.front();
  cmd.indirectObj = cmd.prsiTable.empty() ? nullptr : cmd.prsiTable.front();
  // Legacy mirrors
  cmd.isAll = cmd.nc1IsAll;
  if (cmd.isAll) {
    cmd.allObjects = cmd.prsoTable;
  }
}

ParseExpected Parser::tryParse(std::string_view input) {
  if (input.empty()) {
    return std::unexpected(ParseError{ParseErrorCode::EMPTY_INPUT, "Please enter a command."});
  }
  ParsedCommand cmd = parse(std::string(input));
  if (cmd.verb == 0 && !cmd.isDirection) {
    return std::unexpected(ParseError{ParseErrorCode::SYNTAX_ERROR, "I don't understand that sentence."});
  }
  return cmd;
}
