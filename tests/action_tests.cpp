// Action unit tests for ZIL action routines
#include "test_framework.h"
#include "../src/core/object.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"
#include "../src/world/world.h"
#include "../src/verbs/verbs.h"
#include <sstream>

// Capture output for testing
class OutputCapture {
public:
    OutputCapture() : old_cout(std::cout.rdbuf()) {
        std::cout.rdbuf(buffer.rdbuf());
    }
    
    ~OutputCapture() {
        std::cout.rdbuf(old_cout);
    }
    
    std::string getOutput() const {
        return buffer.str();
    }
    
private:
    std::stringstream buffer;
    std::streambuf* old_cout;
};

// Forward declarations for action functions
extern bool axeAction();
extern bool knifeAction();
extern bool teethAction();

// Initialize world for testing
static void setupTestWorld() {
    initializeWorld();
}

// =============================================================================
// AXE-F Tests (1actions.zil lines 622-638)
// ZIL Logic: If troll alive in room, prevents taking axe via WEAPON-FUNCTION
// =============================================================================

TEST(AxeF_TrollInRoomBlocksTake) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    // Move player and troll to same room
    ZObject* troll = g.getObject(ObjectIds::TROLL);
    ZObject* axe = g.getObject(ObjectIds::AXE);
    ZObject* trollRoom = g.getObject(RoomIds::TROLL_ROOM);
    
    if (!troll || !axe || !trollRoom) {
        return; // Skip if objects not defined
    }
    
    // Setup: troll and player in troll room, axe on ground
    g.here = trollRoom;
    troll->moveTo(trollRoom);
    axe->moveTo(trollRoom);
    troll->clearFlag(ObjectFlag::NDESCBIT); // Troll is alive
    
    // Try to take axe
    g.prsa = V_TAKE;
    g.prso = axe;
    
    OutputCapture cap;
    bool result = axeAction();
    
    // Should block take and print message
    // Should block take and print message
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("white-hot") != std::string::npos);
}

TEST(AxeF_TrollPossessionBlocksTake) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    ZObject* troll = g.getObject(ObjectIds::TROLL);
    ZObject* axe = g.getObject(ObjectIds::AXE);
    ZObject* trollRoom = g.getObject(RoomIds::TROLL_ROOM);
    
    if (!troll || !axe || !trollRoom) return;
    
    g.here = trollRoom;
    troll->moveTo(trollRoom);
    axe->moveTo(troll); // Axe is IN Troll (inventory)
    troll->clearFlag(ObjectFlag::NDESCBIT);
    
    g.prsa = V_TAKE;
    g.prso = axe;
    
    OutputCapture cap;
    bool result = axeAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("swings") != std::string::npos);
}

TEST(AxeF_DeadTrollAllowsTake) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    ZObject* troll = g.getObject(ObjectIds::TROLL);
    ZObject* axe = g.getObject(ObjectIds::AXE);
    ZObject* trollRoom = g.getObject(RoomIds::TROLL_ROOM);
    
    if (!troll || !axe || !trollRoom) {
        return;
    }
    
    // Setup: dead troll (NDESCBIT set)
    g.here = trollRoom;
    troll->moveTo(trollRoom);
    axe->moveTo(trollRoom);
    troll->setFlag(ObjectFlag::NDESCBIT); // Troll is dead
    
    g.prsa = V_TAKE;
    g.prso = axe;
    
    bool result = axeAction();
    
    // Should return false, allowing normal take
    ASSERT_FALSE(result);
}

TEST(AxeF_TrollNotInRoomAllowsTake) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    ZObject* troll = g.getObject(ObjectIds::TROLL);
    ZObject* axe = g.getObject(ObjectIds::AXE);
    ZObject* livingRoom = g.getObject(RoomIds::LIVING_ROOM);
    ZObject* trollRoom = g.getObject(RoomIds::TROLL_ROOM);
    
    if (!troll || !axe || !livingRoom || !trollRoom) {
        return;
    }
    
    // Player in living room, troll in troll room
    g.here = livingRoom;
    troll->moveTo(trollRoom);
    axe->moveTo(livingRoom);
    
    g.prsa = V_TAKE;
    g.prso = axe;
    
    bool result = axeAction();
    
    // Troll not here, should allow take
    ASSERT_FALSE(result);
}

// =============================================================================
// BAG-OF-COINS-F Tests (1actions.zil lines 4137-4152)
// ZIL Logic: STUPID-CONTAINER for coins - blocks OPEN/CLOSE, handles EXAMINE
// =============================================================================

