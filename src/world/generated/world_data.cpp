// GENERATED FILE - DO NOT EDIT.
// Produced by scratchpad/worldgen/emit_cpp.py from zil/1dungeon.zil, zil/gglobals.zil.
// Strings are verbatim ZIL text after ZILCH conversion (| -> newline, line
// break -> space, line break after | dropped); vocabulary words as written.

#include "world_data.h"

namespace zork::zil {
namespace {

constexpr std::string_view kFl_WEST_OF_HOUSE[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_WEST_OF_HOUSE[] = {"WHITE-HOUSE", "BOARD", "FOREST"};
constexpr ExitDef kEx_WEST_OF_HOUSE[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "NORTH-OF-HOUSE", .line = 1242}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "SOUTH-OF-HOUSE", .line = 1243}, {.dir = "NE", .kind = ExitKind::UEXIT, .to = "NORTH-OF-HOUSE", .line = 1244}, {.dir = "SE", .kind = ExitKind::UEXIT, .to = "SOUTH-OF-HOUSE", .line = 1245}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "FOREST-1", .line = 1246}, {.dir = "EAST", .kind = ExitKind::NEXIT, .text = "The door is boarded and you can't remove the boards.", .line = 1247}, {.dir = "SW", .kind = ExitKind::CEXIT, .to = "STONE-BARROW", .flag = "WON-FLAG", .line = 1248}, {.dir = "IN", .kind = ExitKind::CEXIT, .to = "STONE-BARROW", .flag = "WON-FLAG", .line = 1249}};
constexpr std::string_view kFl_STONE_BARROW[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr ExitDef kEx_STONE_BARROW[] = {{.dir = "NE", .kind = ExitKind::UEXIT, .to = "WEST-OF-HOUSE", .line = 1260}};
constexpr std::string_view kFl_NORTH_OF_HOUSE[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_NORTH_OF_HOUSE[] = {"BOARDED-WINDOW", "BOARD", "WHITE-HOUSE", "FOREST"};
constexpr ExitDef kEx_NORTH_OF_HOUSE[] = {{.dir = "SW", .kind = ExitKind::UEXIT, .to = "WEST-OF-HOUSE", .line = 1271}, {.dir = "SE", .kind = ExitKind::UEXIT, .to = "EAST-OF-HOUSE", .line = 1272}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "WEST-OF-HOUSE", .line = 1273}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "EAST-OF-HOUSE", .line = 1274}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "PATH", .line = 1275}, {.dir = "SOUTH", .kind = ExitKind::NEXIT, .text = "The windows are all boarded.", .line = 1276}};
constexpr std::string_view kFl_SOUTH_OF_HOUSE[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_SOUTH_OF_HOUSE[] = {"BOARDED-WINDOW", "BOARD", "WHITE-HOUSE", "FOREST"};
constexpr ExitDef kEx_SOUTH_OF_HOUSE[] = {{.dir = "WEST", .kind = ExitKind::UEXIT, .to = "WEST-OF-HOUSE", .line = 1286}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "EAST-OF-HOUSE", .line = 1287}, {.dir = "NE", .kind = ExitKind::UEXIT, .to = "EAST-OF-HOUSE", .line = 1288}, {.dir = "NW", .kind = ExitKind::UEXIT, .to = "WEST-OF-HOUSE", .line = 1289}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "FOREST-3", .line = 1290}, {.dir = "NORTH", .kind = ExitKind::NEXIT, .text = "The windows are all boarded.", .line = 1291}};
constexpr std::string_view kFl_EAST_OF_HOUSE[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_EAST_OF_HOUSE[] = {"WHITE-HOUSE", "KITCHEN-WINDOW", "FOREST"};
constexpr ExitDef kEx_EAST_OF_HOUSE[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "NORTH-OF-HOUSE", .line = 1298}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "SOUTH-OF-HOUSE", .line = 1299}, {.dir = "SW", .kind = ExitKind::UEXIT, .to = "SOUTH-OF-HOUSE", .line = 1300}, {.dir = "NW", .kind = ExitKind::UEXIT, .to = "NORTH-OF-HOUSE", .line = 1301}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "CLEARING", .line = 1302}, {.dir = "WEST", .kind = ExitKind::DEXIT, .to = "KITCHEN", .door = "KITCHEN-WINDOW", .line = 1303}, {.dir = "IN", .kind = ExitKind::DEXIT, .to = "KITCHEN", .door = "KITCHEN-WINDOW", .line = 1304}};
constexpr std::string_view kFl_FOREST_1[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_FOREST_1[] = {"TREE", "SONGBIRD", "WHITE-HOUSE", "FOREST"};
constexpr ExitDef kEx_FOREST_1[] = {{.dir = "UP", .kind = ExitKind::NEXIT, .text = "There is no tree here suitable for climbing.", .line = 1315}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "GRATING-CLEARING", .line = 1316}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "PATH", .line = 1317}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "FOREST-3", .line = 1318}, {.dir = "WEST", .kind = ExitKind::NEXIT, .text = "You would need a machete to go further west.", .line = 1319}};
constexpr std::string_view kFl_FOREST_2[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_FOREST_2[] = {"TREE", "SONGBIRD", "WHITE-HOUSE", "FOREST"};
constexpr ExitDef kEx_FOREST_2[] = {{.dir = "UP", .kind = ExitKind::NEXIT, .text = "There is no tree here suitable for climbing.", .line = 1328}, {.dir = "NORTH", .kind = ExitKind::NEXIT, .text = "The forest becomes impenetrable to the north.", .line = 1329}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "MOUNTAINS", .line = 1330}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "CLEARING", .line = 1331}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "PATH", .line = 1332}};
constexpr std::string_view kFl_MOUNTAINS[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_MOUNTAINS[] = {"TREE", "WHITE-HOUSE"};
constexpr ExitDef kEx_MOUNTAINS[] = {{.dir = "UP", .kind = ExitKind::NEXIT, .text = "The mountains are impassable.", .line = 1341}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "FOREST-2", .line = 1342}, {.dir = "EAST", .kind = ExitKind::NEXIT, .text = "The mountains are impassable.", .line = 1343}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "FOREST-2", .line = 1344}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "FOREST-2", .line = 1345}};
constexpr std::string_view kFl_FOREST_3[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_FOREST_3[] = {"TREE", "SONGBIRD", "WHITE-HOUSE", "FOREST"};
constexpr ExitDef kEx_FOREST_3[] = {{.dir = "UP", .kind = ExitKind::NEXIT, .text = "There is no tree here suitable for climbing.", .line = 1353}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "CLEARING", .line = 1354}, {.dir = "EAST", .kind = ExitKind::NEXIT, .text = "The rank undergrowth prevents eastward movement.", .line = 1355}, {.dir = "SOUTH", .kind = ExitKind::NEXIT, .text = "Storm-tossed trees block your way.", .line = 1356}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "FOREST-1", .line = 1357}, {.dir = "NW", .kind = ExitKind::UEXIT, .to = "SOUTH-OF-HOUSE", .line = 1358}};
constexpr std::string_view kFl_PATH[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_PATH[] = {"TREE", "SONGBIRD", "WHITE-HOUSE", "FOREST"};
constexpr ExitDef kEx_PATH[] = {{.dir = "UP", .kind = ExitKind::UEXIT, .to = "UP-A-TREE", .line = 1370}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "GRATING-CLEARING", .line = 1371}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "FOREST-2", .line = 1372}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "NORTH-OF-HOUSE", .line = 1373}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "FOREST-1", .line = 1374}};
constexpr std::string_view kFl_UP_A_TREE[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_UP_A_TREE[] = {"TREE", "FOREST", "SONGBIRD", "WHITE-HOUSE"};
constexpr ExitDef kEx_UP_A_TREE[] = {{.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "PATH", .line = 1382}, {.dir = "UP", .kind = ExitKind::NEXIT, .text = "You cannot climb any higher.", .line = 1383}};
constexpr std::string_view kFl_GRATING_CLEARING[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_GRATING_CLEARING[] = {"WHITE-HOUSE", "GRATE"};
constexpr ExitDef kEx_GRATING_CLEARING[] = {{.dir = "NORTH", .kind = ExitKind::NEXIT, .text = "The forest becomes impenetrable to the north.", .line = 1391}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "FOREST-2", .line = 1392}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "FOREST-1", .line = 1393}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "PATH", .line = 1394}, {.dir = "DOWN", .kind = ExitKind::FEXIT, .routine = "GRATING-EXIT", .line = 1395}};
constexpr std::string_view kFl_CLEARING[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_CLEARING[] = {"TREE", "SONGBIRD", "WHITE-HOUSE", "FOREST"};
constexpr ExitDef kEx_CLEARING[] = {{.dir = "UP", .kind = ExitKind::NEXIT, .text = "There is no tree here suitable for climbing.", .line = 1416}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "CANYON-VIEW", .line = 1417}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "FOREST-2", .line = 1418}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "FOREST-3", .line = 1419}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "EAST-OF-HOUSE", .line = 1420}};
constexpr std::string_view kFl_KITCHEN[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_KITCHEN[] = {"KITCHEN-WINDOW", "CHIMNEY", "STAIRS"};
constexpr ExitDef kEx_KITCHEN[] = {{.dir = "EAST", .kind = ExitKind::DEXIT, .to = "EAST-OF-HOUSE", .door = "KITCHEN-WINDOW", .line = 1432}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "LIVING-ROOM", .line = 1433}, {.dir = "OUT", .kind = ExitKind::DEXIT, .to = "EAST-OF-HOUSE", .door = "KITCHEN-WINDOW", .line = 1434}, {.dir = "UP", .kind = ExitKind::UEXIT, .to = "ATTIC", .line = 1435}, {.dir = "DOWN", .kind = ExitKind::CEXIT, .to = "STUDIO", .text = "Only Santa Claus climbs down chimneys.", .flag = "FALSE-FLAG", .line = 1436}};
constexpr std::string_view kFl_ATTIC[] = {"RLANDBIT", "SACREDBIT"};
constexpr std::string_view kGl_ATTIC[] = {"STAIRS"};
constexpr ExitDef kEx_ATTIC[] = {{.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "KITCHEN", .line = 1447}};
constexpr std::string_view kFl_LIVING_ROOM[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_LIVING_ROOM[] = {"STAIRS"};
constexpr PseudoDef kPs_LIVING_ROOM[] = {{"NAILS", "NAILS-PSEUDO"}, {"NAIL", "NAILS-PSEUDO"}};
constexpr ExitDef kEx_LIVING_ROOM[] = {{.dir = "EAST", .kind = ExitKind::UEXIT, .to = "KITCHEN", .line = 1454}, {.dir = "WEST", .kind = ExitKind::CEXIT, .to = "STRANGE-PASSAGE", .text = "The door is nailed shut.", .flag = "MAGIC-FLAG", .line = 1455}, {.dir = "DOWN", .kind = ExitKind::FEXIT, .routine = "TRAP-DOOR-EXIT", .line = 1456}};
constexpr std::string_view kFl_CELLAR[] = {"RLANDBIT"};
constexpr std::string_view kGl_CELLAR[] = {"TRAP-DOOR", "SLIDE", "STAIRS"};
constexpr ExitDef kEx_CELLAR[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "TROLL-ROOM", .line = 1469}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "EAST-OF-CHASM", .line = 1470}, {.dir = "UP", .kind = ExitKind::DEXIT, .to = "LIVING-ROOM", .door = "TRAP-DOOR", .line = 1471}, {.dir = "WEST", .kind = ExitKind::NEXIT, .text = "You try to ascend the ramp, but it is impossible, and you slide back down.", .line = 1472}};
constexpr std::string_view kFl_TROLL_ROOM[] = {"RLANDBIT"};
constexpr ExitDef kEx_TROLL_ROOM[] = {{.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "CELLAR", .line = 1486}, {.dir = "EAST", .kind = ExitKind::CEXIT, .to = "EW-PASSAGE", .text = "The troll fends you off with a menacing gesture.", .flag = "TROLL-FLAG", .line = 1487}, {.dir = "WEST", .kind = ExitKind::CEXIT, .to = "MAZE-1", .text = "The troll fends you off with a menacing gesture.", .flag = "TROLL-FLAG", .line = 1489}};
constexpr std::string_view kFl_EAST_OF_CHASM[] = {"RLANDBIT"};
constexpr PseudoDef kPs_EAST_OF_CHASM[] = {{"CHASM", "CHASM-PSEUDO"}};
constexpr ExitDef kEx_EAST_OF_CHASM[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "CELLAR", .line = 1501}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "GALLERY", .line = 1502}, {.dir = "DOWN", .kind = ExitKind::NEXIT, .text = "The chasm probably leads straight to the infernal regions.", .line = 1503}};
constexpr std::string_view kFl_GALLERY[] = {"RLANDBIT", "ONBIT"};
constexpr ExitDef kEx_GALLERY[] = {{.dir = "WEST", .kind = ExitKind::UEXIT, .to = "EAST-OF-CHASM", .line = 1514}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "STUDIO", .line = 1515}};
constexpr std::string_view kFl_STUDIO[] = {"RLANDBIT"};
constexpr std::string_view kGl_STUDIO[] = {"CHIMNEY"};
constexpr PseudoDef kPs_STUDIO[] = {{"DOOR", "DOOR-PSEUDO"}, {"PAINT", "PAINT-PSEUDO"}};
constexpr ExitDef kEx_STUDIO[] = {{.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "GALLERY", .line = 1528}, {.dir = "UP", .kind = ExitKind::FEXIT, .routine = "UP-CHIMNEY-FUNCTION", .line = 1529}};
constexpr std::string_view kFl_MAZE_1[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_MAZE_1[] = {{.dir = "EAST", .kind = ExitKind::UEXIT, .to = "TROLL-ROOM", .line = 1542}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "MAZE-1", .line = 1543}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "MAZE-2", .line = 1544}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "MAZE-4", .line = 1545}};
constexpr std::string_view kFl_MAZE_2[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_MAZE_2[] = {{.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "MAZE-1", .line = 1552}, {.dir = "DOWN", .kind = ExitKind::FEXIT, .routine = "MAZE-DIODES", .line = 1553}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "MAZE-3", .line = 1554}};
constexpr std::string_view kFl_MAZE_3[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_MAZE_3[] = {{.dir = "WEST", .kind = ExitKind::UEXIT, .to = "MAZE-2", .line = 1561}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "MAZE-4", .line = 1562}, {.dir = "UP", .kind = ExitKind::UEXIT, .to = "MAZE-5", .line = 1563}};
constexpr std::string_view kFl_MAZE_4[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_MAZE_4[] = {{.dir = "WEST", .kind = ExitKind::UEXIT, .to = "MAZE-3", .line = 1570}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "MAZE-1", .line = 1571}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "DEAD-END-1", .line = 1572}};
constexpr std::string_view kFl_DEAD_END_1[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_DEAD_END_1[] = {{.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "MAZE-4", .line = 1579}};
constexpr std::string_view kFl_MAZE_5[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_MAZE_5[] = {{.dir = "EAST", .kind = ExitKind::UEXIT, .to = "DEAD-END-2", .line = 1587}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "MAZE-3", .line = 1588}, {.dir = "SW", .kind = ExitKind::UEXIT, .to = "MAZE-6", .line = 1589}};
constexpr std::string_view kFl_DEAD_END_2[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_DEAD_END_2[] = {{.dir = "WEST", .kind = ExitKind::UEXIT, .to = "MAZE-5", .line = 1596}};
constexpr std::string_view kFl_MAZE_6[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_MAZE_6[] = {{.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "MAZE-5", .line = 1603}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "MAZE-7", .line = 1604}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "MAZE-6", .line = 1605}, {.dir = "UP", .kind = ExitKind::UEXIT, .to = "MAZE-9", .line = 1606}};
constexpr std::string_view kFl_MAZE_7[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_MAZE_7[] = {{.dir = "UP", .kind = ExitKind::UEXIT, .to = "MAZE-14", .line = 1613}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "MAZE-6", .line = 1614}, {.dir = "DOWN", .kind = ExitKind::FEXIT, .routine = "MAZE-DIODES", .line = 1615}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "MAZE-8", .line = 1616}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "MAZE-15", .line = 1617}};
constexpr std::string_view kFl_MAZE_8[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_MAZE_8[] = {{.dir = "NE", .kind = ExitKind::UEXIT, .to = "MAZE-7", .line = 1624}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "MAZE-8", .line = 1625}, {.dir = "SE", .kind = ExitKind::UEXIT, .to = "DEAD-END-3", .line = 1626}};
constexpr std::string_view kFl_DEAD_END_3[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_DEAD_END_3[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "MAZE-8", .line = 1633}};
constexpr std::string_view kFl_MAZE_9[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_MAZE_9[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "MAZE-6", .line = 1640}, {.dir = "DOWN", .kind = ExitKind::FEXIT, .routine = "MAZE-DIODES", .line = 1641}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "MAZE-10", .line = 1642}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "MAZE-13", .line = 1643}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "MAZE-12", .line = 1644}, {.dir = "NW", .kind = ExitKind::UEXIT, .to = "MAZE-9", .line = 1645}};
constexpr std::string_view kFl_MAZE_10[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_MAZE_10[] = {{.dir = "EAST", .kind = ExitKind::UEXIT, .to = "MAZE-9", .line = 1652}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "MAZE-13", .line = 1653}, {.dir = "UP", .kind = ExitKind::UEXIT, .to = "MAZE-11", .line = 1654}};
constexpr std::string_view kFl_MAZE_11[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_MAZE_11[] = {{.dir = "NE", .kind = ExitKind::UEXIT, .to = "GRATING-ROOM", .line = 1661}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "MAZE-10", .line = 1662}, {.dir = "NW", .kind = ExitKind::UEXIT, .to = "MAZE-13", .line = 1663}, {.dir = "SW", .kind = ExitKind::UEXIT, .to = "MAZE-12", .line = 1664}};
constexpr std::string_view kFl_GRATING_ROOM[] = {"RLANDBIT"};
constexpr std::string_view kGl_GRATING_ROOM[] = {"GRATE"};
constexpr ExitDef kEx_GRATING_ROOM[] = {{.dir = "SW", .kind = ExitKind::UEXIT, .to = "MAZE-11", .line = 1670}, {.dir = "UP", .kind = ExitKind::DEXIT, .to = "GRATING-CLEARING", .text = "The grating is closed.", .door = "GRATE", .line = 1671}};
constexpr std::string_view kFl_MAZE_12[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_MAZE_12[] = {{.dir = "DOWN", .kind = ExitKind::FEXIT, .routine = "MAZE-DIODES", .line = 1681}, {.dir = "SW", .kind = ExitKind::UEXIT, .to = "MAZE-11", .line = 1682}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "MAZE-13", .line = 1683}, {.dir = "UP", .kind = ExitKind::UEXIT, .to = "MAZE-9", .line = 1684}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "DEAD-END-4", .line = 1685}};
constexpr std::string_view kFl_DEAD_END_4[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_DEAD_END_4[] = {{.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "MAZE-12", .line = 1692}};
constexpr std::string_view kFl_MAZE_13[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_MAZE_13[] = {{.dir = "EAST", .kind = ExitKind::UEXIT, .to = "MAZE-9", .line = 1699}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "MAZE-12", .line = 1700}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "MAZE-10", .line = 1701}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "MAZE-11", .line = 1702}};
constexpr std::string_view kFl_MAZE_14[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_MAZE_14[] = {{.dir = "WEST", .kind = ExitKind::UEXIT, .to = "MAZE-15", .line = 1709}, {.dir = "NW", .kind = ExitKind::UEXIT, .to = "MAZE-14", .line = 1710}, {.dir = "NE", .kind = ExitKind::UEXIT, .to = "MAZE-7", .line = 1711}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "MAZE-7", .line = 1712}};
constexpr std::string_view kFl_MAZE_15[] = {"RLANDBIT", "MAZEBIT"};
constexpr ExitDef kEx_MAZE_15[] = {{.dir = "WEST", .kind = ExitKind::UEXIT, .to = "MAZE-14", .line = 1719}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "MAZE-7", .line = 1720}, {.dir = "SE", .kind = ExitKind::UEXIT, .to = "CYCLOPS-ROOM", .line = 1721}};
constexpr std::string_view kFl_CYCLOPS_ROOM[] = {"RLANDBIT"};
constexpr std::string_view kGl_CYCLOPS_ROOM[] = {"STAIRS"};
constexpr ExitDef kEx_CYCLOPS_ROOM[] = {{.dir = "NW", .kind = ExitKind::UEXIT, .to = "MAZE-15", .line = 1731}, {.dir = "EAST", .kind = ExitKind::CEXIT, .to = "STRANGE-PASSAGE", .text = "The east wall is solid rock.", .flag = "MAGIC-FLAG", .line = 1732}, {.dir = "UP", .kind = ExitKind::CEXIT, .to = "TREASURE-ROOM", .text = "The cyclops doesn't look like he'll let you past.", .flag = "CYCLOPS-FLAG", .line = 1734}};
constexpr std::string_view kFl_STRANGE_PASSAGE[] = {"RLANDBIT"};
constexpr ExitDef kEx_STRANGE_PASSAGE[] = {{.dir = "WEST", .kind = ExitKind::UEXIT, .to = "CYCLOPS-ROOM", .line = 1747}, {.dir = "IN", .kind = ExitKind::UEXIT, .to = "CYCLOPS-ROOM", .line = 1748}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "LIVING-ROOM", .line = 1749}};
constexpr std::string_view kFl_TREASURE_ROOM[] = {"RLANDBIT"};
constexpr std::string_view kGl_TREASURE_ROOM[] = {"STAIRS"};
constexpr ExitDef kEx_TREASURE_ROOM[] = {{.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "CYCLOPS-ROOM", .line = 1759}};
constexpr std::string_view kFl_RESERVOIR_SOUTH[] = {"RLANDBIT"};
constexpr std::string_view kGl_RESERVOIR_SOUTH[] = {"GLOBAL-WATER"};
constexpr PseudoDef kPs_RESERVOIR_SOUTH[] = {{"LAKE", "LAKE-PSEUDO"}, {"CHASM", "CHASM-PSEUDO"}};
constexpr ExitDef kEx_RESERVOIR_SOUTH[] = {{.dir = "SE", .kind = ExitKind::UEXIT, .to = "DEEP-CANYON", .line = 1772}, {.dir = "SW", .kind = ExitKind::UEXIT, .to = "CHASM-ROOM", .line = 1773}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "DAM-ROOM", .line = 1774}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "STREAM-VIEW", .line = 1775}, {.dir = "NORTH", .kind = ExitKind::CEXIT, .to = "RESERVOIR", .text = "You would drown.", .flag = "LOW-TIDE", .line = 1776}};
constexpr std::string_view kFl_RESERVOIR[] = {"NONLANDBIT"};
constexpr std::string_view kGl_RESERVOIR[] = {"GLOBAL-WATER"};
constexpr PseudoDef kPs_RESERVOIR[] = {{"STREAM", "STREAM-PSEUDO"}};
constexpr ExitDef kEx_RESERVOIR[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "RESERVOIR-NORTH", .line = 1786}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "RESERVOIR-SOUTH", .line = 1787}, {.dir = "UP", .kind = ExitKind::UEXIT, .to = "IN-STREAM", .line = 1788}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "IN-STREAM", .line = 1789}, {.dir = "DOWN", .kind = ExitKind::NEXIT, .text = "The dam blocks your way.", .line = 1790}};
constexpr std::string_view kFl_RESERVOIR_NORTH[] = {"RLANDBIT"};
constexpr std::string_view kGl_RESERVOIR_NORTH[] = {"GLOBAL-WATER", "STAIRS"};
constexpr PseudoDef kPs_RESERVOIR_NORTH[] = {{"LAKE", "LAKE-PSEUDO"}};
constexpr ExitDef kEx_RESERVOIR_NORTH[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "ATLANTIS-ROOM", .line = 1799}, {.dir = "SOUTH", .kind = ExitKind::CEXIT, .to = "RESERVOIR", .text = "You would drown.", .flag = "LOW-TIDE", .line = 1800}};
constexpr std::string_view kFl_STREAM_VIEW[] = {"RLANDBIT"};
constexpr std::string_view kGl_STREAM_VIEW[] = {"GLOBAL-WATER"};
constexpr PseudoDef kPs_STREAM_VIEW[] = {{"STREAM", "STREAM-PSEUDO"}};
constexpr ExitDef kEx_STREAM_VIEW[] = {{.dir = "EAST", .kind = ExitKind::UEXIT, .to = "RESERVOIR-SOUTH", .line = 1813}, {.dir = "WEST", .kind = ExitKind::NEXIT, .text = "The stream emerges from a spot too small for you to enter.", .line = 1814}};
constexpr std::string_view kFl_IN_STREAM[] = {"NONLANDBIT"};
constexpr std::string_view kGl_IN_STREAM[] = {"GLOBAL-WATER"};
constexpr PseudoDef kPs_IN_STREAM[] = {{"STREAM", "STREAM-PSEUDO"}};
constexpr ExitDef kEx_IN_STREAM[] = {{.dir = "UP", .kind = ExitKind::NEXIT, .text = "The channel is too narrow.", .line = 1826}, {.dir = "WEST", .kind = ExitKind::NEXIT, .text = "The channel is too narrow.", .line = 1827}, {.dir = "LAND", .kind = ExitKind::UEXIT, .to = "STREAM-VIEW", .line = 1828}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "RESERVOIR", .line = 1829}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "RESERVOIR", .line = 1830}};
constexpr std::string_view kFl_MIRROR_ROOM_1[] = {"RLANDBIT"};
constexpr ExitDef kEx_MIRROR_ROOM_1[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "COLD-PASSAGE", .line = 1842}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "TWISTING-PASSAGE", .line = 1843}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "SMALL-CAVE", .line = 1844}};
constexpr std::string_view kFl_MIRROR_ROOM_2[] = {"RLANDBIT", "ONBIT"};
constexpr ExitDef kEx_MIRROR_ROOM_2[] = {{.dir = "WEST", .kind = ExitKind::UEXIT, .to = "WINDING-PASSAGE", .line = 1851}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "NARROW-PASSAGE", .line = 1852}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "TINY-CAVE", .line = 1853}};
constexpr std::string_view kFl_SMALL_CAVE[] = {"RLANDBIT"};
constexpr std::string_view kGl_SMALL_CAVE[] = {"STAIRS"};
constexpr ExitDef kEx_SMALL_CAVE[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "MIRROR-ROOM-1", .line = 1863}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "ATLANTIS-ROOM", .line = 1864}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "ATLANTIS-ROOM", .line = 1865}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "TWISTING-PASSAGE", .line = 1866}};
constexpr std::string_view kFl_TINY_CAVE[] = {"RLANDBIT"};
constexpr std::string_view kGl_TINY_CAVE[] = {"STAIRS"};
constexpr ExitDef kEx_TINY_CAVE[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "MIRROR-ROOM-2", .line = 1876}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "WINDING-PASSAGE", .line = 1877}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "ENTRANCE-TO-HADES", .line = 1878}};
constexpr std::string_view kFl_COLD_PASSAGE[] = {"RLANDBIT"};
constexpr ExitDef kEx_COLD_PASSAGE[] = {{.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "MIRROR-ROOM-1", .line = 1889}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "SLIDE-ROOM", .line = 1890}};
constexpr std::string_view kFl_NARROW_PASSAGE[] = {"RLANDBIT"};
constexpr ExitDef kEx_NARROW_PASSAGE[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "ROUND-ROOM", .line = 1899}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "MIRROR-ROOM-2", .line = 1900}};
constexpr std::string_view kFl_WINDING_PASSAGE[] = {"RLANDBIT"};
constexpr ExitDef kEx_WINDING_PASSAGE[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "MIRROR-ROOM-2", .line = 1909}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "TINY-CAVE", .line = 1910}};
constexpr std::string_view kFl_TWISTING_PASSAGE[] = {"RLANDBIT"};
constexpr ExitDef kEx_TWISTING_PASSAGE[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "MIRROR-ROOM-1", .line = 1919}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "SMALL-CAVE", .line = 1920}};
constexpr std::string_view kFl_ATLANTIS_ROOM[] = {"RLANDBIT"};
constexpr std::string_view kGl_ATLANTIS_ROOM[] = {"STAIRS"};
constexpr ExitDef kEx_ATLANTIS_ROOM[] = {{.dir = "UP", .kind = ExitKind::UEXIT, .to = "SMALL-CAVE", .line = 1929}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "RESERVOIR-NORTH", .line = 1930}};
constexpr std::string_view kFl_EW_PASSAGE[] = {"RLANDBIT"};
constexpr std::string_view kGl_EW_PASSAGE[] = {"STAIRS"};
constexpr ExitDef kEx_EW_PASSAGE[] = {{.dir = "EAST", .kind = ExitKind::UEXIT, .to = "ROUND-ROOM", .line = 1944}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "TROLL-ROOM", .line = 1945}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "CHASM-ROOM", .line = 1946}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "CHASM-ROOM", .line = 1947}};
constexpr std::string_view kFl_ROUND_ROOM[] = {"RLANDBIT"};
constexpr ExitDef kEx_ROUND_ROOM[] = {{.dir = "EAST", .kind = ExitKind::UEXIT, .to = "LOUD-ROOM", .line = 1958}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "EW-PASSAGE", .line = 1959}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "NS-PASSAGE", .line = 1960}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "NARROW-PASSAGE", .line = 1961}, {.dir = "SE", .kind = ExitKind::UEXIT, .to = "ENGRAVINGS-CAVE", .line = 1962}};
constexpr std::string_view kFl_DEEP_CANYON[] = {"RLANDBIT"};
constexpr std::string_view kGl_DEEP_CANYON[] = {"STAIRS"};
constexpr ExitDef kEx_DEEP_CANYON[] = {{.dir = "NW", .kind = ExitKind::UEXIT, .to = "RESERVOIR-SOUTH", .line = 1968}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "DAM-ROOM", .line = 1969}, {.dir = "SW", .kind = ExitKind::UEXIT, .to = "NS-PASSAGE", .line = 1970}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "LOUD-ROOM", .line = 1971}};
constexpr std::string_view kFl_DAMP_CAVE[] = {"RLANDBIT"};
constexpr std::string_view kGl_DAMP_CAVE[] = {"CRACK"};
constexpr ExitDef kEx_DAMP_CAVE[] = {{.dir = "WEST", .kind = ExitKind::UEXIT, .to = "LOUD-ROOM", .line = 1982}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "WHITE-CLIFFS-NORTH", .line = 1983}, {.dir = "SOUTH", .kind = ExitKind::NEXIT, .text = "It is too narrow for most insects.", .line = 1984}};
constexpr std::string_view kFl_LOUD_ROOM[] = {"RLANDBIT"};
constexpr std::string_view kGl_LOUD_ROOM[] = {"STAIRS"};
constexpr ExitDef kEx_LOUD_ROOM[] = {{.dir = "EAST", .kind = ExitKind::UEXIT, .to = "DAMP-CAVE", .line = 1991}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "ROUND-ROOM", .line = 1992}, {.dir = "UP", .kind = ExitKind::UEXIT, .to = "DEEP-CANYON", .line = 1993}};
constexpr std::string_view kFl_NS_PASSAGE[] = {"RLANDBIT"};
constexpr ExitDef kEx_NS_PASSAGE[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "CHASM-ROOM", .line = 2003}, {.dir = "NE", .kind = ExitKind::UEXIT, .to = "DEEP-CANYON", .line = 2004}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "ROUND-ROOM", .line = 2005}};
constexpr std::string_view kFl_CHASM_ROOM[] = {"RLANDBIT"};
constexpr std::string_view kGl_CHASM_ROOM[] = {"CRACK", "STAIRS"};
constexpr PseudoDef kPs_CHASM_ROOM[] = {{"CHASM", "CHASM-PSEUDO"}};
constexpr ExitDef kEx_CHASM_ROOM[] = {{.dir = "NE", .kind = ExitKind::UEXIT, .to = "RESERVOIR-SOUTH", .line = 2014}, {.dir = "SW", .kind = ExitKind::UEXIT, .to = "EW-PASSAGE", .line = 2015}, {.dir = "UP", .kind = ExitKind::UEXIT, .to = "EW-PASSAGE", .line = 2016}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "NS-PASSAGE", .line = 2017}, {.dir = "DOWN", .kind = ExitKind::NEXIT, .text = "Are you out of your mind?", .line = 2018}};
constexpr std::string_view kFl_ENTRANCE_TO_HADES[] = {"RLANDBIT", "ONBIT"};
constexpr std::string_view kGl_ENTRANCE_TO_HADES[] = {"BODIES"};
constexpr PseudoDef kPs_ENTRANCE_TO_HADES[] = {{"GATE", "GATE-PSEUDO"}, {"GATES", "GATE-PSEUDO"}};
constexpr ExitDef kEx_ENTRANCE_TO_HADES[] = {{.dir = "UP", .kind = ExitKind::UEXIT, .to = "TINY-CAVE", .line = 2030}, {.dir = "IN", .kind = ExitKind::CEXIT, .to = "LAND-OF-LIVING-DEAD", .text = "Some invisible force prevents you from passing through the gate.", .flag = "LLD-FLAG", .line = 2031}, {.dir = "SOUTH", .kind = ExitKind::CEXIT, .to = "LAND-OF-LIVING-DEAD", .text = "Some invisible force prevents you from passing through the gate.", .flag = "LLD-FLAG", .line = 2033}};
constexpr std::string_view kFl_LAND_OF_LIVING_DEAD[] = {"RLANDBIT", "ONBIT"};
constexpr std::string_view kGl_LAND_OF_LIVING_DEAD[] = {"BODIES"};
constexpr ExitDef kEx_LAND_OF_LIVING_DEAD[] = {{.dir = "OUT", .kind = ExitKind::UEXIT, .to = "ENTRANCE-TO-HADES", .line = 2048}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "ENTRANCE-TO-HADES", .line = 2049}};
constexpr std::string_view kFl_ENGRAVINGS_CAVE[] = {"RLANDBIT"};
constexpr ExitDef kEx_ENGRAVINGS_CAVE[] = {{.dir = "NW", .kind = ExitKind::UEXIT, .to = "ROUND-ROOM", .line = 2062}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "DOME-ROOM", .line = 2063}};
constexpr std::string_view kFl_EGYPT_ROOM[] = {"RLANDBIT"};
constexpr std::string_view kGl_EGYPT_ROOM[] = {"STAIRS"};
constexpr ExitDef kEx_EGYPT_ROOM[] = {{.dir = "WEST", .kind = ExitKind::UEXIT, .to = "NORTH-TEMPLE", .line = 2072}, {.dir = "UP", .kind = ExitKind::UEXIT, .to = "NORTH-TEMPLE", .line = 2073}};
constexpr std::string_view kFl_DOME_ROOM[] = {"RLANDBIT"};
constexpr PseudoDef kPs_DOME_ROOM[] = {{"DOME", "DOME-PSEUDO"}};
constexpr ExitDef kEx_DOME_ROOM[] = {{.dir = "WEST", .kind = ExitKind::UEXIT, .to = "ENGRAVINGS-CAVE", .line = 2080}, {.dir = "DOWN", .kind = ExitKind::CEXIT, .to = "TORCH-ROOM", .text = "You cannot go down without fracturing many bones.", .flag = "DOME-FLAG", .line = 2081}};
constexpr std::string_view kFl_TORCH_ROOM[] = {"RLANDBIT"};
constexpr std::string_view kGl_TORCH_ROOM[] = {"STAIRS"};
constexpr PseudoDef kPs_TORCH_ROOM[] = {{"DOME", "DOME-PSEUDO"}};
constexpr ExitDef kEx_TORCH_ROOM[] = {{.dir = "UP", .kind = ExitKind::NEXIT, .text = "You cannot reach the rope.", .line = 2090}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "NORTH-TEMPLE", .line = 2091}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "NORTH-TEMPLE", .line = 2092}};
constexpr std::string_view kFl_NORTH_TEMPLE[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_NORTH_TEMPLE[] = {"STAIRS"};
constexpr ExitDef kEx_NORTH_TEMPLE[] = {{.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "EGYPT-ROOM", .line = 2107}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "EGYPT-ROOM", .line = 2108}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "TORCH-ROOM", .line = 2109}, {.dir = "OUT", .kind = ExitKind::UEXIT, .to = "TORCH-ROOM", .line = 2110}, {.dir = "UP", .kind = ExitKind::UEXIT, .to = "TORCH-ROOM", .line = 2111}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "SOUTH-TEMPLE", .line = 2112}};
constexpr std::string_view kFl_SOUTH_TEMPLE[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr ExitDef kEx_SOUTH_TEMPLE[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "NORTH-TEMPLE", .line = 2124}, {.dir = "DOWN", .kind = ExitKind::CEXIT, .to = "TINY-CAVE", .text = "You haven't a prayer of getting the coffin down there.", .flag = "COFFIN-CURE", .line = 2125}};
constexpr std::string_view kFl_DAM_ROOM[] = {"RLANDBIT", "ONBIT"};
constexpr std::string_view kGl_DAM_ROOM[] = {"GLOBAL-WATER"};
constexpr ExitDef kEx_DAM_ROOM[] = {{.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "DEEP-CANYON", .line = 2138}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "DAM-BASE", .line = 2139}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "DAM-BASE", .line = 2140}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "DAM-LOBBY", .line = 2141}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "RESERVOIR-SOUTH", .line = 2142}};
constexpr std::string_view kFl_DAM_LOBBY[] = {"RLANDBIT", "ONBIT"};
constexpr ExitDef kEx_DAM_LOBBY[] = {{.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "DAM-ROOM", .line = 2154}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "MAINTENANCE-ROOM", .line = 2155}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "MAINTENANCE-ROOM", .line = 2156}};
constexpr std::string_view kFl_MAINTENANCE_ROOM[] = {"RLANDBIT"};
constexpr ExitDef kEx_MAINTENANCE_ROOM[] = {{.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "DAM-LOBBY", .line = 2168}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "DAM-LOBBY", .line = 2169}};
constexpr std::string_view kFl_DAM_BASE[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_DAM_BASE[] = {"GLOBAL-WATER", "RIVER"};
constexpr ExitDef kEx_DAM_BASE[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "DAM-ROOM", .line = 2185}, {.dir = "UP", .kind = ExitKind::UEXIT, .to = "DAM-ROOM", .line = 2186}};
constexpr std::string_view kFl_RIVER_1[] = {"NONLANDBIT", "SACREDBIT", "ONBIT"};
constexpr std::string_view kGl_RIVER_1[] = {"GLOBAL-WATER", "RIVER"};
constexpr ExitDef kEx_RIVER_1[] = {{.dir = "UP", .kind = ExitKind::NEXIT, .text = "You cannot go upstream due to strong currents.", .line = 2196}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "DAM-BASE", .line = 2197}, {.dir = "LAND", .kind = ExitKind::UEXIT, .to = "DAM-BASE", .line = 2198}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "RIVER-2", .line = 2199}, {.dir = "EAST", .kind = ExitKind::NEXIT, .text = "The White Cliffs prevent your landing here.", .line = 2200}};
constexpr std::string_view kFl_RIVER_2[] = {"NONLANDBIT", "SACREDBIT"};
constexpr std::string_view kGl_RIVER_2[] = {"GLOBAL-WATER", "RIVER"};
constexpr ExitDef kEx_RIVER_2[] = {{.dir = "UP", .kind = ExitKind::NEXIT, .text = "You cannot go upstream due to strong currents.", .line = 2211}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "RIVER-3", .line = 2212}, {.dir = "LAND", .kind = ExitKind::NEXIT, .text = "There is no safe landing spot here.", .line = 2213}, {.dir = "EAST", .kind = ExitKind::NEXIT, .text = "The White Cliffs prevent your landing here.", .line = 2214}, {.dir = "WEST", .kind = ExitKind::NEXIT, .text = "Just in time you steer away from the rocks.", .line = 2215}};
constexpr std::string_view kFl_RIVER_3[] = {"NONLANDBIT", "SACREDBIT"};
constexpr std::string_view kGl_RIVER_3[] = {"GLOBAL-WATER", "RIVER"};
constexpr ExitDef kEx_RIVER_3[] = {{.dir = "UP", .kind = ExitKind::NEXIT, .text = "You cannot go upstream due to strong currents.", .line = 2226}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "RIVER-4", .line = 2227}, {.dir = "LAND", .kind = ExitKind::UEXIT, .to = "WHITE-CLIFFS-NORTH", .line = 2228}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "WHITE-CLIFFS-NORTH", .line = 2229}};
constexpr std::string_view kFl_WHITE_CLIFFS_NORTH[] = {"RLANDBIT", "SACREDBIT"};
constexpr std::string_view kGl_WHITE_CLIFFS_NORTH[] = {"GLOBAL-WATER", "WHITE-CLIFF", "RIVER"};
constexpr ExitDef kEx_WHITE_CLIFFS_NORTH[] = {{.dir = "SOUTH", .kind = ExitKind::CEXIT, .to = "WHITE-CLIFFS-SOUTH", .text = "The path is too narrow.", .flag = "DEFLATE", .line = 2240}, {.dir = "WEST", .kind = ExitKind::CEXIT, .to = "DAMP-CAVE", .text = "The path is too narrow.", .flag = "DEFLATE", .line = 2241}};
constexpr std::string_view kFl_WHITE_CLIFFS_SOUTH[] = {"RLANDBIT", "SACREDBIT"};
constexpr std::string_view kGl_WHITE_CLIFFS_SOUTH[] = {"GLOBAL-WATER", "WHITE-CLIFF", "RIVER"};
constexpr ExitDef kEx_WHITE_CLIFFS_SOUTH[] = {{.dir = "NORTH", .kind = ExitKind::CEXIT, .to = "WHITE-CLIFFS-NORTH", .text = "The path is too narrow.", .flag = "DEFLATE", .line = 2252}};
constexpr std::string_view kFl_RIVER_4[] = {"NONLANDBIT", "SACREDBIT"};
constexpr std::string_view kGl_RIVER_4[] = {"GLOBAL-WATER", "RIVER"};
constexpr ExitDef kEx_RIVER_4[] = {{.dir = "UP", .kind = ExitKind::NEXIT, .text = "You cannot go upstream due to strong currents.", .line = 2265}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "RIVER-5", .line = 2266}, {.dir = "LAND", .kind = ExitKind::NEXIT, .text = "You can land either to the east or the west.", .line = 2267}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "WHITE-CLIFFS-SOUTH", .line = 2268}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "SANDY-BEACH", .line = 2269}};
constexpr std::string_view kFl_RIVER_5[] = {"NONLANDBIT", "SACREDBIT", "ONBIT"};
constexpr std::string_view kGl_RIVER_5[] = {"GLOBAL-WATER", "RIVER"};
constexpr ExitDef kEx_RIVER_5[] = {{.dir = "UP", .kind = ExitKind::NEXIT, .text = "You cannot go upstream due to strong currents.", .line = 2280}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "SHORE", .line = 2281}, {.dir = "LAND", .kind = ExitKind::UEXIT, .to = "SHORE", .line = 2282}};
constexpr std::string_view kFl_SHORE[] = {"RLANDBIT", "SACREDBIT", "ONBIT"};
constexpr std::string_view kGl_SHORE[] = {"GLOBAL-WATER", "RIVER"};
constexpr ExitDef kEx_SHORE[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "SANDY-BEACH", .line = 2293}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "ARAGAIN-FALLS", .line = 2294}};
constexpr std::string_view kFl_SANDY_BEACH[] = {"RLANDBIT", "SACREDBIT"};
constexpr std::string_view kGl_SANDY_BEACH[] = {"GLOBAL-WATER", "RIVER"};
constexpr ExitDef kEx_SANDY_BEACH[] = {{.dir = "NE", .kind = ExitKind::UEXIT, .to = "SANDY-CAVE", .line = 2306}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "SHORE", .line = 2307}};
constexpr std::string_view kFl_SANDY_CAVE[] = {"RLANDBIT"};
constexpr ExitDef kEx_SANDY_CAVE[] = {{.dir = "SW", .kind = ExitKind::UEXIT, .to = "SANDY-BEACH", .line = 2316}};
constexpr std::string_view kFl_ARAGAIN_FALLS[] = {"RLANDBIT", "SACREDBIT", "ONBIT"};
constexpr std::string_view kGl_ARAGAIN_FALLS[] = {"GLOBAL-WATER", "RIVER", "RAINBOW"};
constexpr ExitDef kEx_ARAGAIN_FALLS[] = {{.dir = "WEST", .kind = ExitKind::CEXIT, .to = "ON-RAINBOW", .flag = "RAINBOW-FLAG", .line = 2322}, {.dir = "DOWN", .kind = ExitKind::NEXIT, .text = "It's a long way...", .line = 2323}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "SHORE", .line = 2324}, {.dir = "UP", .kind = ExitKind::CEXIT, .to = "ON-RAINBOW", .flag = "RAINBOW-FLAG", .line = 2325}};
constexpr std::string_view kFl_ON_RAINBOW[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_ON_RAINBOW[] = {"RAINBOW"};
constexpr ExitDef kEx_ON_RAINBOW[] = {{.dir = "WEST", .kind = ExitKind::UEXIT, .to = "END-OF-RAINBOW", .line = 2337}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "ARAGAIN-FALLS", .line = 2338}};
constexpr std::string_view kFl_END_OF_RAINBOW[] = {"RLANDBIT", "ONBIT"};
constexpr std::string_view kGl_END_OF_RAINBOW[] = {"GLOBAL-WATER", "RAINBOW", "RIVER"};
constexpr ExitDef kEx_END_OF_RAINBOW[] = {{.dir = "UP", .kind = ExitKind::CEXIT, .to = "ON-RAINBOW", .flag = "RAINBOW-FLAG", .line = 2351}, {.dir = "NE", .kind = ExitKind::CEXIT, .to = "ON-RAINBOW", .flag = "RAINBOW-FLAG", .line = 2352}, {.dir = "EAST", .kind = ExitKind::CEXIT, .to = "ON-RAINBOW", .flag = "RAINBOW-FLAG", .line = 2353}, {.dir = "SW", .kind = ExitKind::UEXIT, .to = "CANYON-BOTTOM", .line = 2354}};
constexpr std::string_view kFl_CANYON_BOTTOM[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_CANYON_BOTTOM[] = {"GLOBAL-WATER", "CLIMBABLE-CLIFF", "RIVER"};
constexpr ExitDef kEx_CANYON_BOTTOM[] = {{.dir = "UP", .kind = ExitKind::UEXIT, .to = "CLIFF-MIDDLE", .line = 2365}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "END-OF-RAINBOW", .line = 2366}};
constexpr std::string_view kFl_CLIFF_MIDDLE[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_CLIFF_MIDDLE[] = {"CLIMBABLE-CLIFF", "RIVER"};
constexpr ExitDef kEx_CLIFF_MIDDLE[] = {{.dir = "UP", .kind = ExitKind::UEXIT, .to = "CANYON-VIEW", .line = 2379}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "CANYON-BOTTOM", .line = 2380}};
constexpr std::string_view kFl_CANYON_VIEW[] = {"RLANDBIT", "ONBIT", "SACREDBIT"};
constexpr std::string_view kGl_CANYON_VIEW[] = {"CLIMBABLE-CLIFF", "RIVER", "RAINBOW"};
constexpr ExitDef kEx_CANYON_VIEW[] = {{.dir = "EAST", .kind = ExitKind::UEXIT, .to = "CLIFF-MIDDLE", .line = 2397}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "CLIFF-MIDDLE", .line = 2398}, {.dir = "NW", .kind = ExitKind::UEXIT, .to = "CLEARING", .line = 2399}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "FOREST-3", .line = 2400}, {.dir = "SOUTH", .kind = ExitKind::NEXIT, .text = "Storm-tossed trees block your way.", .line = 2401}};
constexpr std::string_view kFl_MINE_ENTRANCE[] = {"RLANDBIT"};
constexpr ExitDef kEx_MINE_ENTRANCE[] = {{.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "SLIDE-ROOM", .line = 2426}, {.dir = "IN", .kind = ExitKind::UEXIT, .to = "SQUEEKY-ROOM", .line = 2427}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "SQUEEKY-ROOM", .line = 2428}};
constexpr std::string_view kFl_SQUEEKY_ROOM[] = {"RLANDBIT"};
constexpr ExitDef kEx_SQUEEKY_ROOM[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "BAT-ROOM", .line = 2437}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "MINE-ENTRANCE", .line = 2438}};
constexpr std::string_view kFl_BAT_ROOM[] = {"RLANDBIT", "SACREDBIT"};
constexpr ExitDef kEx_BAT_ROOM[] = {{.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "SQUEEKY-ROOM", .line = 2444}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "SHAFT-ROOM", .line = 2445}};
constexpr std::string_view kFl_SHAFT_ROOM[] = {"RLANDBIT"};
constexpr PseudoDef kPs_SHAFT_ROOM[] = {{"CHAIN", "CHAIN-PSEUDO"}};
constexpr ExitDef kEx_SHAFT_ROOM[] = {{.dir = "DOWN", .kind = ExitKind::NEXIT, .text = "You wouldn't fit and would die if you could.", .line = 2457}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "BAT-ROOM", .line = 2458}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "SMELLY-ROOM", .line = 2459}};
constexpr std::string_view kFl_SMELLY_ROOM[] = {"RLANDBIT"};
constexpr std::string_view kGl_SMELLY_ROOM[] = {"STAIRS"};
constexpr PseudoDef kPs_SMELLY_ROOM[] = {{"ODOR", "GAS-PSEUDO"}, {"GAS", "GAS-PSEUDO"}};
constexpr ExitDef kEx_SMELLY_ROOM[] = {{.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "GAS-ROOM", .line = 2470}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "SHAFT-ROOM", .line = 2471}};
constexpr std::string_view kFl_GAS_ROOM[] = {"RLANDBIT", "SACREDBIT"};
constexpr std::string_view kGl_GAS_ROOM[] = {"STAIRS"};
constexpr PseudoDef kPs_GAS_ROOM[] = {{"GAS", "GAS-PSEUDO"}, {"ODOR", "GAS-PSEUDO"}};
constexpr ExitDef kEx_GAS_ROOM[] = {{.dir = "UP", .kind = ExitKind::UEXIT, .to = "SMELLY-ROOM", .line = 2482}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "MINE-1", .line = 2483}};
constexpr std::string_view kFl_LADDER_TOP[] = {"RLANDBIT"};
constexpr std::string_view kGl_LADDER_TOP[] = {"LADDER", "STAIRS"};
constexpr ExitDef kEx_LADDER_TOP[] = {{.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "LADDER-BOTTOM", .line = 2496}, {.dir = "UP", .kind = ExitKind::UEXIT, .to = "MINE-4", .line = 2497}};
constexpr std::string_view kFl_LADDER_BOTTOM[] = {"RLANDBIT"};
constexpr std::string_view kGl_LADDER_BOTTOM[] = {"LADDER"};
constexpr ExitDef kEx_LADDER_BOTTOM[] = {{.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "DEAD-END-5", .line = 2508}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "TIMBER-ROOM", .line = 2509}, {.dir = "UP", .kind = ExitKind::UEXIT, .to = "LADDER-TOP", .line = 2510}};
constexpr std::string_view kFl_DEAD_END_5[] = {"RLANDBIT"};
constexpr ExitDef kEx_DEAD_END_5[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "LADDER-BOTTOM", .line = 2518}};
constexpr std::string_view kFl_TIMBER_ROOM[] = {"RLANDBIT", "SACREDBIT"};
constexpr ExitDef kEx_TIMBER_ROOM[] = {{.dir = "EAST", .kind = ExitKind::UEXIT, .to = "LADDER-BOTTOM", .line = 2529}, {.dir = "WEST", .kind = ExitKind::CEXIT, .to = "LOWER-SHAFT", .text = "You cannot fit through this passage with that load.", .flag = "EMPTY-HANDED", .line = 2530}};
constexpr std::string_view kFl_LOWER_SHAFT[] = {"RLANDBIT", "SACREDBIT"};
constexpr PseudoDef kPs_LOWER_SHAFT[] = {{"CHAIN", "CHAIN-PSEUDO"}};
constexpr ExitDef kEx_LOWER_SHAFT[] = {{.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "MACHINE-ROOM", .line = 2543}, {.dir = "OUT", .kind = ExitKind::CEXIT, .to = "TIMBER-ROOM", .text = "You cannot fit through this passage with that load.", .flag = "EMPTY-HANDED", .line = 2544}, {.dir = "EAST", .kind = ExitKind::CEXIT, .to = "TIMBER-ROOM", .text = "You cannot fit through this passage with that load.", .flag = "EMPTY-HANDED", .line = 2547}};
constexpr std::string_view kFl_MACHINE_ROOM[] = {"RLANDBIT"};
constexpr ExitDef kEx_MACHINE_ROOM[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "LOWER-SHAFT", .line = 2557}};
constexpr std::string_view kFl_MINE_1[] = {"RLANDBIT"};
constexpr ExitDef kEx_MINE_1[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "GAS-ROOM", .line = 2569}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "MINE-1", .line = 2570}, {.dir = "NE", .kind = ExitKind::UEXIT, .to = "MINE-2", .line = 2571}};
constexpr std::string_view kFl_MINE_2[] = {"RLANDBIT"};
constexpr ExitDef kEx_MINE_2[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "MINE-2", .line = 2578}, {.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "MINE-1", .line = 2579}, {.dir = "SE", .kind = ExitKind::UEXIT, .to = "MINE-3", .line = 2580}};
constexpr std::string_view kFl_MINE_3[] = {"RLANDBIT"};
constexpr ExitDef kEx_MINE_3[] = {{.dir = "SOUTH", .kind = ExitKind::UEXIT, .to = "MINE-3", .line = 2587}, {.dir = "SW", .kind = ExitKind::UEXIT, .to = "MINE-4", .line = 2588}, {.dir = "EAST", .kind = ExitKind::UEXIT, .to = "MINE-2", .line = 2589}};
constexpr std::string_view kFl_MINE_4[] = {"RLANDBIT"};
constexpr ExitDef kEx_MINE_4[] = {{.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "MINE-3", .line = 2596}, {.dir = "WEST", .kind = ExitKind::UEXIT, .to = "MINE-4", .line = 2597}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "LADDER-TOP", .line = 2598}};
constexpr std::string_view kFl_SLIDE_ROOM[] = {"RLANDBIT"};
constexpr std::string_view kGl_SLIDE_ROOM[] = {"SLIDE"};
constexpr ExitDef kEx_SLIDE_ROOM[] = {{.dir = "EAST", .kind = ExitKind::UEXIT, .to = "COLD-PASSAGE", .line = 2610}, {.dir = "NORTH", .kind = ExitKind::UEXIT, .to = "MINE-ENTRANCE", .line = 2611}, {.dir = "DOWN", .kind = ExitKind::UEXIT, .to = "CELLAR", .line = 2612}};

