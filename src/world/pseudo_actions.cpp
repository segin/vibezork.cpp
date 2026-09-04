#include "pseudo_actions.h"
#include "core/globals.h"
#include "core/io.h"
#include "verbs/verbs.h"

// ZIL: NAILS-PSEUDO
// Source: zil/1actions.zil:376-380
// <ROUTINE NAILS-PSEUDO ()
// 	 <COND (<VERB? TAKE>
// 		<TELL
// "The nails, deeply imbedded in the door, cannot be removed." CR>)>>
bool nailsPseudo() {
    auto &g = Globals::instance();
    if (g.prsa == V_TAKE) {
        printLine("The nails, deeply imbedded in the door, cannot be removed.");
        return RTRUE;
    }
    return RFALSE;
}