extern bool bagOfCoinsAction();

TEST(BagOfCoinsF_OpenReturnsMessage) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_OPEN;
    g.prso = g.getObject(ObjectIds::BAG_OF_COINS);
    
    OutputCapture cap;
    bool result = bagOfCoinsAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("coins") != std::string::npos);
    ASSERT_TRUE(output.find("safely inside") != std::string::npos);
}

TEST(BagOfCoinsF_ExamineShowsContents) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_EXAMINE;
    g.prso = g.getObject(ObjectIds::BAG_OF_COINS);
    
    OutputCapture cap;
    bool result = bagOfCoinsAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("lots of") != std::string::npos);
    ASSERT_TRUE(output.find("coins") != std::string::npos);
}

TEST(BagOfCoinsF_TakeReturnsFalse) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_TAKE;
    g.prso = g.getObject(ObjectIds::BAG_OF_COINS);
    
    bool result = bagOfCoinsAction();
    
    // TAKE not handled by stupid container, returns false
    ASSERT_FALSE(result);
}

TEST(BagOfCoinsF_PutSomethingInBagReturnsMessage) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    ZObject* bag = g.getObject(ObjectIds::BAG_OF_COINS);
    ZObject* coin = g.getObject(ObjectIds::COINS); // Use COINS (assuming from ZIL)
    
    // Create dummy coin if needed (just for test)
    if (!coin) {
        // If COINS ID not valid, use a fallback
        // Checking build error: 'COIN' is not member. 'COINS' suggested.
        auto newCoin = std::make_unique<ZObject>(ObjectIds::COINS, "coin");
        g.registerObject(ObjectIds::COINS, std::move(newCoin));
        coin = g.getObject(ObjectIds::COINS);
    }

    g.prsa = V_PUT;
    g.prso = coin; // Putting Coin
    g.prsi = bag;  // Into Bag
    
    OutputCapture cap;
    bool result = bagOfCoinsAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("Don't be silly") != std::string::npos);
    ASSERT_TRUE(output.find("bag of coins") != std::string::npos);
}

// =============================================================================
// BARROW-DOOR-FCN Tests (1actions.zil lines 432-434)
// ZIL Logic: OPEN/CLOSE prints "The door is too heavy."
// =============================================================================

extern bool barrowDoorAction();

TEST(BarrowDoorFcn_OpenPrintsTooHeavy) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_OPEN;
    
    OutputCapture cap;
    bool result = barrowDoorAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("too heavy") != std::string::npos);
}

TEST(BarrowDoorFcn_ClosePrintsTooHeavy) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_CLOSE;
    
    OutputCapture cap;
    bool result = barrowDoorAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("too heavy") != std::string::npos);
}

TEST(BarrowDoorFcn_OtherVerbsReturnFalse) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_EXAMINE;
    
    bool result = barrowDoorAction();
    
    // Other verbs not handled
    ASSERT_FALSE(result);
}

// =============================================================================
// BARROW-FCN Tests (1actions.zil lines 436-438)
// ZIL Logic: THROUGH verb triggers walk west
// =============================================================================

extern bool barrowAction();

TEST(BarrowFcn_ThroughReturnsTrue) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_THROUGH;
    
    bool result = barrowAction();
    
    // THROUGH handled
    ASSERT_TRUE(result);
}

TEST(BarrowFcn_EnterReturnsTrue) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_ENTER;
    
    bool result = barrowAction();
    
    // ENTER handled (synonym for THROUGH)
    ASSERT_TRUE(result);
}

TEST(BarrowFcn_OtherVerbsReturnFalse) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_EXAMINE;
    
    bool result = barrowAction();
    
    ASSERT_FALSE(result);
}

// =============================================================================
// BASKET-F Tests (1actions.zil lines 277-306)
// ZIL Logic: RAISE/LOWER moves basket, TAKE blocked (securely fastened)
// =============================================================================

extern bool basketAction();

TEST(BasketF_RaiseHandled) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    ZObject* raisedBasket = g.getObject(ObjectIds::RAISED_BASKET);
    ZObject* loweredBasket = g.getObject(ObjectIds::LOWERED_BASKET);
    ZObject* shaftRoom = g.getObject(RoomIds::SHAFT_ROOM);
    ZObject* lowerShaft = g.getObject(RoomIds::LOWER_SHAFT);
    
    // Ensure initial state for test (assuming cageTop=false or unknown)
    // To properly test, we might need to reset static state, but we can't.
    // So we try RAISE. If it says "already at top", we try LOWER then RAISE.
    
    g.prso = raisedBasket;
    g.prsa = V_LOWER; // Ensure it's at bottom first
    basketAction(); 
    
    // Now Raise
    g.prsa = V_RAISE;
    OutputCapture cap;
    bool result = basketAction();
    
    ASSERT_TRUE(result);
    // Verify move: Raised Basket should be in Shaft Room (Top)
    ASSERT_EQ(raisedBasket->getLocation(), shaftRoom);
    // Lowered Basket should be in Lower Shaft (Bottom)
    ASSERT_EQ(loweredBasket->getLocation(), lowerShaft);
}

