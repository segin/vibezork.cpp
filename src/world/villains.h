#pragma once

class ZObject;

/**
 * @file villains.h
 * @brief The villain ACTION routines of zil/1actions.zil.
 *
 * These take the ZIL routine's optional MODE argument, because the melee
 * engine calls them with F-BUSY?, F-DEAD, F-UNCONSCIOUS, F-CONSCIOUS and
 * F-FIRST? as well as for ordinary verb dispatch (MODE 0).
 */
namespace Villains {

/// ZIL: <ROUTINE TROLL-FCN ("OPTIONAL" (MODE <>))> (1actions.zil:640-764)
int trollFcn(int mode);

/// ZIL: <ROUTINE WEAPON-FUNCTION (W V)> (1actions.zil:629-638)
bool weaponFunction(ZObject *weapon, ZObject *villain);

/// ZIL: <ROUTINE AXE-F ()> (1actions.zil:622-624)
bool axeF();

/// ZIL: <ROUTINE STILETTO-FUNCTION ()> (1actions.zil:626-627)
bool stilettoFunction();

} // namespace Villains
