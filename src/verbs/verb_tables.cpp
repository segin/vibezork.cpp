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

// ZIL: <GLOBAL JUMPLOSS <LTABLE 0 ...>> (gverbs.zil:844-848)
GMacros::ZilRandomTable<std::string> &jumploss() {
  static GMacros::ZilRandomTable<std::string> table{
      "You should have looked before you leaped.",
      "In the movies, your life would be passing before your eyes.",
      "Geronimo...",
  };
  return table;
}

// ZIL: <GLOBAL WHEEEEE <LTABLE 0 ...>> (gverbs.zil:1272-1276)
GMacros::ZilRandomTable<std::string> &wheeeee() {
  static GMacros::ZilRandomTable<std::string> table{
      "Very good. Now you can go to the second grade.",
      "Are you enjoying yourself?",
      "Wheeeeeeeeee!!!!!",
      "Do you expect me to applaud?",
  };
  return table;
}

// ZIL: <GLOBAL DUMMY <LTABLE 0 ...>> (gverbs.zil:2213-2217)
GMacros::ZilRandomTable<std::string> &dummy() {
  static GMacros::ZilRandomTable<std::string> table{
      "Look around.",
      "Too late for that.",
      "Have your eyes checked.",
  };
  return table;
}

// ZIL: <GLOBAL SWIMYUKS <LTABLE 0 ...>> (gverbs.zil:2194-2196)
GMacros::ZilRandomTable<std::string> &swimyuks() {
  static GMacros::ZilRandomTable<std::string> table{
      "You can't swim in the dungeon.",
  };
  return table;
}

} // namespace VerbTables