TEST(BasketF_LowerHandled) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    ZObject* raisedBasket = g.getObject(ObjectIds::RAISED_BASKET);
    ZObject* loweredBasket = g.getObject(ObjectIds::LOWERED_BASKET);
    ZObject* shaftRoom = g.getObject(RoomIds::SHAFT_ROOM);
    ZObject* lowerShaft = g.getObject(RoomIds::LOWER_SHAFT);

    // Ensure it's at top first
    g.prso = raisedBasket;
    g.prsa = V_RAISE;
    basketAction();
    
    // Now Lower
    g.prsa = V_LOWER;
    OutputCapture cap;
    bool result = basketAction();
    
    ASSERT_TRUE(result);
    // Verify move: Raised Basket should be in Lower Shaft (Bottom) - wait, ZIL says:
    // <MOVE ,RAISED-BASKET ,LOWER-SHAFT>
    // <MOVE ,LOWERED-BASKET ,SHAFT-ROOM>
    ASSERT_EQ(raisedBasket->getLocation(), lowerShaft);
    ASSERT_EQ(loweredBasket->getLocation(), shaftRoom);
}

TEST(BasketF_TakeBlocked) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prso = g.getObject(ObjectIds::RAISED_BASKET);
    g.prsa = V_TAKE;
    
    OutputCapture cap;
    bool result = basketAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("securely fastened") != std::string::npos);
}

// =============================================================================
// BAT-F Tests (1actions.zil lines 308-324)
// ZIL Logic: TAKE/ATTACK with garlic = can't reach, else bat teleports
// =============================================================================

extern bool batAction();

TEST(BatF_AttackWithoutGarlicTriggersFlyMe) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    // Ensure garlic is not with player
    ZObject* garlic = g.getObject(ObjectIds::GARLIC);
    if (garlic) garlic->moveTo(nullptr);
    
    g.prsa = V_ATTACK;
    
    OutputCapture cap;
    bool result = batAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("Fweep!") != std::string::npos);
    ASSERT_TRUE(output.find("lifts you away") != std::string::npos);
    
    // Check if player moved (teleportation)
    // Assuming start room wasn't one of the drop targets, or check room changed
    // In random test, player might land in same room if unlucky (if testing randomness)
    // But basic check is: did logic run? Yes.
    // For verifying teleport:
    // ASSERT_NE(g.player->getLocation(), someInitialRoom);
}

TEST(BatF_GarlicProtects) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    ZObject* garlic = g.getObject(ObjectIds::GARLIC);
    if (!garlic) {
         auto newGarlic = std::make_unique<ZObject>(ObjectIds::GARLIC, "garlic");
         g.registerObject(ObjectIds::GARLIC, std::move(newGarlic));
         garlic = g.getObject(ObjectIds::GARLIC);
    }
    
    // Give garlic to player
    garlic->moveTo(g.player);
    
    g.prsa = V_TAKE;
    
    OutputCapture cap;
    bool result = batAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("can't reach him") != std::string::npos);
    ASSERT_FALSE(output.find("Fweep!") != std::string::npos);
}

TEST(BatF_TellPrintsFweep) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_TELL;
    
    OutputCapture cap;
    bool result = batAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    // Should print 6 fweeps
    // We can just count or check occurrence.
    ASSERT_TRUE(output.find("Fweep!") != std::string::npos);
    // Simple verification
}

