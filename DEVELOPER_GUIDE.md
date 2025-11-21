# Zork C++ Developer Guide

Quick reference for converting ZIL code to C++.

## ZIL to C++ Translation Guide

### Objects

**ZIL:**
```lisp
<OBJECT MAILBOX
    (IN WEST-OF-HOUSE)
    (SYNONYM MAILBOX BOX)
    (ADJECTIVE SMALL)
    (DESC "small mailbox")
    (FLAGS CONTBIT TRYTAKEBIT)
    (CAPACITY 10)
    (ACTION MAILBOX-F)>
```

**C++:**
```cpp
auto mailbox = std::make_unique<ZObject>(OBJ_MAILBOX, "small mailbox");
mailbox->addSynonym("mailbox");
mailbox->addSynonym("box");
mailbox->addAdjective("small");
mailbox->setFlag(ObjectFlag::CONTBIT);
mailbox->setFlag(ObjectFlag::TRYTAKEBIT);
mailbox->setProperty(P_CAPACITY, 10);
mailbox->setAction(mailboxAction);
mailbox->moveTo(g.getObject(ROOM_WEST_OF_HOUSE));
g.registerObject(OBJ_MAILBOX, std::move(mailbox));
```

### Rooms

**ZIL:**
```lisp
<ROOM WEST-OF-HOUSE
    (IN ROOMS)
    (DESC "West of House")
    (NORTH TO NORTH-OF-HOUSE)
    (SOUTH TO SOUTH-OF-HOUSE)
    (EAST "The door is boarded.")
    (ACTION WEST-HOUSE)
    (FLAGS RLANDBIT ONBIT SACREDBIT)>
```

**C++:**
```cpp
auto westOfHouse = std::make_unique<ZRoom>(
    ROOM_WEST_OF_HOUSE,
    "West of House",
    ""
);
westOfHouse->setFlag(ObjectFlag::RLANDBIT);
westOfHouse->setFlag(ObjectFlag::ONBIT);
westOfHouse->setFlag(ObjectFlag::SACREDBIT);
westOfHouse->setRoomAction(westHouseAction);
westOfHouse->setExit(Direction::NORTH, RoomExit(ROOM_NORTH_OF_HOUSE));
westOfHouse->setExit(Direction::SOUTH, RoomExit(ROOM_SOUTH_OF_HOUSE));
westOfHouse->setExit(Direction::EAST, RoomExit("The door is boarded."));
g.registerObject(ROOM_WEST_OF_HOUSE, std::move(westOfHouse));
```

### Room Actions

**ZIL:**
```lisp
<ROUTINE WEST-HOUSE (RARG)
    <COND (<EQUAL? .RARG ,M-LOOK>
        <TELL "You are standing in an open field..." CR>)>>
```

**C++:**
```cpp
void westHouseAction(int rarg) {
    if (rarg == M_LOOK) {
        printLine("You are standing in an open field...");
    }
}
```

### Object Actions

**ZIL:**
```lisp
<ROUTINE MAILBOX-F ()
    <COND (<AND <VERB? TAKE> <EQUAL? ,PRSO ,MAILBOX>>
        <TELL "It is securely anchored." CR>)>>
```

**C++:**
```cpp
bool mailboxAction() {
    auto& g = Globals::instance();
    if (g.prsa == V_TAKE && g.prso && g.prso->getId() == OBJ_MAILBOX) {
        printLine("It is securely anchored.");
        return RTRUE;
    }
    return RFALSE;
}
```

### Verbs

**ZIL:**
```lisp
<ROUTINE V-LOOK ()
    <COND (<FSET? ,HERE ,TOUCHBIT>
        <DESCRIBE-ROOM>)>>
```

**C++:**
```cpp
bool vLook() {
    auto& g = Globals::instance();
    if (g.here) {
        ZRoom* room = dynamic_cast<ZRoom*>(g.here);
        if (room) {
            room->performRoomAction(M_LOOK);
        }
    }
    return RTRUE;
}
```

## Common Patterns

### Checking Flags
```cpp
// ZIL: <FSET? ,OBJ ,TAKEBIT>
if (obj->hasFlag(ObjectFlag::TAKEBIT))

// ZIL: <FSET ,OBJ ,TAKEBIT>
obj->setFlag(ObjectFlag::TAKEBIT);

// ZIL: <FCLEAR ,OBJ ,TAKEBIT>
obj->clearFlag(ObjectFlag::TAKEBIT);
```

### Checking Verbs
```cpp
// ZIL: <VERB? TAKE DROP>
if (g.prsa == V_TAKE || g.prsa == V_DROP)
```

### Checking Objects
```cpp
// ZIL: <EQUAL? ,PRSO ,MAILBOX>
if (g.prso && g.prso->getId() == OBJ_MAILBOX)
```

