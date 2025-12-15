#include "core/globals.h"
#include "core/io.h"
#include "verbs/verbs.h"

// SONGBIRD-F - Songbird bauble drop helper
// ZIL: Only handles TAKE (returns RTRUE - object is takeable but action is
// handled) Source: 1actions.zil lines 3586-3588
bool songbirdAction() {
  auto &g = Globals::instance();

  // The songbird itself is not directly interactable beyond default take
  // This is mainly a placeholder if needed
  // ZIL just returns RTRUE for any verb, meaning "handled"

  return RTRUE; // All verbs handled by returning true
}
