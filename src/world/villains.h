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

/// ZIL: <ROUTINE ROBBER-FUNCTION ("OPTIONAL" (MODE <>))> (1actions.zil:1947-2085)
int robberFunction(int mode);

/// ZIL: <ROUTINE I-THIEF ()> (1actions.zil:3890-3930). The thief demon.
bool iThief();

/// ZIL: <ROUTINE THIEF-VS-ADVENTURER (HERE?)> (1actions.zil:1764-1873).
/// True when the thief has finished with the player this turn.
bool thiefVsAdventurer(bool here);

/// ZIL: <ROUTINE ROB (WHAT WHERE "OPTIONAL" (PROB <>))> (1actions.zil:3978-3990)
bool rob(ZObject *what, ZObject *where, int probability = 0);

/// ZIL: <ROUTINE STEAL-JUNK (RM)> (1actions.zil:3957-3976)
bool stealJunk(ZObject *room);

/// ZIL: <ROUTINE DROP-JUNK (RM)> (1actions.zil:3932-3949)
bool dropJunk(ZObject *room);

/// ZIL: <ROUTINE ROB-MAZE (RM)> (1actions.zil:1917-1933)
bool robMaze(ZObject *room);

/// ZIL: <ROUTINE DEPOSIT-BOOTY (RM)> (1actions.zil:1898-1912)
bool depositBooty(ZObject *room);

/// ZIL: <ROUTINE RECOVER-STILETTO ()> (1actions.zil:3951-3955)
void recoverStiletto();

/// ZIL: <ROUTINE HACK-TREASURES ()> (1actions.zil:1888-1896)
void hackTreasures();

/// ZIL: <ROUTINE STOLE-LIGHT? ()> (1actions.zil:1876-1882)
bool stoleLight();

/// ZIL: <ROUTINE MOVE-ALL (FROM TO)> (1actions.zil:2114-2122)
void moveAll(ZObject *from, ZObject *to);

/// ZIL: <ROUTINE LARGE-BAG-F ()> (1actions.zil:2088-2112)
bool largeBagF();

/// ZIL: <ROUTINE CHALICE-FCN ()> (1actions.zil:2124-2136)
bool chaliceFcn();

/// ZIL: <ROUTINE TREASURE-ROOM-FCN (RARG)> (1actions.zil:2138-2150)
int treasureRoomFcn(int rarg);

/// ZIL: <ROUTINE THIEF-IN-TREASURE ()> (1actions.zil:2152-2163)
void thiefInTreasure();

/// ZIL LDESC strings the thief switches between (1actions.zil:2087-2094).
extern const char *const kRobberCDesc;
extern const char *const kRobberUDesc;

} // namespace Villains
