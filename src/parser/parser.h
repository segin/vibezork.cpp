#pragma once
#include "core/types.h"
#include "world/rooms.h"
#include <string>
#include <string_view>
#include <vector>

/**
 * @file parser.h
 * @brief The parse result MAIN-LOOP-1 consumes, plus a thin facade over
 * GParser::parser() for tests and tools.
 *
 * The parser itself is the verbatim GPARSER.ZIL port in parser/gparser.*.
 * ParsedCommand is a snapshot of its result tables (PRSA, P-PRSO, P-PRSI,
 * P-GETFLAGS, P-SYNTAX, P-WALK-DIR) taken after PARSER succeeds.
 */

class Parser;

// Global facade instance (defined in parser_instance.cpp)
Parser& getGlobalParser();

struct ParsedCommand {
    VerbId verb = 0;                  // PRSA (0 when the parse failed)
    ZObject* directObj = nullptr;     // first entry of prsoTable, if any
    ZObject* indirectObj = nullptr;   // first entry of prsiTable, if any
    std::vector<std::string> words;   // the typed words (P-LEXV texts)
    Direction direction = Direction::NORTH;
    bool isDirection = false;         // P-WALK-DIR set

    // ZIL: P-PRSO / P-PRSI match tables (gparser.zil:964-965); the size is
    // P-MATCHLEN. The IT object (gglobals.zil:42-46) may appear in either
    // table and is substituted by MAIN-LOOP-1 / PERFORM (gmain.zil:45-64,
    // 194-203).
    std::vector<ZObject*> prsoTable;
    std::vector<ZObject*> prsiTable;
    // ZIL: ,P-GETFLAGS after SNARFEM: 0, P-ALL, P-ONE or P-INHIBIT
    // (gparser.zil:970-973).
    int getFlags = 0;
    // ZIL: <EQUAL? <GET <GET ,P-ITBL ,P-NC1> 0> ,W?ALL> -- the first noun
    // clause began with the word ALL (gmain.zil:122, 129).
    bool nc1IsAll = false;
    // ZIL: <BAND <GETB ,P-SYNTAX ,P-SBITS> ,P-SONUMS> -- number of objects
    // the matched syntax expects (gmain.zil:83).
    int objectsExpected = 0;

    // Convenience mirrors: nc1IsAll, the P-PRSO table and the first P-BUTS
    // entry.
    bool isAll = false;
    std::vector<ZObject*> allObjects;
    ZObject* exceptObject = nullptr;
};

// Snapshot of the GParser tables after a successful PARSER call.
ParsedCommand buildParsedCommand();

// Facade: runs PARSER on one line without the prompt. Constructing a
// Parser resets the parser state (P-OFLAG, P-CONT, AGAIN buffers...).
class Parser {
public:
    Parser();
    ParsedCommand parse(const std::string& input);
    // ZIL: ,P-OFLAG
    bool isOrphaned() const;
    // Forget a pending orphan (tests)
    void clearOrphan();
};
