#include "parser.h"
#include "core/globals.h"
#include "parser/gparser.h"

Parser::Parser() {
  GParser::resetState();
  GParser::invalidateDictionary();
}

bool Parser::isOrphaned() const { return Globals::instance().pOflag; }

void Parser::clearOrphan() {
  Globals::instance().pOflag = false;
  Globals::instance().pCont = 0;
}

ParsedCommand Parser::parse(const std::string &input) {
  auto &g = Globals::instance();
  GParser::setNextInput(input);
  GParser::setPromptEnabled(false);
  bool won = GParser::parser();
  GParser::setPromptEnabled(true);
  g.pWon = won;
  if (!won) {
    // ZIL: (T <SETG P-CONT <>>) (gmain.zil:162-163)
    g.pCont = 0;
    ParsedCommand cmd;
    const auto &lexv = GParser::state().lexv;
    for (int i = 0; i < lexv.count && i < static_cast<int>(lexv.e.size()); ++i) {
      cmd.words.push_back(lexv.e[i].text);
    }
    return cmd;
  }
  return buildParsedCommand();
}

ParsedCommand buildParsedCommand() {
  auto &g = Globals::instance();
  const auto &s = GParser::state();
  ParsedCommand cmd;
  cmd.verb = g.prsa;
  for (int i = 0; i < s.lexv.count && i < static_cast<int>(s.lexv.e.size()); ++i) {
    cmd.words.push_back(s.lexv.e[i].text);
  }
  if (g.pWalkDir) {
    cmd.isDirection = true;
    cmd.direction = *g.pWalkDir;
  }
  cmd.prsoTable = s.prso;
  cmd.prsiTable = s.prsi;
  cmd.directObj = cmd.prsoTable.empty() ? nullptr : cmd.prsoTable.front();
  cmd.indirectObj = cmd.prsiTable.empty() ? nullptr : cmd.prsiTable.front();
  cmd.getFlags = g.pGetFlags;
  const GParser::DictWord *nc1Word = GParser::wordAt(s.itbl.nc1);
  cmd.nc1IsAll = nc1Word && nc1Word == GParser::W("all");
  cmd.objectsExpected = s.syntax ? s.syntax->nobj : 0;
  cmd.isAll = cmd.nc1IsAll;
  if (cmd.nc1IsAll) cmd.allObjects = cmd.prsoTable;
  cmd.exceptObject = s.buts.empty() ? nullptr : s.buts.front();
  return cmd;
}
