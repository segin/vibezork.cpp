#include "verb_tables.h"

namespace VerbTables {

// ZIL: <GLOBAL YUKS
//         <LTABLE
//          0
//          "A valiant attempt."
//          "You can't be serious."
//          ;"Not bloody likely."
//          "An interesting idea..."
//          "What a concept!">>
// Source: zil/gverbs.zil:2210-2216
// The third entry is commented out in the source, so the table holds four.
GMacros::ZilRandomTable<std::string> &yuks() {
  static GMacros::ZilRandomTable<std::string> table{
      "A valiant attempt.",
      "You can't be serious.",
      "An interesting idea...",
      "What a concept!",
  };
  return table;
}

// ZIL: <GLOBAL HELLOS <LTABLE 0 "Hello." "Good day."
//         "Nice weather we've been having lately." "Goodbye.">>
// Source: zil/gverbs.zil:2200-2204
GMacros::ZilRandomTable<std::string> &hellos() {
  static GMacros::ZilRandomTable<std::string> table{
      "Hello.",
      "Good day.",
      "Nice weather we've been having lately.",
      "Goodbye.",
  };
  return table;
}

// ZIL: <GLOBAL HO-HUM <LTABLE 0 " doesn't seem to work."
//         " isn't notably helpful." " has no effect.">>
// Source: zil/gverbs.zil:2030-2036. Each entry begins with a space because
// HACK-HACK prints it straight after the object name.
GMacros::ZilRandomTable<std::string> &hoHum() {
  static GMacros::ZilRandomTable<std::string> table{
      " doesn't seem to work.",
      " isn't notably helpful.",
      " has no effect.",
  };
  return table;
}

} // namespace VerbTables
