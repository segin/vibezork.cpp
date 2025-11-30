// Missing objects from ZIL 1dungeon.zil
// This file adds objects that were not yet implemented in world.cpp

#include "world.h"
#include "objects.h"
#include "rooms.h"
#include "core/globals.h"
#include "core/io.h"
#include <memory>

// Forward declaration
void fixObjectValues();

void initializeMissingObjects() {
    auto& g = Globals::instance();
    
    // ===== MAZE-5 OBJECTS (Skeleton area) =====
    
    // SKULL (Crystal skull) - treasure in Land of Living Dead
    auto skull = std::make_unique<ZObject>(ObjectIds::SKULL, "crystal skull");
    skull->addSynonym("skull");
    skull->addSynonym("head");
    skull->addSynonym("treasure");
    skull->addAdjective("crystal");
    skull->setFlag(ObjectFlag::TAKEBIT);
    skull->setProperty(P_VALUE, 10);
    skull->setProperty(P_TVALUE, 10);
    skull->setText("Lying in one corner of the room is a beautifully carved crystal skull. It appears to be grinning at you rather nastily.");
    skull->moveTo(g.getObject(RoomIds::LAND_OF_LIVING_DEAD));
    g.registerObject(ObjectIds::SKULL, std::move(skull));
    
    // BONES (Skeleton) - in Maze-5
    auto bones = std::make_unique<ZObject>(ObjectIds::BONES, "skeleton");
    bones->addSynonym("bones");
    bones->addSynonym("skeleton");
    bones->addSynonym("body");
    bones->setFlag(ObjectFlag::TRYTAKEBIT);
    bones->setFlag(ObjectFlag::NDESCBIT);
    bones->moveTo(g.getObject(RoomIds::MAZE_5));
    g.registerObject(ObjectIds::BONES, std::move(bones));
    
    // BURNED_OUT_LANTERN - in Maze-5
    auto burnedLantern = std::make_unique<ZObject>(ObjectIds::BURNED_OUT_LANTERN, "burned-out lantern");
    burnedLantern->addSynonym("lantern");
    burnedLantern->addSynonym("lamp");
    burnedLantern->addAdjective("rusty");
    burnedLantern->addAdjective("burned");
    burnedLantern->addAdjective("dead");
    burnedLantern->addAdjective("useless");
    burnedLantern->setFlag(ObjectFlag::TAKEBIT);
    burnedLantern->setProperty(P_SIZE, 20);
    burnedLantern->setText("The deceased adventurer's useless lantern is here.");
    burnedLantern->moveTo(g.getObject(RoomIds::MAZE_5));
    g.registerObject(ObjectIds::BURNED_OUT_LANTERN, std::move(burnedLantern));

    // RUSTY_KNIFE - in Maze-5
    auto rustyKnife = std::make_unique<ZObject>(ObjectIds::RUSTY_KNIFE, "rusty knife");
    rustyKnife->addSynonym("knife");
    rustyKnife->addSynonym("knives");
    rustyKnife->addAdjective("rusty");
    rustyKnife->setFlag(ObjectFlag::TAKEBIT);
    rustyKnife->setFlag(ObjectFlag::TRYTAKEBIT);
    rustyKnife->setFlag(ObjectFlag::WEAPONBIT);
    rustyKnife->setFlag(ObjectFlag::TOOLBIT);
    rustyKnife->setProperty(P_SIZE, 20);
    rustyKnife->setText("Beside the skeleton is a rusty knife.");
    rustyKnife->moveTo(g.getObject(RoomIds::MAZE_5));
    g.registerObject(ObjectIds::RUSTY_KNIFE, std::move(rustyKnife));
    
    // KEYS (Skeleton key) - in Maze-5
    auto keys = std::make_unique<ZObject>(ObjectIds::KEYS, "skeleton key");
    keys->addSynonym("key");
    keys->addAdjective("skeleton");
    keys->setFlag(ObjectFlag::TAKEBIT);
    keys->setFlag(ObjectFlag::TOOLBIT);
    keys->setProperty(P_SIZE, 10);
    keys->moveTo(g.getObject(RoomIds::MAZE_5));
    g.registerObject(ObjectIds::KEYS, std::move(keys));
    
    // ===== NATURAL SCENERY =====
    
    // LEAVES (Pile of leaves) - in Grating Clearing
    auto leaves = std::make_unique<ZObject>(ObjectIds::LEAVES, "pile of leaves");
    leaves->addSynonym("leaves");
    leaves->addSynonym("leaf");
    leaves->addSynonym("pile");
    leaves->setFlag(ObjectFlag::TAKEBIT);
    leaves->setFlag(ObjectFlag::BURNBIT);
    leaves->setFlag(ObjectFlag::TRYTAKEBIT);
    leaves->setProperty(P_SIZE, 25);
    leaves->setLongDesc("On the ground is a pile of leaves.");
    leaves->moveTo(g.getObject(RoomIds::GRATING_CLEARING));
    g.registerObject(ObjectIds::LEAVES, std::move(leaves));
    
    // COAL (Small pile of coal) - in Dead End 5
    auto coal = std::make_unique<ZObject>(ObjectIds::COAL, "small pile of coal");
    coal->addSynonym("coal");
    coal->addSynonym("pile");
    coal->addSynonym("heap");
    coal->addAdjective("small");
    coal->setFlag(ObjectFlag::TAKEBIT);
    coal->setFlag(ObjectFlag::BURNBIT);
    coal->setProperty(P_SIZE, 20);
    coal->moveTo(g.getObject(RoomIds::DEAD_END_5));
    g.registerObject(ObjectIds::COAL, std::move(coal));
    
    // TIMBERS (Broken timbers) - in Timber Room
    auto timbers = std::make_unique<ZObject>(ObjectIds::TIMBERS, "broken timber");
    timbers->addSynonym("timbers");
    timbers->addSynonym("pile");
    timbers->addAdjective("wooden");
    timbers->addAdjective("broken");
    timbers->setFlag(ObjectFlag::TAKEBIT);
    timbers->setProperty(P_SIZE, 50);
    timbers->moveTo(g.getObject(RoomIds::TIMBER_ROOM));
    g.registerObject(ObjectIds::TIMBERS, std::move(timbers));
    
    // SAND (Sand) - in Sandy Cave
    auto sand = std::make_unique<ZObject>(ObjectIds::SAND, "sand");
    sand->addSynonym("sand");
    sand->setFlag(ObjectFlag::NDESCBIT);
    sand->moveTo(g.getObject(RoomIds::SANDY_CAVE));
    g.registerObject(ObjectIds::SAND, std::move(sand));

    // ===== TEMPLE OBJECTS =====
    
    // PRAYER (Ancient prayer inscription) - in North Temple
    auto prayer = std::make_unique<ZObject>(ObjectIds::PRAYER, "prayer");
    prayer->addSynonym("prayer");
    prayer->addSynonym("inscription");
    prayer->addAdjective("ancient");
    prayer->addAdjective("old");
    prayer->setFlag(ObjectFlag::READBIT);
    prayer->setFlag(ObjectFlag::SACREDBIT);
    prayer->setFlag(ObjectFlag::NDESCBIT);
    prayer->setText(
        "The prayer is inscribed in an ancient script, rarely used today. It seems "
        "to be a philippic against small insects, absent-mindedness, and the picking "
        "up and dropping of small objects. The final verse consigns trespassers to "
        "the land of the dead. All evidence indicates that the beliefs of the ancient "
        "Zorkers were obscure."
    );
    prayer->moveTo(g.getObject(RoomIds::NORTH_TEMPLE));
    g.registerObject(ObjectIds::PRAYER, std::move(prayer));
    
    // ENGRAVINGS (Wall engravings) - in Engravings Cave
    auto engravings = std::make_unique<ZObject>(ObjectIds::ENGRAVINGS, "wall with engravings");
    engravings->addSynonym("wall");
    engravings->addSynonym("engravings");
    engravings->addSynonym("inscription");
    engravings->addAdjective("old");
    engravings->addAdjective("ancient");
    engravings->setFlag(ObjectFlag::READBIT);
    engravings->setFlag(ObjectFlag::SACREDBIT);
    engravings->setLongDesc("There are old engravings on the walls here.");
    engravings->setText(
        "The engravings were incised in the living rock of the cave wall by "
        "an unknown hand. They depict, in symbolic form, the beliefs of the "
        "ancient Zorkers. Skillfully interwoven with the bas reliefs are excerpts "
        "illustrating the major religious tenets of that time. Unfortunately, a "
        "later age seems to have considered them blasphemous and just as skillfully "
        "excised them."
    );
    engravings->moveTo(g.getObject(RoomIds::ENGRAVINGS_CAVE));
    g.registerObject(ObjectIds::ENGRAVINGS, std::move(engravings));
    
    // ===== DOORS =====
    
    // TRAP_DOOR - in Living Room
    auto trapDoor = std::make_unique<ZObject>(ObjectIds::TRAP_DOOR, "trap door");
    trapDoor->addSynonym("door");
    trapDoor->addSynonym("trapdoor");
    trapDoor->addSynonym("trap-door");
    trapDoor->addSynonym("cover");
    trapDoor->addAdjective("trap");
    trapDoor->addAdjective("dusty");
    trapDoor->setFlag(ObjectFlag::DOORBIT);
    trapDoor->setFlag(ObjectFlag::NDESCBIT);
    trapDoor->setFlag(ObjectFlag::INVISIBLE);  // Hidden under rug initially
    trapDoor->moveTo(g.getObject(RoomIds::LIVING_ROOM));
    g.registerObject(ObjectIds::TRAP_DOOR, std::move(trapDoor));
    
    // WOODEN_DOOR - in Living Room (west door)
    auto woodenDoor = std::make_unique<ZObject>(ObjectIds::WOODEN_DOOR, "wooden door");
    woodenDoor->addSynonym("door");
    woodenDoor->addSynonym("lettering");
    woodenDoor->addSynonym("writing");
    woodenDoor->addAdjective("wooden");
    woodenDoor->addAdjective("gothic");
    woodenDoor->addAdjective("strange");
    woodenDoor->addAdjective("west");
    woodenDoor->setFlag(ObjectFlag::READBIT);
    woodenDoor->setFlag(ObjectFlag::DOORBIT);
    woodenDoor->setFlag(ObjectFlag::NDESCBIT);
    woodenDoor->setFlag(ObjectFlag::TRANSBIT);
    woodenDoor->setText("The engravings translate to \"This space intentionally left blank.\"");
    woodenDoor->moveTo(g.getObject(RoomIds::LIVING_ROOM));
    g.registerObject(ObjectIds::WOODEN_DOOR, std::move(woodenDoor));
    
    // BARROW_DOOR - in Stone Barrow
    auto barrowDoor = std::make_unique<ZObject>(ObjectIds::BARROW_DOOR, "stone door");
    barrowDoor->addSynonym("door");
    barrowDoor->addAdjective("huge");
    barrowDoor->addAdjective("stone");
    barrowDoor->setFlag(ObjectFlag::DOORBIT);
    barrowDoor->setFlag(ObjectFlag::NDESCBIT);
    barrowDoor->setFlag(ObjectFlag::OPENBIT);  // Initially open
    barrowDoor->moveTo(g.getObject(RoomIds::STONE_BARROW));
    g.registerObject(ObjectIds::BARROW_DOOR, std::move(barrowDoor));
    
    // GRATE (Grating) - global object for grating clearing/room
    auto grate = std::make_unique<ZObject>(ObjectIds::GRATE, "grating");
    grate->addSynonym("grate");
    grate->addSynonym("grating");
    grate->setFlag(ObjectFlag::DOORBIT);
    grate->setFlag(ObjectFlag::NDESCBIT);
    grate->setFlag(ObjectFlag::INVISIBLE);  // Hidden until leaves are moved
    g.registerObject(ObjectIds::GRATE, std::move(grate));

    // ===== GLOBAL SCENERY OBJECTS =====
    
    // TREE (Large tree) - global for forest areas
    auto tree = std::make_unique<ZObject>(ObjectIds::TREE, "tree");
    tree->addSynonym("tree");
    tree->addSynonym("branch");
    tree->addAdjective("large");
    tree->addAdjective("storm");
    tree->setFlag(ObjectFlag::NDESCBIT);
    tree->setFlag(ObjectFlag::CLIMBBIT);
    g.registerObject(ObjectIds::TREE, std::move(tree));
    
    // SONGBIRD - global for forest areas
    auto songbird = std::make_unique<ZObject>(ObjectIds::SONGBIRD, "songbird");
    songbird->addSynonym("bird");
    songbird->addSynonym("songbird");
    songbird->addAdjective("song");
    songbird->setFlag(ObjectFlag::NDESCBIT);
    g.registerObject(ObjectIds::SONGBIRD, std::move(songbird));
    
    // GLOBAL_WATER - global water object
    auto globalWater = std::make_unique<ZObject>(ObjectIds::GLOBAL_WATER, "water");
    globalWater->addSynonym("water");
    globalWater->addSynonym("quantity");
    globalWater->setFlag(ObjectFlag::DRINKBIT);
    g.registerObject(ObjectIds::GLOBAL_WATER, std::move(globalWater));
    
    // BOARDED_WINDOW - global for house exterior
    auto boardedWindow = std::make_unique<ZObject>(ObjectIds::BOARDED_WINDOW, "boarded window");
    boardedWindow->addSynonym("window");
    boardedWindow->addAdjective("boarded");
    boardedWindow->setFlag(ObjectFlag::NDESCBIT);
    g.registerObject(ObjectIds::BOARDED_WINDOW, std::move(boardedWindow));
    
    // LADDER - global for rooms with ladders
    auto ladder = std::make_unique<ZObject>(ObjectIds::LADDER, "wooden ladder");
    ladder->addSynonym("ladder");
    ladder->addAdjective("wooden");
    ladder->addAdjective("rickety");
    ladder->addAdjective("narrow");
    ladder->setFlag(ObjectFlag::NDESCBIT);
    ladder->setFlag(ObjectFlag::CLIMBBIT);
    g.registerObject(ObjectIds::LADDER, std::move(ladder));
    
    // SLIDE (Chute/slide) - global for slide room
    auto slide = std::make_unique<ZObject>(ObjectIds::SLIDE, "chute");
    slide->addSynonym("chute");
    slide->addSynonym("ramp");
    slide->addSynonym("slide");
    slide->addAdjective("steep");
    slide->addAdjective("metal");
    slide->addAdjective("twisting");
    slide->setFlag(ObjectFlag::CLIMBBIT);
    g.registerObject(ObjectIds::SLIDE, std::move(slide));
    
    // CRACK - global for rooms with cracks
    auto crack = std::make_unique<ZObject>(ObjectIds::CRACK, "crack");
    crack->addSynonym("crack");
    crack->addAdjective("narrow");
    crack->setFlag(ObjectFlag::NDESCBIT);
    g.registerObject(ObjectIds::CRACK, std::move(crack));
    
    // BODIES (Pile of bodies) - global for Hades area
    auto bodies = std::make_unique<ZObject>(ObjectIds::BODIES, "pile of bodies");
    bodies->addSynonym("bodies");
    bodies->addSynonym("body");
    bodies->addSynonym("remains");
    bodies->addSynonym("pile");
    bodies->addAdjective("mangled");
    bodies->setFlag(ObjectFlag::NDESCBIT);
    bodies->setFlag(ObjectFlag::TRYTAKEBIT);
    g.registerObject(ObjectIds::BODIES, std::move(bodies));
    
    // CLIMBABLE_CLIFF - global for canyon areas
    auto climbableCliff = std::make_unique<ZObject>(ObjectIds::CLIMBABLE_CLIFF, "cliff");
    climbableCliff->addSynonym("wall");
    climbableCliff->addSynonym("cliff");
    climbableCliff->addSynonym("walls");
    climbableCliff->addSynonym("ledge");
    climbableCliff->addAdjective("rocky");
    climbableCliff->addAdjective("sheer");
    climbableCliff->setFlag(ObjectFlag::NDESCBIT);
    climbableCliff->setFlag(ObjectFlag::CLIMBBIT);
    g.registerObject(ObjectIds::CLIMBABLE_CLIFF, std::move(climbableCliff));
    
    // WHITE_CLIFF - global for river areas
    auto whiteCliff = std::make_unique<ZObject>(ObjectIds::WHITE_CLIFF, "white cliffs");
    whiteCliff->addSynonym("cliff");
    whiteCliff->addSynonym("cliffs");
    whiteCliff->addAdjective("white");
    whiteCliff->setFlag(ObjectFlag::NDESCBIT);
    whiteCliff->setFlag(ObjectFlag::CLIMBBIT);
    g.registerObject(ObjectIds::WHITE_CLIFF, std::move(whiteCliff));
    
    // CONTROL_PANEL - in Dam Room
    auto controlPanel = std::make_unique<ZObject>(ObjectIds::CONTROL_PANEL, "control panel");
    controlPanel->addSynonym("panel");
    controlPanel->addAdjective("control");
    controlPanel->setFlag(ObjectFlag::NDESCBIT);
    controlPanel->moveTo(g.getObject(RoomIds::DAM_ROOM));
    g.registerObject(ObjectIds::CONTROL_PANEL, std::move(controlPanel));

    // ===== MAINTENANCE ROOM OBJECTS =====
    
    // MACHINE_SWITCH - in Machine Room
    auto machineSwitch = std::make_unique<ZObject>(ObjectIds::MACHINE_SWITCH, "switch");
    machineSwitch->addSynonym("switch");
    machineSwitch->setFlag(ObjectFlag::NDESCBIT);
    machineSwitch->setFlag(ObjectFlag::TURNBIT);
    machineSwitch->moveTo(g.getObject(RoomIds::MACHINE_ROOM));
    g.registerObject(ObjectIds::MACHINE_SWITCH, std::move(machineSwitch));
    
    // LEAK - in Maintenance Room (invisible until dam is opened)
    auto leak = std::make_unique<ZObject>(ObjectIds::LEAK, "leak");
    leak->addSynonym("leak");
    leak->addSynonym("drip");
    leak->addSynonym("pipe");
    leak->setFlag(ObjectFlag::NDESCBIT);
    leak->setFlag(ObjectFlag::INVISIBLE);
    leak->moveTo(g.getObject(RoomIds::MAINTENANCE_ROOM));
    g.registerObject(ObjectIds::LEAK, std::move(leak));
    
    // TUBE (Tube of putty) - in Maintenance Room
    auto tube = std::make_unique<ZObject>(ObjectIds::TUBE, "tube");
    tube->addSynonym("tube");
    tube->addSynonym("tooth");
    tube->addSynonym("paste");
    tube->setFlag(ObjectFlag::TAKEBIT);
    tube->setFlag(ObjectFlag::CONTBIT);
    tube->setFlag(ObjectFlag::READBIT);
    tube->setProperty(P_CAPACITY, 7);
    tube->setProperty(P_SIZE, 5);
    tube->setLongDesc("There is an object which looks like a tube of toothpaste here.");
    tube->setText("---> Frobozz Magic Gunk Company <---\n      All-Purpose Gunk");
    tube->moveTo(g.getObject(RoomIds::MAINTENANCE_ROOM));
    g.registerObject(ObjectIds::TUBE, std::move(tube));
    
    // PUTTY (Viscous material) - inside Tube
    auto putty = std::make_unique<ZObject>(ObjectIds::PUTTY, "viscous material");
    putty->addSynonym("material");
    putty->addSynonym("gunk");
    putty->addAdjective("viscous");
    putty->setFlag(ObjectFlag::TAKEBIT);
    putty->setFlag(ObjectFlag::TOOLBIT);
    putty->setProperty(P_SIZE, 6);
    putty->moveTo(g.getObject(ObjectIds::TUBE));
    g.registerObject(ObjectIds::PUTTY, std::move(putty));
    
    // ===== INTERIOR SCENERY =====
    
    // KITCHEN_TABLE - in Kitchen
    auto kitchenTable = std::make_unique<ZObject>(ObjectIds::KITCHEN_TABLE, "kitchen table");
    kitchenTable->addSynonym("table");
    kitchenTable->addAdjective("kitchen");
    kitchenTable->setFlag(ObjectFlag::NDESCBIT);
    kitchenTable->setFlag(ObjectFlag::CONTBIT);
    kitchenTable->setFlag(ObjectFlag::OPENBIT);
    kitchenTable->setFlag(ObjectFlag::SURFACEBIT);
    kitchenTable->setProperty(P_CAPACITY, 50);
    kitchenTable->moveTo(g.getObject(RoomIds::KITCHEN));
    g.registerObject(ObjectIds::KITCHEN_TABLE, std::move(kitchenTable));
    
    // ATTIC_TABLE - in Attic
    auto atticTable = std::make_unique<ZObject>(ObjectIds::ATTIC_TABLE, "table");
    atticTable->addSynonym("table");
    atticTable->setFlag(ObjectFlag::NDESCBIT);
    atticTable->setFlag(ObjectFlag::CONTBIT);
    atticTable->setFlag(ObjectFlag::OPENBIT);
    atticTable->setFlag(ObjectFlag::SURFACEBIT);
    atticTable->setProperty(P_CAPACITY, 40);
    atticTable->moveTo(g.getObject(RoomIds::ATTIC));
    g.registerObject(ObjectIds::ATTIC_TABLE, std::move(atticTable));
    
    // RAILING - in Dome Room
    auto railing = std::make_unique<ZObject>(ObjectIds::RAILING, "wooden railing");
    railing->addSynonym("railing");
    railing->addSynonym("rail");
    railing->addAdjective("wooden");
    railing->setFlag(ObjectFlag::NDESCBIT);
    railing->moveTo(g.getObject(RoomIds::DOME_ROOM));
    g.registerObject(ObjectIds::RAILING, std::move(railing));
    
    // WALL - global wall object
    auto wall = std::make_unique<ZObject>(ObjectIds::WALL, "surrounding wall");
    wall->addSynonym("wall");
    wall->addSynonym("walls");
    wall->addAdjective("surrounding");
    g.registerObject(ObjectIds::WALL, std::move(wall));
    
    // GRANITE_WALL - global granite wall
    auto graniteWall = std::make_unique<ZObject>(ObjectIds::GRANITE_WALL, "granite wall");
    graniteWall->addSynonym("wall");
    graniteWall->addAdjective("granite");
    g.registerObject(ObjectIds::GRANITE_WALL, std::move(graniteWall));
    
    // TEETH (Overboard teeth) - global object
    auto teeth = std::make_unique<ZObject>(ObjectIds::TEETH, "set of teeth");
    teeth->addSynonym("overboard");
    teeth->addSynonym("teeth");
    teeth->setFlag(ObjectFlag::NDESCBIT);
    g.registerObject(ObjectIds::TEETH, std::move(teeth));

    // ===== SPECIAL OBJECTS =====
    
    // BARROW (Stone barrow) - in Stone Barrow room
    auto barrow = std::make_unique<ZObject>(ObjectIds::BARROW, "stone barrow");
    barrow->addSynonym("barrow");
    barrow->addSynonym("tomb");
    barrow->addAdjective("massive");
    barrow->addAdjective("stone");
    barrow->setFlag(ObjectFlag::NDESCBIT);
    barrow->moveTo(g.getObject(RoomIds::STONE_BARROW));
    g.registerObject(ObjectIds::BARROW, std::move(barrow));
    
    // GHOSTS - in Entrance to Hades
    auto ghosts = std::make_unique<ZObject>(ObjectIds::GHOSTS, "number of ghosts");
    ghosts->addSynonym("ghosts");
    ghosts->addSynonym("spirits");
    ghosts->addSynonym("fiends");
    ghosts->addSynonym("force");
    ghosts->addAdjective("invisible");
    ghosts->addAdjective("evil");
    ghosts->setFlag(ObjectFlag::ACTORBIT);
    ghosts->setFlag(ObjectFlag::NDESCBIT);
    ghosts->moveTo(g.getObject(RoomIds::ENTRANCE_TO_HADES));
    g.registerObject(ObjectIds::GHOSTS, std::move(ghosts));
    
    // BAT - in Bat Room
    auto bat = std::make_unique<ZObject>(ObjectIds::BAT, "bat");
    bat->addSynonym("bat");
    bat->addSynonym("vampire");
    bat->addAdjective("vampire");
    bat->addAdjective("deranged");
    bat->setFlag(ObjectFlag::ACTORBIT);
    bat->setFlag(ObjectFlag::TRYTAKEBIT);
    bat->moveTo(g.getObject(RoomIds::BAT_ROOM));
    g.registerObject(ObjectIds::BAT, std::move(bat));
    
    // BELL - in North Temple
    auto bell = std::make_unique<ZObject>(ObjectIds::BELL, "brass bell");
    bell->addSynonym("bell");
    bell->addAdjective("small");
    bell->addAdjective("brass");
    bell->setFlag(ObjectFlag::TAKEBIT);
    bell->moveTo(g.getObject(RoomIds::NORTH_TEMPLE));
    g.registerObject(ObjectIds::BELL, std::move(bell));
    
    // HOT_BELL - created when bell is heated
    auto hotBell = std::make_unique<ZObject>(ObjectIds::HOT_BELL, "red hot brass bell");
    hotBell->addSynonym("bell");
    hotBell->addAdjective("brass");
    hotBell->addAdjective("hot");
    hotBell->addAdjective("red");
    hotBell->addAdjective("small");
    hotBell->setFlag(ObjectFlag::TRYTAKEBIT);
    hotBell->setLongDesc("On the ground is a red hot bell.");
    // Starts nowhere - created when bell is heated
    g.registerObject(ObjectIds::HOT_BELL, std::move(hotBell));
    
    // BROKEN_LAMP - created when lamp runs out of battery
    auto brokenLamp = std::make_unique<ZObject>(ObjectIds::BROKEN_LAMP, "broken lantern");
    brokenLamp->addSynonym("lamp");
    brokenLamp->addSynonym("lantern");
    brokenLamp->addAdjective("broken");
    brokenLamp->setFlag(ObjectFlag::TAKEBIT);
    // Starts nowhere - created when lamp breaks
    g.registerObject(ObjectIds::BROKEN_LAMP, std::move(brokenLamp));
    
    // ===== FIX OBJECT PLACEMENTS =====
    
    // Move sack to kitchen table (not directly in kitchen)
    ZObject* sack = g.getObject(ObjectIds::SACK);
    ZObject* kTable = g.getObject(ObjectIds::KITCHEN_TABLE);
    if (sack && kTable) {
        sack->moveTo(kTable);
    }
    
    // Move bottle to kitchen table
    ZObject* bottle = g.getObject(ObjectIds::BOTTLE);
    if (bottle && kTable) {
        bottle->moveTo(kTable);
    }
    
    // Move knife to attic table
    ZObject* knife = g.getObject(ObjectIds::KNIFE);
    ZObject* aTable = g.getObject(ObjectIds::ATTIC_TABLE);
    if (knife && aTable) {
        knife->moveTo(aTable);
    }
    
    // Move book to altar
    ZObject* book = g.getObject(ObjectIds::BOOK);
    ZObject* altar = g.getObject(ObjectIds::ALTAR);
    if (book && altar) {
        book->moveTo(altar);
    }
    
    // Move altar to South Temple
    if (altar) {
        altar->moveTo(g.getObject(RoomIds::SOUTH_TEMPLE));
    }
    
    // Move pedestal to Torch Room
    ZObject* pedestal = g.getObject(ObjectIds::PEDESTAL);
    if (pedestal) {
        pedestal->moveTo(g.getObject(RoomIds::TORCH_ROOM));
    }
    
    // Move torch to pedestal
    ZObject* torch = g.getObject(ObjectIds::TORCH);
    if (torch && pedestal) {
        torch->moveTo(pedestal);
    }
    
    // Move tool chest to Maintenance Room
    ZObject* toolChest = g.getObject(ObjectIds::TOOL_CHEST);
    if (toolChest) {
        toolChest->moveTo(g.getObject(RoomIds::MAINTENANCE_ROOM));
    }
    
    // Move guide to Dam Lobby
    ZObject* guide = g.getObject(ObjectIds::GUIDE);
    if (guide) {
        guide->moveTo(g.getObject(RoomIds::DAM_LOBBY));
    }
    
    // Move buoy to River-4
    ZObject* buoy = g.getObject(ObjectIds::BUOY);
    if (buoy) {
        buoy->moveTo(g.getObject(RoomIds::RIVER_4));
    }
    
    // Move emerald inside buoy
    ZObject* emerald = g.getObject(ObjectIds::EMERALD);
    if (emerald && buoy) {
        emerald->moveTo(buoy);
    }
    
    // Move egg to nest (nest is in Up-a-Tree)
    ZObject* egg = g.getObject(ObjectIds::EGG);
    ZObject* nest = g.getObject(ObjectIds::NEST);
    if (egg && nest) {
        egg->moveTo(nest);
    }
    
    // Move jade to Bat Room (per ZIL)
    ZObject* jade = g.getObject(ObjectIds::JADE);
    if (jade) {
        jade->moveTo(g.getObject(RoomIds::BAT_ROOM));
    }
    
    // Move coins to Maze-5 (per ZIL)
    ZObject* coins = g.getObject(ObjectIds::COINS);
    if (coins) {
        coins->moveTo(g.getObject(RoomIds::MAZE_5));
    }
    
    // Move bracelet to Gas Room (per ZIL)
    ZObject* bracelet = g.getObject(ObjectIds::BRACELET);
    if (bracelet) {
        bracelet->moveTo(g.getObject(RoomIds::GAS_ROOM));
    }
    
    // Move inflatable boat to Dam Base (per ZIL)
    ZObject* inflatableBoat = g.getObject(ObjectIds::BOAT_INFLATABLE);
    if (inflatableBoat) {
        inflatableBoat->moveTo(g.getObject(RoomIds::DAM_BASE));
    }
    
    // Fix object values to match ZIL
    fixObjectValues();
}