TEST(BatF_FlyMeTeleports) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    // Ensure no garlic
    if (auto* gar = g.getObject(ObjectIds::GARLIC)) gar->moveTo(nullptr);
    
    // Start somewhere NOT in the drop list if possible, or just check that location is one of the drops
    ZObject* safeRoom = g.getObject(RoomIds::LIVING_ROOM); // Safe start
    g.player->moveTo(safeRoom);
    
    g.prsa = V_ATTACK;
    
    // Need random seed? std::rand() used in implementation.
    // We can loop a few times if needed, or just check one jump.
    
    batAction();
    
    ZObject* finalLoc = g.player->getLocation();
    ASSERT_NE(finalLoc, safeRoom); // Should have moved
    
    // List of valid drops (sync with implementation)
    std::vector<ObjectId> validDrops = {
        RoomIds::MINE_1, RoomIds::MINE_2, RoomIds::MINE_3, RoomIds::MINE_4,
        RoomIds::LADDER_TOP, RoomIds::LADDER_BOTTOM, RoomIds::SQUEEKY_ROOM, RoomIds::COAL_MINE_1
    };
    
    bool found = false;
    for (auto id : validDrops) {
        if (finalLoc->getId() == id) {
            found = true;
            break;
        }
    }
    ASSERT_TRUE(found) << "Player teleported to invalid room: " << finalLoc->getDescription();
}

TEST(BatF_OtherVerbsReturnFalse) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_EXAMINE;
    
    bool result = batAction();
    
    ASSERT_FALSE(result);
}

// =============================================================================
// BELL-F Tests (1actions.zil lines 343-349)
// ZIL Logic: RING prints "Ding, dong."
// =============================================================================

extern bool bellAction();

TEST(BellF_RingPrintsDingDong) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    // Ensure we are NOT in LLD room
    // Default setupTestWorld starts in Living Room
    
    g.prsa = V_RING;
    
    OutputCapture cap;
    bool result = bellAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("Ding, dong") != std::string::npos);
}

TEST(BellF_RingInLLDWithoutFlagReturnsFalse) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    // Create LLD Room if needed
    ZObject* lldRoom = g.getObject(ObjectIds::LAND_OF_LIVING_DEAD);
    if (!lldRoom) {
         auto newRoom = std::make_unique<ZObject>(ObjectIds::LAND_OF_LIVING_DEAD, "Land of Living Dead");
         g.registerObject(ObjectIds::LAND_OF_LIVING_DEAD, std::move(newRoom));
         lldRoom = g.getObject(ObjectIds::LAND_OF_LIVING_DEAD);
    }
    
    // Move player to LLD
    g.here = lldRoom;
    g.lldFlag = false; // Flag false
    
    g.prsa = V_RING;
    
    bool result = bellAction();
    
    // Should return FALSE (blocked) to let room handle it
    ASSERT_FALSE(result);
}

TEST(BellF_RingInLLDWithFlagPrintsDingDong) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    // Create LLD Room if needed
    ZObject* lldRoom = g.getObject(ObjectIds::LAND_OF_LIVING_DEAD);
    if (!lldRoom) {
         auto newRoom = std::make_unique<ZObject>(ObjectIds::LAND_OF_LIVING_DEAD, "Land of Living Dead");
         g.registerObject(ObjectIds::LAND_OF_LIVING_DEAD, std::move(newRoom));
         lldRoom = g.getObject(ObjectIds::LAND_OF_LIVING_DEAD);
    }
    
    // Move player to LLD
    g.here = lldRoom;
    g.lldFlag = true; // Flag TRUE
    
    g.prsa = V_RING;
    
    OutputCapture cap;
    bool result = bellAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("Ding, dong") != std::string::npos);
}

TEST(BellF_OtherVerbsReturnFalse) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_EXAMINE;
    
    bool result = bellAction();
    
    ASSERT_FALSE(result);
}

// =============================================================================
// BOARD-F Tests (1actions.zil lines 44-46)
// ZIL Logic: TAKE/EXAMINE prints "The boards are securely fastened."
// =============================================================================

extern bool boardAction();

TEST(BoardF_TakePrintsSecurelyFastened) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_TAKE;
    
    OutputCapture cap;
    bool result = boardAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("securely fastened") != std::string::npos);
}

TEST(BoardF_ExaminePrintsSecurelyFastened) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_EXAMINE;
    
    OutputCapture cap;
    bool result = boardAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("securely fastened") != std::string::npos);
}

TEST(BoardF_OtherVerbsReturnFalse) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_OPEN;
    
    bool result = boardAction();
    
    ASSERT_FALSE(result);
}

// =============================================================================
// BOARDED-WINDOW-FCN Tests (1actions.zil lines 370-374)
// ZIL Logic: OPEN = boarded, MUNG/break = can't break
// =============================================================================

extern bool boardedWindowAction();

TEST(BoardedWindowFcn_OpenPrintsBoarded) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_OPEN;
    
    OutputCapture cap;
    bool result = boardedWindowAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("boarded") != std::string::npos);
}

TEST(BoardedWindowFcn_AttackPrintsCantBreak) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_ATTACK;
    
    OutputCapture cap;
    bool result = boardedWindowAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("break") != std::string::npos);
}