### Output
```cpp
// ZIL: <TELL "Hello" CR>
printLine("Hello");

// ZIL: <TELL "Hello">
print("Hello");

// ZIL: <CRLF>
crlf();

// ZIL: <TELL "The " D ,OBJ " is here." CR>
print("The ");
printDesc(obj);
printLine(" is here.");
```

### Movement
```cpp
// ZIL: <MOVE ,OBJ ,ROOM>
obj->moveTo(room);

// ZIL: <LOC ,OBJ>
obj->getLocation()

// ZIL: <FIRST? ,CONTAINER>
container->getContents()
```

### Properties
```cpp
// ZIL: <GETP ,OBJ ,P?SIZE>
obj->getProperty(P_SIZE)

// ZIL: <PUTP ,OBJ ,P?SIZE 10>
obj->setProperty(P_SIZE, 10);
```

## Naming Conventions

### IDs
- Rooms: `ROOM_WEST_OF_HOUSE`
- Objects: `OBJ_MAILBOX`
- Verbs: `V_TAKE`
- Properties: `P_SIZE`
- Flags: `ObjectFlag::TAKEBIT`

### Functions
- Verbs: `vTake()`, `vDrop()`
- Actions: `mailboxAction()`, `westHouseAction()`
- Utilities: `camelCase`

### Variables
- Global state: `Globals::instance()`
- Local: `camelCase`

## File Organization

### Adding a New Room
1. Add ID to `world/rooms.h`
2. Create room in `world/world.cpp` `initializeWorld()`
3. Set exits, flags, action handler
4. Register with globals

### Adding a New Object
1. Add ID to `world/objects.h`
2. Create object in `world/world.cpp` `initializeWorld()`
3. Set synonyms, adjectives, flags, properties
4. Set action handler if needed
5. Move to initial location
6. Register with globals

### Adding a New Verb
1. Add ID to `verbs/verbs.h`
2. Implement handler in `verbs/verbs.cpp`
3. Add to verb dispatch table in `main.cpp`
4. Add synonyms to parser in `parser/parser.cpp`

### Adding an Action
1. Implement function in `world/world.cpp` or `actions/`
2. Set as action handler on object/room
3. Check verb and object IDs
4. Return RTRUE if handled, RFALSE otherwise

## Testing

### Unit Test
```cpp
TEST(MyTest) {
    ZObject obj(1, "test");
    obj.setFlag(ObjectFlag::TAKEBIT);
    ASSERT_TRUE(obj.hasFlag(ObjectFlag::TAKEBIT));
}
```

### Transcript Test
```cpp
TEST(MyTranscript) {
    std::vector<TranscriptStep> steps = {
        {"take mailbox", {"securely anchored"}},
        {"open mailbox", {"Opening", "leaflet"}},
    };
    runTranscript(steps);
}
```

## Common Pitfalls

1. **Forgetting to register objects**: Always call `g.registerObject()`
2. **Not checking null pointers**: Always check `g.prso` before using
3. **Wrong return values**: Actions return bool, verbs return bool
4. **Missing virtual destructor**: ZObject needs virtual destructor for polymorphism
5. **Forgetting to move objects**: Use `moveTo()` for initial placement

## Debugging Tips

1. **Print current state**: Add debug output for `g.here`, `g.prso`, `g.prsa`
2. **Check object IDs**: Verify IDs match between creation and lookup
3. **Verify flags**: Print flag values to ensure they're set correctly
4. **Test incrementally**: Add one room/object at a time
5. **Use the tests**: Write tests as you add features

## Quick Reference

### Global State
```cpp
auto& g = Globals::instance();
g.here      // Current room
g.winner    // Current actor (player)
g.prso      // Direct object
g.prsi      // Indirect object
g.prsa      // Current verb
g.lit       // Is room lit?
g.score     // Current score
g.moves     // Move count
```

### Common Checks
```cpp
// Is object takeable?
obj->hasFlag(ObjectFlag::TAKEBIT)

// Is object a container?
obj->hasFlag(ObjectFlag::CONTBIT)

// Is object open?
obj->hasFlag(ObjectFlag::OPENBIT)

// Is object lit?
obj->hasFlag(ObjectFlag::ONBIT) && obj->hasFlag(ObjectFlag::LIGHTBIT)
```

## Resources

- **Spec**: `.kiro/specs/zork-port.md`
- **Status**: `PROJECT_STATUS.md`
- **Tests**: `tests/README.md`
- **Original ZIL**: Root directory `*.zil` files

## Getting Help

1. Check this guide for common patterns
2. Look at existing implementations in `world/world.cpp`
3. Refer to original ZIL source
4. Check the spec for architecture decisions
5. Write tests to verify behavior
