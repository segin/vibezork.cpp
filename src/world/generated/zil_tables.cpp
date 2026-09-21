// GENERATED FILE - DO NOT EDIT.
// Produced by tools/worldgen/melee_tables.py from zil/1actions.zil, zil/gverbs.zil.
// Strings are verbatim ZIL text after ZILCH conversion (| -> newline, line
// break -> space, line break after | dropped); vocabulary words as written.

#include "zil_tables.h"

namespace zork::zil {
namespace {
constexpr MeleePart kHERO_MELEE_0_0[] = {{"Your ", MeleePart::Text}, {{}, MeleePart::Weapon}, {" misses the ", MeleePart::Text}, {{}, MeleePart::Defender}, {" by an inch.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_0_1[] = {{"A good slash, but it misses the ", MeleePart::Text}, {{}, MeleePart::Defender}, {" by a mile.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_0_2[] = {{"You charge, but the ", MeleePart::Text}, {{}, MeleePart::Defender}, {" jumps nimbly aside.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_0_3[] = {{"Clang! Crash! The ", MeleePart::Text}, {{}, MeleePart::Defender}, {" parries.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_0_4[] = {{"A quick stroke, but the ", MeleePart::Text}, {{}, MeleePart::Defender}, {" is on guard.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_0_5[] = {{"A good stroke, but it's too slow; the ", MeleePart::Text}, {{}, MeleePart::Defender}, {" dodges.", MeleePart::Text}};
constexpr MeleeMsg kHERO_MELEE_0[] = {kHERO_MELEE_0_0, kHERO_MELEE_0_1, kHERO_MELEE_0_2, kHERO_MELEE_0_3, kHERO_MELEE_0_4, kHERO_MELEE_0_5};
constexpr MeleePart kHERO_MELEE_1_0[] = {{"Your ", MeleePart::Text}, {{}, MeleePart::Weapon}, {" crashes down, knocking the ", MeleePart::Text}, {{}, MeleePart::Defender}, {" into dreamland.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_1_1[] = {{"The ", MeleePart::Text}, {{}, MeleePart::Defender}, {" is battered into unconsciousness.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_1_2[] = {{"A furious exchange, and the ", MeleePart::Text}, {{}, MeleePart::Defender}, {" is knocked out!", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_1_3[] = {{"The haft of your ", MeleePart::Text}, {{}, MeleePart::Weapon}, {" knocks out the ", MeleePart::Text}, {{}, MeleePart::Defender}, {".", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_1_4[] = {{"The ", MeleePart::Text}, {{}, MeleePart::Defender}, {" is knocked out!", MeleePart::Text}};
constexpr MeleeMsg kHERO_MELEE_1[] = {kHERO_MELEE_1_0, kHERO_MELEE_1_1, kHERO_MELEE_1_2, kHERO_MELEE_1_3, kHERO_MELEE_1_4};
constexpr MeleePart kHERO_MELEE_2_0[] = {{"It's curtains for the ", MeleePart::Text}, {{}, MeleePart::Defender}, {" as your ", MeleePart::Text}, {{}, MeleePart::Weapon}, {" removes his head.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_2_1[] = {{"The fatal blow strikes the ", MeleePart::Text}, {{}, MeleePart::Defender}, {" square in the heart: He dies.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_2_2[] = {{"The ", MeleePart::Text}, {{}, MeleePart::Defender}, {" takes a fatal blow and slumps to the floor dead.", MeleePart::Text}};
constexpr MeleeMsg kHERO_MELEE_2[] = {kHERO_MELEE_2_0, kHERO_MELEE_2_1, kHERO_MELEE_2_2};
constexpr MeleePart kHERO_MELEE_3_0[] = {{"The ", MeleePart::Text}, {{}, MeleePart::Defender}, {" is struck on the arm; blood begins to trickle down.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_3_1[] = {{"Your ", MeleePart::Text}, {{}, MeleePart::Weapon}, {" pinks the ", MeleePart::Text}, {{}, MeleePart::Defender}, {" on the wrist, but it's not serious.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_3_2[] = {{"Your stroke lands, but it was only the flat of the blade.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_3_3[] = {{"The blow lands, making a shallow gash in the ", MeleePart::Text}, {{}, MeleePart::Defender}, {"'s arm!", MeleePart::Text}};
constexpr MeleeMsg kHERO_MELEE_3[] = {kHERO_MELEE_3_0, kHERO_MELEE_3_1, kHERO_MELEE_3_2, kHERO_MELEE_3_3};
constexpr MeleePart kHERO_MELEE_4_0[] = {{"The ", MeleePart::Text}, {{}, MeleePart::Defender}, {" receives a deep gash in his side.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_4_1[] = {{"A savage blow on the thigh! The ", MeleePart::Text}, {{}, MeleePart::Defender}, {" is stunned but can still fight!", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_4_2[] = {{"Slash! Your blow lands! That one hit an artery, it could be serious!", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_4_3[] = {{"Slash! Your stroke connects! This could be serious!", MeleePart::Text}};
constexpr MeleeMsg kHERO_MELEE_4[] = {kHERO_MELEE_4_0, kHERO_MELEE_4_1, kHERO_MELEE_4_2, kHERO_MELEE_4_3};
constexpr MeleePart kHERO_MELEE_5_0[] = {{"The ", MeleePart::Text}, {{}, MeleePart::Defender}, {" is staggered, and drops to his knees.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_5_1[] = {{"The ", MeleePart::Text}, {{}, MeleePart::Defender}, {" is momentarily disoriented and can't fight back.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_5_2[] = {{"The force of your blow knocks the ", MeleePart::Text}, {{}, MeleePart::Defender}, {" back, stunned.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_5_3[] = {{"The ", MeleePart::Text}, {{}, MeleePart::Defender}, {" is confused and can't fight back.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_5_4[] = {{"The quickness of your thrust knocks the ", MeleePart::Text}, {{}, MeleePart::Defender}, {" back, stunned.", MeleePart::Text}};
constexpr MeleeMsg kHERO_MELEE_5[] = {kHERO_MELEE_5_0, kHERO_MELEE_5_1, kHERO_MELEE_5_2, kHERO_MELEE_5_3, kHERO_MELEE_5_4};
constexpr MeleePart kHERO_MELEE_6_0[] = {{"The ", MeleePart::Text}, {{}, MeleePart::Defender}, {"'s weapon is knocked to the floor, leaving him unarmed.", MeleePart::Text}};
constexpr MeleePart kHERO_MELEE_6_1[] = {{"The ", MeleePart::Text}, {{}, MeleePart::Defender}, {" is disarmed by a subtle feint past his guard.", MeleePart::Text}};
constexpr MeleeMsg kHERO_MELEE_6[] = {kHERO_MELEE_6_0, kHERO_MELEE_6_1};
constexpr MeleeResult kHERO_MELEE_results[] = {kHERO_MELEE_0, kHERO_MELEE_1, kHERO_MELEE_2, kHERO_MELEE_3, kHERO_MELEE_4, kHERO_MELEE_5, kHERO_MELEE_6};
}  // namespace
const std::span<const MeleeResult> HERO_MELEE = kHERO_MELEE_results;
namespace {
constexpr MeleePart kCYCLOPS_MELEE_0_0[] = {{"The Cyclops misses, but the backwash almost knocks you over.", MeleePart::Text}};
constexpr MeleePart kCYCLOPS_MELEE_0_1[] = {{"The Cyclops rushes you, but runs into the wall.", MeleePart::Text}};
constexpr MeleeMsg kCYCLOPS_MELEE_0[] = {kCYCLOPS_MELEE_0_0, kCYCLOPS_MELEE_0_1};
constexpr MeleePart kCYCLOPS_MELEE_1_0[] = {{"The Cyclops sends you crashing to the floor, unconscious.", MeleePart::Text}};
constexpr MeleeMsg kCYCLOPS_MELEE_1[] = {kCYCLOPS_MELEE_1_0};
constexpr MeleePart kCYCLOPS_MELEE_2_0[] = {{"The Cyclops breaks your neck with a massive smash.", MeleePart::Text}};
constexpr MeleeMsg kCYCLOPS_MELEE_2[] = {kCYCLOPS_MELEE_2_0};
constexpr MeleePart kCYCLOPS_MELEE_3_0[] = {{"A quick punch, but it was only a glancing blow.", MeleePart::Text}};
constexpr MeleePart kCYCLOPS_MELEE_3_1[] = {{"A glancing blow from the Cyclops' fist.", MeleePart::Text}};
constexpr MeleeMsg kCYCLOPS_MELEE_3[] = {kCYCLOPS_MELEE_3_0, kCYCLOPS_MELEE_3_1};
constexpr MeleePart kCYCLOPS_MELEE_4_0[] = {{"The monster smashes his huge fist into your chest, breaking several ribs.", MeleePart::Text}};
constexpr MeleePart kCYCLOPS_MELEE_4_1[] = {{"The Cyclops almost knocks the wind out of you with a quick punch.", MeleePart::Text}};
constexpr MeleeMsg kCYCLOPS_MELEE_4[] = {kCYCLOPS_MELEE_4_0, kCYCLOPS_MELEE_4_1};
constexpr MeleePart kCYCLOPS_MELEE_5_0[] = {{"The Cyclops lands a punch that knocks the wind out of you.", MeleePart::Text}};
constexpr MeleePart kCYCLOPS_MELEE_5_1[] = {{"Heedless of your weapons, the Cyclops tosses you against the rock wall of the room.", MeleePart::Text}};
constexpr MeleeMsg kCYCLOPS_MELEE_5[] = {kCYCLOPS_MELEE_5_0, kCYCLOPS_MELEE_5_1};
constexpr MeleePart kCYCLOPS_MELEE_6_0[] = {{"The Cyclops grabs your ", MeleePart::Text}, {{}, MeleePart::Weapon}, {", tastes it, and throws it to the ground in disgust.", MeleePart::Text}};
constexpr MeleePart kCYCLOPS_MELEE_6_1[] = {{"The monster grabs you on the wrist, squeezes, and you drop your ", MeleePart::Text}, {{}, MeleePart::Weapon}, {" in pain.", MeleePart::Text}};
constexpr MeleeMsg kCYCLOPS_MELEE_6[] = {kCYCLOPS_MELEE_6_0, kCYCLOPS_MELEE_6_1};
constexpr MeleePart kCYCLOPS_MELEE_7_0[] = {{"The Cyclops seems unable to decide whether to broil or stew his dinner.", MeleePart::Text}};
constexpr MeleeMsg kCYCLOPS_MELEE_7[] = {kCYCLOPS_MELEE_7_0};
constexpr MeleePart kCYCLOPS_MELEE_8_0[] = {{"The Cyclops, no sportsman, dispatches his unconscious victim.", MeleePart::Text}};
constexpr MeleeMsg kCYCLOPS_MELEE_8[] = {kCYCLOPS_MELEE_8_0};
constexpr MeleeResult kCYCLOPS_MELEE_results[] = {kCYCLOPS_MELEE_0, kCYCLOPS_MELEE_1, kCYCLOPS_MELEE_2, kCYCLOPS_MELEE_3, kCYCLOPS_MELEE_4, kCYCLOPS_MELEE_5, kCYCLOPS_MELEE_6, kCYCLOPS_MELEE_7, kCYCLOPS_MELEE_8};
}  // namespace
const std::span<const MeleeResult> CYCLOPS_MELEE = kCYCLOPS_MELEE_results;
namespace {
constexpr MeleePart kTROLL_MELEE_0_0[] = {{"The troll swings his axe, but it misses.", MeleePart::Text}};
constexpr MeleePart kTROLL_MELEE_0_1[] = {{"The troll's axe barely misses your ear.", MeleePart::Text}};
constexpr MeleePart kTROLL_MELEE_0_2[] = {{"The axe sweeps past as you jump aside.", MeleePart::Text}};
constexpr MeleePart kTROLL_MELEE_0_3[] = {{"The axe crashes against the rock, throwing sparks!", MeleePart::Text}};
constexpr MeleeMsg kTROLL_MELEE_0[] = {kTROLL_MELEE_0_0, kTROLL_MELEE_0_1, kTROLL_MELEE_0_2, kTROLL_MELEE_0_3};
constexpr MeleePart kTROLL_MELEE_1_0[] = {{"The flat of the troll's axe hits you delicately on the head, knocking you out.", MeleePart::Text}};
constexpr MeleeMsg kTROLL_MELEE_1[] = {kTROLL_MELEE_1_0};
constexpr MeleePart kTROLL_MELEE_2_0[] = {{"The troll neatly removes your head.", MeleePart::Text}};
constexpr MeleePart kTROLL_MELEE_2_1[] = {{"The troll's axe stroke cleaves you from the nave to the chops.", MeleePart::Text}};
constexpr MeleePart kTROLL_MELEE_2_2[] = {{"The troll's axe removes your head.", MeleePart::Text}};
constexpr MeleeMsg kTROLL_MELEE_2[] = {kTROLL_MELEE_2_0, kTROLL_MELEE_2_1, kTROLL_MELEE_2_2};
constexpr MeleePart kTROLL_MELEE_3_0[] = {{"The axe gets you right in the side. Ouch!", MeleePart::Text}};
constexpr MeleePart kTROLL_MELEE_3_1[] = {{"The flat of the troll's axe skins across your forearm.", MeleePart::Text}};
constexpr MeleePart kTROLL_MELEE_3_2[] = {{"The troll's swing almost knocks you over as you barely parry in time.", MeleePart::Text}};
constexpr MeleePart kTROLL_MELEE_3_3[] = {{"The troll swings his axe, and it nicks your arm as you dodge.", MeleePart::Text}};
constexpr MeleeMsg kTROLL_MELEE_3[] = {kTROLL_MELEE_3_0, kTROLL_MELEE_3_1, kTROLL_MELEE_3_2, kTROLL_MELEE_3_3};
constexpr MeleePart kTROLL_MELEE_4_0[] = {{"The troll charges, and his axe slashes you on your ", MeleePart::Text}, {{}, MeleePart::Weapon}, {" arm.", MeleePart::Text}};
constexpr MeleePart kTROLL_MELEE_4_1[] = {{"An axe stroke makes a deep wound in your leg.", MeleePart::Text}};
constexpr MeleePart kTROLL_MELEE_4_2[] = {{"The troll's axe swings down, gashing your shoulder.", MeleePart::Text}};
constexpr MeleeMsg kTROLL_MELEE_4[] = {kTROLL_MELEE_4_0, kTROLL_MELEE_4_1, kTROLL_MELEE_4_2};
constexpr MeleePart kTROLL_MELEE_5_0[] = {{"The troll hits you with a glancing blow, and you are momentarily stunned.", MeleePart::Text}};
constexpr MeleePart kTROLL_MELEE_5_1[] = {{"The troll swings; the blade turns on your armor but crashes broadside into your head.", MeleePart::Text}};
constexpr MeleePart kTROLL_MELEE_5_2[] = {{"You stagger back under a hail of axe strokes.", MeleePart::Text}};
constexpr MeleePart kTROLL_MELEE_5_3[] = {{"The troll's mighty blow drops you to your knees.", MeleePart::Text}};
constexpr MeleeMsg kTROLL_MELEE_5[] = {kTROLL_MELEE_5_0, kTROLL_MELEE_5_1, kTROLL_MELEE_5_2, kTROLL_MELEE_5_3};
constexpr MeleePart kTROLL_MELEE_6_0[] = {{"The axe hits your ", MeleePart::Text}, {{}, MeleePart::Weapon}, {" and knocks it spinning.", MeleePart::Text}};
constexpr MeleePart kTROLL_MELEE_6_1[] = {{"The troll swings, you parry, but the force of his blow knocks your ", MeleePart::Text}, {{}, MeleePart::Weapon}, {" away.", MeleePart::Text}};
constexpr MeleePart kTROLL_MELEE_6_2[] = {{"The axe knocks your ", MeleePart::Text}, {{}, MeleePart::Weapon}, {" out of your hand. It falls to the floor.", MeleePart::Text}};
constexpr MeleeMsg kTROLL_MELEE_6[] = {kTROLL_MELEE_6_0, kTROLL_MELEE_6_1, kTROLL_MELEE_6_2};
constexpr MeleePart kTROLL_MELEE_7_0[] = {{"The troll hesitates, fingering his axe.", MeleePart::Text}};
constexpr MeleePart kTROLL_MELEE_7_1[] = {{"The troll scratches his head ruminatively:  Might you be magically protected, he wonders?", MeleePart::Text}};
constexpr MeleeMsg kTROLL_MELEE_7[] = {kTROLL_MELEE_7_0, kTROLL_MELEE_7_1};
constexpr MeleePart kTROLL_MELEE_8_0[] = {{"Conquering his fears, the troll puts you to death.", MeleePart::Text}};
constexpr MeleeMsg kTROLL_MELEE_8[] = {kTROLL_MELEE_8_0};
constexpr MeleeResult kTROLL_MELEE_results[] = {kTROLL_MELEE_0, kTROLL_MELEE_1, kTROLL_MELEE_2, kTROLL_MELEE_3, kTROLL_MELEE_4, kTROLL_MELEE_5, kTROLL_MELEE_6, kTROLL_MELEE_7, kTROLL_MELEE_8};
}  // namespace
const std::span<const MeleeResult> TROLL_MELEE = kTROLL_MELEE_results;
namespace {
constexpr MeleePart kTHIEF_MELEE_0_0[] = {{"The thief stabs nonchalantly with his stiletto and misses.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_0_1[] = {{"You dodge as the thief comes in low.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_0_2[] = {{"You parry a lightning thrust, and the thief salutes you with a grim nod.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_0_3[] = {{"The thief tries to sneak past your guard, but you twist away.", MeleePart::Text}};
constexpr MeleeMsg kTHIEF_MELEE_0[] = {kTHIEF_MELEE_0_0, kTHIEF_MELEE_0_1, kTHIEF_MELEE_0_2, kTHIEF_MELEE_0_3};
constexpr MeleePart kTHIEF_MELEE_1_0[] = {{"Shifting in the midst of a thrust, the thief knocks you unconscious with the haft of his stiletto.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_1_1[] = {{"The thief knocks you out.", MeleePart::Text}};
constexpr MeleeMsg kTHIEF_MELEE_1[] = {kTHIEF_MELEE_1_0, kTHIEF_MELEE_1_1};
constexpr MeleePart kTHIEF_MELEE_2_0[] = {{"Finishing you off, the thief inserts his blade into your heart.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_2_1[] = {{"The thief comes in from the side, feints, and inserts the blade into your ribs.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_2_2[] = {{"The thief bows formally, raises his stiletto, and with a wry grin, ends the battle and your life.", MeleePart::Text}};
constexpr MeleeMsg kTHIEF_MELEE_2[] = {kTHIEF_MELEE_2_0, kTHIEF_MELEE_2_1, kTHIEF_MELEE_2_2};
constexpr MeleePart kTHIEF_MELEE_3_0[] = {{"A quick thrust pinks your left arm, and blood starts to trickle down.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_3_1[] = {{"The thief draws blood, raking his stiletto across your arm.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_3_2[] = {{"The stiletto flashes faster than you can follow, and blood wells from your leg.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_3_3[] = {{"The thief slowly approaches, strikes like a snake, and leaves you wounded.", MeleePart::Text}};
constexpr MeleeMsg kTHIEF_MELEE_3[] = {kTHIEF_MELEE_3_0, kTHIEF_MELEE_3_1, kTHIEF_MELEE_3_2, kTHIEF_MELEE_3_3};
constexpr MeleePart kTHIEF_MELEE_4_0[] = {{"The thief strikes like a snake! The resulting wound is serious.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_4_1[] = {{"The thief stabs a deep cut in your upper arm.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_4_2[] = {{"The stiletto touches your forehead, and the blood obscures your vision.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_4_3[] = {{"The thief strikes at your wrist, and suddenly your grip is slippery with blood.", MeleePart::Text}};
constexpr MeleeMsg kTHIEF_MELEE_4[] = {kTHIEF_MELEE_4_0, kTHIEF_MELEE_4_1, kTHIEF_MELEE_4_2, kTHIEF_MELEE_4_3};
constexpr MeleePart kTHIEF_MELEE_5_0[] = {{"The butt of his stiletto cracks you on the skull, and you stagger back.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_5_1[] = {{"The thief rams the haft of his blade into your stomach, leaving you out of breath.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_5_2[] = {{"The thief attacks, and you fall back desperately.", MeleePart::Text}};
constexpr MeleeMsg kTHIEF_MELEE_5[] = {kTHIEF_MELEE_5_0, kTHIEF_MELEE_5_1, kTHIEF_MELEE_5_2};
constexpr MeleePart kTHIEF_MELEE_6_0[] = {{"A long, theatrical slash. You catch it on your ", MeleePart::Text}, {{}, MeleePart::Weapon}, {", but the thief twists his knife, and the ", MeleePart::Text}, {{}, MeleePart::Weapon}, {" goes flying.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_6_1[] = {{"The thief neatly flips your ", MeleePart::Text}, {{}, MeleePart::Weapon}, {" out of your hands, and it drops to the floor.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_6_2[] = {{"You parry a low thrust, and your ", MeleePart::Text}, {{}, MeleePart::Weapon}, {" slips out of your hand.", MeleePart::Text}};
constexpr MeleeMsg kTHIEF_MELEE_6[] = {kTHIEF_MELEE_6_0, kTHIEF_MELEE_6_1, kTHIEF_MELEE_6_2};
constexpr MeleePart kTHIEF_MELEE_7_0[] = {{"The thief, a man of superior breeding, pauses for a moment to consider the propriety of finishing you off.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_7_1[] = {{"The thief amuses himself by searching your pockets.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_7_2[] = {{"The thief entertains himself by rifling your pack.", MeleePart::Text}};
constexpr MeleeMsg kTHIEF_MELEE_7[] = {kTHIEF_MELEE_7_0, kTHIEF_MELEE_7_1, kTHIEF_MELEE_7_2};
constexpr MeleePart kTHIEF_MELEE_8_0[] = {{"The thief, forgetting his essentially genteel upbringing, cuts your throat.", MeleePart::Text}};
constexpr MeleePart kTHIEF_MELEE_8_1[] = {{"The thief, a pragmatist, dispatches you as a threat to his livelihood.", MeleePart::Text}};
constexpr MeleeMsg kTHIEF_MELEE_8[] = {kTHIEF_MELEE_8_0, kTHIEF_MELEE_8_1};
constexpr MeleeResult kTHIEF_MELEE_results[] = {kTHIEF_MELEE_0, kTHIEF_MELEE_1, kTHIEF_MELEE_2, kTHIEF_MELEE_3, kTHIEF_MELEE_4, kTHIEF_MELEE_5, kTHIEF_MELEE_6, kTHIEF_MELEE_7, kTHIEF_MELEE_8};
}  // namespace
const std::span<const MeleeResult> THIEF_MELEE = kTHIEF_MELEE_results;
namespace {
}  // namespace
}  // namespace zork::zil
