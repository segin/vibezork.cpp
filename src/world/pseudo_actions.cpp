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

// ZIL: LAKE-PSEUDO
// Source: zil/1actions.zil:3177-3184
// <ROUTINE LAKE-PSEUDO ()
// 	 <COND (,LOW-TIDE
// 		<TELL "There's not much lake left...." CR>)
// 	       (<VERB? CROSS>
// 		<TELL "It's too wide to cross." CR>)
// 	       (<VERB? THROUGH>
// 		<TELL "You can't swim in this lake." CR>)>>
bool lakePseudo() {
    auto &g = Globals::instance();
    if (g.lowTide) {
        printLine("There's not much lake left....");
        return RTRUE;
    }
    if (g.prsa == V_CROSS) {
        printLine("It's too wide to cross.");
        return RTRUE;
    }
    if (g.prsa == V_THROUGH) {
        printLine("You can't swim in this lake.");
        return RTRUE;
    }
    return RFALSE;
}