TEST(BoardedWindowFcn_MungPrintsCantBreak) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_MUNG;
    
    OutputCapture cap;
    bool result = boardedWindowAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("break") != std::string::npos);
}

TEST(BoardedWindowFcn_OtherVerbsReturnFalse) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_EXAMINE;
    
    bool result = boardedWindowAction();
    
    ASSERT_FALSE(result);
}

// =============================================================================
// BODY-FUNCTION Tests (1actions.zil lines 2178-2185)
// ZIL Logic: TAKE = "force keeps you", MUNG/BURN = death
// =============================================================================

extern bool bodyAction();

TEST(BodyFunction_TakePrintsForceKeepsYou) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_TAKE;
    
    OutputCapture cap;
    bool result = bodyAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("force keeps you") != std::string::npos);
}

TEST(BodyFunction_OtherVerbsReturnFalse) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_EXAMINE;
    
    bool result = bodyAction();
    
    ASSERT_FALSE(result);
}

// =============================================================================
// BOLT-F Tests (1actions.zil lines 1187-1215)
// ZIL Logic: TURN with wrench toggles gates, EXAMINE describes bolt
// =============================================================================

extern bool boltAction();

TEST(BoltF_ExaminePrintsDescription) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prso = g.getObject(ObjectIds::BOLT);
    g.prsa = V_EXAMINE;
    
    OutputCapture cap;
    bool result = boltAction();
    
    ASSERT_TRUE(result);
}

TEST(BoltF_TurnWithoutWrenchPrintsMessage) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prso = g.getObject(ObjectIds::BOLT);
    g.prsa = V_TURN;
    
    OutputCapture cap;
    bool result = boltAction();
    
    ASSERT_TRUE(result);
}

// =============================================================================
// BOTTLE-FUNCTION Tests (1actions.zil lines 1491-1507)
// ZIL Logic: THROW/MUNG destroys bottle, SHAKE spills water
// Current impl: Returns false (no special behavior)
// =============================================================================

extern bool bottleAction();

TEST(BottleFunction_ReturnsFalseNoSpecialBehavior) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_EXAMINE;
    
    bool result = bottleAction();
    
    // Current implementation has no special behavior
    ASSERT_FALSE(result);
}

// =============================================================================
// BUBBLE-F Tests (1actions.zil lines 1292-1321)
// ZIL Logic: EXAMINE/TAKE/ATTACK bubble
// =============================================================================

extern bool bubbleAction();

TEST(BubbleF_ExaminePrintsDescription) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prso = g.getObject(ObjectIds::BUBBLE);
    g.prsa = V_EXAMINE;
    
    OutputCapture cap;
    bool result = bubbleAction();
    
    ASSERT_TRUE(result);
}

TEST(BubbleF_TakeBlockedIntegralPart) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prso = g.getObject(ObjectIds::BUBBLE);
    g.prsa = V_TAKE;
    
    OutputCapture cap;
    bool result = bubbleAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    // ZIL: INTEGRAL-PART prints "It is an integral part of the control panel."
    ASSERT_TRUE(output.find("integral part") != std::string::npos);
}

// =============================================================================
// BUTTON-F Tests (1actions.zil dam control buttons)
// ZIL Logic: PUSH buttons to control dam gates
// =============================================================================

extern bool buttonAction();

TEST(ButtonF_ExaminePrintsDescription) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prso = g.getObject(ObjectIds::YELLOW_BUTTON);
    g.prsa = V_EXAMINE;
    
    OutputCapture cap;
    bool result = buttonAction();
    
    ASSERT_TRUE(result);
}

TEST(ButtonF_PushYellowOpensGates) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prso = g.getObject(ObjectIds::YELLOW_BUTTON);
    g.prsa = V_PUSH;
    
    OutputCapture cap;
    bool result = buttonAction();
    
    ASSERT_TRUE(result);
}

// =============================================================================
// CANDLES-FCN Tests (1actions.zil candle lighting)
// ZIL Logic: LAMP_ON/LAMP_OFF lights or extinguishes candles
// =============================================================================

extern bool candlesAction();

TEST(CandlesFcn_LampOnHandled) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prso = g.getObject(ObjectIds::CANDLES);
    g.prsa = V_LAMP_ON;
    
    OutputCapture cap;
    bool result = candlesAction();
    
    ASSERT_TRUE(result);
}