// Additional function to fix object values per ZIL
void fixObjectValues() {
    auto& g = Globals::instance();
    
    // Fix TRIDENT values (ZIL: VALUE 4, TVALUE 11)
    ZObject* trident = g.getObject(ObjectIds::TRIDENT);
    if (trident) {
        trident->setProperty(P_VALUE, 4);
        trident->setProperty(P_TVALUE, 11);
        trident->setProperty(P_SIZE, 20);
    }
    
    // Fix CHALICE values (ZIL: VALUE 10, TVALUE 5, SIZE 10)
    ZObject* chalice = g.getObject(ObjectIds::CHALICE);
    if (chalice) {
        chalice->setProperty(P_VALUE, 10);
        chalice->setProperty(P_TVALUE, 5);
        chalice->setProperty(P_SIZE, 10);
        chalice->setFlag(ObjectFlag::TRYTAKEBIT);
        chalice->setFlag(ObjectFlag::CONTBIT);
        chalice->setProperty(P_CAPACITY, 5);
    }
    
    // Fix COFFIN values (ZIL: VALUE 10, TVALUE 15, SIZE 55, CAPACITY 35)
    ZObject* coffin = g.getObject(ObjectIds::COFFIN);
    if (coffin) {
        coffin->setProperty(P_VALUE, 10);
        coffin->setProperty(P_TVALUE, 15);
        coffin->setProperty(P_SIZE, 55);
        coffin->setProperty(P_CAPACITY, 35);
    }
    
    // Fix TORCH values (ZIL: VALUE 14, TVALUE 6, SIZE 20)
    ZObject* torch = g.getObject(ObjectIds::TORCH);
    if (torch) {
        torch->setProperty(P_VALUE, 14);
        torch->setProperty(P_TVALUE, 6);
        torch->setProperty(P_SIZE, 20);
        torch->setFlag(ObjectFlag::FLAMEBIT);
    }
    
    // Fix SCEPTRE values (ZIL: VALUE 4, TVALUE 6, SIZE 3)
    ZObject* sceptre = g.getObject(ObjectIds::SCEPTRE);
    if (sceptre) {
        sceptre->setProperty(P_VALUE, 4);
        sceptre->setProperty(P_TVALUE, 6);
        sceptre->setProperty(P_SIZE, 3);
    }
    
    // Fix BAR values (ZIL: VALUE 10, TVALUE 5, SIZE 20, SACREDBIT)
    ZObject* bar = g.getObject(ObjectIds::BAR);
    if (bar) {
        bar->setProperty(P_VALUE, 10);
        bar->setProperty(P_TVALUE, 5);
        bar->setProperty(P_SIZE, 20);
        bar->setFlag(ObjectFlag::SACREDBIT);
    }
    
    // Fix POT_OF_GOLD (ZIL: VALUE 10, TVALUE 10, SIZE 15, INVISIBLE initially)
    ZObject* pot = g.getObject(ObjectIds::POT_OF_GOLD);
    if (pot) {
        pot->setProperty(P_VALUE, 10);
        pot->setProperty(P_TVALUE, 10);
        pot->setProperty(P_SIZE, 15);
        pot->setFlag(ObjectFlag::INVISIBLE);  // Hidden until rainbow is solid
    }
    
    // Fix SCARAB (ZIL: VALUE 5, TVALUE 5, SIZE 8, INVISIBLE initially)
    ZObject* scarab = g.getObject(ObjectIds::SCARAB);
    if (scarab) {
        scarab->setProperty(P_VALUE, 5);
        scarab->setProperty(P_TVALUE, 5);
        scarab->setProperty(P_SIZE, 8);
        scarab->setFlag(ObjectFlag::INVISIBLE);  // Hidden in sand
    }
    
    // Fix JEWELS/TRUNK (ZIL: VALUE 15, TVALUE 5, SIZE 35, INVISIBLE initially)
    ZObject* jewels = g.getObject(ObjectIds::JEWELS);
    if (jewels) {
        jewels->setProperty(P_VALUE, 15);
        jewels->setProperty(P_TVALUE, 5);
        jewels->setProperty(P_SIZE, 35);
        jewels->setFlag(ObjectFlag::INVISIBLE);  // Hidden in reservoir
        // Move to RESERVOIR room
        jewels->moveTo(g.getObject(RoomIds::RESERVOIR));
    }
    
    // Fix COINS (ZIL: VALUE 10, TVALUE 5, SIZE 15)
    ZObject* coins = g.getObject(ObjectIds::COINS);
    if (coins) {
        coins->setProperty(P_VALUE, 10);
        coins->setProperty(P_TVALUE, 5);
        coins->setProperty(P_SIZE, 15);
    }
    
    // Fix DIAMOND (ZIL: VALUE 10, TVALUE 10, no SIZE specified)
    ZObject* diamond = g.getObject(ObjectIds::DIAMOND);
    if (diamond) {
        diamond->setProperty(P_VALUE, 10);
        diamond->setProperty(P_TVALUE, 10);
        // Diamond starts nowhere - appears when thief is killed
        diamond->moveTo(nullptr);
    }
    
    // Fix EMERALD (ZIL: VALUE 5, TVALUE 10)
    ZObject* emerald = g.getObject(ObjectIds::EMERALD);
    if (emerald) {
        emerald->setProperty(P_VALUE, 5);
        emerald->setProperty(P_TVALUE, 10);
    }
    
    // Fix PAINTING (ZIL: VALUE 4, TVALUE 6, SIZE 15)
    ZObject* painting = g.getObject(ObjectIds::PAINTING);
    if (painting) {
        painting->setProperty(P_VALUE, 4);
        painting->setProperty(P_TVALUE, 6);
        painting->setProperty(P_SIZE, 15);
    }
    
    // Fix JADE (ZIL: VALUE 5, TVALUE 5, SIZE 10)
    ZObject* jade = g.getObject(ObjectIds::JADE);
    if (jade) {
        jade->setProperty(P_VALUE, 5);
        jade->setProperty(P_TVALUE, 5);
        jade->setProperty(P_SIZE, 10);
    }
    
    // Fix BRACELET (ZIL: VALUE 5, TVALUE 5, SIZE 10)
    ZObject* bracelet = g.getObject(ObjectIds::BRACELET);
    if (bracelet) {
        bracelet->setProperty(P_VALUE, 5);
        bracelet->setProperty(P_TVALUE, 5);
        bracelet->setProperty(P_SIZE, 10);
    }
    
    // Fix LAMP (ZIL: SIZE 15, starts OFF)
    ZObject* lamp = g.getObject(ObjectIds::LAMP);
    if (lamp) {
        lamp->setProperty(P_SIZE, 15);
        lamp->clearFlag(ObjectFlag::ONBIT);  // Starts off
    }
    
    // Fix SWORD (ZIL: SIZE 30, TVALUE 0 - not a treasure)
    ZObject* sword = g.getObject(ObjectIds::SWORD);
    if (sword) {
        sword->setProperty(P_SIZE, 30);
        sword->setProperty(P_TVALUE, 0);
        sword->setFlag(ObjectFlag::TRYTAKEBIT);
    }
    
    // Fix ROPE (ZIL: SIZE 10, SACREDBIT, TRYTAKEBIT)
    ZObject* rope = g.getObject(ObjectIds::ROPE);
    if (rope) {
        rope->setProperty(P_SIZE, 10);
        rope->setFlag(ObjectFlag::SACREDBIT);
        rope->setFlag(ObjectFlag::TRYTAKEBIT);
    }
    
    // Fix AXE (ZIL: SIZE 25, NDESCBIT, starts with troll)
    ZObject* axe = g.getObject(ObjectIds::AXE);
    if (axe) {
        axe->setProperty(P_SIZE, 25);
        axe->setFlag(ObjectFlag::NDESCBIT);
        // Move to troll
        ZObject* troll = g.getObject(ObjectIds::TROLL);
        if (troll) {
            axe->moveTo(troll);
        }
    }
    
    // Fix CANDLES (ZIL: SIZE 10, starts ON with FLAMEBIT)
    ZObject* candles = g.getObject(ObjectIds::CANDLES);
    if (candles) {
        candles->setProperty(P_SIZE, 10);
        candles->setFlag(ObjectFlag::ONBIT);
        candles->setFlag(ObjectFlag::FLAMEBIT);
    }
    
    // Fix CYCLOPS strength (ZIL: STRENGTH 10000 - essentially invincible in combat)
    ZObject* cyclops = g.getObject(ObjectIds::CYCLOPS);
    if (cyclops) {
        cyclops->setProperty(P_STRENGTH, 10000);
    }
    
    // Fix TROLL strength (ZIL: STRENGTH 2)
    ZObject* troll = g.getObject(ObjectIds::TROLL);
    if (troll) {
        troll->setProperty(P_STRENGTH, 2);
        troll->setFlag(ObjectFlag::OPENBIT);  // Can receive items
    }
    
    // Fix THIEF strength (ZIL: STRENGTH 5)
    ZObject* thief = g.getObject(ObjectIds::THIEF);
    if (thief) {
        thief->setProperty(P_STRENGTH, 5);
        thief->setFlag(ObjectFlag::INVISIBLE);  // Starts invisible
        thief->setFlag(ObjectFlag::CONTBIT);
        thief->setFlag(ObjectFlag::OPENBIT);
        // Move thief to Round Room (per ZIL)
        thief->moveTo(g.getObject(RoomIds::ROUND_ROOM));
    }
    
    // Fix MAP (ZIL: INVISIBLE, in trophy case)
    ZObject* map = g.getObject(ObjectIds::MAP);
    if (map) {
        map->setFlag(ObjectFlag::INVISIBLE);
        ZObject* trophyCase = g.getObject(ObjectIds::TROPHY_CASE);
        if (trophyCase) {
            map->moveTo(trophyCase);
        }
    }
}
