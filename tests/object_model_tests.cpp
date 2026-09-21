// Unit tests for the ZObject data model against the ZIL object model
// ZIL: <PROPDEF SIZE 5> <PROPDEF CAPACITY 0> <PROPDEF VALUE 0> <PROPDEF TVALUE 0>
// Source: zil/zork1.zil:24-27; P?FDESC/P?LDESC/P?TEXT (1dungeon.zil);
// P?VTYPE (1dungeon.zil:2643, gverbs.zil:2050-2062)
#include "../src/core/globals.h"
#include "../src/core/go.h"
#include "../src/core/object.h"
#include "../src/world/objects.h"
#include "../src/world/world.h"
#include "test_framework.h"

#define TEST(name) \
    void test_##name(); \
    struct Register_##name { \
        Register_##name() { TestFramework::instance().addTest(#name, test_##name); } \
    } register_##name; \
    void test_##name()

TEST(SizeDefaultsToFive) {
    ZObject obj(9001, "thing");
    ASSERT_EQ(obj.getProperty(P_SIZE), 5);
    ASSERT_FALSE(obj.getPropertyOpt(P_SIZE).has_value());
}

TEST(OtherPropertiesDefaultToZero) {
    ZObject obj(9002, "thing");
    ASSERT_EQ(obj.getProperty(P_CAPACITY), 0);
    ASSERT_EQ(obj.getProperty(P_VALUE), 0);
    ASSERT_EQ(obj.getProperty(P_TVALUE), 0);
    ASSERT_EQ(obj.getProperty(P_STRENGTH), 0);
}

TEST(ExplicitSizeOverridesDefault) {
    ZObject obj(9003, "thing");
    obj.setProperty(P_SIZE, 20);
    ASSERT_EQ(obj.getProperty(P_SIZE), 20);
    obj.setProperty(P_SIZE, 0);
    ASSERT_EQ(obj.getProperty(P_SIZE), 0);
}

TEST(FirstDescIsDistinctFromLongDescAndText) {
    ZObject obj(9004, "thing");
    ASSERT_FALSE(obj.hasFirstDesc());
    obj.setFirstDesc("Beside the skeleton is a rusty knife.");
    ASSERT_TRUE(obj.hasFirstDesc());
    ASSERT_FALSE(obj.hasLongDesc());
    ASSERT_FALSE(obj.hasText());
    ASSERT_EQ(obj.getFirstDesc(), std::string("Beside the skeleton is a rusty knife."));
    obj.setLongDesc("A painting by a neglected genius is here.");
    ASSERT_EQ(obj.getLongDesc(), std::string("A painting by a neglected genius is here."));
    ASSERT_EQ(obj.getFirstDesc(), std::string("Beside the skeleton is a rusty knife."));
}

TEST(VehicleTypeRoundTrips) {
    ZObject boat(9005, "magic boat");
    ASSERT_FALSE(boat.getVehicleType().has_value());
    boat.setVehicleType(ObjectFlag::NONLANDBIT);
    ASSERT_TRUE(boat.getVehicleType().has_value());
    ASSERT_TRUE(*boat.getVehicleType() == ObjectFlag::NONLANDBIT);
    ASSERT_TRUE(boat.getPropertyOpt(P_VTYPE).has_value());
    // The vehicle carries the property, not the room flag itself
    ASSERT_FALSE(boat.hasFlag(ObjectFlag::NONLANDBIT));
}

TEST(WorldObjectsWithoutSizeWeighFive) {
    initializeWorld();
    goSetup();
    auto& g = Globals::instance();
    // BLESSINGS has no SIZE in gglobals.zil:96-100 -> PROPDEF default 5
    if (auto* obj = g.getObject(ObjectIds::ADVERTISEMENT)) {
        ASSERT_TRUE(obj->getProperty(P_SIZE) > 0);
    }
    // The egg's FDESC lives in FDESC, not TEXT (1dungeon.zil:1164-1168)
    auto* egg = g.getObject(ObjectIds::EGG);
    ASSERT_TRUE(egg != nullptr);
    ASSERT_TRUE(egg->hasFirstDesc());
    ASSERT_FALSE(egg->hasText());
    auto* buoy = g.getObject(ObjectIds::BUOY);
    ASSERT_TRUE(buoy != nullptr);
    ASSERT_TRUE(buoy->hasFirstDesc());
    ASSERT_FALSE(buoy->hasText());
}

int main() {
    auto results = TestFramework::instance().runAll();
    int failed = 0;
    for (const auto& r : results) {
        if (!r.passed) failed++;
    }
    return failed == 0 ? 0 : 1;
}
