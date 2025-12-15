// TRUNK-F - Trunk stupid container
// ZIL: STUPID-CONTAINER - blocks OPEN, CLOSE (already handled), EXAMINE shows
// contents Source: 1actions.zil lines 4140-4152
bool trunkAction() {
  auto &g = Globals::instance();

  // STUPID-CONTAINER pattern: OPEN prints message
  if (g.prsa == V_OPEN) {
    printLine("The cover is too clumsy for you to open. You'll have to smash "
              "the treasure chest.");
    return RTRUE;
  }

  // CLOSE not needed - container already closed

  // EXAMINE shows contents (implicit "look inside")
  if (g.prsa == V_EXAMINE) {
    printLine("The treasure chest is a large metal box with an ornate cover.");
    return RTRUE;
  }

  return RFALSE;
}