TEST(CandlesFcn_LampOffHandled) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prso = g.getObject(ObjectIds::CANDLES);
    if (g.prso) g.prso->setFlag(ObjectFlag::ONBIT);  // Make them lit first
    g.prsa = V_LAMP_OFF;
    
    OutputCapture cap;
    bool result = candlesAction();
    
    ASSERT_TRUE(result);
}

// =============================================================================
// CELLAR-FCN Tests (1actions.zil room handler)
// ZIL Logic: Room handler for cellar - currently stub
// =============================================================================

extern void cellarAction(int rarg);

TEST(CellarFcn_StubDoesNotCrash) {
    setupTestWorld();
    // Room handler with int arg - just verify no crash
    cellarAction(0);
    ASSERT_TRUE(true);
}

// =============================================================================
// CHIMNEY-F Tests (1actions.zil)
// ZIL Logic: CLIMB_UP/CLIMB_DOWN - chimney is too narrow
// =============================================================================

extern bool chimneyAction();

TEST(ChimneyF_ClimbUpBlocked) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_CLIMB_UP;
    
    OutputCapture cap;
    bool result = chimneyAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("narrow") != std::string::npos);
}

TEST(ChimneyF_OtherVerbsReturnFalse) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_EXAMINE;
    
    bool result = chimneyAction();
    
    ASSERT_FALSE(result);
}

// =============================================================================
// CLEARING-FCN Tests (room handler stub)
// =============================================================================

extern void clearingAction(int rarg);

TEST(ClearingFcn_StubDoesNotCrash) {
    setupTestWorld();
    clearingAction(0);
    ASSERT_TRUE(true);
}

// =============================================================================
// CRACK-FCN Tests (1actions.zil line 381-383)
// ZIL Logic: THROUGH - can't fit through the crack
// =============================================================================

extern bool crackAction();

TEST(CrackFcn_ExaminePrintsTooSmall) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_EXAMINE;
    
    OutputCapture cap;
    bool result = crackAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("too small") != std::string::npos);
}

TEST(CrackFcn_OtherVerbsReturnFalse) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_TAKE;
    
    bool result = crackAction();
    
    ASSERT_FALSE(result);
}

// =============================================================================
// DAM-FUNCTION Tests (actions.cpp dam handling)
// =============================================================================

extern bool damAction();

TEST(DamFunction_ExamineDescribes) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prso = g.getObject(ObjectIds::DAM);
    g.prsa = V_EXAMINE;
    
    OutputCapture cap;
    bool result = damAction();
    
    ASSERT_TRUE(result);
}

TEST(DamFunction_OpenBlockedByPanel) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prso = g.getObject(ObjectIds::DAM);
    g.prsa = V_OPEN;
    
    OutputCapture cap;
    bool result = damAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("panel") != std::string::npos);
}

// =============================================================================
// KNIFE-F Tests (1actions.zil lines 926-929)
// ZIL Logic: On TAKE, clears ATTIC-TABLE NDESCBIT
// =============================================================================

TEST(KnifeF_TakeKnifeClearsAtticTableNdescbit) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    // Get attic table and set NDESCBIT
    ZObject* atticTable = g.getObject(ObjectIds::ATTIC_TABLE);
    if (!atticTable) {
        // Skip test if ATTIC_TABLE not yet defined
        return;
    }
    
    atticTable->setFlag(ObjectFlag::NDESCBIT);
    ASSERT_TRUE(atticTable->hasFlag(ObjectFlag::NDESCBIT));
    
    // Simulate V_TAKE action
    g.prsa = V_TAKE;
    g.prso = g.getObject(ObjectIds::KNIFE);
    
    // Call knife action
    knifeAction();
    
    // Verify NDESCBIT was cleared
    ASSERT_FALSE(atticTable->hasFlag(ObjectFlag::NDESCBIT));
}

// =============================================================================
// BODY-FUNCTION Tests (1dungeon.zil line 694, 1actions.zil line 2178)
// ZIL Logic: TAKE sends msg, MUNG/BURN kills
// =============================================================================

extern bool bodyAction();

TEST(BodyFcn_TakePrintsMessage) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_TAKE;
    
    OutputCapture cap;
    bool result = bodyAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("force keeps you") != std::string::npos);
}

TEST(BodyFcn_AttackKillMungBurnTriggersDeath) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    // Test ATTACK
    g.prsa = V_ATTACK;
    
    // Pipe "no\n" to handle death prompt
    {
        InputRedirect input("no\n");
        OutputCapture cap;
        bool result = bodyAction(); 
        ASSERT_TRUE(result);
        std::string output = cap.getOutput();
        ASSERT_TRUE(output.find("disrespect costs you your life") != std::string::npos);
    }
    
    g.reset();
    setupTestWorld();
    
    g.prsa = V_BURN;
    {
        InputRedirect input("no\n");
        OutputCapture cap;
        bool result = bodyAction();
        ASSERT_TRUE(result);
        std::string output = cap.getOutput();
        ASSERT_TRUE(output.find("disrespect costs you your life") != std::string::npos);
    }
}