constexpr RoomDef kRoomsArr[] = {
    {.def_index = 122, .name = "WEST-OF-HOUSE", .desc = "West of House", .flags = kFl_WEST_OF_HOUSE, .action = "WEST-HOUSE", .globals = kGl_WEST_OF_HOUSE, .exits = kEx_WEST_OF_HOUSE, .line = 1239},
    {.def_index = 123, .name = "STONE-BARROW", .desc = "Stone Barrow", .ldesc = "You are standing in front of a massive barrow of stone. In the east face is a huge stone door which is open. You cannot see into the dark of the tomb.", .flags = kFl_STONE_BARROW, .action = "STONE-BARROW-FCN", .exits = kEx_STONE_BARROW, .line = 1254},
    {.def_index = 124, .name = "NORTH-OF-HOUSE", .desc = "North of House", .ldesc = "You are facing the north side of a white house. There is no door here, and all the windows are boarded up. To the north a narrow path winds through the trees.", .flags = kFl_NORTH_OF_HOUSE, .globals = kGl_NORTH_OF_HOUSE, .exits = kEx_NORTH_OF_HOUSE, .line = 1264},
    {.def_index = 125, .name = "SOUTH-OF-HOUSE", .desc = "South of House", .ldesc = "You are facing the south side of a white house. There is no door here, and all the windows are boarded.", .flags = kFl_SOUTH_OF_HOUSE, .globals = kGl_SOUTH_OF_HOUSE, .exits = kEx_SOUTH_OF_HOUSE, .line = 1280},
    {.def_index = 126, .name = "EAST-OF-HOUSE", .desc = "Behind House", .flags = kFl_EAST_OF_HOUSE, .action = "EAST-HOUSE", .globals = kGl_EAST_OF_HOUSE, .exits = kEx_EAST_OF_HOUSE, .line = 1295},
    {.def_index = 127, .name = "FOREST-1", .desc = "Forest", .ldesc = "This is a forest, with trees in all directions. To the east, there appears to be sunlight.", .flags = kFl_FOREST_1, .action = "FOREST-ROOM", .globals = kGl_FOREST_1, .exits = kEx_FOREST_1, .line = 1309},
    {.def_index = 128, .name = "FOREST-2", .desc = "Forest", .ldesc = "This is a dimly lit forest, with large trees all around.", .flags = kFl_FOREST_2, .action = "FOREST-ROOM", .globals = kGl_FOREST_2, .exits = kEx_FOREST_2, .line = 1324},
    {.def_index = 129, .name = "MOUNTAINS", .desc = "Forest", .ldesc = "The forest thins out, revealing impassable mountains.", .flags = kFl_MOUNTAINS, .globals = kGl_MOUNTAINS, .exits = kEx_MOUNTAINS, .line = 1337},
    {.def_index = 130, .name = "FOREST-3", .desc = "Forest", .ldesc = "This is a dimly lit forest, with large trees all around.", .flags = kFl_FOREST_3, .action = "FOREST-ROOM", .globals = kGl_FOREST_3, .exits = kEx_FOREST_3, .line = 1349},
    {.def_index = 131, .name = "PATH", .desc = "Forest Path", .ldesc = "This is a path winding through a dimly lit forest. The path heads north-south here. One particularly large tree with some low branches stands at the edge of the path.", .flags = kFl_PATH, .action = "FOREST-ROOM", .globals = kGl_PATH, .exits = kEx_PATH, .line = 1363},
    {.def_index = 132, .name = "UP-A-TREE", .desc = "Up a Tree", .flags = kFl_UP_A_TREE, .action = "TREE-ROOM", .globals = kGl_UP_A_TREE, .exits = kEx_UP_A_TREE, .line = 1379},
    {.def_index = 133, .name = "GRATING-CLEARING", .desc = "Clearing", .flags = kFl_GRATING_CLEARING, .action = "CLEARING-FCN", .globals = kGl_GRATING_CLEARING, .exits = kEx_GRATING_CLEARING, .line = 1388},
    {.def_index = 134, .name = "CLEARING", .desc = "Clearing", .ldesc = "You are in a small clearing in a well marked forest path that extends to the east and west.", .flags = kFl_CLEARING, .action = "FOREST-ROOM", .globals = kGl_CLEARING, .exits = kEx_CLEARING, .line = 1410},
    {.def_index = 135, .name = "KITCHEN", .desc = "Kitchen", .flags = kFl_KITCHEN, .value = 10, .action = "KITCHEN-FCN", .globals = kGl_KITCHEN, .exits = kEx_KITCHEN, .line = 1429},
    {.def_index = 136, .name = "ATTIC", .desc = "Attic", .ldesc = "This is the attic. The only exit is a stairway leading down.", .flags = kFl_ATTIC, .globals = kGl_ATTIC, .exits = kEx_ATTIC, .line = 1443},
    {.def_index = 137, .name = "LIVING-ROOM", .desc = "Living Room", .flags = kFl_LIVING_ROOM, .action = "LIVING-ROOM-FCN", .globals = kGl_LIVING_ROOM, .pseudo = kPs_LIVING_ROOM, .exits = kEx_LIVING_ROOM, .line = 1451},
    {.def_index = 138, .name = "CELLAR", .desc = "Cellar", .flags = kFl_CELLAR, .value = 25, .action = "CELLAR-FCN", .globals = kGl_CELLAR, .exits = kEx_CELLAR, .line = 1466},
    {.def_index = 139, .name = "TROLL-ROOM", .desc = "The Troll Room", .ldesc = "This is a small room with passages to the east and south and a forbidding hole leading west. Bloodstains and deep scratches (perhaps made by an axe) mar the walls.", .flags = kFl_TROLL_ROOM, .action = "TROLL-ROOM-F", .exits = kEx_TROLL_ROOM, .line = 1479},
    {.def_index = 140, .name = "EAST-OF-CHASM", .desc = "East of Chasm", .ldesc = "You are on the east edge of a chasm, the bottom of which cannot be seen. A narrow passage goes north, and the path you are on continues to the east.", .flags = kFl_EAST_OF_CHASM, .pseudo = kPs_EAST_OF_CHASM, .exits = kEx_EAST_OF_CHASM, .line = 1494},
    {.def_index = 141, .name = "GALLERY", .desc = "Gallery", .ldesc = "This is an art gallery. Most of the paintings have been stolen by vandals with exceptional taste. The vandals left through either the north or west exits.", .flags = kFl_GALLERY, .exits = kEx_GALLERY, .line = 1507},
    {.def_index = 142, .name = "STUDIO", .desc = "Studio", .ldesc = "This appears to have been an artist's studio. The walls and floors are splattered with paints of 69 different colors. Strangely enough, nothing of value is hanging here. At the south end of the room is an open door (also covered with paint). A dark and narrow chimney leads up from a fireplace; although you might be able to get up it, it seems unlikely you could get back down.", .flags = kFl_STUDIO, .globals = kGl_STUDIO, .pseudo = kPs_STUDIO, .exits = kEx_STUDIO, .line = 1518},
    {.def_index = 143, .name = "MAZE-1", .desc = "Maze", .ldesc = "This is part of a maze of twisty little passages, all alike.", .flags = kFl_MAZE_1, .exits = kEx_MAZE_1, .line = 1538},
    {.def_index = 144, .name = "MAZE-2", .desc = "Maze", .ldesc = "This is part of a maze of twisty little passages, all alike.", .flags = kFl_MAZE_2, .exits = kEx_MAZE_2, .line = 1548},
    {.def_index = 145, .name = "MAZE-3", .desc = "Maze", .ldesc = "This is part of a maze of twisty little passages, all alike.", .flags = kFl_MAZE_3, .exits = kEx_MAZE_3, .line = 1557},
    {.def_index = 146, .name = "MAZE-4", .desc = "Maze", .ldesc = "This is part of a maze of twisty little passages, all alike.", .flags = kFl_MAZE_4, .exits = kEx_MAZE_4, .line = 1566},
    {.def_index = 147, .name = "DEAD-END-1", .desc = "Dead End", .ldesc = "You have come to a dead end in the maze.", .flags = kFl_DEAD_END_1, .exits = kEx_DEAD_END_1, .line = 1575},
    {.def_index = 148, .name = "MAZE-5", .desc = "Maze", .ldesc = "This is part of a maze of twisty little passages, all alike. A skeleton, probably the remains of a luckless adventurer, lies here.", .flags = kFl_MAZE_5, .exits = kEx_MAZE_5, .line = 1582},
    {.def_index = 149, .name = "DEAD-END-2", .desc = "Dead End", .ldesc = "You have come to a dead end in the maze.", .flags = kFl_DEAD_END_2, .exits = kEx_DEAD_END_2, .line = 1592},
    {.def_index = 150, .name = "MAZE-6", .desc = "Maze", .ldesc = "This is part of a maze of twisty little passages, all alike.", .flags = kFl_MAZE_6, .exits = kEx_MAZE_6, .line = 1599},
    {.def_index = 151, .name = "MAZE-7", .desc = "Maze", .ldesc = "This is part of a maze of twisty little passages, all alike.", .flags = kFl_MAZE_7, .exits = kEx_MAZE_7, .line = 1609},
    {.def_index = 152, .name = "MAZE-8", .desc = "Maze", .ldesc = "This is part of a maze of twisty little passages, all alike.", .flags = kFl_MAZE_8, .exits = kEx_MAZE_8, .line = 1620},
    {.def_index = 153, .name = "DEAD-END-3", .desc = "Dead End", .ldesc = "You have come to a dead end in the maze.", .flags = kFl_DEAD_END_3, .exits = kEx_DEAD_END_3, .line = 1629},
    {.def_index = 154, .name = "MAZE-9", .desc = "Maze", .ldesc = "This is part of a maze of twisty little passages, all alike.", .flags = kFl_MAZE_9, .exits = kEx_MAZE_9, .line = 1636},
    {.def_index = 155, .name = "MAZE-10", .desc = "Maze", .ldesc = "This is part of a maze of twisty little passages, all alike.", .flags = kFl_MAZE_10, .exits = kEx_MAZE_10, .line = 1648},
    {.def_index = 156, .name = "MAZE-11", .desc = "Maze", .ldesc = "This is part of a maze of twisty little passages, all alike.", .flags = kFl_MAZE_11, .exits = kEx_MAZE_11, .line = 1657},
    {.def_index = 157, .name = "GRATING-ROOM", .desc = "Grating Room", .flags = kFl_GRATING_ROOM, .action = "MAZE-11-FCN", .globals = kGl_GRATING_ROOM, .exits = kEx_GRATING_ROOM, .line = 1667},
    {.def_index = 158, .name = "MAZE-12", .desc = "Maze", .ldesc = "This is part of a maze of twisty little passages, all alike.", .flags = kFl_MAZE_12, .exits = kEx_MAZE_12, .line = 1677},
    {.def_index = 159, .name = "DEAD-END-4", .desc = "Dead End", .ldesc = "You have come to a dead end in the maze.", .flags = kFl_DEAD_END_4, .exits = kEx_DEAD_END_4, .line = 1688},
    {.def_index = 160, .name = "MAZE-13", .desc = "Maze", .ldesc = "This is part of a maze of twisty little passages, all alike.", .flags = kFl_MAZE_13, .exits = kEx_MAZE_13, .line = 1695},
    {.def_index = 161, .name = "MAZE-14", .desc = "Maze", .ldesc = "This is part of a maze of twisty little passages, all alike.", .flags = kFl_MAZE_14, .exits = kEx_MAZE_14, .line = 1705},
    {.def_index = 162, .name = "MAZE-15", .desc = "Maze", .ldesc = "This is part of a maze of twisty little passages, all alike.", .flags = kFl_MAZE_15, .exits = kEx_MAZE_15, .line = 1715},
    {.def_index = 163, .name = "CYCLOPS-ROOM", .desc = "Cyclops Room", .flags = kFl_CYCLOPS_ROOM, .action = "CYCLOPS-ROOM-FCN", .globals = kGl_CYCLOPS_ROOM, .exits = kEx_CYCLOPS_ROOM, .line = 1728},
    {.def_index = 164, .name = "STRANGE-PASSAGE", .desc = "Strange Passage", .ldesc = "This is a long passage. To the west is one entrance. On the east there is an old wooden door, with a large opening in it (about cyclops sized).", .flags = kFl_STRANGE_PASSAGE, .exits = kEx_STRANGE_PASSAGE, .line = 1740},
    {.def_index = 165, .name = "TREASURE-ROOM", .desc = "Treasure Room", .ldesc = "This is a large room, whose east wall is solid granite. A number of discarded bags, which crumble at your touch, are scattered about on the floor. There is an exit down a staircase.", .flags = kFl_TREASURE_ROOM, .value = 25, .action = "TREASURE-ROOM-FCN", .globals = kGl_TREASURE_ROOM, .exits = kEx_TREASURE_ROOM, .line = 1752},
    {.def_index = 166, .name = "RESERVOIR-SOUTH", .desc = "Reservoir South", .flags = kFl_RESERVOIR_SOUTH, .action = "RESERVOIR-SOUTH-FCN", .globals = kGl_RESERVOIR_SOUTH, .pseudo = kPs_RESERVOIR_SOUTH, .exits = kEx_RESERVOIR_SOUTH, .line = 1769},
    {.def_index = 167, .name = "RESERVOIR", .desc = "Reservoir", .flags = kFl_RESERVOIR, .action = "RESERVOIR-FCN", .globals = kGl_RESERVOIR, .pseudo = kPs_RESERVOIR, .exits = kEx_RESERVOIR, .line = 1783},
    {.def_index = 168, .name = "RESERVOIR-NORTH", .desc = "Reservoir North", .flags = kFl_RESERVOIR_NORTH, .action = "RESERVOIR-NORTH-FCN", .globals = kGl_RESERVOIR_NORTH, .pseudo = kPs_RESERVOIR_NORTH, .exits = kEx_RESERVOIR_NORTH, .line = 1796},
    {.def_index = 169, .name = "STREAM-VIEW", .desc = "Stream View", .ldesc = "You are standing on a path beside a gently flowing stream. The path follows the stream, which flows from west to east.", .flags = kFl_STREAM_VIEW, .globals = kGl_STREAM_VIEW, .pseudo = kPs_STREAM_VIEW, .exits = kEx_STREAM_VIEW, .line = 1807},
    {.def_index = 170, .name = "IN-STREAM", .desc = "Stream", .ldesc = "You are on the gently flowing stream. The upstream route is too narrow to navigate, and the downstream route is invisible due to twisting walls. There is a narrow beach to land on.", .flags = kFl_IN_STREAM, .globals = kGl_IN_STREAM, .pseudo = kPs_IN_STREAM, .exits = kEx_IN_STREAM, .line = 1819},
    {.def_index = 171, .name = "MIRROR-ROOM-1", .desc = "Mirror Room", .flags = kFl_MIRROR_ROOM_1, .action = "MIRROR-ROOM", .exits = kEx_MIRROR_ROOM_1, .line = 1839},
    {.def_index = 172, .name = "MIRROR-ROOM-2", .desc = "Mirror Room", .flags = kFl_MIRROR_ROOM_2, .action = "MIRROR-ROOM", .exits = kEx_MIRROR_ROOM_2, .line = 1848},
    {.def_index = 173, .name = "SMALL-CAVE", .desc = "Cave", .ldesc = "This is a tiny cave with entrances west and north, and a staircase leading down.", .flags = kFl_SMALL_CAVE, .globals = kGl_SMALL_CAVE, .exits = kEx_SMALL_CAVE, .line = 1857},
    {.def_index = 174, .name = "TINY-CAVE", .desc = "Cave", .ldesc = "This is a tiny cave with entrances west and north, and a dark, forbidding staircase leading down.", .flags = kFl_TINY_CAVE, .action = "CAVE2-ROOM", .globals = kGl_TINY_CAVE, .exits = kEx_TINY_CAVE, .line = 1870},
    {.def_index = 175, .name = "COLD-PASSAGE", .desc = "Cold Passage", .ldesc = "This is a cold and damp corridor where a long east-west passageway turns into a southward path.", .flags = kFl_COLD_PASSAGE, .exits = kEx_COLD_PASSAGE, .line = 1883},
    {.def_index = 176, .name = "NARROW-PASSAGE", .desc = "Narrow Passage", .ldesc = "This is a long and narrow corridor where a long north-south passageway briefly narrows even further.", .flags = kFl_NARROW_PASSAGE, .exits = kEx_NARROW_PASSAGE, .line = 1893},
    {.def_index = 177, .name = "WINDING-PASSAGE", .desc = "Winding Passage", .ldesc = "This is a winding passage. It seems that there are only exits on the east and north.", .flags = kFl_WINDING_PASSAGE, .exits = kEx_WINDING_PASSAGE, .line = 1903},
    {.def_index = 178, .name = "TWISTING-PASSAGE", .desc = "Twisting Passage", .ldesc = "This is a winding passage. It seems that there are only exits on the east and north.", .flags = kFl_TWISTING_PASSAGE, .exits = kEx_TWISTING_PASSAGE, .line = 1913},
    {.def_index = 179, .name = "ATLANTIS-ROOM", .desc = "Atlantis Room", .ldesc = "This is an ancient room, long under water. There is an exit to the south and a staircase leading up.", .flags = kFl_ATLANTIS_ROOM, .globals = kGl_ATLANTIS_ROOM, .exits = kEx_ATLANTIS_ROOM, .line = 1923},
    {.def_index = 180, .name = "EW-PASSAGE", .desc = "East-West Passage", .ldesc = "This is a narrow east-west passageway. There is a narrow stairway leading down at the north end of the room.", .flags = kFl_EW_PASSAGE, .value = 5, .globals = kGl_EW_PASSAGE, .exits = kEx_EW_PASSAGE, .line = 1938},
    {.def_index = 181, .name = "ROUND-ROOM", .desc = "Round Room", .ldesc = "This is a circular stone room with passages in all directions. Several of them have unfortunately been blocked by cave-ins.", .flags = kFl_ROUND_ROOM, .exits = kEx_ROUND_ROOM, .line = 1952},
    {.def_index = 182, .name = "DEEP-CANYON", .desc = "Deep Canyon", .flags = kFl_DEEP_CANYON, .action = "DEEP-CANYON-F", .globals = kGl_DEEP_CANYON, .exits = kEx_DEEP_CANYON, .line = 1965},
    {.def_index = 183, .name = "DAMP-CAVE", .desc = "Damp Cave", .ldesc = "This cave has exits to the west and east, and narrows to a crack toward the south. The earth is particularly damp here.", .flags = kFl_DAMP_CAVE, .globals = kGl_DAMP_CAVE, .exits = kEx_DAMP_CAVE, .line = 1976},
    {.def_index = 184, .name = "LOUD-ROOM", .desc = "Loud Room", .flags = kFl_LOUD_ROOM, .action = "LOUD-ROOM-FCN", .globals = kGl_LOUD_ROOM, .exits = kEx_LOUD_ROOM, .line = 1988},
    {.def_index = 185, .name = "NS-PASSAGE", .desc = "North-South Passage", .ldesc = "This is a high north-south passage, which forks to the northeast.", .flags = kFl_NS_PASSAGE, .exits = kEx_NS_PASSAGE, .line = 1998},
    {.def_index = 186, .name = "CHASM-ROOM", .desc = "Chasm", .ldesc = "A chasm runs southwest to northeast and the path follows it. You are on the south side of the chasm, where a crack opens into a passage.", .flags = kFl_CHASM_ROOM, .globals = kGl_CHASM_ROOM, .pseudo = kPs_CHASM_ROOM, .exits = kEx_CHASM_ROOM, .line = 2008},
    {.def_index = 187, .name = "ENTRANCE-TO-HADES", .desc = "Entrance to Hades", .flags = kFl_ENTRANCE_TO_HADES, .action = "LLD-ROOM", .globals = kGl_ENTRANCE_TO_HADES, .pseudo = kPs_ENTRANCE_TO_HADES, .exits = kEx_ENTRANCE_TO_HADES, .line = 2027},
    {.def_index = 188, .name = "LAND-OF-LIVING-DEAD", .desc = "Land of the Dead", .ldesc = "You have entered the Land of the Living Dead. Thousands of lost souls can be heard weeping and moaning. In the corner are stacked the remains of dozens of previous adventurers less fortunate than yourself. A passage exits to the north.", .flags = kFl_LAND_OF_LIVING_DEAD, .globals = kGl_LAND_OF_LIVING_DEAD, .exits = kEx_LAND_OF_LIVING_DEAD, .line = 2040},
    {.def_index = 189, .name = "ENGRAVINGS-CAVE", .desc = "Engravings Cave", .ldesc = "You have entered a low cave with passages leading northwest and east.", .flags = kFl_ENGRAVINGS_CAVE, .exits = kEx_ENGRAVINGS_CAVE, .line = 2057},
    {.def_index = 190, .name = "EGYPT-ROOM", .desc = "Egyptian Room", .ldesc = "This is a room which looks like an Egyptian tomb. There is an ascending staircase to the west.", .flags = kFl_EGYPT_ROOM, .globals = kGl_EGYPT_ROOM, .exits = kEx_EGYPT_ROOM, .line = 2066},
    {.def_index = 191, .name = "DOME-ROOM", .desc = "Dome Room", .flags = kFl_DOME_ROOM, .action = "DOME-ROOM-FCN", .pseudo = kPs_DOME_ROOM, .exits = kEx_DOME_ROOM, .line = 2077},
    {.def_index = 192, .name = "TORCH-ROOM", .desc = "Torch Room", .flags = kFl_TORCH_ROOM, .action = "TORCH-ROOM-FCN", .globals = kGl_TORCH_ROOM, .pseudo = kPs_TORCH_ROOM, .exits = kEx_TORCH_ROOM, .line = 2087},
    {.def_index = 193, .name = "NORTH-TEMPLE", .desc = "Temple", .ldesc = "This is the north end of a large temple. On the east wall is an ancient inscription, probably a prayer in a long-forgotten language. Below the prayer is a staircase leading down. The west wall is solid granite. The exit to the north end of the room is through huge marble pillars.", .flags = kFl_NORTH_TEMPLE, .globals = kGl_NORTH_TEMPLE, .exits = kEx_NORTH_TEMPLE, .line = 2098},
    {.def_index = 194, .name = "SOUTH-TEMPLE", .desc = "Altar", .ldesc = "This is the south end of a large temple. In front of you is what appears to be an altar. In one corner is a small hole in the floor which leads into darkness. You probably could not get back up it.", .flags = kFl_SOUTH_TEMPLE, .action = "SOUTH-TEMPLE-FCN", .exits = kEx_SOUTH_TEMPLE, .line = 2116},
    {.def_index = 195, .name = "DAM-ROOM", .desc = "Dam", .flags = kFl_DAM_ROOM, .action = "DAM-ROOM-FCN", .globals = kGl_DAM_ROOM, .exits = kEx_DAM_ROOM, .line = 2135},
    {.def_index = 196, .name = "DAM-LOBBY", .desc = "Dam Lobby", .ldesc = "This room appears to have been the waiting room for groups touring the dam. There are open doorways here to the north and east marked \"Private\", and there is a path leading south over the top of the dam.", .flags = kFl_DAM_LOBBY, .exits = kEx_DAM_LOBBY, .line = 2147},
    {.def_index = 197, .name = "MAINTENANCE-ROOM", .desc = "Maintenance Room", .ldesc = "This is what appears to have been the maintenance room for Flood Control Dam #3. Apparently, this room has been ransacked recently, for most of the valuable equipment is gone. On the wall in front of you is a group of buttons colored blue, yellow, brown, and red. There are doorways to the west and south.", .flags = kFl_MAINTENANCE_ROOM, .exits = kEx_MAINTENANCE_ROOM, .line = 2159},
    {.def_index = 198, .name = "DAM-BASE", .desc = "Dam Base", .ldesc = "You are at the base of Flood Control Dam #3, which looms above you and to the north. The river Frigid is flowing by here. Along the river are the White Cliffs which seem to form giant walls stretching from north to south along the shores of the river as it winds its way downstream.", .flags = kFl_DAM_BASE, .globals = kGl_DAM_BASE, .exits = kEx_DAM_BASE, .line = 2176},
    {.def_index = 199, .name = "RIVER-1", .desc = "Frigid River", .ldesc = "You are on the Frigid River in the vicinity of the Dam. The river flows quietly here. There is a landing on the west shore.", .flags = kFl_RIVER_1, .globals = kGl_RIVER_1, .exits = kEx_RIVER_1, .line = 2190},
    {.def_index = 200, .name = "RIVER-2", .desc = "Frigid River", .ldesc = "The river turns a corner here making it impossible to see the Dam. The White Cliffs loom on the east bank and large rocks prevent landing on the west.", .flags = kFl_RIVER_2, .globals = kGl_RIVER_2, .exits = kEx_RIVER_2, .line = 2204},
    {.def_index = 201, .name = "RIVER-3", .desc = "Frigid River", .ldesc = "The river descends here into a valley. There is a narrow beach on the west shore below the cliffs. In the distance a faint rumbling can be heard.", .flags = kFl_RIVER_3, .globals = kGl_RIVER_3, .exits = kEx_RIVER_3, .line = 2219},
    {.def_index = 202, .name = "WHITE-CLIFFS-NORTH", .desc = "White Cliffs Beach", .ldesc = "You are on a narrow strip of beach which runs along the base of the White Cliffs. There is a narrow path heading south along the Cliffs and a tight passage leading west into the cliffs themselves.", .flags = kFl_WHITE_CLIFFS_NORTH, .action = "WHITE-CLIFFS-FUNCTION", .globals = kGl_WHITE_CLIFFS_NORTH, .exits = kEx_WHITE_CLIFFS_NORTH, .line = 2233},
    {.def_index = 203, .name = "WHITE-CLIFFS-SOUTH", .desc = "White Cliffs Beach", .ldesc = "You are on a rocky, narrow strip of beach beside the Cliffs. A narrow path leads north along the shore.", .flags = kFl_WHITE_CLIFFS_SOUTH, .action = "WHITE-CLIFFS-FUNCTION", .globals = kGl_WHITE_CLIFFS_SOUTH, .exits = kEx_WHITE_CLIFFS_SOUTH, .line = 2246},
    {.def_index = 204, .name = "RIVER-4", .desc = "Frigid River", .ldesc = "The river is running faster here and the sound ahead appears to be that of rushing water. On the east shore is a sandy beach. A small area of beach can also be seen below the cliffs on the west shore.", .flags = kFl_RIVER_4, .action = "RIVR4-ROOM", .globals = kGl_RIVER_4, .exits = kEx_RIVER_4, .line = 2258},
    {.def_index = 205, .name = "RIVER-5", .desc = "Frigid River", .ldesc = "The sound of rushing water is nearly unbearable here. On the east shore is a large landing area.", .flags = kFl_RIVER_5, .globals = kGl_RIVER_5, .exits = kEx_RIVER_5, .line = 2274},
    {.def_index = 206, .name = "SHORE", .desc = "Shore", .ldesc = "You are on the east shore of the river. The water here seems somewhat treacherous. A path travels from north to south here, the south end quickly turning around a sharp corner.", .flags = kFl_SHORE, .globals = kGl_SHORE, .exits = kEx_SHORE, .line = 2286},
    {.def_index = 207, .name = "SANDY-BEACH", .desc = "Sandy Beach", .ldesc = "You are on a large sandy beach on the east shore of the river, which is flowing quickly by. A path runs beside the river to the south here, and a passage is partially buried in sand to the northeast.", .flags = kFl_SANDY_BEACH, .globals = kGl_SANDY_BEACH, .exits = kEx_SANDY_BEACH, .line = 2298},
    {.def_index = 208, .name = "SANDY-CAVE", .desc = "Sandy Cave", .ldesc = "This is a sand-filled cave whose exit is to the southwest.", .flags = kFl_SANDY_CAVE, .exits = kEx_SANDY_CAVE, .line = 2311},
    {.def_index = 209, .name = "ARAGAIN-FALLS", .desc = "Aragain Falls", .flags = kFl_ARAGAIN_FALLS, .action = "FALLS-ROOM", .globals = kGl_ARAGAIN_FALLS, .exits = kEx_ARAGAIN_FALLS, .line = 2319},
    {.def_index = 210, .name = "ON-RAINBOW", .desc = "On the Rainbow", .ldesc = "You are on top of a rainbow (I bet you never thought you would walk on a rainbow), with a magnificent view of the Falls. The rainbow travels east-west here.", .flags = kFl_ON_RAINBOW, .globals = kGl_ON_RAINBOW, .exits = kEx_ON_RAINBOW, .line = 2330},
    {.def_index = 211, .name = "END-OF-RAINBOW", .desc = "End of Rainbow", .ldesc = "You are on a small, rocky beach on the continuation of the Frigid River past the Falls. The beach is narrow due to the presence of the White Cliffs. The river canyon opens here and sunlight shines in from above. A rainbow crosses over the falls to the east and a narrow path continues to the southwest.", .flags = kFl_END_OF_RAINBOW, .globals = kGl_END_OF_RAINBOW, .exits = kEx_END_OF_RAINBOW, .line = 2342},
    {.def_index = 212, .name = "CANYON-BOTTOM", .desc = "Canyon Bottom", .ldesc = "You are beneath the walls of the river canyon which may be climbable here. The lesser part of the runoff of Aragain Falls flows by below. To the north is a narrow path.", .flags = kFl_CANYON_BOTTOM, .globals = kGl_CANYON_BOTTOM, .exits = kEx_CANYON_BOTTOM, .line = 2358},
    {.def_index = 213, .name = "CLIFF-MIDDLE", .desc = "Rocky Ledge", .ldesc = "You are on a ledge about halfway up the wall of the river canyon. You can see from here that the main flow from Aragain Falls twists along a passage which it is impossible for you to enter. Below you is the canyon bottom. Above you is more cliff, which appears climbable.", .flags = kFl_CLIFF_MIDDLE, .globals = kGl_CLIFF_MIDDLE, .exits = kEx_CLIFF_MIDDLE, .line = 2370},
    {.def_index = 214, .name = "CANYON-VIEW", .desc = "Canyon View", .ldesc = "You are at the top of the Great Canyon on its west wall. From here there is a marvelous view of the canyon and parts of the Frigid River upstream. Across the canyon, the walls of the White Cliffs join the mighty ramparts of the Flathead Mountains to the east. Following the Canyon upstream to the north, Aragain Falls may be seen, complete with rainbow. The mighty Frigid River flows out from a great dark cavern. To the west and south can be seen an immense forest, stretching for miles around. A path leads northwest. It is possible to climb down into the canyon from here.", .flags = kFl_CANYON_VIEW, .action = "CANYON-VIEW-F", .globals = kGl_CANYON_VIEW, .exits = kEx_CANYON_VIEW, .line = 2384},
    {.def_index = 215, .name = "MINE-ENTRANCE", .desc = "Mine Entrance", .ldesc = "You are standing at the entrance of what might have been a coal mine. The shaft enters the west wall, and there is another exit on the south end of the room.", .flags = kFl_MINE_ENTRANCE, .exits = kEx_MINE_ENTRANCE, .line = 2418},
    {.def_index = 216, .name = "SQUEEKY-ROOM", .desc = "Squeaky Room", .ldesc = "You are in a small room. Strange squeaky sounds may be heard coming from the passage at the north end. You may also escape to the east.", .flags = kFl_SQUEEKY_ROOM, .exits = kEx_SQUEEKY_ROOM, .line = 2431},
    {.def_index = 217, .name = "BAT-ROOM", .desc = "Bat Room", .flags = kFl_BAT_ROOM, .action = "BATS-ROOM", .exits = kEx_BAT_ROOM, .line = 2441},
    {.def_index = 218, .name = "SHAFT-ROOM", .desc = "Shaft Room", .ldesc = "This is a large room, in the middle of which is a small shaft descending through the floor into darkness below. To the west and the north are exits from this room. Constructed over the top of the shaft is a metal framework to which a heavy iron chain is attached.", .flags = kFl_SHAFT_ROOM, .pseudo = kPs_SHAFT_ROOM, .exits = kEx_SHAFT_ROOM, .line = 2449},
    {.def_index = 219, .name = "SMELLY-ROOM", .desc = "Smelly Room", .ldesc = "This is a small nondescript room. However, from the direction of a small descending staircase a foul odor can be detected. To the south is a narrow tunnel.", .flags = kFl_SMELLY_ROOM, .globals = kGl_SMELLY_ROOM, .pseudo = kPs_SMELLY_ROOM, .exits = kEx_SMELLY_ROOM, .line = 2463},
    {.def_index = 220, .name = "GAS-ROOM", .desc = "Gas Room", .ldesc = "This is a small room which smells strongly of coal gas. There is a short climb up some stairs and a narrow tunnel leading east.", .flags = kFl_GAS_ROOM, .action = "BOOM-ROOM", .globals = kGl_GAS_ROOM, .pseudo = kPs_GAS_ROOM, .exits = kEx_GAS_ROOM, .line = 2476},
    {.def_index = 221, .name = "LADDER-TOP", .desc = "Ladder Top", .ldesc = "This is a very small room. In the corner is a rickety wooden ladder, leading downward. It might be safe to descend. There is also a staircase leading upward.", .flags = kFl_LADDER_TOP, .globals = kGl_LADDER_TOP, .exits = kEx_LADDER_TOP, .line = 2489},
    {.def_index = 222, .name = "LADDER-BOTTOM", .desc = "Ladder Bottom", .ldesc = "This is a rather wide room. On one side is the bottom of a narrow wooden ladder. To the west and the south are passages leaving the room.", .flags = kFl_LADDER_BOTTOM, .globals = kGl_LADDER_BOTTOM, .exits = kEx_LADDER_BOTTOM, .line = 2501},
    {.def_index = 223, .name = "DEAD-END-5", .desc = "Dead End", .ldesc = "You have come to a dead end in the mine.", .flags = kFl_DEAD_END_5, .exits = kEx_DEAD_END_5, .line = 2514},
    {.def_index = 224, .name = "TIMBER-ROOM", .desc = "Timber Room", .ldesc = "This is a long and narrow passage, which is cluttered with broken timbers. A wide passage comes from the east and turns at the west end of the room into a very narrow passageway. From the west comes a strong draft.", .flags = kFl_TIMBER_ROOM, .action = "NO-OBJS", .exits = kEx_TIMBER_ROOM, .line = 2521},
    {.def_index = 225, .name = "LOWER-SHAFT", .desc = "Drafty Room", .ldesc = "This is a small drafty room in which is the bottom of a long shaft. To the south is a passageway and to the east a very narrow passage. In the shaft can be seen a heavy iron chain.", .flags = kFl_LOWER_SHAFT, .action = "NO-OBJS", .pseudo = kPs_LOWER_SHAFT, .exits = kEx_LOWER_SHAFT, .line = 2536},
    {.def_index = 226, .name = "MACHINE-ROOM", .desc = "Machine Room", .flags = kFl_MACHINE_ROOM, .action = "MACHINE-ROOM-FCN", .exits = kEx_MACHINE_ROOM, .line = 2554},
    {.def_index = 227, .name = "MINE-1", .desc = "Coal Mine", .ldesc = "This is a nondescript part of a coal mine.", .flags = kFl_MINE_1, .exits = kEx_MINE_1, .line = 2565},
    {.def_index = 228, .name = "MINE-2", .desc = "Coal Mine", .ldesc = "This is a nondescript part of a coal mine.", .flags = kFl_MINE_2, .exits = kEx_MINE_2, .line = 2574},
    {.def_index = 229, .name = "MINE-3", .desc = "Coal Mine", .ldesc = "This is a nondescript part of a coal mine.", .flags = kFl_MINE_3, .exits = kEx_MINE_3, .line = 2583},
    {.def_index = 230, .name = "MINE-4", .desc = "Coal Mine", .ldesc = "This is a nondescript part of a coal mine.", .flags = kFl_MINE_4, .exits = kEx_MINE_4, .line = 2592},
    {.def_index = 231, .name = "SLIDE-ROOM", .desc = "Slide Room", .ldesc = "This is a small chamber, which appears to have been part of a coal mine. On the south wall of the chamber the letters \"Granite Wall\" are etched in the rock. To the east is a long passage, and there is a steep metal slide twisting downward. To the north is a small opening.", .flags = kFl_SLIDE_ROOM, .globals = kGl_SLIDE_ROOM, .exits = kEx_SLIDE_ROOM, .line = 2601},
};

