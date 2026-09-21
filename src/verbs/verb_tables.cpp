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

} // namespace VerbTables