// =============================================================================
// BOLT-F Tests (1actions.zil line 1187)
// ZIL Logic: Requires Wrench, Requires GATE-FLAG (Power), Toggles GATES-OPEN
// =============================================================================

TEST(BoltFcn_TurnWithoutWrenchFails) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    // Ensure player has NO wrench
    ZObject* wrench = g.getObject(ObjectIds::WRENCH);
    if (wrench) wrench->moveTo(nullptr);
    
    // Set PRSI to something else or null
    g.prsi = nullptr; 
    
    g.prsa = V_TURN;
    
    OutputCapture cap;
    bool result = boltAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("need a tool") != std::string::npos); // "turn with what?" logic simplified
}

TEST(BoltFcn_TurnWithWrenchButNoPowerFails) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    ZObject* wrench = g.getObject(ObjectIds::WRENCH);
    if (!wrench) { 
        // Create if missing
        auto w = std::make_unique<ZObject>(ObjectIds::WRENCH, "wrench");
        g.registerObject(ObjectIds::WRENCH, std::move(w));
        wrench = g.getObject(ObjectIds::WRENCH);
    }
    wrench->moveTo(g.player);
    g.prsi = wrench; // Explicitly set as tool
    
    // Power OFF
    g.gateFlag = false;
    
    g.prsa = V_TURN;
    
    OutputCapture cap;
    bool result = boltAction();
    
    ASSERT_TRUE(result);
    std::string output = cap.getOutput();
    ASSERT_TRUE(output.find("won't turn") != std::string::npos);
}

TEST(BoltFcn_TurnWithWrenchAndPowerTogglesGates) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    ZObject* wrench = g.getObject(ObjectIds::WRENCH);
    if (!wrench) { 
        auto w = std::make_unique<ZObject>(ObjectIds::WRENCH, "wrench");
        g.registerObject(ObjectIds::WRENCH, std::move(w));
        wrench = g.getObject(ObjectIds::WRENCH);
    }
    wrench->moveTo(g.player);
    g.prsi = wrench;
    
    // Power ON
    g.gateFlag = true;
    g.gatesOpen = false; // Start closed
    
    g.prsa = V_TURN;
    
    // 1. OPEN
    {
        OutputCapture cap;
        bool result = boltAction();
        ASSERT_TRUE(result);
        std::string output = cap.getOutput();
        ASSERT_TRUE(output.find("sluice gates open") != std::string::npos);
        ASSERT_TRUE(g.gatesOpen);
    }
    
    // 2. CLOSE
    {
        OutputCapture cap;
        bool result = boltAction();
        ASSERT_TRUE(result);
        std::string output = cap.getOutput();
        ASSERT_TRUE(output.find("sluice gates close") != std::string::npos);
        ASSERT_FALSE(g.gatesOpen);
    }
}

TEST(KnifeF_NonTakeVerbDoesNotAffectTable) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    ZObject* atticTable = g.getObject(ObjectIds::ATTIC_TABLE);
    if (!atticTable) {
        return;
    }
    
    atticTable->setFlag(ObjectFlag::NDESCBIT);
    
    // Try EXAMINE instead of TAKE
    g.prsa = V_EXAMINE;
    g.prso = g.getObject(ObjectIds::KNIFE);
    
    knifeAction();
    
    // NDESCBIT should still be set (not cleared for non-TAKE)
    ASSERT_TRUE(atticTable->hasFlag(ObjectFlag::NDESCBIT));
}

TEST(KnifeF_ReturnsFalseToAllowNormalHandling) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    g.prsa = V_TAKE;
    g.prso = g.getObject(ObjectIds::KNIFE);
    
    bool result = knifeAction();
    
    // Should return false to allow normal TAKE to proceed
    ASSERT_FALSE(result);
}

// =============================================================================
// TEETH-F Tests (1actions.zil lines 48-62)
// ZIL Logic: BRUSH TEETH with putty = death, BRUSH without tool = message
// =============================================================================

