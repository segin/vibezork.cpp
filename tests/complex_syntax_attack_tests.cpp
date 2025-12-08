#include "test_framework.h"
#include "../src/verbs/verbs.h"
#include "../src/core/globals.h"
#include "../src/world/rooms.h"
#include "../src/world/objects.h"
#include "../src/parser/parser.h"
#include <sstream>

// Output capture helpers
static std::streambuf* oldCoutPtr;
static std::stringstream captureStream;

static void startCapture() {
    oldCoutPtr = std::cout.rdbuf();
    captureStream.str("");
    captureStream.clear();
    std::cout.rdbuf(captureStream.rdbuf());
}

static std::string stopCapture() {
    std::cout.rdbuf(oldCoutPtr);
    return captureStream.str();
}

TEST(AttackWithWeaponSuccess) {
    auto& g = Globals::instance();
    g.reset();
    
    // Setup Room
    auto room = std::make_unique<ZRoom>(100, "Arena", "A dusty arena.");
    g.here = room.get();
    g.registerObject(100, std::move(room));
    
    // Setup Player
    auto player = std::make_unique<ZObject>(1, "player");
    player->setFlag(ObjectFlag::ACTORBIT);
    player->moveTo(g.here);
    g.winner = player.get();
    g.player = player.get();
    g.registerObject(1, std::move(player));

    // Setup Troll (Actor)
    auto troll = std::make_unique<ZObject>(200, "troll");
    troll->setFlag(ObjectFlag::ACTORBIT);
    troll->moveTo(g.here);
    ZObject* trollPtr = troll.get();
    g.registerObject(200, std::move(troll));
    
    // Setup Sword (Weapon)
    auto sword = std::make_unique<ZObject>(300, "sword");
    sword->setFlag(ObjectFlag::WEAPONBIT);
    sword->setFlag(ObjectFlag::TAKEBIT);
    sword->moveTo(g.player); // Held
    ZObject* swordPtr = sword.get();
    g.registerObject(300, std::move(sword));
    
    // "attack troll with sword"
    g.prso = trollPtr;
    g.prsi = swordPtr;
    
    startCapture();
    bool result = Verbs::vAttack();
    std::string output = stopCapture();
    
    // We expect successful execution (RTRUE).
    // Output might depend on combat system RNG or missing combat messages, 
    // but definitely NOT one of the failure messages.
    ASSERT_TRUE(result);
    ASSERT_NOT_CONTAINS(output, "suicidal");
    ASSERT_NOT_CONTAINS(output, "strange people");
    ASSERT_NOT_CONTAINS(output, "holding");
}

TEST(AttackBareHandsFailure) {
    auto& g = Globals::instance();
    g.reset();
    
    // ... (Setup similar to above without weapon)
    auto room = std::make_unique<ZRoom>(100, "Arena", "A dusty arena.");
    g.here = room.get();
    g.registerObject(100, std::move(room));
    
    auto player = std::make_unique<ZObject>(1, "player");
    player->setFlag(ObjectFlag::ACTORBIT);
    player->moveTo(g.here);
    g.winner = player.get();
    g.player = player.get();
    g.registerObject(1, std::move(player));

    auto troll = std::make_unique<ZObject>(200, "troll");
    troll->setFlag(ObjectFlag::ACTORBIT);
    troll->moveTo(g.here);
    ZObject* trollPtr = troll.get();
    g.registerObject(200, std::move(troll));
    
    // "attack troll" (PRSI is null)
    g.prso = trollPtr;
    g.prsi = nullptr;
    
    startCapture();
    Verbs::vAttack();
    std::string output = stopCapture();
    
    ASSERT_CONTAINS(output, "bare hands is suicidal");
}

TEST(AttackNotWeaponFailure) {
    auto& g = Globals::instance();
    g.reset();
    
    auto room = std::make_unique<ZRoom>(100, "Arena", "Desc");
    g.here = room.get();
    g.registerObject(100, std::move(room));
    
    auto player = std::make_unique<ZObject>(1, "player"); 
    player->moveTo(g.here);
    g.winner = player.get();
    g.player = player.get();
    g.registerObject(1, std::move(player));
    
    auto troll = std::make_unique<ZObject>(200, "troll");
    troll->setFlag(ObjectFlag::ACTORBIT);
    troll->moveTo(g.here);
    ZObject* trollPtr = troll.get();
    g.registerObject(200, std::move(troll));
    
    // Book (Not a weapon)
    auto book = std::make_unique<ZObject>(300, "book");
    book->moveTo(g.player);
    ZObject* bookPtr = book.get();
    g.registerObject(300, std::move(book));
    
    // "attack troll with book"
    g.prso = trollPtr;
    g.prsi = bookPtr;
    
    startCapture();
    Verbs::vAttack();
    std::string output = stopCapture();
    
    ASSERT_CONTAINS(output, "suicidal"); // "attack the troll with a book is suicidal"
}

TEST(AttackNotHeldFailure) {
    auto& g = Globals::instance();
    g.reset();
    
    auto room = std::make_unique<ZRoom>(100, "Arena", "Desc");
    g.here = room.get();
    g.registerObject(100, std::move(room));
    
    auto player = std::make_unique<ZObject>(1, "player"); 
    player->moveTo(g.here);
    g.winner = player.get();
    g.player = player.get();
    g.registerObject(1, std::move(player));
    
    auto troll = std::make_unique<ZObject>(200, "troll");
    troll->setFlag(ObjectFlag::ACTORBIT);
    troll->moveTo(g.here);
    ZObject* trollPtr = troll.get();
    g.registerObject(200, std::move(troll));

    auto sword = std::make_unique<ZObject>(300, "sword");
    sword->setFlag(ObjectFlag::WEAPONBIT);
    sword->moveTo(g.here); // On ground
    ZObject* swordPtr = sword.get();
    g.registerObject(300, std::move(sword));
    
    // "attack troll with sword"
    g.prso = trollPtr;
    g.prsi = swordPtr;
    
    startCapture();
    Verbs::vAttack();
    std::string output = stopCapture();
    
    ASSERT_CONTAINS(output, "aren't even holding");
}

int main() {
    std::cout << "Running Attack Syntax Tests\n";
    auto results = TestFramework::instance().runAll();
    int failed = 0;
    for (const auto& r : results) {
        if (!r.passed) failed++;
    }
    return failed > 0 ? 1 : 0;
}
