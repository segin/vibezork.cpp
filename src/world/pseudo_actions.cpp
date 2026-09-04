#include "pseudo_actions.h"
#include "core/globals.h"
#include "core/io.h"
#include "verbs/verbs.h"
#include "world/objects.h"

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

// ZIL: STREAM-PSEUDO
// Source: zil/1actions.zil:3185-3190
// <ROUTINE STREAM-PSEUDO ()
// 	 <COND (<VERB? SWIM THROUGH>
// 		<TELL "You can't swim in the stream." CR>)
// 	       (<VERB? CROSS>
// 		<TELL "The other side is a sheer rock cliff." CR>)>>
bool streamPseudo() {
    auto &g = Globals::instance();
    if (g.prsa == V_SWIM || g.prsa == V_THROUGH) {
        printLine("You can't swim in the stream.");
        return RTRUE;
    }
    if (g.prsa == V_CROSS) {
        printLine("The other side is a sheer rock cliff.");
        return RTRUE;
    }
    return RFALSE;
}

// ZIL: CHASM-PSEUDO
// Source: zil/1actions.zil:3191-3202
// <ROUTINE CHASM-PSEUDO ()
// 	 <COND (<OR <VERB? LEAP>
// 		    <AND <VERB? PUT> <EQUAL? ,PRSO ,ME>>>
// 		<TELL
// "You look before leaping, and realize that you would never survive." CR>)
// 	       (<VERB? CROSS>
// 		<TELL "It's too far to jump, and there's no bridge." CR>)
// 	       (<AND <VERB? PUT THROW-OFF> <EQUAL? ,PRSI ,PSEUDO-OBJECT>>
// 		<TELL
// "The " D ,PRSO " drops out of sight into the chasm." CR>
// 		<REMOVE-CAREFULLY ,PRSO>)>>
bool chasmPseudo() {
    auto &g = Globals::instance();
    if (g.prsa == V_LEAP || (g.prsa == V_PUT && g.prso && (g.prso->getId() == ObjectIds::ME || g.prso == g.player))) {
        printLine("You look before leaping, and realize that you would never survive.");
        return RTRUE;
    }
    if (g.prsa == V_CROSS) {
        printLine("It's too far to jump, and there's no bridge.");
        return RTRUE;
    }
    if ((g.prsa == V_PUT || g.prsa == V_THROW_OFF) && g.prsi && g.prsi->getId() == ObjectIds::PSEUDO_OBJECT) {
        if (g.prso) {
            print("The ");
            print(g.prso->getDesc());
            printLine(" drops out of sight into the chasm.");
            Verbs::removeCarefully(g.prso);
        }
        return RTRUE;
    }
    return RFALSE;
}

// ZIL: DOME-PSEUDO
// Source: zil/1actions.zil:3203-3206
// <ROUTINE DOME-PSEUDO ()
// 	 <COND (<VERB? KISS>
// 		<TELL "No." CR>)>>
bool domePseudo() {
    auto &g = Globals::instance();
    if (g.prsa == V_KISS) {
        printLine("No.");
        return RTRUE;
    }
    return RFALSE;
}

// ZIL: GATE-PSEUDO
// Source: zil/1actions.zil:3207-3215
// <ROUTINE GATE-PSEUDO ()
// 	 <COND (<VERB? THROUGH>
// 		<DO-WALK ,P?IN>
// 		<RTRUE>)
// 	       (T
// 		<TELL
// "The gate is protected by an invisible force. It makes your
// teeth ache to touch it." CR>)>>
bool gatePseudo() {
    auto &g = Globals::instance();
    if (g.prsa == V_THROUGH) {
        Verbs::doWalk(Direction::IN);
        return RTRUE;
    }
    printLine("The gate is protected by an invisible force. It makes your "
              "teeth ache to touch it.");
    return RTRUE;
}

// ZIL: DOOR-PSEUDO
// Source: zil/1actions.zil:3216-3221
// <ROUTINE DOOR-PSEUDO () ;"in Studio"
// 	 <COND (<VERB? OPEN CLOSE>
// 		<TELL "The door won't budge." CR>)
// 	       (<VERB? THROUGH>
// 		<DO-WALK ,P?SOUTH>)>>
bool doorPseudo() {
    auto &g = Globals::instance();
    if (g.prsa == V_OPEN || g.prsa == V_CLOSE) {
        printLine("The door won't budge.");
        return RTRUE;
    }
    if (g.prsa == V_THROUGH) {
        Verbs::doWalk(Direction::SOUTH);
        return RTRUE;
    }
    return RFALSE;
}
