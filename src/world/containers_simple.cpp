// SANDWICH-BAG-FCN - Lunch bag container
// ZIL: Handles SMELL when LUNCH is inside
// Source: 1actions.zil lines 3106-3109
bool sandwichBagAction() {
  auto &g = Globals::instance();

  // Handle SMELL when lunch is in bag
  if (g.prsa == V_SMELL && g.prso) {
    // Check if LUNCH is in the bag
    ZObject *lunch = g.getObject(ObjectIds::LUNCH);
    if (lunch && lunch->getLocation() == g.prso) {
      printLine("It smells of hot peppers.");
      return RTRUE;
    }
  }

  return RFALSE;
}