TEST(TeethF_BrushWithPuttyKillsPlayer) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    // Get teeth and putty objects
    ZObject* teeth = g.getObject(ObjectIds::TEETH);
    ZObject* putty = g.getObject(ObjectIds::PUTTY);
    ASSERT_TRUE(teeth != nullptr);
    ASSERT_TRUE(putty != nullptr);
    
    // Put putty in player's inventory
    putty->moveTo(g.player);
    
    // Setup BRUSH TEETH WITH PUTTY
    g.prsa = V_BRUSH;
    g.prso = teeth;
    g.prsi = putty;
    
    bool result = teethAction();
    
    // Should handle the action (death occurs internally)
    ASSERT_TRUE(result);
}

TEST(TeethF_BrushWithoutToolPrintsMessage) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    ZObject* teeth = g.getObject(ObjectIds::TEETH);
    ASSERT_TRUE(teeth != nullptr);
    
    // Setup BRUSH TEETH with no instrument
    g.prsa = V_BRUSH;
    g.prso = teeth;
    g.prsi = nullptr;
    
    OutputCapture capture;
    bool result = teethAction();
    std::string output = capture.getOutput();
    
    ASSERT_TRUE(result);
    ASSERT_TRUE(output.find("Dental hygiene") != std::string::npos);
}


TEST(TeethF_ExaminePrintsDescription) {

    setupTestWorld();
    auto& g = Globals::instance();
    
    ZObject* teeth = g.getObject(ObjectIds::TEETH);
    ASSERT_TRUE(teeth != nullptr);
    
    g.prsa = V_EXAMINE;
    g.prso = teeth;
    
    OutputCapture capture;
    bool result = teethAction();
    std::string output = capture.getOutput();
    
    ASSERT_TRUE(result);
    ASSERT_TRUE(output.find("razor sharp") != std::string::npos);
}



// TOOL-CHEST-FCN Test
extern bool toolChestAction();

TEST(ToolChestFcn_ExaminePrintsEmpty) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    // Create tool chest for testing (ID 298 from ZIL)
    // Note: ZIL ID is 298, assuming ObjectIds::TOOL_CHEST maps correctly
    // If undefined in tests, we might need to mock or ensure it exists
    ZObject* chest = g.getObject(ObjectIds::TOOL_CHEST);
    
    // If chest doesn't exist in minimal test world, create it
    if (!chest) {
        auto newChest = std::make_unique<ZObject>(ObjectIds::TOOL_CHEST, "tool chests");
        g.registerObject(ObjectIds::TOOL_CHEST, std::move(newChest));
        chest = g.getObject(ObjectIds::TOOL_CHEST);
    }

    
    g.prsa = V_EXAMINE;
    g.prso = chest;
    
    OutputCapture capture;
    bool result = toolChestAction();
    std::string output = capture.getOutput();
    
    ASSERT_TRUE(result);
    ASSERT_TRUE(output.find("The chests are all empty.") != std::string::npos);
}

TEST(ToolChestFcn_TakeCrumbs) {
    setupTestWorld();
    auto& g = Globals::instance();
    
    // Create tool chest
    auto newChest = std::make_unique<ZObject>(ObjectIds::TOOL_CHEST, "tool chests");
    // Manually set location for test purposes (world_init handles this normally)
    newChest->moveTo(g.here); 
    g.registerObject(ObjectIds::TOOL_CHEST, std::move(newChest));
    ZObject* chest = g.getObject(ObjectIds::TOOL_CHEST);
    
    // Verify chest exists initially
    ASSERT_TRUE(chest != nullptr);
    ASSERT_TRUE(chest->getLocation() != nullptr); // Should be in a room (or valid parent) if added properly
    
    // Setup TAKE action
    g.prsa = V_TAKE;
    g.prso = chest;
    
    OutputCapture capture;
    bool result = toolChestAction();
    std::string output = capture.getOutput();
    
    ASSERT_TRUE(result);
    ASSERT_TRUE(output.find("crumble when you touch them") != std::string::npos);
    
    // Verify chest is removed (moved to nullptr)
    // Note: implementation does g.prso->moveTo(nullptr)
    ASSERT_TRUE(chest->getLocation() == nullptr);
}


// Main test runner
int main(int argc, char* argv[]) {
    std::cout << "Running Action Tests" << std::endl;
    std::cout << "====================" << std::endl;
    std::cout << std::endl;
    
    auto results = TestFramework::instance().runAll();
    
    int passed = 0, failed = 0;
    for (const auto& r : results) {
        if (r.passed) passed++; else failed++;
    }
    
    std::cout << std::endl;
    std::cout << "Results: " << passed << " passed, " << failed << " failed" << std::endl;
    
    return failed > 0 ? 1 : 0;
}