constexpr std::string_view kFl_BOARD[] = {"NDESCBIT"};
constexpr Word kSyn_BOARD[] = {{"BOARDS", "boards"}, {"BOARD", "board"}};
constexpr std::string_view kFl_TEETH[] = {"NDESCBIT"};
constexpr Word kSyn_TEETH[] = {{"OVERBOARD", "overbo"}, {"TEETH", "teeth"}};
constexpr Word kSyn_WALL[] = {{"WALL", "wall"}, {"WALLS", "walls"}};
constexpr Word kAdj_WALL[] = {{"SURROUNDING", "surrou"}};
constexpr Word kSyn_GRANITE_WALL[] = {{"WALL", "wall"}};
constexpr Word kAdj_GRANITE_WALL[] = {{"GRANITE", "granit"}};
constexpr std::string_view kFl_SONGBIRD[] = {"NDESCBIT"};
constexpr Word kSyn_SONGBIRD[] = {{"BIRD", "bird"}, {"SONGBIRD", "songbi"}};
constexpr Word kAdj_SONGBIRD[] = {{"SONG", "song"}};
constexpr std::string_view kFl_WHITE_HOUSE[] = {"NDESCBIT"};
constexpr Word kSyn_WHITE_HOUSE[] = {{"HOUSE", "house"}};
constexpr Word kAdj_WHITE_HOUSE[] = {{"WHITE", "white"}, {"BEAUTI", "beauti"}, {"COLONI", "coloni"}};
constexpr std::string_view kFl_FOREST[] = {"NDESCBIT"};
constexpr Word kSyn_FOREST[] = {{"FOREST", "forest"}, {"TREES", "trees"}, {"PINES", "pines"}, {"HEMLOCKS", "hemloc"}};
constexpr std::string_view kFl_TREE[] = {"NDESCBIT", "CLIMBBIT"};
constexpr Word kSyn_TREE[] = {{"TREE", "tree"}, {"BRANCH", "branch"}};
constexpr Word kAdj_TREE[] = {{"LARGE", "large"}, {"STORM", "storm"}};
constexpr std::string_view kFl_MOUNTAIN_RANGE[] = {"NDESCBIT", "CLIMBBIT"};
constexpr Word kSyn_MOUNTAIN_RANGE[] = {{"MOUNTAIN", "mounta"}, {"RANGE", "range"}};
constexpr Word kAdj_MOUNTAIN_RANGE[] = {{"IMPASSABLE", "impass"}, {"FLATHEAD", "flathe"}};
constexpr std::string_view kFl_GLOBAL_WATER[] = {"DRINKBIT"};
constexpr Word kSyn_GLOBAL_WATER[] = {{"WATER", "water"}, {"QUANTITY", "quanti"}};
constexpr std::string_view kFl_WATER[] = {"TRYTAKEBIT", "TAKEBIT", "DRINKBIT"};
constexpr Word kSyn_WATER[] = {{"WATER", "water"}, {"QUANTITY", "quanti"}, {"LIQUID", "liquid"}, {"H2O", "h2o"}};
constexpr std::string_view kFl_KITCHEN_WINDOW[] = {"DOORBIT", "NDESCBIT"};
constexpr Word kSyn_KITCHEN_WINDOW[] = {{"WINDOW", "window"}};
constexpr Word kAdj_KITCHEN_WINDOW[] = {{"KITCHEN", "kitche"}, {"SMALL", "small"}};
constexpr std::string_view kFl_CHIMNEY[] = {"CLIMBBIT", "NDESCBIT"};
constexpr Word kSyn_CHIMNEY[] = {{"CHIMNEY", "chimne"}};
constexpr Word kAdj_CHIMNEY[] = {{"DARK", "dark"}, {"NARROW", "narrow"}};
constexpr std::string_view kFl_GHOSTS[] = {"ACTORBIT", "NDESCBIT"};
constexpr Word kSyn_GHOSTS[] = {{"GHOSTS", "ghosts"}, {"SPIRITS", "spirit"}, {"FIENDS", "fiends"}, {"FORCE", "force"}};
constexpr Word kAdj_GHOSTS[] = {{"INVISIBLE", "invisi"}, {"EVIL", "evil"}};
constexpr std::string_view kFl_SKULL[] = {"TAKEBIT"};
constexpr Word kSyn_SKULL[] = {{"SKULL", "skull"}, {"HEAD", "head"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_SKULL[] = {{"CRYSTAL", "crysta"}};
constexpr std::string_view kFl_LOWERED_BASKET[] = {"TRYTAKEBIT"};
constexpr Word kSyn_LOWERED_BASKET[] = {{"CAGE", "cage"}, {"DUMBWAITER", "dumbwa"}, {"BASKET", "basket"}};
constexpr Word kAdj_LOWERED_BASKET[] = {{"LOWERED", "lowere"}};
constexpr std::string_view kFl_RAISED_BASKET[] = {"TRANSBIT", "TRYTAKEBIT", "CONTBIT", "OPENBIT"};
constexpr Word kSyn_RAISED_BASKET[] = {{"CAGE", "cage"}, {"DUMBWAITER", "dumbwa"}, {"BASKET", "basket"}};
constexpr std::string_view kFl_LUNCH[] = {"TAKEBIT", "FOODBIT"};
constexpr Word kSyn_LUNCH[] = {{"FOOD", "food"}, {"SANDWICH", "sandwi"}, {"LUNCH", "lunch"}, {"DINNER", "dinner"}};
constexpr Word kAdj_LUNCH[] = {{"HOT", "hot"}, {"PEPPER", "pepper"}};
constexpr std::string_view kFl_BAT[] = {"ACTORBIT", "TRYTAKEBIT"};
constexpr Word kSyn_BAT[] = {{"BAT", "bat"}, {"VAMPIRE", "vampir"}};
constexpr Word kAdj_BAT[] = {{"VAMPIRE", "vampir"}, {"DERANGED", "derang"}};
constexpr std::string_view kFl_BELL[] = {"TAKEBIT"};
constexpr Word kSyn_BELL[] = {{"BELL", "bell"}};
constexpr Word kAdj_BELL[] = {{"SMALL", "small"}, {"BRASS", "brass"}};
constexpr std::string_view kFl_HOT_BELL[] = {"TRYTAKEBIT"};
constexpr Word kSyn_HOT_BELL[] = {{"BELL", "bell"}};
constexpr Word kAdj_HOT_BELL[] = {{"BRASS", "brass"}, {"HOT", "hot"}, {"RED", "red"}, {"SMALL", "small"}};
constexpr std::string_view kFl_AXE[] = {"WEAPONBIT", "TRYTAKEBIT", "TAKEBIT", "NDESCBIT"};
constexpr Word kSyn_AXE[] = {{"AXE", "axe"}, {"AX", "ax"}};
constexpr Word kAdj_AXE[] = {{"BLOODY", "bloody"}};
constexpr std::string_view kFl_BOLT[] = {"NDESCBIT", "TURNBIT", "TRYTAKEBIT"};
constexpr Word kSyn_BOLT[] = {{"BOLT", "bolt"}, {"NUT", "nut"}};
constexpr Word kAdj_BOLT[] = {{"METAL", "metal"}, {"LARGE", "large"}};
constexpr std::string_view kFl_BUBBLE[] = {"NDESCBIT", "TRYTAKEBIT"};
constexpr Word kSyn_BUBBLE[] = {{"BUBBLE", "bubble"}};
constexpr Word kAdj_BUBBLE[] = {{"SMALL", "small"}, {"GREEN", "green"}, {"PLASTIC", "plasti"}};
constexpr std::string_view kFl_ALTAR[] = {"NDESCBIT", "SURFACEBIT", "CONTBIT", "OPENBIT"};
constexpr Word kSyn_ALTAR[] = {{"ALTAR", "altar"}};
constexpr std::string_view kFl_BOOK[] = {"READBIT", "TAKEBIT", "CONTBIT", "BURNBIT", "TURNBIT"};
constexpr Word kSyn_BOOK[] = {{"BOOK", "book"}, {"PRAYER", "prayer"}, {"PAGE", "page"}, {"BOOKS", "books"}};
constexpr Word kAdj_BOOK[] = {{"LARGE", "large"}, {"BLACK", "black"}};
constexpr std::string_view kFl_BROKEN_LAMP[] = {"TAKEBIT"};
constexpr Word kSyn_BROKEN_LAMP[] = {{"LAMP", "lamp"}, {"LANTERN", "lanter"}};
constexpr Word kAdj_BROKEN_LAMP[] = {{"BROKEN", "broken"}};
constexpr std::string_view kFl_SCEPTRE[] = {"TAKEBIT", "WEAPONBIT"};
constexpr Word kSyn_SCEPTRE[] = {{"SCEPTRE", "sceptr"}, {"SCEPTER", "scepte"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_SCEPTRE[] = {{"SHARP", "sharp"}, {"EGYPTIAN", "egypti"}, {"ANCIENT", "ancien"}, {"ENAMELED", "enamel"}};
constexpr std::string_view kFl_TIMBERS[] = {"TAKEBIT"};
constexpr Word kSyn_TIMBERS[] = {{"TIMBERS", "timber"}, {"PILE", "pile"}};
constexpr Word kAdj_TIMBERS[] = {{"WOODEN", "wooden"}, {"BROKEN", "broken"}};
constexpr std::string_view kFl_SLIDE[] = {"CLIMBBIT"};
constexpr Word kSyn_SLIDE[] = {{"CHUTE", "chute"}, {"RAMP", "ramp"}, {"SLIDE", "slide"}};
constexpr Word kAdj_SLIDE[] = {{"STEEP", "steep"}, {"METAL", "metal"}, {"TWISTING", "twisti"}};
constexpr std::string_view kFl_KITCHEN_TABLE[] = {"NDESCBIT", "CONTBIT", "OPENBIT", "SURFACEBIT"};
constexpr Word kSyn_KITCHEN_TABLE[] = {{"TABLE", "table"}};
constexpr Word kAdj_KITCHEN_TABLE[] = {{"KITCHEN", "kitche"}};
constexpr std::string_view kFl_ATTIC_TABLE[] = {"NDESCBIT", "CONTBIT", "OPENBIT", "SURFACEBIT"};
constexpr Word kSyn_ATTIC_TABLE[] = {{"TABLE", "table"}};
constexpr std::string_view kFl_SANDWICH_BAG[] = {"TAKEBIT", "CONTBIT", "BURNBIT"};
constexpr Word kSyn_SANDWICH_BAG[] = {{"BAG", "bag"}, {"SACK", "sack"}};
constexpr Word kAdj_SANDWICH_BAG[] = {{"BROWN", "brown"}, {"ELONGATED", "elonga"}, {"SMELLY", "smelly"}};
constexpr std::string_view kFl_TOOL_CHEST[] = {"CONTBIT", "OPENBIT", "TRYTAKEBIT", "SACREDBIT"};
constexpr Word kSyn_TOOL_CHEST[] = {{"CHEST", "chest"}, {"CHESTS", "chests"}, {"GROUP", "group"}, {"TOOLCHESTS", "toolch"}};
constexpr Word kAdj_TOOL_CHEST[] = {{"TOOL", "tool"}};
constexpr std::string_view kFl_YELLOW_BUTTON[] = {"NDESCBIT"};
constexpr Word kSyn_YELLOW_BUTTON[] = {{"BUTTON", "button"}, {"SWITCH", "switch"}};
constexpr Word kAdj_YELLOW_BUTTON[] = {{"YELLOW", "yellow"}};
constexpr std::string_view kFl_BROWN_BUTTON[] = {"NDESCBIT"};
constexpr Word kSyn_BROWN_BUTTON[] = {{"BUTTON", "button"}, {"SWITCH", "switch"}};
constexpr Word kAdj_BROWN_BUTTON[] = {{"BROWN", "brown"}};
constexpr std::string_view kFl_RED_BUTTON[] = {"NDESCBIT"};
constexpr Word kSyn_RED_BUTTON[] = {{"BUTTON", "button"}, {"SWITCH", "switch"}};
constexpr Word kAdj_RED_BUTTON[] = {{"RED", "red"}};
constexpr std::string_view kFl_BLUE_BUTTON[] = {"NDESCBIT"};
constexpr Word kSyn_BLUE_BUTTON[] = {{"BUTTON", "button"}, {"SWITCH", "switch"}};
constexpr Word kAdj_BLUE_BUTTON[] = {{"BLUE", "blue"}};
constexpr std::string_view kFl_TROPHY_CASE[] = {"TRANSBIT", "CONTBIT", "NDESCBIT", "TRYTAKEBIT", "SEARCHBIT"};
constexpr Word kSyn_TROPHY_CASE[] = {{"CASE", "case"}};
constexpr Word kAdj_TROPHY_CASE[] = {{"TROPHY", "trophy"}};
constexpr std::string_view kFl_RUG[] = {"NDESCBIT", "TRYTAKEBIT"};
constexpr Word kSyn_RUG[] = {{"RUG", "rug"}, {"CARPET", "carpet"}};
constexpr Word kAdj_RUG[] = {{"LARGE", "large"}, {"ORIENTAL", "orient"}};
constexpr std::string_view kFl_CHALICE[] = {"TAKEBIT", "TRYTAKEBIT", "CONTBIT"};
constexpr Word kSyn_CHALICE[] = {{"CHALICE", "chalic"}, {"CUP", "cup"}, {"SILVER", "silver"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_CHALICE[] = {{"SILVER", "silver"}, {"ENGRAVINGS", "engrav"}};
constexpr std::string_view kFl_GARLIC[] = {"TAKEBIT", "FOODBIT"};
constexpr Word kSyn_GARLIC[] = {{"GARLIC", "garlic"}, {"CLOVE", "clove"}};
constexpr std::string_view kFl_TRIDENT[] = {"TAKEBIT"};
constexpr Word kSyn_TRIDENT[] = {{"TRIDENT", "triden"}, {"FORK", "fork"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_TRIDENT[] = {{"POSEIDON", "poseid"}, {"OWN", "own"}, {"CRYSTAL", "crysta"}};
constexpr std::string_view kFl_CYCLOPS[] = {"ACTORBIT", "NDESCBIT", "TRYTAKEBIT"};
constexpr Word kSyn_CYCLOPS[] = {{"CYCLOPS", "cyclop"}, {"MONSTER", "monste"}, {"EYE", "eye"}};
constexpr Word kAdj_CYCLOPS[] = {{"HUNGRY", "hungry"}, {"GIANT", "giant"}};
constexpr std::string_view kFl_DAM[] = {"NDESCBIT", "TRYTAKEBIT"};
constexpr Word kSyn_DAM[] = {{"DAM", "dam"}, {"GATE", "gate"}, {"GATES", "gates"}, {"FCD#3", "fcd#"}};
constexpr std::string_view kFl_TRAP_DOOR[] = {"DOORBIT", "NDESCBIT", "INVISIBLE"};
constexpr Word kSyn_TRAP_DOOR[] = {{"DOOR", "door"}, {"TRAPDOOR", "trapdo"}, {"TRAP-DOOR", "trap-"}, {"COVER", "cover"}};
constexpr Word kAdj_TRAP_DOOR[] = {{"TRAP", "trap"}, {"DUSTY", "dusty"}};
constexpr std::string_view kFl_BOARDED_WINDOW[] = {"NDESCBIT"};
constexpr Word kSyn_BOARDED_WINDOW[] = {{"WINDOW", "window"}};
constexpr Word kAdj_BOARDED_WINDOW[] = {{"BOARDED", "boarde"}};
constexpr std::string_view kFl_FRONT_DOOR[] = {"DOORBIT", "NDESCBIT"};
constexpr Word kSyn_FRONT_DOOR[] = {{"DOOR", "door"}};
constexpr Word kAdj_FRONT_DOOR[] = {{"FRONT", "front"}, {"BOARDED", "boarde"}};
constexpr std::string_view kFl_BARROW_DOOR[] = {"DOORBIT", "NDESCBIT", "OPENBIT"};
constexpr Word kSyn_BARROW_DOOR[] = {{"DOOR", "door"}};
constexpr Word kAdj_BARROW_DOOR[] = {{"HUGE", "huge"}, {"STONE", "stone"}};
constexpr std::string_view kFl_BARROW[] = {"NDESCBIT"};
constexpr Word kSyn_BARROW[] = {{"BARROW", "barrow"}, {"TOMB", "tomb"}};
constexpr Word kAdj_BARROW[] = {{"MASSIVE", "massiv"}, {"STONE", "stone"}};
constexpr std::string_view kFl_BOTTLE[] = {"TAKEBIT", "TRANSBIT", "CONTBIT"};
constexpr Word kSyn_BOTTLE[] = {{"BOTTLE", "bottle"}, {"CONTAINER", "contai"}};
constexpr Word kAdj_BOTTLE[] = {{"CLEAR", "clear"}, {"GLASS", "glass"}};
constexpr std::string_view kFl_CRACK[] = {"NDESCBIT"};
constexpr Word kSyn_CRACK[] = {{"CRACK", "crack"}};
constexpr Word kAdj_CRACK[] = {{"NARROW", "narrow"}};
constexpr std::string_view kFl_COFFIN[] = {"TAKEBIT", "CONTBIT", "SACREDBIT", "SEARCHBIT"};
constexpr Word kSyn_COFFIN[] = {{"COFFIN", "coffin"}, {"CASKET", "casket"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_COFFIN[] = {{"SOLID", "solid"}, {"GOLD", "gold"}};
constexpr std::string_view kFl_GRATE[] = {"DOORBIT", "NDESCBIT", "INVISIBLE"};
constexpr Word kSyn_GRATE[] = {{"GRATE", "grate"}, {"GRATING", "gratin"}};
constexpr std::string_view kFl_PUMP[] = {"TAKEBIT", "TOOLBIT"};
constexpr Word kSyn_PUMP[] = {{"PUMP", "pump"}, {"AIR-PUMP", "air-p"}, {"TOOL", "tool"}, {"TOOLS", "tools"}};
constexpr Word kAdj_PUMP[] = {{"SMALL", "small"}, {"HAND-HELD", "hand-"}};
constexpr std::string_view kFl_DIAMOND[] = {"TAKEBIT"};
constexpr Word kSyn_DIAMOND[] = {{"DIAMOND", "diamon"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_DIAMOND[] = {{"HUGE", "huge"}, {"ENORMOUS", "enormo"}};
constexpr std::string_view kFl_JADE[] = {"TAKEBIT"};
constexpr Word kSyn_JADE[] = {{"FIGURINE", "figuri"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_JADE[] = {{"EXQUISITE", "exquis"}, {"JADE", "jade"}};
constexpr std::string_view kFl_KNIFE[] = {"TAKEBIT", "WEAPONBIT", "TRYTAKEBIT"};
constexpr Word kSyn_KNIFE[] = {{"KNIVES", "knives"}, {"KNIFE", "knife"}, {"BLADE", "blade"}};
constexpr Word kAdj_KNIFE[] = {{"NASTY", "nasty"}, {"UNRUSTY", "unrust"}};
constexpr std::string_view kFl_BONES[] = {"TRYTAKEBIT", "NDESCBIT"};
constexpr Word kSyn_BONES[] = {{"BONES", "bones"}, {"SKELETON", "skelet"}, {"BODY", "body"}};
constexpr std::string_view kFl_BURNED_OUT_LANTERN[] = {"TAKEBIT"};
constexpr Word kSyn_BURNED_OUT_LANTERN[] = {{"LANTERN", "lanter"}, {"LAMP", "lamp"}};
constexpr Word kAdj_BURNED_OUT_LANTERN[] = {{"RUSTY", "rusty"}, {"BURNED", "burned"}, {"DEAD", "dead"}, {"USELESS", "useles"}};
constexpr std::string_view kFl_BAG_OF_COINS[] = {"TAKEBIT"};
constexpr Word kSyn_BAG_OF_COINS[] = {{"BAG", "bag"}, {"COINS", "coins"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_BAG_OF_COINS[] = {{"OLD", "old"}, {"LEATHER", "leathe"}};
constexpr std::string_view kFl_LAMP[] = {"TAKEBIT", "LIGHTBIT"};
constexpr Word kSyn_LAMP[] = {{"LAMP", "lamp"}, {"LANTERN", "lanter"}, {"LIGHT", "light"}};
constexpr Word kAdj_LAMP[] = {{"BRASS", "brass"}};
constexpr std::string_view kFl_EMERALD[] = {"TAKEBIT"};
constexpr Word kSyn_EMERALD[] = {{"EMERALD", "emeral"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_EMERALD[] = {{"LARGE", "large"}};
constexpr std::string_view kFl_ADVERTISEMENT[] = {"READBIT", "TAKEBIT", "BURNBIT"};
constexpr Word kSyn_ADVERTISEMENT[] = {{"ADVERTISEMENT", "advert"}, {"LEAFLET", "leafle"}, {"BOOKLET", "bookle"}, {"MAIL", "mail"}};
constexpr Word kAdj_ADVERTISEMENT[] = {{"SMALL", "small"}};
constexpr std::string_view kFl_LEAK[] = {"NDESCBIT", "INVISIBLE"};
constexpr Word kSyn_LEAK[] = {{"LEAK", "leak"}, {"DRIP", "drip"}, {"PIPE", "pipe"}};
constexpr std::string_view kFl_MACHINE[] = {"CONTBIT", "NDESCBIT", "TRYTAKEBIT"};
constexpr Word kSyn_MACHINE[] = {{"MACHINE", "machin"}, {"PDP10", "pdp1"}, {"DRYER", "dryer"}, {"LID", "lid"}};
constexpr std::string_view kFl_INFLATED_BOAT[] = {"TAKEBIT", "BURNBIT", "VEHBIT", "OPENBIT", "SEARCHBIT"};
constexpr Word kSyn_INFLATED_BOAT[] = {{"BOAT", "boat"}, {"RAFT", "raft"}};
constexpr Word kAdj_INFLATED_BOAT[] = {{"INFLAT", "inflat"}, {"MAGIC", "magic"}, {"PLASTIC", "plasti"}, {"SEAWORTHY", "seawor"}};
constexpr std::string_view kFl_MAILBOX[] = {"CONTBIT", "TRYTAKEBIT"};
constexpr Word kSyn_MAILBOX[] = {{"MAILBOX", "mailbo"}, {"BOX", "box"}};
constexpr Word kAdj_MAILBOX[] = {{"SMALL", "small"}};
constexpr std::string_view kFl_MATCH[] = {"READBIT", "TAKEBIT"};
constexpr Word kSyn_MATCH[] = {{"MATCH", "match"}, {"MATCHES", "matche"}, {"MATCHBOOK", "matchb"}};
constexpr Word kAdj_MATCH[] = {{"MATCH", "match"}};
constexpr std::string_view kFl_MIRROR_2[] = {"TRYTAKEBIT", "NDESCBIT"};
constexpr Word kSyn_MIRROR_2[] = {{"REFLECTION", "reflec"}, {"MIRROR", "mirror"}, {"ENORMOUS", "enormo"}};
constexpr std::string_view kFl_MIRROR_1[] = {"TRYTAKEBIT", "NDESCBIT"};
constexpr Word kSyn_MIRROR_1[] = {{"REFLECTION", "reflec"}, {"MIRROR", "mirror"}, {"ENORMOUS", "enormo"}};
constexpr std::string_view kFl_PAINTING[] = {"TAKEBIT", "BURNBIT"};
constexpr Word kSyn_PAINTING[] = {{"PAINTING", "painti"}, {"ART", "art"}, {"CANVAS", "canvas"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_PAINTING[] = {{"BEAUTI", "beauti"}};
constexpr std::string_view kFl_CANDLES[] = {"TAKEBIT", "FLAMEBIT", "ONBIT", "LIGHTBIT"};
constexpr Word kSyn_CANDLES[] = {{"CANDLES", "candle"}, {"PAIR", "pair"}};
constexpr Word kAdj_CANDLES[] = {{"BURNING", "burnin"}};
constexpr std::string_view kFl_GUNK[] = {"TAKEBIT", "TRYTAKEBIT"};
constexpr Word kSyn_GUNK[] = {{"GUNK", "gunk"}, {"PIECE", "piece"}, {"SLAG", "slag"}};
constexpr Word kAdj_GUNK[] = {{"SMALL", "small"}, {"VITREOUS", "vitreo"}};
constexpr std::string_view kFl_BODIES[] = {"NDESCBIT", "TRYTAKEBIT"};
constexpr Word kSyn_BODIES[] = {{"BODIES", "bodies"}, {"BODY", "body"}, {"REMAINS", "remain"}, {"PILE", "pile"}};
constexpr Word kAdj_BODIES[] = {{"MANGLED", "mangle"}};
constexpr std::string_view kFl_LEAVES[] = {"TAKEBIT", "BURNBIT", "TRYTAKEBIT"};
constexpr Word kSyn_LEAVES[] = {{"LEAVES", "leaves"}, {"LEAF", "leaf"}, {"PILE", "pile"}};
constexpr std::string_view kFl_PUNCTURED_BOAT[] = {"TAKEBIT", "BURNBIT"};
constexpr Word kSyn_PUNCTURED_BOAT[] = {{"BOAT", "boat"}, {"PILE", "pile"}, {"PLASTIC", "plasti"}};
constexpr Word kAdj_PUNCTURED_BOAT[] = {{"PLASTIC", "plasti"}, {"PUNCTURE", "punctu"}, {"LARGE", "large"}};
constexpr std::string_view kFl_INFLATABLE_BOAT[] = {"TAKEBIT", "BURNBIT"};
constexpr Word kSyn_INFLATABLE_BOAT[] = {{"BOAT", "boat"}, {"PILE", "pile"}, {"PLASTIC", "plasti"}, {"VALVE", "valve"}};
constexpr Word kAdj_INFLATABLE_BOAT[] = {{"PLASTIC", "plasti"}, {"INFLAT", "inflat"}};
constexpr std::string_view kFl_BAR[] = {"TAKEBIT", "SACREDBIT"};
constexpr Word kSyn_BAR[] = {{"BAR", "bar"}, {"PLATINUM", "platin"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_BAR[] = {{"PLATINUM", "platin"}, {"LARGE", "large"}};
constexpr std::string_view kFl_POT_OF_GOLD[] = {"TAKEBIT", "INVISIBLE"};
constexpr Word kSyn_POT_OF_GOLD[] = {{"POT", "pot"}, {"GOLD", "gold"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_POT_OF_GOLD[] = {{"GOLD", "gold"}};
constexpr std::string_view kFl_PRAYER[] = {"READBIT", "SACREDBIT", "NDESCBIT"};
constexpr Word kSyn_PRAYER[] = {{"PRAYER", "prayer"}, {"INSCRIPTION", "inscri"}};
constexpr Word kAdj_PRAYER[] = {{"ANCIENT", "ancien"}, {"OLD", "old"}};
constexpr std::string_view kFl_RAILING[] = {"NDESCBIT"};
constexpr Word kSyn_RAILING[] = {{"RAILING", "railin"}, {"RAIL", "rail"}};
constexpr Word kAdj_RAILING[] = {{"WOODEN", "wooden"}};
constexpr std::string_view kFl_RAINBOW[] = {"NDESCBIT", "CLIMBBIT"};
constexpr Word kSyn_RAINBOW[] = {{"RAINBOW", "rainbo"}};
constexpr std::string_view kFl_RIVER[] = {"NDESCBIT"};
constexpr Word kSyn_RIVER[] = {{"RIVER", "river"}};
constexpr Word kAdj_RIVER[] = {{"FRIGID", "frigid"}};
constexpr std::string_view kFl_BUOY[] = {"TAKEBIT", "CONTBIT"};
constexpr Word kSyn_BUOY[] = {{"BUOY", "buoy"}};
constexpr Word kAdj_BUOY[] = {{"RED", "red"}};
constexpr std::string_view kFl_ROPE[] = {"TAKEBIT", "SACREDBIT", "TRYTAKEBIT"};
constexpr Word kSyn_ROPE[] = {{"ROPE", "rope"}, {"HEMP", "hemp"}, {"COIL", "coil"}};
constexpr Word kAdj_ROPE[] = {{"LARGE", "large"}};
constexpr std::string_view kFl_RUSTY_KNIFE[] = {"TAKEBIT", "TRYTAKEBIT", "WEAPONBIT", "TOOLBIT"};
constexpr Word kSyn_RUSTY_KNIFE[] = {{"KNIVES", "knives"}, {"KNIFE", "knife"}};
constexpr Word kAdj_RUSTY_KNIFE[] = {{"RUSTY", "rusty"}};
constexpr std::string_view kFl_SAND[] = {"NDESCBIT"};
constexpr Word kSyn_SAND[] = {{"SAND", "sand"}};
constexpr std::string_view kFl_BRACELET[] = {"TAKEBIT"};
constexpr Word kSyn_BRACELET[] = {{"BRACELET", "bracel"}, {"JEWEL", "jewel"}, {"SAPPHIRE", "sapphi"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_BRACELET[] = {{"SAPPHIRE", "sapphi"}};
constexpr std::string_view kFl_SCREWDRIVER[] = {"TAKEBIT", "TOOLBIT"};
constexpr Word kSyn_SCREWDRIVER[] = {{"SCREWDRIVER", "screwd"}, {"TOOL", "tool"}, {"TOOLS", "tools"}, {"DRIVER", "driver"}};
constexpr Word kAdj_SCREWDRIVER[] = {{"SCREW", "screw"}};
constexpr std::string_view kFl_KEYS[] = {"TAKEBIT", "TOOLBIT"};
constexpr Word kSyn_KEYS[] = {{"KEY", "key"}};
constexpr Word kAdj_KEYS[] = {{"SKELETON", "skelet"}};
constexpr std::string_view kFl_SHOVEL[] = {"TAKEBIT", "TOOLBIT"};
constexpr Word kSyn_SHOVEL[] = {{"SHOVEL", "shovel"}, {"TOOL", "tool"}, {"TOOLS", "tools"}};
constexpr std::string_view kFl_COAL[] = {"TAKEBIT", "BURNBIT"};
constexpr Word kSyn_COAL[] = {{"COAL", "coal"}, {"PILE", "pile"}, {"HEAP", "heap"}};
constexpr Word kAdj_COAL[] = {{"SMALL", "small"}};
constexpr std::string_view kFl_LADDER[] = {"NDESCBIT", "CLIMBBIT"};
constexpr Word kSyn_LADDER[] = {{"LADDER", "ladder"}};
constexpr Word kAdj_LADDER[] = {{"WOODEN", "wooden"}, {"RICKETY", "ricket"}, {"NARROW", "narrow"}};
constexpr std::string_view kFl_SCARAB[] = {"TAKEBIT", "INVISIBLE"};
constexpr Word kSyn_SCARAB[] = {{"SCARAB", "scarab"}, {"BUG", "bug"}, {"BEETLE", "beetle"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_SCARAB[] = {{"BEAUTI", "beauti"}, {"CARVED", "carved"}, {"JEWELED", "jewele"}};
constexpr std::string_view kFl_LARGE_BAG[] = {"TRYTAKEBIT", "NDESCBIT"};
constexpr Word kSyn_LARGE_BAG[] = {{"BAG", "bag"}};
constexpr Word kAdj_LARGE_BAG[] = {{"LARGE", "large"}, {"THIEFS", "thiefs"}};
constexpr std::string_view kFl_STILETTO[] = {"WEAPONBIT", "TRYTAKEBIT", "TAKEBIT", "NDESCBIT"};
constexpr Word kSyn_STILETTO[] = {{"STILETTO", "stilet"}};
constexpr Word kAdj_STILETTO[] = {{"VICIOUS", "viciou"}};
constexpr std::string_view kFl_MACHINE_SWITCH[] = {"NDESCBIT", "TURNBIT"};
constexpr Word kSyn_MACHINE_SWITCH[] = {{"SWITCH", "switch"}};
constexpr std::string_view kFl_WOODEN_DOOR[] = {"READBIT", "DOORBIT", "NDESCBIT", "TRANSBIT"};
constexpr Word kSyn_WOODEN_DOOR[] = {{"DOOR", "door"}, {"LETTERING", "letter"}, {"WRITING", "writin"}};
constexpr Word kAdj_WOODEN_DOOR[] = {{"WOODEN", "wooden"}, {"GOTHIC", "gothic"}, {"STRANGE", "strang"}, {"WEST", "west"}};
constexpr std::string_view kFl_SWORD[] = {"TAKEBIT", "WEAPONBIT", "TRYTAKEBIT"};
constexpr Word kSyn_SWORD[] = {{"SWORD", "sword"}, {"ORCRIST", "orcris"}, {"GLAMDRING", "glamdr"}, {"BLADE", "blade"}};
constexpr Word kAdj_SWORD[] = {{"ELVISH", "elvish"}, {"OLD", "old"}, {"ANTIQUE", "antiqu"}};
constexpr std::string_view kFl_MAP[] = {"INVISIBLE", "READBIT", "TAKEBIT"};
constexpr Word kSyn_MAP[] = {{"PARCHMENT", "parchm"}, {"MAP", "map"}};
constexpr Word kAdj_MAP[] = {{"ANTIQUE", "antiqu"}, {"OLD", "old"}, {"ANCIENT", "ancien"}};
constexpr std::string_view kFl_BOAT_LABEL[] = {"READBIT", "TAKEBIT", "BURNBIT"};
constexpr Word kSyn_BOAT_LABEL[] = {{"LABEL", "label"}, {"FINEPRINT", "finepr"}, {"PRINT", "print"}};
constexpr Word kAdj_BOAT_LABEL[] = {{"TAN", "tan"}, {"FINE", "fine"}};
constexpr std::string_view kFl_THIEF[] = {"ACTORBIT", "INVISIBLE", "CONTBIT", "OPENBIT", "TRYTAKEBIT"};
constexpr Word kSyn_THIEF[] = {{"THIEF", "thief"}, {"ROBBER", "robber"}, {"MAN", "man"}, {"PERSON", "person"}};
constexpr Word kAdj_THIEF[] = {{"SHADY", "shady"}, {"SUSPICIOUS", "suspic"}, {"SEEDY", "seedy"}};
constexpr std::string_view kFl_PEDESTAL[] = {"NDESCBIT", "CONTBIT", "OPENBIT", "SURFACEBIT"};
constexpr Word kSyn_PEDESTAL[] = {{"PEDESTAL", "pedest"}};
constexpr Word kAdj_PEDESTAL[] = {{"WHITE", "white"}, {"MARBLE", "marble"}};
constexpr std::string_view kFl_TORCH[] = {"TAKEBIT", "FLAMEBIT", "ONBIT", "LIGHTBIT"};
constexpr Word kSyn_TORCH[] = {{"TORCH", "torch"}, {"IVORY", "ivory"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_TORCH[] = {{"FLAMING", "flamin"}, {"IVORY", "ivory"}};
constexpr std::string_view kFl_GUIDE[] = {"READBIT", "TAKEBIT", "BURNBIT"};
constexpr Word kSyn_GUIDE[] = {{"GUIDE", "guide"}, {"BOOK", "book"}, {"BOOKS", "books"}, {"GUIDEBOOKS", "guideb"}};
constexpr Word kAdj_GUIDE[] = {{"TOUR", "tour"}, {"GUIDE", "guide"}};
constexpr std::string_view kFl_TROLL[] = {"ACTORBIT", "OPENBIT", "TRYTAKEBIT"};
constexpr Word kSyn_TROLL[] = {{"TROLL", "troll"}};
constexpr Word kAdj_TROLL[] = {{"NASTY", "nasty"}};
constexpr std::string_view kFl_TRUNK[] = {"TAKEBIT", "INVISIBLE"};
constexpr Word kSyn_TRUNK[] = {{"TRUNK", "trunk"}, {"CHEST", "chest"}, {"JEWELS", "jewels"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_TRUNK[] = {{"OLD", "old"}};
constexpr std::string_view kFl_TUBE[] = {"TAKEBIT", "CONTBIT", "READBIT"};
constexpr Word kSyn_TUBE[] = {{"TUBE", "tube"}, {"TOOTH", "tooth"}, {"PASTE", "paste"}};
constexpr std::string_view kFl_PUTTY[] = {"TAKEBIT", "TOOLBIT"};
constexpr Word kSyn_PUTTY[] = {{"MATERIAL", "materi"}, {"GUNK", "gunk"}};
constexpr Word kAdj_PUTTY[] = {{"VISCOUS", "viscou"}};
constexpr std::string_view kFl_ENGRAVINGS[] = {"READBIT", "SACREDBIT"};
constexpr Word kSyn_ENGRAVINGS[] = {{"WALL", "wall"}, {"ENGRAVINGS", "engrav"}, {"INSCRIPTION", "inscri"}};
constexpr Word kAdj_ENGRAVINGS[] = {{"OLD", "old"}, {"ANCIENT", "ancien"}};
constexpr std::string_view kFl_OWNERS_MANUAL[] = {"READBIT", "TAKEBIT"};
constexpr Word kSyn_OWNERS_MANUAL[] = {{"MANUAL", "manual"}, {"PIECE", "piece"}, {"PAPER", "paper"}};
constexpr Word kAdj_OWNERS_MANUAL[] = {{"ZORK", "zork"}, {"OWNERS", "owners"}, {"SMALL", "small"}};
constexpr std::string_view kFl_CLIMBABLE_CLIFF[] = {"NDESCBIT", "CLIMBBIT"};
constexpr Word kSyn_CLIMBABLE_CLIFF[] = {{"WALL", "wall"}, {"CLIFF", "cliff"}, {"WALLS", "walls"}, {"LEDGE", "ledge"}};
constexpr Word kAdj_CLIMBABLE_CLIFF[] = {{"ROCKY", "rocky"}, {"SHEER", "sheer"}};
constexpr std::string_view kFl_WHITE_CLIFF[] = {"NDESCBIT", "CLIMBBIT"};
constexpr Word kSyn_WHITE_CLIFF[] = {{"CLIFF", "cliff"}, {"CLIFFS", "cliffs"}};
constexpr Word kAdj_WHITE_CLIFF[] = {{"WHITE", "white"}};
constexpr std::string_view kFl_WRENCH[] = {"TAKEBIT", "TOOLBIT"};
constexpr Word kSyn_WRENCH[] = {{"WRENCH", "wrench"}, {"TOOL", "tool"}, {"TOOLS", "tools"}};
constexpr std::string_view kFl_CONTROL_PANEL[] = {"NDESCBIT"};
constexpr Word kSyn_CONTROL_PANEL[] = {{"PANEL", "panel"}};
constexpr Word kAdj_CONTROL_PANEL[] = {{"CONTROL", "contro"}};
constexpr std::string_view kFl_NEST[] = {"TAKEBIT", "BURNBIT", "CONTBIT", "OPENBIT", "SEARCHBIT"};
constexpr Word kSyn_NEST[] = {{"NEST", "nest"}};
constexpr Word kAdj_NEST[] = {{"BIRDS", "birds"}};
constexpr std::string_view kFl_EGG[] = {"TAKEBIT", "CONTBIT", "SEARCHBIT"};
constexpr Word kSyn_EGG[] = {{"EGG", "egg"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_EGG[] = {{"BIRDS", "birds"}, {"ENCRUSTED", "encrus"}, {"JEWELED", "jewele"}};
constexpr std::string_view kFl_BROKEN_EGG[] = {"TAKEBIT", "CONTBIT", "OPENBIT"};
constexpr Word kSyn_BROKEN_EGG[] = {{"EGG", "egg"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_BROKEN_EGG[] = {{"BROKEN", "broken"}, {"BIRDS", "birds"}, {"ENCRUSTED", "encrus"}, {"JEWEL", "jewel"}};
constexpr std::string_view kFl_BAUBLE[] = {"TAKEBIT"};
constexpr Word kSyn_BAUBLE[] = {{"BAUBLE", "bauble"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_BAUBLE[] = {{"BRASS", "brass"}, {"BEAUTI", "beauti"}};
constexpr std::string_view kFl_CANARY[] = {"TAKEBIT", "SEARCHBIT"};
constexpr Word kSyn_CANARY[] = {{"CANARY", "canary"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_CANARY[] = {{"CLOCKWORK", "clockw"}, {"GOLD", "gold"}, {"GOLDEN", "golden"}};
constexpr std::string_view kFl_BROKEN_CANARY[] = {"TAKEBIT"};
constexpr Word kSyn_BROKEN_CANARY[] = {{"CANARY", "canary"}, {"TREASURE", "treasu"}};
constexpr Word kAdj_BROKEN_CANARY[] = {{"BROKEN", "broken"}, {"CLOCKWORK", "clockw"}, {"GOLD", "gold"}, {"GOLDEN", "golden"}};
constexpr std::string_view kFl_GLOBAL_OBJECTS[] = {"RMUNGBIT", "INVISIBLE", "TOUCHBIT", "SURFACEBIT", "TRYTAKEBIT", "OPENBIT", "SEARCHBIT", "TRANSBIT", "ONBIT", "RLANDBIT", "FIGHTBIT", "STAGGERED", "WEARBIT"};
constexpr Word kSyn_LOCAL_GLOBALS[] = {{"ZZMGCK", "zzmgck"}};
constexpr std::string_view kGl_LOCAL_GLOBALS[] = {"GLOBAL-OBJECTS"};
constexpr PseudoDef kPs_LOCAL_GLOBALS[] = {{"FOOBAR", "V-WALK"}};
constexpr ExitDef kEx_ROOMS[] = {{.dir = "IN", .kind = ExitKind::UEXIT, .to = "ROOMS", .line = 29}};
constexpr std::string_view kFl_INTNUM[] = {"TOOLBIT"};
constexpr Word kSyn_INTNUM[] = {{"INTNUM", "intnum"}};
constexpr std::string_view kFl_IT[] = {"NDESCBIT", "TOUCHBIT"};
constexpr Word kSyn_IT[] = {{"IT", "it"}, {"THEM", "them"}, {"HER", "her"}, {"HIM", "him"}};
constexpr std::string_view kFl_BLESSINGS[] = {"NDESCBIT"};
constexpr Word kSyn_BLESSINGS[] = {{"BLESSINGS", "blessi"}, {"GRACES", "graces"}};
constexpr std::string_view kFl_STAIRS[] = {"NDESCBIT", "CLIMBBIT"};
constexpr Word kSyn_STAIRS[] = {{"STAIRS", "stairs"}, {"STEPS", "steps"}, {"STAIRCASE", "stairc"}, {"STAIRWAY", "stairw"}};
constexpr Word kAdj_STAIRS[] = {{"STONE", "stone"}, {"DARK", "dark"}, {"MARBLE", "marble"}, {"FORBIDDING", "forbid"}, {"STEEP", "steep"}};
constexpr std::string_view kFl_SAILOR[] = {"NDESCBIT"};
constexpr Word kSyn_SAILOR[] = {{"SAILOR", "sailor"}, {"FOOTPAD", "footpa"}, {"AVIATOR", "aviato"}};
constexpr Word kSyn_GROUND[] = {{"GROUND", "ground"}, {"SAND", "sand"}, {"DIRT", "dirt"}, {"FLOOR", "floor"}};
constexpr Word kSyn_GRUE[] = {{"GRUE", "grue"}};
constexpr Word kAdj_GRUE[] = {{"LURKING", "lurkin"}, {"SINISTER", "sinist"}, {"HUNGRY", "hungry"}, {"SILENT", "silent"}};
constexpr std::string_view kFl_LUNGS[] = {"NDESCBIT"};
constexpr Word kSyn_LUNGS[] = {{"LUNGS", "lungs"}, {"AIR", "air"}, {"MOUTH", "mouth"}, {"BREATH", "breath"}};
constexpr std::string_view kFl_ME[] = {"ACTORBIT"};
constexpr Word kSyn_ME[] = {{"ME", "me"}, {"MYSELF", "myself"}, {"SELF", "self"}, {"CRETIN", "cretin"}};
constexpr std::string_view kFl_ADVENTURER[] = {"NDESCBIT", "INVISIBLE", "SACREDBIT", "ACTORBIT"};
constexpr Word kSyn_ADVENTURER[] = {{"ADVENTURER", "advent"}};
constexpr std::string_view kFl_PATHOBJ[] = {"NDESCBIT"};
constexpr Word kSyn_PATHOBJ[] = {{"TRAIL", "trail"}, {"PATH", "path"}};
constexpr Word kAdj_PATHOBJ[] = {{"FOREST", "forest"}, {"NARROW", "narrow"}, {"LONG", "long"}, {"WINDING", "windin"}};
constexpr Word kSyn_ZORKMID[] = {{"ZORKMID", "zorkmi"}};
constexpr std::string_view kFl_HANDS[] = {"NDESCBIT", "TOOLBIT"};
constexpr Word kSyn_HANDS[] = {{"PAIR", "pair"}, {"HANDS", "hands"}, {"HAND", "hand"}};
constexpr Word kAdj_HANDS[] = {{"BARE", "bare"}};

constexpr ObjectDef kObjectsArr[] = {
    {.def_index = 0, .name = "BOARD", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_BOARD, .desc = "board", .flags = kFl_BOARD, .action = "BOARD-F", .file = "1dungeon.zil", .line = 13},
    {.def_index = 1, .name = "TEETH", .role = ObjRole::Game, .in = "GLOBAL-OBJECTS", .synonyms = kSyn_TEETH, .desc = "set of teeth", .flags = kFl_TEETH, .action = "TEETH-F", .file = "1dungeon.zil", .line = 20},
    {.def_index = 2, .name = "WALL", .role = ObjRole::Game, .in = "GLOBAL-OBJECTS", .synonyms = kSyn_WALL, .adjectives = kAdj_WALL, .desc = "surrounding wall", .file = "1dungeon.zil", .line = 27},
    {.def_index = 3, .name = "GRANITE-WALL", .role = ObjRole::Game, .in = "GLOBAL-OBJECTS", .synonyms = kSyn_GRANITE_WALL, .adjectives = kAdj_GRANITE_WALL, .desc = "granite wall", .action = "GRANITE-WALL-F", .file = "1dungeon.zil", .line = 33},
    {.def_index = 4, .name = "SONGBIRD", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_SONGBIRD, .adjectives = kAdj_SONGBIRD, .desc = "songbird", .flags = kFl_SONGBIRD, .action = "SONGBIRD-F", .file = "1dungeon.zil", .line = 40},
    {.def_index = 5, .name = "WHITE-HOUSE", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_WHITE_HOUSE, .adjectives = kAdj_WHITE_HOUSE, .desc = "white house", .flags = kFl_WHITE_HOUSE, .action = "WHITE-HOUSE-F", .file = "1dungeon.zil", .line = 48},
    {.def_index = 6, .name = "FOREST", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_FOREST, .desc = "forest", .flags = kFl_FOREST, .action = "FOREST-F", .file = "1dungeon.zil", .line = 56},
    {.def_index = 7, .name = "TREE", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_TREE, .adjectives = kAdj_TREE, .desc = "tree", .flags = kFl_TREE, .file = "1dungeon.zil", .line = 63},
    {.def_index = 8, .name = "MOUNTAIN-RANGE", .role = ObjRole::Game, .in = "MOUNTAINS", .synonyms = kSyn_MOUNTAIN_RANGE, .adjectives = kAdj_MOUNTAIN_RANGE, .desc = "mountain range", .flags = kFl_MOUNTAIN_RANGE, .action = "MOUNTAIN-RANGE-F", .file = "1dungeon.zil", .line = 70},
    {.def_index = 9, .name = "GLOBAL-WATER", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_GLOBAL_WATER, .desc = "water", .flags = kFl_GLOBAL_WATER, .action = "WATER-F", .file = "1dungeon.zil", .line = 78},
    {.def_index = 10, .name = "WATER", .role = ObjRole::Game, .in = "BOTTLE", .synonyms = kSyn_WATER, .desc = "quantity of water", .flags = kFl_WATER, .size = 4, .action = "WATER-F", .file = "1dungeon.zil", .line = 85},
    {.def_index = 11, .name = "KITCHEN-WINDOW", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_KITCHEN_WINDOW, .adjectives = kAdj_KITCHEN_WINDOW, .desc = "kitchen window", .flags = kFl_KITCHEN_WINDOW, .action = "KITCHEN-WINDOW-F", .file = "1dungeon.zil", .line = 93},
    {.def_index = 12, .name = "CHIMNEY", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_CHIMNEY, .adjectives = kAdj_CHIMNEY, .desc = "chimney", .flags = kFl_CHIMNEY, .action = "CHIMNEY-F", .file = "1dungeon.zil", .line = 101},
    {.def_index = 13, .name = "GHOSTS", .role = ObjRole::Game, .in = "ENTRANCE-TO-HADES", .synonyms = kSyn_GHOSTS, .adjectives = kAdj_GHOSTS, .desc = "number of ghosts", .flags = kFl_GHOSTS, .action = "GHOSTS-F", .file = "1dungeon.zil", .line = 109},
    {.def_index = 14, .name = "SKULL", .role = ObjRole::Game, .in = "LAND-OF-LIVING-DEAD", .synonyms = kSyn_SKULL, .adjectives = kAdj_SKULL, .desc = "crystal skull", .fdesc = "Lying in one corner of the room is a beautifully carved crystal skull. It appears to be grinning at you rather nastily.", .flags = kFl_SKULL, .value = 10, .tvalue = 10, .file = "1dungeon.zil", .line = 117},
    {.def_index = 15, .name = "LOWERED-BASKET", .role = ObjRole::Game, .in = "LOWER-SHAFT", .synonyms = kSyn_LOWERED_BASKET, .adjectives = kAdj_LOWERED_BASKET, .desc = "basket", .ldesc = "From the chain is suspended a basket.", .flags = kFl_LOWERED_BASKET, .action = "BASKET-F", .file = "1dungeon.zil", .line = 129},
    {.def_index = 16, .name = "RAISED-BASKET", .role = ObjRole::Game, .in = "SHAFT-ROOM", .synonyms = kSyn_RAISED_BASKET, .desc = "basket", .ldesc = "At the end of the chain is a basket.", .flags = kFl_RAISED_BASKET, .capacity = 50, .action = "BASKET-F", .file = "1dungeon.zil", .line = 138},
    {.def_index = 17, .name = "LUNCH", .role = ObjRole::Game, .in = "SANDWICH-BAG", .synonyms = kSyn_LUNCH, .adjectives = kAdj_LUNCH, .desc = "lunch", .ldesc = "A hot pepper sandwich is here.", .flags = kFl_LUNCH, .file = "1dungeon.zil", .line = 147},
    {.def_index = 18, .name = "BAT", .role = ObjRole::Game, .in = "BAT-ROOM", .synonyms = kSyn_BAT, .adjectives = kAdj_BAT, .desc = "bat", .flags = kFl_BAT, .action = "BAT-F", .descfcn = "BAT-D", .file = "1dungeon.zil", .line = 155},
    {.def_index = 19, .name = "BELL", .role = ObjRole::Game, .in = "NORTH-TEMPLE", .synonyms = kSyn_BELL, .adjectives = kAdj_BELL, .desc = "brass bell", .flags = kFl_BELL, .action = "BELL-F", .file = "1dungeon.zil", .line = 164},
    {.def_index = 20, .name = "HOT-BELL", .role = ObjRole::Game, .synonyms = kSyn_HOT_BELL, .adjectives = kAdj_HOT_BELL, .desc = "red hot brass bell", .ldesc = "On the ground is a red hot bell.", .flags = kFl_HOT_BELL, .action = "HOT-BELL-F", .file = "1dungeon.zil", .line = 172},
    {.def_index = 21, .name = "AXE", .role = ObjRole::Game, .in = "TROLL", .synonyms = kSyn_AXE, .adjectives = kAdj_AXE, .desc = "bloody axe", .flags = kFl_AXE, .size = 25, .action = "AXE-F", .file = "1dungeon.zil", .line = 180},
    {.def_index = 22, .name = "BOLT", .role = ObjRole::Game, .in = "DAM-ROOM", .synonyms = kSyn_BOLT, .adjectives = kAdj_BOLT, .desc = "bolt", .flags = kFl_BOLT, .action = "BOLT-F", .file = "1dungeon.zil", .line = 189},
    {.def_index = 23, .name = "BUBBLE", .role = ObjRole::Game, .in = "DAM-ROOM", .synonyms = kSyn_BUBBLE, .adjectives = kAdj_BUBBLE, .desc = "green bubble", .flags = kFl_BUBBLE, .action = "BUBBLE-F", .file = "1dungeon.zil", .line = 197},
    {.def_index = 24, .name = "ALTAR", .role = ObjRole::Game, .in = "SOUTH-TEMPLE", .synonyms = kSyn_ALTAR, .desc = "altar", .flags = kFl_ALTAR, .capacity = 50, .file = "1dungeon.zil", .line = 205},
    {.def_index = 25, .name = "BOOK", .role = ObjRole::Game, .in = "ALTAR", .synonyms = kSyn_BOOK, .adjectives = kAdj_BOOK, .desc = "black book", .fdesc = "On the altar is a large black book, open to page 569.", .text = "Commandment #12592\n\nOh ye who go about saying unto each:  \"Hello sailor\":\nDost thou know the magnitude of thy sin before the gods?\nYea, verily, thou shalt be ground between two stones.\nShall the angry gods cast thy body into the whirlpool?\nSurely, thy eye shall be put out with a sharp stick!\nEven unto the ends of the earth shalt thou wander and\nUnto the land of the dead shalt thou be sent at last.\nSurely thou shalt repent of thy cunning.", .flags = kFl_BOOK, .size = 10, .action = "BLACK-BOOK", .file = "1dungeon.zil", .line = 212},
    {.def_index = 26, .name = "BROKEN-LAMP", .role = ObjRole::Game, .synonyms = kSyn_BROKEN_LAMP, .adjectives = kAdj_BROKEN_LAMP, .desc = "broken lantern", .flags = kFl_BROKEN_LAMP, .file = "1dungeon.zil", .line = 233},
    {.def_index = 27, .name = "SCEPTRE", .role = ObjRole::Game, .in = "COFFIN", .synonyms = kSyn_SCEPTRE, .adjectives = kAdj_SCEPTRE, .desc = "sceptre", .fdesc = "A sceptre, possibly that of ancient Egypt itself, is in the coffin. The sceptre is ornamented with colored enamel, and tapers to a sharp point.", .ldesc = "An ornamented sceptre, tapering to a sharp point, is here.", .flags = kFl_SCEPTRE, .size = 3, .value = 4, .tvalue = 6, .action = "SCEPTRE-FUNCTION", .file = "1dungeon.zil", .line = 239},
    {.def_index = 28, .name = "TIMBERS", .role = ObjRole::Game, .in = "TIMBER-ROOM", .synonyms = kSyn_TIMBERS, .adjectives = kAdj_TIMBERS, .desc = "broken timber", .flags = kFl_TIMBERS, .size = 50, .file = "1dungeon.zil", .line = 255},
    {.def_index = 29, .name = "SLIDE", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_SLIDE, .adjectives = kAdj_SLIDE, .desc = "chute", .flags = kFl_SLIDE, .action = "SLIDE-FUNCTION", .file = "1dungeon.zil", .line = 263},
    {.def_index = 30, .name = "KITCHEN-TABLE", .role = ObjRole::Game, .in = "KITCHEN", .synonyms = kSyn_KITCHEN_TABLE, .adjectives = kAdj_KITCHEN_TABLE, .desc = "kitchen table", .flags = kFl_KITCHEN_TABLE, .capacity = 50, .file = "1dungeon.zil", .line = 271},
    {.def_index = 31, .name = "ATTIC-TABLE", .role = ObjRole::Game, .in = "ATTIC", .synonyms = kSyn_ATTIC_TABLE, .desc = "table", .flags = kFl_ATTIC_TABLE, .capacity = 40, .file = "1dungeon.zil", .line = 279},
    {.def_index = 32, .name = "SANDWICH-BAG", .role = ObjRole::Game, .in = "KITCHEN-TABLE", .synonyms = kSyn_SANDWICH_BAG, .adjectives = kAdj_SANDWICH_BAG, .desc = "brown sack", .fdesc = "On the table is an elongated brown sack, smelling of hot peppers.", .flags = kFl_SANDWICH_BAG, .size = 9, .capacity = 9, .action = "SANDWICH-BAG-FCN", .file = "1dungeon.zil", .line = 286},
    {.def_index = 33, .name = "TOOL-CHEST", .role = ObjRole::Game, .in = "MAINTENANCE-ROOM", .synonyms = kSyn_TOOL_CHEST, .adjectives = kAdj_TOOL_CHEST, .desc = "group of tool chests", .flags = kFl_TOOL_CHEST, .action = "TOOL-CHEST-FCN", .file = "1dungeon.zil", .line = 298},
    {.def_index = 34, .name = "YELLOW-BUTTON", .role = ObjRole::Game, .in = "MAINTENANCE-ROOM", .synonyms = kSyn_YELLOW_BUTTON, .adjectives = kAdj_YELLOW_BUTTON, .desc = "yellow button", .flags = kFl_YELLOW_BUTTON, .action = "BUTTON-F", .file = "1dungeon.zil", .line = 306},
    {.def_index = 35, .name = "BROWN-BUTTON", .role = ObjRole::Game, .in = "MAINTENANCE-ROOM", .synonyms = kSyn_BROWN_BUTTON, .adjectives = kAdj_BROWN_BUTTON, .desc = "brown button", .flags = kFl_BROWN_BUTTON, .action = "BUTTON-F", .file = "1dungeon.zil", .line = 314},
    {.def_index = 36, .name = "RED-BUTTON", .role = ObjRole::Game, .in = "MAINTENANCE-ROOM", .synonyms = kSyn_RED_BUTTON, .adjectives = kAdj_RED_BUTTON, .desc = "red button", .flags = kFl_RED_BUTTON, .action = "BUTTON-F", .file = "1dungeon.zil", .line = 322},
    {.def_index = 37, .name = "BLUE-BUTTON", .role = ObjRole::Game, .in = "MAINTENANCE-ROOM", .synonyms = kSyn_BLUE_BUTTON, .adjectives = kAdj_BLUE_BUTTON, .desc = "blue button", .flags = kFl_BLUE_BUTTON, .action = "BUTTON-F", .file = "1dungeon.zil", .line = 330},
    {.def_index = 38, .name = "TROPHY-CASE", .role = ObjRole::Game, .in = "LIVING-ROOM", .synonyms = kSyn_TROPHY_CASE, .adjectives = kAdj_TROPHY_CASE, .desc = "trophy case", .flags = kFl_TROPHY_CASE, .capacity = 10000, .action = "TROPHY-CASE-FCN", .file = "1dungeon.zil", .line = 338},
    {.def_index = 39, .name = "RUG", .role = ObjRole::Game, .in = "LIVING-ROOM", .synonyms = kSyn_RUG, .adjectives = kAdj_RUG, .desc = "carpet", .flags = kFl_RUG, .action = "RUG-FCN", .file = "1dungeon.zil", .line = 347},
    {.def_index = 40, .name = "CHALICE", .role = ObjRole::Game, .in = "TREASURE-ROOM", .synonyms = kSyn_CHALICE, .adjectives = kAdj_CHALICE, .desc = "chalice", .ldesc = "There is a silver chalice, intricately engraved, here.", .flags = kFl_CHALICE, .size = 10, .capacity = 5, .value = 10, .tvalue = 5, .action = "CHALICE-FCN", .file = "1dungeon.zil", .line = 355},
    {.def_index = 41, .name = "GARLIC", .role = ObjRole::Game, .in = "SANDWICH-BAG", .synonyms = kSyn_GARLIC, .desc = "clove of garlic", .flags = kFl_GARLIC, .size = 4, .action = "GARLIC-F", .file = "1dungeon.zil", .line = 368},
    {.def_index = 42, .name = "TRIDENT", .role = ObjRole::Game, .in = "ATLANTIS-ROOM", .synonyms = kSyn_TRIDENT, .adjectives = kAdj_TRIDENT, .desc = "crystal trident", .fdesc = "On the shore lies Poseidon's own crystal trident.", .flags = kFl_TRIDENT, .size = 20, .value = 4, .tvalue = 11, .file = "1dungeon.zil", .line = 376},
    {.def_index = 43, .name = "CYCLOPS", .role = ObjRole::Game, .in = "CYCLOPS-ROOM", .synonyms = kSyn_CYCLOPS, .adjectives = kAdj_CYCLOPS, .desc = "cyclops", .flags = kFl_CYCLOPS, .strength = 10000, .action = "CYCLOPS-FCN", .file = "1dungeon.zil", .line = 387},
    {.def_index = 44, .name = "DAM", .role = ObjRole::Game, .in = "DAM-ROOM", .synonyms = kSyn_DAM, .desc = "dam", .flags = kFl_DAM, .action = "DAM-FUNCTION", .file = "1dungeon.zil", .line = 396},
    {.def_index = 45, .name = "TRAP-DOOR", .role = ObjRole::Game, .in = "LIVING-ROOM", .synonyms = kSyn_TRAP_DOOR, .adjectives = kAdj_TRAP_DOOR, .desc = "trap door", .flags = kFl_TRAP_DOOR, .action = "TRAP-DOOR-FCN", .file = "1dungeon.zil", .line = 403},
    {.def_index = 46, .name = "BOARDED-WINDOW", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_BOARDED_WINDOW, .adjectives = kAdj_BOARDED_WINDOW, .desc = "boarded window", .flags = kFl_BOARDED_WINDOW, .action = "BOARDED-WINDOW-FCN", .file = "1dungeon.zil", .line = 411},
    {.def_index = 47, .name = "FRONT-DOOR", .role = ObjRole::Game, .in = "WEST-OF-HOUSE", .synonyms = kSyn_FRONT_DOOR, .adjectives = kAdj_FRONT_DOOR, .desc = "door", .flags = kFl_FRONT_DOOR, .action = "FRONT-DOOR-FCN", .file = "1dungeon.zil", .line = 419},
    {.def_index = 48, .name = "BARROW-DOOR", .role = ObjRole::Game, .in = "STONE-BARROW", .synonyms = kSyn_BARROW_DOOR, .adjectives = kAdj_BARROW_DOOR, .desc = "stone door", .flags = kFl_BARROW_DOOR, .action = "BARROW-DOOR-FCN", .file = "1dungeon.zil", .line = 427},
    {.def_index = 49, .name = "BARROW", .role = ObjRole::Game, .in = "STONE-BARROW", .synonyms = kSyn_BARROW, .adjectives = kAdj_BARROW, .desc = "stone barrow", .flags = kFl_BARROW, .action = "BARROW-FCN", .file = "1dungeon.zil", .line = 435},
    {.def_index = 50, .name = "BOTTLE", .role = ObjRole::Game, .in = "KITCHEN-TABLE", .synonyms = kSyn_BOTTLE, .adjectives = kAdj_BOTTLE, .desc = "glass bottle", .fdesc = "A bottle is sitting on the table.", .flags = kFl_BOTTLE, .capacity = 4, .action = "BOTTLE-FUNCTION", .file = "1dungeon.zil", .line = 443},
    {.def_index = 51, .name = "CRACK", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_CRACK, .adjectives = kAdj_CRACK, .desc = "crack", .flags = kFl_CRACK, .action = "CRACK-FCN", .file = "1dungeon.zil", .line = 453},
    {.def_index = 52, .name = "COFFIN", .role = ObjRole::Game, .in = "EGYPT-ROOM", .synonyms = kSyn_COFFIN, .adjectives = kAdj_COFFIN, .desc = "gold coffin", .ldesc = "The solid-gold coffin used for the burial of Ramses II is here.", .flags = kFl_COFFIN, .size = 55, .capacity = 35, .value = 10, .tvalue = 15, .file = "1dungeon.zil", .line = 461},
    {.def_index = 53, .name = "GRATE", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_GRATE, .desc = "grating", .flags = kFl_GRATE, .action = "GRATE-FUNCTION", .file = "1dungeon.zil", .line = 474},
    {.def_index = 54, .name = "PUMP", .role = ObjRole::Game, .in = "RESERVOIR-NORTH", .synonyms = kSyn_PUMP, .adjectives = kAdj_PUMP, .desc = "hand-held air pump", .flags = kFl_PUMP, .file = "1dungeon.zil", .line = 481},
    {.def_index = 55, .name = "DIAMOND", .role = ObjRole::Game, .synonyms = kSyn_DIAMOND, .adjectives = kAdj_DIAMOND, .desc = "huge diamond", .ldesc = "There is an enormous diamond (perfectly cut) here.", .flags = kFl_DIAMOND, .value = 10, .tvalue = 10, .file = "1dungeon.zil", .line = 488},
    {.def_index = 56, .name = "JADE", .role = ObjRole::Game, .in = "BAT-ROOM", .synonyms = kSyn_JADE, .adjectives = kAdj_JADE, .desc = "jade figurine", .ldesc = "There is an exquisite jade figurine here.", .flags = kFl_JADE, .size = 10, .value = 5, .tvalue = 5, .file = "1dungeon.zil", .line = 497},
    {.def_index = 57, .name = "KNIFE", .role = ObjRole::Game, .in = "ATTIC-TABLE", .synonyms = kSyn_KNIFE, .adjectives = kAdj_KNIFE, .desc = "nasty knife", .fdesc = "On a table is a nasty-looking knife.", .flags = kFl_KNIFE, .action = "KNIFE-F", .file = "1dungeon.zil", .line = 508},
    {.def_index = 58, .name = "BONES", .role = ObjRole::Game, .in = "MAZE-5", .synonyms = kSyn_BONES, .desc = "skeleton", .flags = kFl_BONES, .action = "SKELETON", .file = "1dungeon.zil", .line = 517},
    {.def_index = 59, .name = "BURNED-OUT-LANTERN", .role = ObjRole::Game, .in = "MAZE-5", .synonyms = kSyn_BURNED_OUT_LANTERN, .adjectives = kAdj_BURNED_OUT_LANTERN, .desc = "burned-out lantern", .fdesc = "The deceased adventurer's useless lantern is here.", .flags = kFl_BURNED_OUT_LANTERN, .size = 20, .file = "1dungeon.zil", .line = 524},
    {.def_index = 60, .name = "BAG-OF-COINS", .role = ObjRole::Game, .in = "MAZE-5", .synonyms = kSyn_BAG_OF_COINS, .adjectives = kAdj_BAG_OF_COINS, .desc = "leather bag of coins", .ldesc = "An old leather bag, bulging with coins, is here.", .flags = kFl_BAG_OF_COINS, .size = 15, .value = 10, .tvalue = 5, .action = "BAG-OF-COINS-F", .file = "1dungeon.zil", .line = 533},
    {.def_index = 61, .name = "LAMP", .role = ObjRole::Game, .in = "LIVING-ROOM", .synonyms = kSyn_LAMP, .adjectives = kAdj_LAMP, .desc = "brass lantern", .fdesc = "A battery-powered brass lantern is on the trophy case.", .ldesc = "There is a brass lantern (battery-powered) here.", .flags = kFl_LAMP, .size = 15, .action = "LANTERN", .file = "1dungeon.zil", .line = 545},
    {.def_index = 62, .name = "EMERALD", .role = ObjRole::Game, .in = "BUOY", .synonyms = kSyn_EMERALD, .adjectives = kAdj_EMERALD, .desc = "large emerald", .flags = kFl_EMERALD, .value = 5, .tvalue = 10, .file = "1dungeon.zil", .line = 556},
    {.def_index = 63, .name = "ADVERTISEMENT", .role = ObjRole::Game, .in = "MAILBOX", .synonyms = kSyn_ADVERTISEMENT, .adjectives = kAdj_ADVERTISEMENT, .desc = "leaflet", .ldesc = "A small leaflet is on the ground.", .text = "\"WELCOME TO ZORK!\n\nZORK is a game of adventure, danger, and low cunning. In it you will explore some of the most amazing territory ever seen by mortals. No computer should be without one!\"", .flags = kFl_ADVERTISEMENT, .size = 2, .file = "1dungeon.zil", .line = 565},
    {.def_index = 64, .name = "LEAK", .role = ObjRole::Game, .in = "MAINTENANCE-ROOM", .synonyms = kSyn_LEAK, .desc = "leak", .flags = kFl_LEAK, .action = "LEAK-FUNCTION", .file = "1dungeon.zil", .line = 580},
    {.def_index = 65, .name = "MACHINE", .role = ObjRole::Game, .in = "MACHINE-ROOM", .synonyms = kSyn_MACHINE, .desc = "machine", .flags = kFl_MACHINE, .capacity = 50, .action = "MACHINE-F", .file = "1dungeon.zil", .line = 587},
    {.def_index = 66, .name = "INFLATED-BOAT", .role = ObjRole::Game, .synonyms = kSyn_INFLATED_BOAT, .adjectives = kAdj_INFLATED_BOAT, .desc = "magic boat", .flags = kFl_INFLATED_BOAT, .size = 20, .capacity = 100, .vtype = "NONLANDBIT", .action = "RBOAT-FUNCTION", .file = "1dungeon.zil", .line = 595},
    {.def_index = 67, .name = "MAILBOX", .role = ObjRole::Game, .in = "WEST-OF-HOUSE", .synonyms = kSyn_MAILBOX, .adjectives = kAdj_MAILBOX, .desc = "small mailbox", .flags = kFl_MAILBOX, .capacity = 10, .action = "MAILBOX-F", .file = "1dungeon.zil", .line = 605},
    {.def_index = 68, .name = "MATCH", .role = ObjRole::Game, .in = "DAM-LOBBY", .synonyms = kSyn_MATCH, .adjectives = kAdj_MATCH, .desc = "matchbook", .ldesc = "There is a matchbook whose cover says \"Visit Beautiful FCD#3\" here.", .text = "\n(Close cover before striking)\n\nYOU too can make BIG MONEY in the exciting field of PAPER SHUFFLING!\n\nMr. Anderson of Muddle, Mass. says: \"Before I took this course I was a lowly bit twiddler. Now with what I learned at GUE Tech I feel really important and can obfuscate and confuse with the best.\"\n\nDr. Blank had this to say: \"Ten short days ago all I could look forward to was a dead-end job as a doctor. Now I have a promising future and make really big Zorkmids.\"\n\nGUE Tech can't promise these fantastic results to everyone. But when you earn your degree from GUE Tech, your future will be brighter.", .flags = kFl_MATCH, .size = 2, .action = "MATCH-FUNCTION", .file = "1dungeon.zil", .line = 614},
    {.def_index = 69, .name = "MIRROR-2", .role = ObjRole::Game, .in = "MIRROR-ROOM-2", .synonyms = kSyn_MIRROR_2, .desc = "mirror", .flags = kFl_MIRROR_2, .action = "MIRROR-MIRROR", .file = "1dungeon.zil", .line = 641},
    {.def_index = 70, .name = "MIRROR-1", .role = ObjRole::Game, .in = "MIRROR-ROOM-1", .synonyms = kSyn_MIRROR_1, .desc = "mirror", .flags = kFl_MIRROR_1, .action = "MIRROR-MIRROR", .file = "1dungeon.zil", .line = 648},
    {.def_index = 71, .name = "PAINTING", .role = ObjRole::Game, .in = "GALLERY", .synonyms = kSyn_PAINTING, .adjectives = kAdj_PAINTING, .desc = "painting", .fdesc = "Fortunately, there is still one chance for you to be a vandal, for on the far wall is a painting of unparalleled beauty.", .ldesc = "A painting by a neglected genius is here.", .flags = kFl_PAINTING, .size = 15, .value = 4, .tvalue = 6, .action = "PAINTING-FCN", .file = "1dungeon.zil", .line = 655},
    {.def_index = 72, .name = "CANDLES", .role = ObjRole::Game, .in = "SOUTH-TEMPLE", .synonyms = kSyn_CANDLES, .adjectives = kAdj_CANDLES, .desc = "pair of candles", .fdesc = "On the two ends of the altar are burning candles.", .flags = kFl_CANDLES, .size = 10, .action = "CANDLES-FCN", .file = "1dungeon.zil", .line = 670},
    {.def_index = 73, .name = "GUNK", .role = ObjRole::Game, .synonyms = kSyn_GUNK, .adjectives = kAdj_GUNK, .desc = "small piece of vitreous slag", .flags = kFl_GUNK, .size = 10, .action = "GUNK-FUNCTION", .file = "1dungeon.zil", .line = 680},
    {.def_index = 74, .name = "BODIES", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_BODIES, .adjectives = kAdj_BODIES, .desc = "pile of bodies", .flags = kFl_BODIES, .action = "BODY-FUNCTION", .file = "1dungeon.zil", .line = 688},
    {.def_index = 75, .name = "LEAVES", .role = ObjRole::Game, .in = "GRATING-CLEARING", .synonyms = kSyn_LEAVES, .desc = "pile of leaves", .ldesc = "On the ground is a pile of leaves.", .flags = kFl_LEAVES, .size = 25, .action = "LEAF-PILE", .file = "1dungeon.zil", .line = 696},
    {.def_index = 76, .name = "PUNCTURED-BOAT", .role = ObjRole::Game, .synonyms = kSyn_PUNCTURED_BOAT, .adjectives = kAdj_PUNCTURED_BOAT, .desc = "punctured boat", .flags = kFl_PUNCTURED_BOAT, .size = 20, .action = "DBOAT-FUNCTION", .file = "1dungeon.zil", .line = 705},
    {.def_index = 77, .name = "INFLATABLE-BOAT", .role = ObjRole::Game, .in = "DAM-BASE", .synonyms = kSyn_INFLATABLE_BOAT, .adjectives = kAdj_INFLATABLE_BOAT, .desc = "pile of plastic", .ldesc = "There is a folded pile of plastic here which has a small valve attached.", .flags = kFl_INFLATABLE_BOAT, .size = 20, .action = "IBOAT-FUNCTION", .file = "1dungeon.zil", .line = 713},
    {.def_index = 78, .name = "BAR", .role = ObjRole::Game, .in = "LOUD-ROOM", .synonyms = kSyn_BAR, .adjectives = kAdj_BAR, .desc = "platinum bar", .ldesc = "On the ground is a large platinum bar.", .flags = kFl_BAR, .size = 20, .value = 10, .tvalue = 5, .file = "1dungeon.zil", .line = 725},
    {.def_index = 79, .name = "POT-OF-GOLD", .role = ObjRole::Game, .in = "END-OF-RAINBOW", .synonyms = kSyn_POT_OF_GOLD, .adjectives = kAdj_POT_OF_GOLD, .desc = "pot of gold", .fdesc = "At the end of the rainbow is a pot of gold.", .flags = kFl_POT_OF_GOLD, .size = 15, .value = 10, .tvalue = 10, .file = "1dungeon.zil", .line = 736},
    {.def_index = 80, .name = "PRAYER", .role = ObjRole::Game, .in = "NORTH-TEMPLE", .synonyms = kSyn_PRAYER, .adjectives = kAdj_PRAYER, .desc = "prayer", .text = "The prayer is inscribed in an ancient script, rarely used today. It seems to be a philippic against small insects, absent-mindedness, and the picking up and dropping of small objects. The final verse consigns trespassers to the land of the dead. All evidence indicates that the beliefs of the ancient Zorkers were obscure.", .flags = kFl_PRAYER, .file = "1dungeon.zil", .line = 747},
    {.def_index = 81, .name = "RAILING", .role = ObjRole::Game, .in = "DOME-ROOM", .synonyms = kSyn_RAILING, .adjectives = kAdj_RAILING, .desc = "wooden railing", .flags = kFl_RAILING, .file = "1dungeon.zil", .line = 760},
    {.def_index = 82, .name = "RAINBOW", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_RAINBOW, .desc = "rainbow", .flags = kFl_RAINBOW, .action = "RAINBOW-FCN", .file = "1dungeon.zil", .line = 767},
    {.def_index = 83, .name = "RIVER", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_RIVER, .adjectives = kAdj_RIVER, .desc = "river", .flags = kFl_RIVER, .action = "RIVER-FUNCTION", .file = "1dungeon.zil", .line = 774},
    {.def_index = 84, .name = "BUOY", .role = ObjRole::Game, .in = "RIVER-4", .synonyms = kSyn_BUOY, .adjectives = kAdj_BUOY, .desc = "red buoy", .fdesc = "There is a red buoy here (probably a warning).", .flags = kFl_BUOY, .size = 10, .capacity = 20, .action = "TREASURE-INSIDE", .file = "1dungeon.zil", .line = 782},
    {.def_index = 85, .name = "ROPE", .role = ObjRole::Game, .in = "ATTIC", .synonyms = kSyn_ROPE, .adjectives = kAdj_ROPE, .desc = "rope", .fdesc = "A large coil of rope is lying in the corner.", .flags = kFl_ROPE, .size = 10, .action = "ROPE-FUNCTION", .file = "1dungeon.zil", .line = 797},
    {.def_index = 86, .name = "RUSTY-KNIFE", .role = ObjRole::Game, .in = "MAZE-5", .synonyms = kSyn_RUSTY_KNIFE, .adjectives = kAdj_RUSTY_KNIFE, .desc = "rusty knife", .fdesc = "Beside the skeleton is a rusty knife.", .flags = kFl_RUSTY_KNIFE, .size = 20, .action = "RUSTY-KNIFE-FCN", .file = "1dungeon.zil", .line = 807},
    {.def_index = 87, .name = "SAND", .role = ObjRole::Game, .in = "SANDY-CAVE", .synonyms = kSyn_SAND, .desc = "sand", .flags = kFl_SAND, .action = "SAND-FUNCTION", .file = "1dungeon.zil", .line = 817},
    {.def_index = 88, .name = "BRACELET", .role = ObjRole::Game, .in = "GAS-ROOM", .synonyms = kSyn_BRACELET, .adjectives = kAdj_BRACELET, .desc = "sapphire-encrusted bracelet", .flags = kFl_BRACELET, .size = 10, .value = 5, .tvalue = 5, .file = "1dungeon.zil", .line = 824},
    {.def_index = 89, .name = "SCREWDRIVER", .role = ObjRole::Game, .in = "MAINTENANCE-ROOM", .synonyms = kSyn_SCREWDRIVER, .adjectives = kAdj_SCREWDRIVER, .desc = "screwdriver", .flags = kFl_SCREWDRIVER, .file = "1dungeon.zil", .line = 834},
    {.def_index = 90, .name = "KEYS", .role = ObjRole::Game, .in = "MAZE-5", .synonyms = kSyn_KEYS, .adjectives = kAdj_KEYS, .desc = "skeleton key", .flags = kFl_KEYS, .size = 10, .file = "1dungeon.zil", .line = 841},
    {.def_index = 91, .name = "SHOVEL", .role = ObjRole::Game, .in = "SANDY-BEACH", .synonyms = kSyn_SHOVEL, .desc = "shovel", .flags = kFl_SHOVEL, .size = 15, .file = "1dungeon.zil", .line = 849},
    {.def_index = 92, .name = "COAL", .role = ObjRole::Game, .in = "DEAD-END-5", .synonyms = kSyn_COAL, .adjectives = kAdj_COAL, .desc = "small pile of coal", .flags = kFl_COAL, .size = 20, .file = "1dungeon.zil", .line = 856},
    {.def_index = 93, .name = "LADDER", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_LADDER, .adjectives = kAdj_LADDER, .desc = "wooden ladder", .flags = kFl_LADDER, .file = "1dungeon.zil", .line = 864},
    {.def_index = 94, .name = "SCARAB", .role = ObjRole::Game, .in = "SANDY-CAVE", .synonyms = kSyn_SCARAB, .adjectives = kAdj_SCARAB, .desc = "beautiful jeweled scarab", .flags = kFl_SCARAB, .size = 8, .value = 5, .tvalue = 5, .file = "1dungeon.zil", .line = 871},
    {.def_index = 95, .name = "LARGE-BAG", .role = ObjRole::Game, .in = "THIEF", .synonyms = kSyn_LARGE_BAG, .adjectives = kAdj_LARGE_BAG, .desc = "large bag", .flags = kFl_LARGE_BAG, .action = "LARGE-BAG-F", .file = "1dungeon.zil", .line = 881},
    {.def_index = 96, .name = "STILETTO", .role = ObjRole::Game, .in = "THIEF", .synonyms = kSyn_STILETTO, .adjectives = kAdj_STILETTO, .desc = "stiletto", .flags = kFl_STILETTO, .size = 10, .action = "STILETTO-FUNCTION", .file = "1dungeon.zil", .line = 889},
    {.def_index = 97, .name = "MACHINE-SWITCH", .role = ObjRole::Game, .in = "MACHINE-ROOM", .synonyms = kSyn_MACHINE_SWITCH, .desc = "switch", .flags = kFl_MACHINE_SWITCH, .action = "MSWITCH-FUNCTION", .file = "1dungeon.zil", .line = 898},
    {.def_index = 98, .name = "WOODEN-DOOR", .role = ObjRole::Game, .in = "LIVING-ROOM", .synonyms = kSyn_WOODEN_DOOR, .adjectives = kAdj_WOODEN_DOOR, .desc = "wooden door", .text = "The engravings translate to \"This space intentionally left blank.\"", .flags = kFl_WOODEN_DOOR, .action = "FRONT-DOOR-FCN", .file = "1dungeon.zil", .line = 905},
    {.def_index = 99, .name = "SWORD", .role = ObjRole::Game, .in = "LIVING-ROOM", .synonyms = kSyn_SWORD, .adjectives = kAdj_SWORD, .desc = "sword", .fdesc = "Above the trophy case hangs an elvish sword of great antiquity.", .flags = kFl_SWORD, .size = 30, .tvalue = 0, .action = "SWORD-FCN", .file = "1dungeon.zil", .line = 915},
    {.def_index = 100, .name = "MAP", .role = ObjRole::Game, .in = "TROPHY-CASE", .synonyms = kSyn_MAP, .adjectives = kAdj_MAP, .desc = "ancient map", .fdesc = "In the trophy case is an ancient parchment which appears to be a map.", .text = "The map shows a forest with three clearings. The largest clearing contains a house. Three paths leave the large clearing. One of these paths, leading southwest, is marked \"To Stone Barrow\".", .flags = kFl_MAP, .size = 2, .file = "1dungeon.zil", .line = 927},
    {.def_index = 101, .name = "BOAT-LABEL", .role = ObjRole::Game, .in = "INFLATED-BOAT", .synonyms = kSyn_BOAT_LABEL, .adjectives = kAdj_BOAT_LABEL, .desc = "tan label", .text = "  !!!!FROBOZZ MAGIC BOAT COMPANY!!!!\n\nHello, Sailor!\n\nInstructions for use:\n\n   To get into a body of water, say \"Launch\".\n   To get to shore, say \"Land\" or the direction in which you want to maneuver the boat.\n\nWarranty:\n\n  This boat is guaranteed against all defects for a period of 76 milliseconds from date of purchase or until first used, whichever comes first.\n\nWarning:\n   This boat is made of thin plastic.\n   Good Luck!", .flags = kFl_BOAT_LABEL, .size = 2, .file = "1dungeon.zil", .line = 941},
    {.def_index = 102, .name = "THIEF", .role = ObjRole::Game, .in = "ROUND-ROOM", .synonyms = kSyn_THIEF, .adjectives = kAdj_THIEF, .desc = "thief", .ldesc = "There is a suspicious-looking individual, holding a large bag, leaning against one wall. He is armed with a deadly stiletto.", .flags = kFl_THIEF, .strength = 5, .action = "ROBBER-FUNCTION", .file = "1dungeon.zil", .line = 968},
    {.def_index = 103, .name = "PEDESTAL", .role = ObjRole::Game, .in = "TORCH-ROOM", .synonyms = kSyn_PEDESTAL, .adjectives = kAdj_PEDESTAL, .desc = "pedestal", .flags = kFl_PEDESTAL, .capacity = 30, .action = "DUMB-CONTAINER", .file = "1dungeon.zil", .line = 980},
    {.def_index = 104, .name = "TORCH", .role = ObjRole::Game, .in = "PEDESTAL", .synonyms = kSyn_TORCH, .adjectives = kAdj_TORCH, .desc = "torch", .fdesc = "Sitting on the pedestal is a flaming torch, made of ivory.", .flags = kFl_TORCH, .size = 20, .value = 14, .tvalue = 6, .action = "TORCH-OBJECT", .file = "1dungeon.zil", .line = 989},
    {.def_index = 105, .name = "GUIDE", .role = ObjRole::Game, .in = "DAM-LOBBY", .synonyms = kSyn_GUIDE, .adjectives = kAdj_GUIDE, .desc = "tour guidebook", .fdesc = "Some guidebooks entitled \"Flood Control Dam #3\" are on the reception desk.", .text = "\"\tFlood Control Dam #3\n\nFCD#3 was constructed in year 783 of the Great Underground Empire to harness the mighty Frigid River. This work was supported by a grant of 37 million zorkmids from your omnipotent local tyrant Lord Dimwit Flathead the Excessive. This impressive structure is composed of 370,000 cubic feet of concrete, is 256 feet tall at the center, and 193 feet wide at the top. The lake created behind the dam has a volume of 1.7 billion cubic feet, an area of 12 million square feet, and a shore line of 36 thousand feet.\n\nThe construction of FCD#3 took 112 days from ground breaking to the dedication. It required a work force of 384 slaves, 34 slave drivers, 12 engineers, 2 turtle doves, and a partridge in a pear tree. The work was managed by a command team composed of 2345 bureaucrats, 2347 secretaries (at least two of whom could type), 12,256 paper shufflers, 52,469 rubber stampers, 245,193 red tape processors, and nearly one million dead trees.\n\nWe will now point out some of the more interesting features of FCD#3 as we conduct you on a guided tour of the facilities:\n\n        1) You start your tour here in the Dam Lobby. You will notice on your right that....", .flags = kFl_GUIDE, .file = "1dungeon.zil", .line = 1001},
    {.def_index = 106, .name = "TROLL", .role = ObjRole::Game, .in = "TROLL-ROOM", .synonyms = kSyn_TROLL, .adjectives = kAdj_TROLL, .desc = "troll", .ldesc = "A nasty-looking troll, brandishing a bloody axe, blocks all passages out of the room.", .flags = kFl_TROLL, .strength = 2, .action = "TROLL-FCN", .file = "1dungeon.zil", .line = 1036},
    {.def_index = 107, .name = "TRUNK", .role = ObjRole::Game, .in = "RESERVOIR", .synonyms = kSyn_TRUNK, .adjectives = kAdj_TRUNK, .desc = "trunk of jewels", .fdesc = "Lying half buried in the mud is an old trunk, bulging with jewels.", .ldesc = "There is an old trunk here, bulging with assorted jewels.", .flags = kFl_TRUNK, .size = 35, .value = 15, .tvalue = 5, .action = "TRUNK-F", .file = "1dungeon.zil", .line = 1048},
    {.def_index = 108, .name = "TUBE", .role = ObjRole::Game, .in = "MAINTENANCE-ROOM", .synonyms = kSyn_TUBE, .desc = "tube", .ldesc = "There is an object which looks like a tube of toothpaste here.", .text = "---> Frobozz Magic Gunk Company <---\n\t  All-Purpose Gunk", .flags = kFl_TUBE, .size = 5, .capacity = 7, .action = "TUBE-FUNCTION", .file = "1dungeon.zil", .line = 1062},
    {.def_index = 109, .name = "PUTTY", .role = ObjRole::Game, .in = "TUBE", .synonyms = kSyn_PUTTY, .adjectives = kAdj_PUTTY, .desc = "viscous material", .flags = kFl_PUTTY, .size = 6, .action = "PUTTY-FCN", .file = "1dungeon.zil", .line = 1076},
    {.def_index = 110, .name = "ENGRAVINGS", .role = ObjRole::Game, .in = "ENGRAVINGS-CAVE", .synonyms = kSyn_ENGRAVINGS, .adjectives = kAdj_ENGRAVINGS, .desc = "wall with engravings", .ldesc = "There are old engravings on the walls here.", .text = "The engravings were incised in the living rock of the cave wall by an unknown hand. They depict, in symbolic form, the beliefs of the ancient Zorkers. Skillfully interwoven with the bas reliefs are excerpts illustrating the major religious tenets of that time. Unfortunately, a later age seems to have considered them blasphemous and just as skillfully excised them.", .flags = kFl_ENGRAVINGS, .file = "1dungeon.zil", .line = 1085},
    {.def_index = 111, .name = "OWNERS-MANUAL", .role = ObjRole::Game, .in = "STUDIO", .synonyms = kSyn_OWNERS_MANUAL, .adjectives = kAdj_OWNERS_MANUAL, .desc = "ZORK owner's manual", .fdesc = "Loosely attached to a wall is a small piece of paper.", .text = "Congratulations!\n\nYou are the privileged owner of ZORK I: The Great Underground Empire, a self-contained and self-maintaining universe. If used and maintained in accordance with normal operating practices for small universes, ZORK will provide many months of trouble-free operation.", .flags = kFl_OWNERS_MANUAL, .file = "1dungeon.zil", .line = 1100},
    {.def_index = 112, .name = "CLIMBABLE-CLIFF", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_CLIMBABLE_CLIFF, .adjectives = kAdj_CLIMBABLE_CLIFF, .desc = "cliff", .flags = kFl_CLIMBABLE_CLIFF, .action = "CLIFF-OBJECT", .file = "1dungeon.zil", .line = 1115},
    {.def_index = 113, .name = "WHITE-CLIFF", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_WHITE_CLIFF, .adjectives = kAdj_WHITE_CLIFF, .desc = "white cliffs", .flags = kFl_WHITE_CLIFF, .action = "WCLIF-OBJECT", .file = "1dungeon.zil", .line = 1123},
    {.def_index = 114, .name = "WRENCH", .role = ObjRole::Game, .in = "MAINTENANCE-ROOM", .synonyms = kSyn_WRENCH, .desc = "wrench", .flags = kFl_WRENCH, .size = 10, .file = "1dungeon.zil", .line = 1131},
    {.def_index = 115, .name = "CONTROL-PANEL", .role = ObjRole::Game, .in = "DAM-ROOM", .synonyms = kSyn_CONTROL_PANEL, .adjectives = kAdj_CONTROL_PANEL, .desc = "control panel", .flags = kFl_CONTROL_PANEL, .file = "1dungeon.zil", .line = 1138},
    {.def_index = 116, .name = "NEST", .role = ObjRole::Game, .in = "UP-A-TREE", .synonyms = kSyn_NEST, .adjectives = kAdj_NEST, .desc = "bird's nest", .fdesc = "Beside you on the branch is a small bird's nest.", .flags = kFl_NEST, .capacity = 20, .file = "1dungeon.zil", .line = 1145},
    {.def_index = 117, .name = "EGG", .role = ObjRole::Game, .in = "NEST", .synonyms = kSyn_EGG, .adjectives = kAdj_EGG, .desc = "jewel-encrusted egg", .fdesc = "In the bird's nest is a large egg encrusted with precious jewels, apparently scavenged by a childless songbird. The egg is covered with fine gold inlay, and ornamented in lapis lazuli and mother-of-pearl. Unlike most eggs, this one is hinged and closed with a delicate looking clasp. The egg appears extremely fragile.", .flags = kFl_EGG, .capacity = 6, .value = 5, .tvalue = 5, .action = "EGG-OBJECT", .file = "1dungeon.zil", .line = 1154},
    {.def_index = 118, .name = "BROKEN-EGG", .role = ObjRole::Game, .synonyms = kSyn_BROKEN_EGG, .adjectives = kAdj_BROKEN_EGG, .desc = "broken jewel-encrusted egg", .ldesc = "There is a somewhat ruined egg here.", .flags = kFl_BROKEN_EGG, .capacity = 6, .tvalue = 2, .file = "1dungeon.zil", .line = 1171},
    {.def_index = 119, .name = "BAUBLE", .role = ObjRole::Game, .synonyms = kSyn_BAUBLE, .adjectives = kAdj_BAUBLE, .desc = "beautiful brass bauble", .flags = kFl_BAUBLE, .value = 1, .tvalue = 1, .file = "1dungeon.zil", .line = 1180},
    {.def_index = 120, .name = "CANARY", .role = ObjRole::Game, .in = "EGG", .synonyms = kSyn_CANARY, .adjectives = kAdj_CANARY, .desc = "golden clockwork canary", .fdesc = "There is a golden clockwork canary nestled in the egg. It has ruby eyes and a silver beak. Through a crystal window below its left wing you can see intricate machinery inside. It appears to have wound down.", .flags = kFl_CANARY, .value = 6, .tvalue = 4, .action = "CANARY-OBJECT", .file = "1dungeon.zil", .line = 1188},
    {.def_index = 121, .name = "BROKEN-CANARY", .role = ObjRole::Game, .in = "BROKEN-EGG", .synonyms = kSyn_BROKEN_CANARY, .adjectives = kAdj_BROKEN_CANARY, .desc = "broken clockwork canary", .fdesc = "There is a golden clockwork canary nestled in the egg. It seems to have recently had a bad experience. The mountings for its jewel-like eyes are empty, and its silver beak is crumpled. Through a cracked crystal window below its left wing you can see the remains of intricate machinery. It is not clear what result winding it would have, as the mainspring seems sprung.", .flags = kFl_BROKEN_CANARY, .tvalue = 1, .action = "CANARY-OBJECT", .file = "1dungeon.zil", .line = 1203},
    {.def_index = 232, .name = "GLOBAL-OBJECTS", .role = ObjRole::CompilerArtifact, .flags = kFl_GLOBAL_OBJECTS, .file = "gglobals.zil", .line = 7},
    {.def_index = 233, .name = "LOCAL-GLOBALS", .role = ObjRole::CompilerArtifact, .in = "GLOBAL-OBJECTS", .synonyms = kSyn_LOCAL_GLOBALS, .fdesc = "F", .ldesc = "F", .size = 0, .capacity = 0, .vtype = "1", .descfcn = "PATH-OBJECT", .globals = kGl_LOCAL_GLOBALS, .pseudo = kPs_LOCAL_GLOBALS, .file = "gglobals.zil", .line = 12},
    {.def_index = 234, .name = "ROOMS", .role = ObjRole::CompilerArtifact, .exits = kEx_ROOMS, .file = "gglobals.zil", .line = 28},
    {.def_index = 235, .name = "INTNUM", .role = ObjRole::Parser, .in = "GLOBAL-OBJECTS", .synonyms = kSyn_INTNUM, .desc = "number", .flags = kFl_INTNUM, .file = "gglobals.zil", .line = 31},
    {.def_index = 236, .name = "PSEUDO-OBJECT", .role = ObjRole::Parser, .in = "LOCAL-GLOBALS", .desc = "pseudo", .action = "CRETIN-FCN", .file = "gglobals.zil", .line = 37},
    {.def_index = 237, .name = "IT", .role = ObjRole::Parser, .in = "GLOBAL-OBJECTS", .synonyms = kSyn_IT, .desc = "random object", .flags = kFl_IT, .file = "gglobals.zil", .line = 42},
    {.def_index = 238, .name = "NOT-HERE-OBJECT", .role = ObjRole::Parser, .desc = "such thing", .action = "NOT-HERE-OBJECT-F", .file = "gglobals.zil", .line = 48},
    {.def_index = 239, .name = "BLESSINGS", .role = ObjRole::Game, .in = "GLOBAL-OBJECTS", .synonyms = kSyn_BLESSINGS, .desc = "blessings", .flags = kFl_BLESSINGS, .file = "gglobals.zil", .line = 96},
    {.def_index = 240, .name = "STAIRS", .role = ObjRole::Game, .in = "LOCAL-GLOBALS", .synonyms = kSyn_STAIRS, .adjectives = kAdj_STAIRS, .desc = "stairs", .flags = kFl_STAIRS, .action = "STAIRS-F", .file = "gglobals.zil", .line = 102},
    {.def_index = 241, .name = "SAILOR", .role = ObjRole::Game, .in = "GLOBAL-OBJECTS", .synonyms = kSyn_SAILOR, .desc = "sailor", .flags = kFl_SAILOR, .action = "SAILOR-FCN", .file = "gglobals.zil", .line = 115},
    {.def_index = 242, .name = "GROUND", .role = ObjRole::Game, .in = "GLOBAL-OBJECTS", .synonyms = kSyn_GROUND, .desc = "ground", .action = "GROUND-FUNCTION", .file = "gglobals.zil", .line = 164},
    {.def_index = 243, .name = "GRUE", .role = ObjRole::Game, .in = "GLOBAL-OBJECTS", .synonyms = kSyn_GRUE, .adjectives = kAdj_GRUE, .desc = "lurking grue", .action = "GRUE-FUNCTION", .file = "gglobals.zil", .line = 184},
    {.def_index = 244, .name = "LUNGS", .role = ObjRole::Game, .in = "GLOBAL-OBJECTS", .synonyms = kSyn_LUNGS, .desc = "blast of air", .flags = kFl_LUNGS, .file = "gglobals.zil", .line = 208},
    {.def_index = 245, .name = "ME", .role = ObjRole::Game, .in = "GLOBAL-OBJECTS", .synonyms = kSyn_ME, .desc = "you", .flags = kFl_ME, .action = "CRETIN-FCN", .file = "gglobals.zil", .line = 214},
    {.def_index = 246, .name = "ADVENTURER", .role = ObjRole::Game, .synonyms = kSyn_ADVENTURER, .desc = "cretin", .flags = kFl_ADVENTURER, .strength = 0, .file = "gglobals.zil", .line = 267},
    {.def_index = 247, .name = "PATHOBJ", .role = ObjRole::Game, .in = "GLOBAL-OBJECTS", .synonyms = kSyn_PATHOBJ, .adjectives = kAdj_PATHOBJ, .desc = "passage", .flags = kFl_PATHOBJ, .action = "PATH-OBJECT", .file = "gglobals.zil", .line = 274},
    {.def_index = 248, .name = "ZORKMID", .role = ObjRole::Game, .in = "GLOBAL-OBJECTS", .synonyms = kSyn_ZORKMID, .desc = "zorkmid", .action = "ZORKMID-FUNCTION", .file = "gglobals.zil", .line = 290},
    {.def_index = 249, .name = "HANDS", .role = ObjRole::Game, .in = "GLOBAL-OBJECTS", .synonyms = kSyn_HANDS, .adjectives = kAdj_HANDS, .desc = "pair of hands", .flags = kFl_HANDS, .file = "gglobals.zil", .line = 304},
};

constexpr std::string_view kDirectionsArr[] = {"NORTH", "EAST", "WEST", "SOUTH", "NE", "NW", "SE", "SW", "UP", "DOWN", "IN", "OUT", "LAND"};
constexpr GlobalDef kGlobalsArr[] = {{"SCORE-MAX", 350}, {"FALSE-FLAG", 0}, {"CYCLOPS-FLAG", 0}, {"DEFLATE", 0}, {"DOME-FLAG", 0}, {"EMPTY-HANDED", 0}, {"LLD-FLAG", 0}, {"LOW-TIDE", 0}, {"MAGIC-FLAG", 0}, {"RAINBOW-FLAG", 0}, {"TROLL-FLAG", 0}, {"WON-FLAG", 0}, {"COFFIN-CURE", 0}, {"LOAD-MAX", 100}, {"LOAD-ALLOWED", 100}};
constexpr std::string_view kTab_HOUSE_AROUND[] = {"WEST-OF-HOUSE", "NORTH-OF-HOUSE", "EAST-OF-HOUSE", "SOUTH-OF-HOUSE", "WEST-OF-HOUSE"};
constexpr std::string_view kTab_FOREST_AROUND[] = {"FOREST-1", "FOREST-2", "FOREST-3", "PATH", "CLEARING", "FOREST-1"};
constexpr std::string_view kTab_IN_HOUSE_AROUND[] = {"LIVING-ROOM", "KITCHEN", "ATTIC", "KITCHEN"};
constexpr std::string_view kTab_ABOVE_GROUND[] = {"WEST-OF-HOUSE", "NORTH-OF-HOUSE", "EAST-OF-HOUSE", "SOUTH-OF-HOUSE", "FOREST-1", "FOREST-2", "FOREST-3", "PATH", "CLEARING", "GRATING-CLEARING", "CANYON-VIEW"};
constexpr WalkTable kWalkTablesArr[] = {{"HOUSE-AROUND", kTab_HOUSE_AROUND}, {"FOREST-AROUND", kTab_FOREST_AROUND}, {"IN-HOUSE-AROUND", kTab_IN_HOUSE_AROUND}, {"ABOVE-GROUND", kTab_ABOVE_GROUND}};
constexpr PropDefault kPropDefaultsArr[] = {{"SIZE", 5}, {"CAPACITY", 0}, {"VALUE", 0}, {"TVALUE", 0}};
constexpr Word kVocabularyArr[] = {{"BOARDS", "boards"}, {"BOARD", "board"}, {"OVERBOARD", "overbo"}, {"TEETH", "teeth"}, {"WALL", "wall"}, {"WALLS", "walls"}, {"SURROUNDING", "surrou"}, {"GRANITE", "granit"}, {"BIRD", "bird"}, {"SONGBIRD", "songbi"}, {"SONG", "song"}, {"HOUSE", "house"}, {"WHITE", "white"}, {"BEAUTI", "beauti"}, {"COLONI", "coloni"}, {"FOREST", "forest"}, {"TREES", "trees"}, {"PINES", "pines"}, {"HEMLOCKS", "hemloc"}, {"TREE", "tree"}, {"BRANCH", "branch"}, {"LARGE", "large"}, {"STORM", "storm"}, {"MOUNTAIN", "mounta"}, {"RANGE", "range"}, {"IMPASSABLE", "impass"}, {"FLATHEAD", "flathe"}, {"WATER", "water"}, {"QUANTITY", "quanti"}, {"LIQUID", "liquid"}, {"H2O", "h2o"}, {"WINDOW", "window"}, {"KITCHEN", "kitche"}, {"SMALL", "small"}, {"CHIMNEY", "chimne"}, {"DARK", "dark"}, {"NARROW", "narrow"}, {"GHOSTS", "ghosts"}, {"SPIRITS", "spirit"}, {"FIENDS", "fiends"}, {"FORCE", "force"}, {"INVISIBLE", "invisi"}, {"EVIL", "evil"}, {"SKULL", "skull"}, {"HEAD", "head"}, {"TREASURE", "treasu"}, {"CRYSTAL", "crysta"}, {"CAGE", "cage"}, {"DUMBWAITER", "dumbwa"}, {"BASKET", "basket"}, {"LOWERED", "lowere"}, {"FOOD", "food"}, {"SANDWICH", "sandwi"}, {"LUNCH", "lunch"}, {"DINNER", "dinner"}, {"HOT", "hot"}, {"PEPPER", "pepper"}, {"BAT", "bat"}, {"VAMPIRE", "vampir"}, {"DERANGED", "derang"}, {"BELL", "bell"}, {"BRASS", "brass"}, {"RED", "red"}, {"AXE", "axe"}, {"AX", "ax"}, {"BLOODY", "bloody"}, {"BOLT", "bolt"}, {"NUT", "nut"}, {"METAL", "metal"}, {"BUBBLE", "bubble"}, {"GREEN", "green"}, {"PLASTIC", "plasti"}, {"ALTAR", "altar"}, {"BOOK", "book"}, {"PRAYER", "prayer"}, {"PAGE", "page"}, {"BOOKS", "books"}, {"BLACK", "black"}, {"LAMP", "lamp"}, {"LANTERN", "lanter"}, {"BROKEN", "broken"}, {"SCEPTRE", "sceptr"}, {"SCEPTER", "scepte"}, {"SHARP", "sharp"}, {"EGYPTIAN", "egypti"}, {"ANCIENT", "ancien"}, {"ENAMELED", "enamel"}, {"TIMBERS", "timber"}, {"PILE", "pile"}, {"WOODEN", "wooden"}, {"CHUTE", "chute"}, {"RAMP", "ramp"}, {"SLIDE", "slide"}, {"STEEP", "steep"}, {"TWISTING", "twisti"}, {"TABLE", "table"}, {"BAG", "bag"}, {"SACK", "sack"}, {"BROWN", "brown"}, {"ELONGATED", "elonga"}, {"SMELLY", "smelly"}, {"CHEST", "chest"}, {"CHESTS", "chests"}, {"GROUP", "group"}, {"TOOLCHESTS", "toolch"}, {"TOOL", "tool"}, {"BUTTON", "button"}, {"SWITCH", "switch"}, {"YELLOW", "yellow"}, {"BLUE", "blue"}, {"CASE", "case"}, {"TROPHY", "trophy"}, {"RUG", "rug"}, {"CARPET", "carpet"}, {"ORIENTAL", "orient"}, {"CHALICE", "chalic"}, {"CUP", "cup"}, {"SILVER", "silver"}, {"ENGRAVINGS", "engrav"}, {"GARLIC", "garlic"}, {"CLOVE", "clove"}, {"TRIDENT", "triden"}, {"FORK", "fork"}, {"POSEIDON", "poseid"}, {"OWN", "own"}, {"CYCLOPS", "cyclop"}, {"MONSTER", "monste"}, {"EYE", "eye"}, {"HUNGRY", "hungry"}, {"GIANT", "giant"}, {"DAM", "dam"}, {"GATE", "gate"}, {"GATES", "gates"}, {"FCD#3", "fcd#"}, {"DOOR", "door"}, {"TRAPDOOR", "trapdo"}, {"TRAP-DOOR", "trap-"}, {"COVER", "cover"}, {"TRAP", "trap"}, {"DUSTY", "dusty"}, {"BOARDED", "boarde"}, {"FRONT", "front"}, {"HUGE", "huge"}, {"STONE", "stone"}, {"BARROW", "barrow"}, {"TOMB", "tomb"}, {"MASSIVE", "massiv"}, {"BOTTLE", "bottle"}, {"CONTAINER", "contai"}, {"CLEAR", "clear"}, {"GLASS", "glass"}, {"CRACK", "crack"}, {"COFFIN", "coffin"}, {"CASKET", "casket"}, {"SOLID", "solid"}, {"GOLD", "gold"}, {"GRATE", "grate"}, {"GRATING", "gratin"}, {"PUMP", "pump"}, {"AIR-PUMP", "air-p"}, {"TOOLS", "tools"}, {"HAND-HELD", "hand-"}, {"DIAMOND", "diamon"}, {"ENORMOUS", "enormo"}, {"FIGURINE", "figuri"}, {"EXQUISITE", "exquis"}, {"JADE", "jade"}, {"KNIVES", "knives"}, {"KNIFE", "knife"}, {"BLADE", "blade"}, {"NASTY", "nasty"}, {"UNRUSTY", "unrust"}, {"BONES", "bones"}, {"SKELETON", "skelet"}, {"BODY", "body"}, {"RUSTY", "rusty"}, {"BURNED", "burned"}, {"DEAD", "dead"}, {"USELESS", "useles"}, {"COINS", "coins"}, {"OLD", "old"}, {"LEATHER", "leathe"}, {"LIGHT", "light"}, {"EMERALD", "emeral"}, {"ADVERTISEMENT", "advert"}, {"LEAFLET", "leafle"}, {"BOOKLET", "bookle"}, {"MAIL", "mail"}, {"LEAK", "leak"}, {"DRIP", "drip"}, {"PIPE", "pipe"}, {"MACHINE", "machin"}, {"PDP10", "pdp1"}, {"DRYER", "dryer"}, {"LID", "lid"}, {"BOAT", "boat"}, {"RAFT", "raft"}, {"INFLAT", "inflat"}, {"MAGIC", "magic"}, {"SEAWORTHY", "seawor"}, {"MAILBOX", "mailbo"}, {"BOX", "box"}, {"MATCH", "match"}, {"MATCHES", "matche"}, {"MATCHBOOK", "matchb"}, {"REFLECTION", "reflec"}, {"MIRROR", "mirror"}, {"PAINTING", "painti"}, {"ART", "art"}, {"CANVAS", "canvas"}, {"CANDLES", "candle"}, {"PAIR", "pair"}, {"BURNING", "burnin"}, {"GUNK", "gunk"}, {"PIECE", "piece"}, {"SLAG", "slag"}, {"VITREOUS", "vitreo"}, {"BODIES", "bodies"}, {"REMAINS", "remain"}, {"MANGLED", "mangle"}, {"LEAVES", "leaves"}, {"LEAF", "leaf"}, {"PUNCTURE", "punctu"}, {"VALVE", "valve"}, {"BAR", "bar"}, {"PLATINUM", "platin"}, {"POT", "pot"}, {"INSCRIPTION", "inscri"}, {"RAILING", "railin"}, {"RAIL", "rail"}, {"RAINBOW", "rainbo"}, {"RIVER", "river"}, {"FRIGID", "frigid"}, {"BUOY", "buoy"}, {"ROPE", "rope"}, {"HEMP", "hemp"}, {"COIL", "coil"}, {"SAND", "sand"}, {"BRACELET", "bracel"}, {"JEWEL", "jewel"}, {"SAPPHIRE", "sapphi"}, {"SCREWDRIVER", "screwd"}, {"DRIVER", "driver"}, {"SCREW", "screw"}, {"KEY", "key"}, {"SHOVEL", "shovel"}, {"COAL", "coal"}, {"HEAP", "heap"}, {"LADDER", "ladder"}, {"RICKETY", "ricket"}, {"SCARAB", "scarab"}, {"BUG", "bug"}, {"BEETLE", "beetle"}, {"CARVED", "carved"}, {"JEWELED", "jewele"}, {"THIEFS", "thiefs"}, {"STILETTO", "stilet"}, {"VICIOUS", "viciou"}, {"LETTERING", "letter"}, {"WRITING", "writin"}, {"GOTHIC", "gothic"}, {"STRANGE", "strang"}, {"WEST", "west"}, {"SWORD", "sword"}, {"ORCRIST", "orcris"}, {"GLAMDRING", "glamdr"}, {"ELVISH", "elvish"}, {"ANTIQUE", "antiqu"}, {"PARCHMENT", "parchm"}, {"MAP", "map"}, {"LABEL", "label"}, {"FINEPRINT", "finepr"}, {"PRINT", "print"}, {"TAN", "tan"}, {"FINE", "fine"}, {"THIEF", "thief"}, {"ROBBER", "robber"}, {"MAN", "man"}, {"PERSON", "person"}, {"SHADY", "shady"}, {"SUSPICIOUS", "suspic"}, {"SEEDY", "seedy"}, {"PEDESTAL", "pedest"}, {"MARBLE", "marble"}, {"TORCH", "torch"}, {"IVORY", "ivory"}, {"FLAMING", "flamin"}, {"GUIDE", "guide"}, {"GUIDEBOOKS", "guideb"}, {"TOUR", "tour"}, {"TROLL", "troll"}, {"TRUNK", "trunk"}, {"JEWELS", "jewels"}, {"TUBE", "tube"}, {"TOOTH", "tooth"}, {"PASTE", "paste"}, {"MATERIAL", "materi"}, {"VISCOUS", "viscou"}, {"MANUAL", "manual"}, {"PAPER", "paper"}, {"ZORK", "zork"}, {"OWNERS", "owners"}, {"CLIFF", "cliff"}, {"LEDGE", "ledge"}, {"ROCKY", "rocky"}, {"SHEER", "sheer"}, {"CLIFFS", "cliffs"}, {"WRENCH", "wrench"}, {"PANEL", "panel"}, {"CONTROL", "contro"}, {"NEST", "nest"}, {"BIRDS", "birds"}, {"EGG", "egg"}, {"ENCRUSTED", "encrus"}, {"BAUBLE", "bauble"}, {"CANARY", "canary"}, {"CLOCKWORK", "clockw"}, {"GOLDEN", "golden"}, {"ZZMGCK", "zzmgck"}, {"INTNUM", "intnum"}, {"IT", "it"}, {"THEM", "them"}, {"HER", "her"}, {"HIM", "him"}, {"BLESSINGS", "blessi"}, {"GRACES", "graces"}, {"STAIRS", "stairs"}, {"STEPS", "steps"}, {"STAIRCASE", "stairc"}, {"STAIRWAY", "stairw"}, {"FORBIDDING", "forbid"}, {"SAILOR", "sailor"}, {"FOOTPAD", "footpa"}, {"AVIATOR", "aviato"}, {"GROUND", "ground"}, {"DIRT", "dirt"}, {"FLOOR", "floor"}, {"GRUE", "grue"}, {"LURKING", "lurkin"}, {"SINISTER", "sinist"}, {"SILENT", "silent"}, {"LUNGS", "lungs"}, {"AIR", "air"}, {"MOUTH", "mouth"}, {"BREATH", "breath"}, {"ME", "me"}, {"MYSELF", "myself"}, {"SELF", "self"}, {"CRETIN", "cretin"}, {"ADVENTURER", "advent"}, {"TRAIL", "trail"}, {"PATH", "path"}, {"LONG", "long"}, {"WINDING", "windin"}, {"ZORKMID", "zorkmi"}, {"HANDS", "hands"}, {"HAND", "hand"}, {"BARE", "bare"}};
constexpr std::string_view kFlagNamesArr[] = {"RLANDBIT", "ONBIT", "SACREDBIT", "MAZEBIT", "NONLANDBIT", "NDESCBIT", "CLIMBBIT", "DRINKBIT", "TRYTAKEBIT", "TAKEBIT", "DOORBIT", "ACTORBIT", "TRANSBIT", "CONTBIT", "OPENBIT", "FOODBIT", "WEAPONBIT", "TURNBIT", "SURFACEBIT", "READBIT", "BURNBIT", "SEARCHBIT", "INVISIBLE", "TOOLBIT", "LIGHTBIT", "VEHBIT", "FLAMEBIT", "RMUNGBIT", "TOUCHBIT", "FIGHTBIT", "STAGGERED", "WEARBIT"};
constexpr std::string_view kRoutineNamesArr[] = {"WEST-HOUSE", "STONE-BARROW-FCN", "EAST-HOUSE", "FOREST-ROOM", "TREE-ROOM", "GRATING-EXIT", "CLEARING-FCN", "KITCHEN-FCN", "NAILS-PSEUDO", "TRAP-DOOR-EXIT", "LIVING-ROOM-FCN", "CELLAR-FCN", "TROLL-ROOM-F", "CHASM-PSEUDO", "DOOR-PSEUDO", "PAINT-PSEUDO", "UP-CHIMNEY-FUNCTION", "MAZE-DIODES", "MAZE-11-FCN", "CYCLOPS-ROOM-FCN", "TREASURE-ROOM-FCN", "LAKE-PSEUDO", "RESERVOIR-SOUTH-FCN", "STREAM-PSEUDO", "RESERVOIR-FCN", "RESERVOIR-NORTH-FCN", "MIRROR-ROOM", "CAVE2-ROOM", "DEEP-CANYON-F", "LOUD-ROOM-FCN", "GATE-PSEUDO", "LLD-ROOM", "DOME-PSEUDO", "DOME-ROOM-FCN", "TORCH-ROOM-FCN", "SOUTH-TEMPLE-FCN", "DAM-ROOM-FCN", "WHITE-CLIFFS-FUNCTION", "RIVR4-ROOM", "FALLS-ROOM", "CANYON-VIEW-F", "BATS-ROOM", "CHAIN-PSEUDO", "GAS-PSEUDO", "BOOM-ROOM", "NO-OBJS", "MACHINE-ROOM-FCN", "BOARD-F", "TEETH-F", "GRANITE-WALL-F", "SONGBIRD-F", "WHITE-HOUSE-F", "FOREST-F", "MOUNTAIN-RANGE-F", "WATER-F", "KITCHEN-WINDOW-F", "CHIMNEY-F", "GHOSTS-F", "BASKET-F", "BAT-F", "BAT-D", "BELL-F", "HOT-BELL-F", "AXE-F", "BOLT-F", "BUBBLE-F", "BLACK-BOOK", "SCEPTRE-FUNCTION", "SLIDE-FUNCTION", "SANDWICH-BAG-FCN", "TOOL-CHEST-FCN", "BUTTON-F", "TROPHY-CASE-FCN", "RUG-FCN", "CHALICE-FCN", "GARLIC-F", "CYCLOPS-FCN", "DAM-FUNCTION", "TRAP-DOOR-FCN", "BOARDED-WINDOW-FCN", "FRONT-DOOR-FCN", "BARROW-DOOR-FCN", "BARROW-FCN", "BOTTLE-FUNCTION", "CRACK-FCN", "GRATE-FUNCTION", "KNIFE-F", "SKELETON", "BAG-OF-COINS-F", "LANTERN", "LEAK-FUNCTION", "MACHINE-F", "RBOAT-FUNCTION", "MAILBOX-F", "MATCH-FUNCTION", "MIRROR-MIRROR", "PAINTING-FCN", "CANDLES-FCN", "GUNK-FUNCTION", "BODY-FUNCTION", "LEAF-PILE", "DBOAT-FUNCTION", "IBOAT-FUNCTION", "RAINBOW-FCN", "RIVER-FUNCTION", "TREASURE-INSIDE", "ROPE-FUNCTION", "RUSTY-KNIFE-FCN", "SAND-FUNCTION", "LARGE-BAG-F", "STILETTO-FUNCTION", "MSWITCH-FUNCTION", "SWORD-FCN", "ROBBER-FUNCTION", "DUMB-CONTAINER", "TORCH-OBJECT", "TROLL-FCN", "TRUNK-F", "TUBE-FUNCTION", "PUTTY-FCN", "CLIFF-OBJECT", "WCLIF-OBJECT", "EGG-OBJECT", "CANARY-OBJECT", "V-WALK", "PATH-OBJECT", "CRETIN-FCN", "NOT-HERE-OBJECT-F", "STAIRS-F", "SAILOR-FCN", "GROUND-FUNCTION", "GRUE-FUNCTION", "ZORKMID-FUNCTION"};

}  // namespace

const std::span<const std::string_view> kDirections = kDirectionsArr;
const std::span<const RoomDef> kRooms = kRoomsArr;
const std::span<const ObjectDef> kObjects = kObjectsArr;
const std::span<const GlobalDef> kGlobals = kGlobalsArr;
const std::span<const WalkTable> kWalkTables = kWalkTablesArr;
const std::span<const PropDefault> kPropDefaults = kPropDefaultsArr;
const std::span<const Word> kVocabulary = kVocabularyArr;
const std::span<const std::string_view> kFlagNames = kFlagNamesArr;
const std::span<const std::string_view> kRoutineNames = kRoutineNamesArr;

static_assert(std::size(kRoomsArr) == kRoomCount);
static_assert(std::size(kObjectsArr) == kObjectCount);

}  // namespace zork::zil
