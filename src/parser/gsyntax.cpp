#include "gsyntax.h"
#include "parser/verb_registry.h"
#include "parser/syntax.h"
#include "core/flags.h"
#include "verbs/verbs.h"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

namespace GSyntax {

namespace {

bool g_initialized = false;
std::vector<std::string> g_buzzwords;
std::unordered_set<std::string> g_buzzwordSet;

std::unordered_map<std::string, std::string> g_prepCanonical;
std::unordered_map<std::string, std::vector<std::string>> g_prepSynonyms;

std::unordered_map<std::string, std::string> g_dirCanonical;
std::unordered_map<std::string, std::vector<std::string>> g_dirSynonyms;

std::unordered_map<std::string, VerbId> g_verbLookup;
std::unordered_map<std::string, std::string> g_verbCanonical;
std::unordered_map<VerbId, std::vector<std::string>> g_verbSynonyms;

std::vector<ZilSyntax> g_syntaxes;
std::unordered_map<VerbId, std::vector<const ZilSyntax*>> g_verbSyntaxMap;
std::unordered_map<std::string, std::vector<const ZilSyntax*>> g_wordSyntaxMap;

std::string toLower(std::string_view sv) {
    std::string s(sv);
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

uint8_t parseScope(const std::vector<std::string>& scopes) {
    uint8_t res = 0;
    for (const auto& sc : scopes) {
        if (sc == "HELD") res |= SH_HELD;
        else if (sc == "CARRIED") res |= SH_CARRIED;
        else if (sc == "ON-GROUND") res |= SH_ON_GROUND;
        else if (sc == "IN-ROOM") res |= SH_IN_ROOM;
    }
    return res;
}

} // namespace
void initialize() {
    if (g_initialized) return;
    g_initialized = true;

    // Buzzwords (BUZZ)
    g_buzzwords.push_back("again");
    g_buzzwordSet.insert("again");
    g_buzzwords.push_back("g");
    g_buzzwordSet.insert("g");
    g_buzzwords.push_back("oops");
    g_buzzwordSet.insert("oops");
    g_buzzwords.push_back("a");
    g_buzzwordSet.insert("a");
    g_buzzwords.push_back("an");
    g_buzzwordSet.insert("an");
    g_buzzwords.push_back("the");
    g_buzzwordSet.insert("the");
    g_buzzwords.push_back("is");
    g_buzzwordSet.insert("is");
    g_buzzwords.push_back("and");
    g_buzzwordSet.insert("and");
    g_buzzwords.push_back("of");
    g_buzzwordSet.insert("of");
    g_buzzwords.push_back("then");
    g_buzzwordSet.insert("then");
    g_buzzwords.push_back("all");
    g_buzzwordSet.insert("all");
    g_buzzwords.push_back("one");
    g_buzzwordSet.insert("one");
    g_buzzwords.push_back("but");
    g_buzzwordSet.insert("but");
    g_buzzwords.push_back("except");
    g_buzzwordSet.insert("except");
    g_buzzwords.push_back(".");
    g_buzzwordSet.insert(".");
    g_buzzwords.push_back(",");
    g_buzzwordSet.insert(",");
    g_buzzwords.push_back("\"");
    g_buzzwordSet.insert("\"");
    g_buzzwords.push_back("yes");
    g_buzzwordSet.insert("yes");
    g_buzzwords.push_back("no");
    g_buzzwordSet.insert("no");
    g_buzzwords.push_back("y");
    g_buzzwordSet.insert("y");
    g_buzzwords.push_back("here");
    g_buzzwordSet.insert("here");

    // Preposition Synonyms
    g_prepCanonical["with"] = "with";
    g_prepSynonyms["with"] = {"with", "using", "through", "thru"};
    g_prepCanonical["using"] = "with";
    g_prepCanonical["through"] = "with";
    g_prepCanonical["thru"] = "with";
    g_prepCanonical["in"] = "in";
    g_prepSynonyms["in"] = {"in", "inside", "into"};
    g_prepCanonical["inside"] = "in";
    g_prepCanonical["into"] = "in";
    g_prepCanonical["on"] = "on";
    g_prepSynonyms["on"] = {"on", "onto"};
    g_prepCanonical["onto"] = "on";
    g_prepCanonical["under"] = "under";
    g_prepSynonyms["under"] = {"under", "underneath", "beneath", "below"};
    g_prepCanonical["underneath"] = "under";
    g_prepCanonical["beneath"] = "under";
    g_prepCanonical["below"] = "under";

    // Direction Synonyms
    g_dirCanonical["north"] = "north";
    g_dirSynonyms["north"] = {"north", "n"};
    g_dirCanonical["n"] = "north";
    g_dirCanonical["south"] = "south";
    g_dirSynonyms["south"] = {"south", "s"};
    g_dirCanonical["s"] = "south";
    g_dirCanonical["east"] = "east";
    g_dirSynonyms["east"] = {"east", "e"};
    g_dirCanonical["e"] = "east";
    g_dirCanonical["west"] = "west";
    g_dirSynonyms["west"] = {"west", "w"};
    g_dirCanonical["w"] = "west";
    g_dirCanonical["down"] = "down";
    g_dirSynonyms["down"] = {"down", "d"};
    g_dirCanonical["d"] = "down";
    g_dirCanonical["up"] = "up";
    g_dirSynonyms["up"] = {"up", "u"};
    g_dirCanonical["u"] = "up";
    g_dirCanonical["nw"] = "nw";
    g_dirSynonyms["nw"] = {"nw", "northwest"};
    g_dirCanonical["northwest"] = "nw";
    g_dirCanonical["ne"] = "ne";
    g_dirSynonyms["ne"] = {"ne", "northe"};
    g_dirCanonical["northe"] = "ne";
    g_dirCanonical["sw"] = "sw";
    g_dirSynonyms["sw"] = {"sw", "southwest"};
    g_dirCanonical["southwest"] = "sw";
    g_dirCanonical["se"] = "se";
    g_dirSynonyms["se"] = {"se", "southe"};
    g_dirCanonical["southe"] = "se";

    // Verb Synonyms
    g_verbLookup["super"] = V_SUPER;
    g_verbCanonical["super"] = "super";
    g_verbSynonyms[V_SUPER].push_back("super");
    g_verbLookup["superbrief"] = V_SUPER;
    g_verbCanonical["superbrief"] = "super";
    g_verbSynonyms[V_SUPER].push_back("superbrief");
    g_verbLookup["inventory"] = V_INVENTORY;
    g_verbCanonical["inventory"] = "inventory";
    g_verbSynonyms[V_INVENTORY].push_back("inventory");
    g_verbLookup["i"] = V_INVENTORY;
    g_verbCanonical["i"] = "inventory";
    g_verbSynonyms[V_INVENTORY].push_back("i");
    g_verbLookup["quit"] = V_QUIT;
    g_verbCanonical["quit"] = "quit";
    g_verbSynonyms[V_QUIT].push_back("quit");
    g_verbLookup["q"] = V_QUIT;
    g_verbCanonical["q"] = "quit";
    g_verbSynonyms[V_QUIT].push_back("q");
    g_verbLookup["answer"] = V_ANSWER;
    g_verbCanonical["answer"] = "answer";
    g_verbSynonyms[V_ANSWER].push_back("answer");
    g_verbLookup["reply"] = V_ANSWER;
    g_verbCanonical["reply"] = "answer";
    g_verbSynonyms[V_ANSWER].push_back("reply");
    g_verbLookup["attack"] = V_ATTACK;
    g_verbCanonical["attack"] = "attack";
    g_verbSynonyms[V_ATTACK].push_back("attack");
    g_verbLookup["fight"] = V_ATTACK;
    g_verbCanonical["fight"] = "attack";
    g_verbSynonyms[V_ATTACK].push_back("fight");
    g_verbLookup["hurt"] = V_ATTACK;
    g_verbCanonical["hurt"] = "attack";
    g_verbSynonyms[V_ATTACK].push_back("hurt");
    g_verbLookup["injure"] = V_ATTACK;
    g_verbCanonical["injure"] = "attack";
    g_verbSynonyms[V_ATTACK].push_back("injure");
    g_verbLookup["hit"] = V_ATTACK;
    g_verbCanonical["hit"] = "attack";
    g_verbSynonyms[V_ATTACK].push_back("hit");
    g_verbLookup["brush"] = V_BRUSH;
    g_verbCanonical["brush"] = "brush";
    g_verbSynonyms[V_BRUSH].push_back("brush");
    g_verbLookup["clean"] = V_BRUSH;
    g_verbCanonical["clean"] = "brush";
    g_verbSynonyms[V_BRUSH].push_back("clean");
    g_verbLookup["burn"] = V_BURN;
    g_verbCanonical["burn"] = "burn";
    g_verbSynonyms[V_BURN].push_back("burn");
    g_verbLookup["incinerate"] = V_BURN;
    g_verbCanonical["incinerate"] = "burn";
    g_verbSynonyms[V_BURN].push_back("incinerate");
    g_verbLookup["ignite"] = V_BURN;
    g_verbCanonical["ignite"] = "burn";
    g_verbSynonyms[V_BURN].push_back("ignite");
    g_verbLookup["chomp"] = V_CHOMP;
    g_verbCanonical["chomp"] = "chomp";
    g_verbSynonyms[V_CHOMP].push_back("chomp");
    g_verbLookup["lose"] = V_CHOMP;
    g_verbCanonical["lose"] = "chomp";
    g_verbSynonyms[V_CHOMP].push_back("lose");
    g_verbLookup["barf"] = V_CHOMP;
    g_verbCanonical["barf"] = "chomp";
    g_verbSynonyms[V_CHOMP].push_back("barf");
    g_verbLookup["climb"] = V_CLIMB;
    g_verbCanonical["climb"] = "climb";
    g_verbSynonyms[V_CLIMB].push_back("climb");
    g_verbLookup["sit"] = V_CLIMB;
    g_verbCanonical["sit"] = "climb";
    g_verbSynonyms[V_CLIMB].push_back("sit");
    g_verbLookup["cross"] = V_CROSS;
    g_verbCanonical["cross"] = "cross";
    g_verbSynonyms[V_CROSS].push_back("cross");
    g_verbLookup["ford"] = V_CROSS;
    g_verbCanonical["ford"] = "cross";
    g_verbSynonyms[V_CROSS].push_back("ford");
    g_verbLookup["cut"] = V_CUT;
    g_verbCanonical["cut"] = "cut";
    g_verbSynonyms[V_CUT].push_back("cut");
    g_verbLookup["slice"] = V_CUT;
    g_verbCanonical["slice"] = "cut";
    g_verbSynonyms[V_CUT].push_back("slice");
    g_verbLookup["pierce"] = V_CUT;
    g_verbCanonical["pierce"] = "cut";
    g_verbSynonyms[V_CUT].push_back("pierce");
    g_verbLookup["curse"] = V_CURSE;
    g_verbCanonical["curse"] = "curse";
    g_verbSynonyms[V_CURSE].push_back("curse");
    g_verbLookup["shit"] = V_CURSE;
    g_verbCanonical["shit"] = "curse";
    g_verbSynonyms[V_CURSE].push_back("shit");
    g_verbLookup["fuck"] = V_CURSE;
    g_verbCanonical["fuck"] = "curse";
    g_verbSynonyms[V_CURSE].push_back("fuck");
    g_verbLookup["damn"] = V_CURSE;
    g_verbCanonical["damn"] = "curse";
    g_verbSynonyms[V_CURSE].push_back("damn");
    g_verbLookup["destroy"] = V_DESTROY;
    g_verbCanonical["destroy"] = "destroy";
    g_verbSynonyms[V_DESTROY].push_back("destroy");
    g_verbLookup["damage"] = V_DESTROY;
    g_verbCanonical["damage"] = "destroy";
    g_verbSynonyms[V_DESTROY].push_back("damage");
    g_verbLookup["break"] = V_DESTROY;
    g_verbCanonical["break"] = "destroy";
    g_verbSynonyms[V_DESTROY].push_back("break");
    g_verbLookup["block"] = V_DESTROY;
    g_verbCanonical["block"] = "destroy";
    g_verbSynonyms[V_DESTROY].push_back("block");
    g_verbLookup["smash"] = V_DESTROY;
    g_verbCanonical["smash"] = "destroy";
    g_verbSynonyms[V_DESTROY].push_back("smash");
    g_verbLookup["drink"] = V_DRINK;
    g_verbCanonical["drink"] = "drink";
    g_verbSynonyms[V_DRINK].push_back("drink");
    g_verbLookup["imbibe"] = V_DRINK;
    g_verbCanonical["imbibe"] = "drink";
    g_verbSynonyms[V_DRINK].push_back("imbibe");
    g_verbLookup["swallow"] = V_DRINK;
    g_verbCanonical["swallow"] = "drink";
    g_verbSynonyms[V_DRINK].push_back("swallow");
    g_verbLookup["eat"] = V_EAT;
    g_verbCanonical["eat"] = "eat";
    g_verbSynonyms[V_EAT].push_back("eat");
    g_verbLookup["consume"] = V_EAT;
    g_verbCanonical["consume"] = "eat";
    g_verbSynonyms[V_EAT].push_back("consume");
    g_verbLookup["taste"] = V_EAT;
    g_verbCanonical["taste"] = "eat";
    g_verbSynonyms[V_EAT].push_back("taste");
    g_verbLookup["bite"] = V_EAT;
    g_verbCanonical["bite"] = "eat";
    g_verbSynonyms[V_EAT].push_back("bite");
    g_verbLookup["examine"] = V_EXAMINE;
    g_verbCanonical["examine"] = "examine";
    g_verbSynonyms[V_EXAMINE].push_back("examine");
    g_verbLookup["describe"] = V_EXAMINE;
    g_verbCanonical["describe"] = "examine";
    g_verbSynonyms[V_EXAMINE].push_back("describe");
    g_verbLookup["what"] = V_EXAMINE;
    g_verbCanonical["what"] = "examine";
    g_verbSynonyms[V_EXAMINE].push_back("what");
    g_verbLookup["whats"] = V_EXAMINE;
    g_verbCanonical["whats"] = "examine";
    g_verbSynonyms[V_EXAMINE].push_back("whats");
    g_verbLookup["exorcise"] = V_EXORCISE;
    g_verbCanonical["exorcise"] = "exorcise";
    g_verbSynonyms[V_EXORCISE].push_back("exorcise");
    g_verbLookup["banish"] = V_EXORCISE;
    g_verbCanonical["banish"] = "exorcise";
    g_verbSynonyms[V_EXORCISE].push_back("banish");
    g_verbLookup["cast"] = V_EXORCISE;
    g_verbCanonical["cast"] = "exorcise";
    g_verbSynonyms[V_EXORCISE].push_back("cast");
    g_verbLookup["drive"] = V_EXORCISE;
    g_verbCanonical["drive"] = "exorcise";
    g_verbSynonyms[V_EXORCISE].push_back("drive");
    g_verbLookup["begone"] = V_EXORCISE;
    g_verbCanonical["begone"] = "exorcise";
    g_verbSynonyms[V_EXORCISE].push_back("begone");
    g_verbLookup["extinguish"] = V_EXTINGUISH;
    g_verbCanonical["extinguish"] = "extinguish";
    g_verbSynonyms[V_EXTINGUISH].push_back("extinguish");
    g_verbLookup["douse"] = V_EXTINGUISH;
    g_verbCanonical["douse"] = "extinguish";
    g_verbSynonyms[V_EXTINGUISH].push_back("douse");
    g_verbLookup["find"] = V_FIND;
    g_verbCanonical["find"] = "find";
    g_verbSynonyms[V_FIND].push_back("find");
    g_verbLookup["where"] = V_FIND;
    g_verbCanonical["where"] = "find";
    g_verbSynonyms[V_FIND].push_back("where");
    g_verbLookup["seek"] = V_FIND;
    g_verbCanonical["seek"] = "find";
    g_verbSynonyms[V_FIND].push_back("seek");
    g_verbLookup["see"] = V_FIND;
    g_verbCanonical["see"] = "find";
    g_verbSynonyms[V_FIND].push_back("see");
    g_verbLookup["follow"] = V_FOLLOW;
    g_verbCanonical["follow"] = "follow";
    g_verbSynonyms[V_FOLLOW].push_back("follow");
    g_verbLookup["pursue"] = V_FOLLOW;
    g_verbCanonical["pursue"] = "follow";
    g_verbSynonyms[V_FOLLOW].push_back("pursue");
    g_verbLookup["chase"] = V_FOLLOW;
    g_verbCanonical["chase"] = "follow";
    g_verbSynonyms[V_FOLLOW].push_back("chase");
    g_verbLookup["come"] = V_FOLLOW;
    g_verbCanonical["come"] = "follow";
    g_verbSynonyms[V_FOLLOW].push_back("come");
    g_verbLookup["give"] = V_GIVE;
    g_verbCanonical["give"] = "give";
    g_verbSynonyms[V_GIVE].push_back("give");
    g_verbLookup["donate"] = V_GIVE;
    g_verbCanonical["donate"] = "give";
    g_verbSynonyms[V_GIVE].push_back("donate");
    g_verbLookup["offer"] = V_GIVE;
    g_verbCanonical["offer"] = "give";
    g_verbSynonyms[V_GIVE].push_back("offer");
    g_verbLookup["feed"] = V_GIVE;
    g_verbCanonical["feed"] = "give";
    g_verbSynonyms[V_GIVE].push_back("feed");
    g_verbLookup["hand"] = V_GIVE;
    g_verbCanonical["hand"] = "give";
    g_verbSynonyms[V_GIVE].push_back("hand");
    g_verbLookup["hello"] = V_HELLO;
    g_verbCanonical["hello"] = "hello";
    g_verbSynonyms[V_HELLO].push_back("hello");
    g_verbLookup["hi"] = V_HELLO;
    g_verbCanonical["hi"] = "hello";
    g_verbSynonyms[V_HELLO].push_back("hi");
    g_verbLookup["incant"] = V_INCANT;
    g_verbCanonical["incant"] = "incant";
    g_verbSynonyms[V_INCANT].push_back("incant");
    g_verbLookup["chant"] = V_INCANT;
    g_verbCanonical["chant"] = "incant";
    g_verbSynonyms[V_INCANT].push_back("chant");
    g_verbLookup["jump"] = V_JUMP;
    g_verbCanonical["jump"] = "jump";
    g_verbSynonyms[V_JUMP].push_back("jump");
    g_verbLookup["leap"] = V_JUMP;
    g_verbCanonical["leap"] = "jump";
    g_verbSynonyms[V_JUMP].push_back("leap");
    g_verbLookup["dive"] = V_JUMP;
    g_verbCanonical["dive"] = "jump";
    g_verbSynonyms[V_JUMP].push_back("dive");
    g_verbLookup["kick"] = V_KICK;
    g_verbCanonical["kick"] = "kick";
    g_verbSynonyms[V_KICK].push_back("kick");
    g_verbLookup["taunt"] = V_KICK;
    g_verbCanonical["taunt"] = "kick";
    g_verbSynonyms[V_KICK].push_back("taunt");
    g_verbLookup["kill"] = V_KILL;
    g_verbCanonical["kill"] = "kill";
    g_verbSynonyms[V_KILL].push_back("kill");
    g_verbLookup["murder"] = V_KILL;
    g_verbCanonical["murder"] = "kill";
    g_verbSynonyms[V_KILL].push_back("murder");
    g_verbLookup["slay"] = V_KILL;
    g_verbCanonical["slay"] = "kill";
    g_verbSynonyms[V_KILL].push_back("slay");
    g_verbLookup["dispatch"] = V_KILL;
    g_verbCanonical["dispatch"] = "kill";
    g_verbSynonyms[V_KILL].push_back("dispatch");
    g_verbLookup["knock"] = V_KNOCK;
    g_verbCanonical["knock"] = "knock";
    g_verbSynonyms[V_KNOCK].push_back("knock");
    g_verbLookup["rap"] = V_KNOCK;
    g_verbCanonical["rap"] = "knock";
    g_verbSynonyms[V_KNOCK].push_back("rap");
    g_verbLookup["look"] = V_LOOK;
    g_verbCanonical["look"] = "look";
    g_verbSynonyms[V_LOOK].push_back("look");
    g_verbLookup["l"] = V_LOOK;
    g_verbCanonical["l"] = "look";
    g_verbSynonyms[V_LOOK].push_back("l");
    g_verbLookup["stare"] = V_LOOK;
    g_verbCanonical["stare"] = "look";
    g_verbSynonyms[V_LOOK].push_back("stare");
    g_verbLookup["gaze"] = V_LOOK;
    g_verbCanonical["gaze"] = "look";
    g_verbSynonyms[V_LOOK].push_back("gaze");
    g_verbLookup["lubricate"] = V_LUBRICATE;
    g_verbCanonical["lubricate"] = "lubricate";
    g_verbSynonyms[V_LUBRICATE].push_back("lubricate");
    g_verbLookup["oil"] = V_LUBRICATE;
    g_verbCanonical["oil"] = "lubricate";
    g_verbSynonyms[V_LUBRICATE].push_back("oil");
    g_verbLookup["grease"] = V_LUBRICATE;
    g_verbCanonical["grease"] = "lubricate";
    g_verbSynonyms[V_LUBRICATE].push_back("grease");
    g_verbLookup["melt"] = V_MELT;
    g_verbCanonical["melt"] = "melt";
    g_verbSynonyms[V_MELT].push_back("melt");
    g_verbLookup["liquify"] = V_MELT;
    g_verbCanonical["liquify"] = "melt";
    g_verbSynonyms[V_MELT].push_back("liquify");
    g_verbLookup["mumble"] = V_MUMBLE;
    g_verbCanonical["mumble"] = "mumble";
    g_verbSynonyms[V_MUMBLE].push_back("mumble");
    g_verbLookup["sigh"] = V_MUMBLE;
    g_verbCanonical["sigh"] = "mumble";
    g_verbSynonyms[V_MUMBLE].push_back("sigh");
    g_verbLookup["odysseus"] = V_ODYSSEUS;
    g_verbCanonical["odysseus"] = "odysseus";
    g_verbSynonyms[V_ODYSSEUS].push_back("odysseus");
    g_verbLookup["ulysses"] = V_ODYSSEUS;
    g_verbCanonical["ulysses"] = "odysseus";
    g_verbSynonyms[V_ODYSSEUS].push_back("ulysses");
    g_verbLookup["plug"] = V_PLUG;
    g_verbCanonical["plug"] = "plug";
    g_verbSynonyms[V_PLUG].push_back("plug");
    g_verbLookup["glue"] = V_PLUG;
    g_verbCanonical["glue"] = "plug";
    g_verbSynonyms[V_PLUG].push_back("glue");
    g_verbLookup["patch"] = V_PLUG;
    g_verbCanonical["patch"] = "plug";
    g_verbSynonyms[V_PLUG].push_back("patch");
    g_verbLookup["repair"] = V_PLUG;
    g_verbCanonical["repair"] = "plug";
    g_verbSynonyms[V_PLUG].push_back("repair");
    g_verbLookup["fix"] = V_PLUG;
    g_verbCanonical["fix"] = "plug";
    g_verbSynonyms[V_PLUG].push_back("fix");
    g_verbLookup["plugh"] = V_PLUGH;
    g_verbCanonical["plugh"] = "plugh";
    g_verbSynonyms[V_PLUGH].push_back("plugh");
    g_verbLookup["xyzzy"] = V_PLUGH;
    g_verbCanonical["xyzzy"] = "plugh";
    g_verbSynonyms[V_PLUGH].push_back("xyzzy");
    g_verbLookup["pour"] = V_POUR;
    g_verbCanonical["pour"] = "pour";
    g_verbSynonyms[V_POUR].push_back("pour");
    g_verbLookup["spill"] = V_POUR;
    g_verbCanonical["spill"] = "pour";
    g_verbSynonyms[V_POUR].push_back("spill");
    g_verbLookup["pull"] = V_PULL;
    g_verbCanonical["pull"] = "pull";
    g_verbSynonyms[V_PULL].push_back("pull");
    g_verbLookup["tug"] = V_PULL;
    g_verbCanonical["tug"] = "pull";
    g_verbSynonyms[V_PULL].push_back("tug");
    g_verbLookup["yank"] = V_PULL;
    g_verbCanonical["yank"] = "pull";
    g_verbSynonyms[V_PULL].push_back("yank");
    g_verbLookup["push"] = V_PUSH;
    g_verbCanonical["push"] = "push";
    g_verbSynonyms[V_PUSH].push_back("push");
    g_verbLookup["press"] = V_PUSH;
    g_verbCanonical["press"] = "push";
    g_verbSynonyms[V_PUSH].push_back("press");
    g_verbLookup["put"] = V_PUT;
    g_verbCanonical["put"] = "put";
    g_verbSynonyms[V_PUT].push_back("put");
    g_verbLookup["stuff"] = V_PUT;
    g_verbCanonical["stuff"] = "put";
    g_verbSynonyms[V_PUT].push_back("stuff");
    g_verbLookup["insert"] = V_PUT;
    g_verbCanonical["insert"] = "put";
    g_verbSynonyms[V_PUT].push_back("insert");
    g_verbLookup["place"] = V_PUT;
    g_verbCanonical["place"] = "put";
    g_verbSynonyms[V_PUT].push_back("place");
    g_verbLookup["hide"] = V_PUT;
    g_verbCanonical["hide"] = "put";
    g_verbSynonyms[V_PUT].push_back("hide");
    g_verbLookup["raise"] = V_RAISE;
    g_verbCanonical["raise"] = "raise";
    g_verbSynonyms[V_RAISE].push_back("raise");
    g_verbLookup["lift"] = V_RAISE;
    g_verbCanonical["lift"] = "raise";
    g_verbSynonyms[V_RAISE].push_back("lift");
    g_verbLookup["rape"] = V_RAPE;
    g_verbCanonical["rape"] = "rape";
    g_verbSynonyms[V_RAPE].push_back("rape");
    g_verbLookup["molest"] = V_RAPE;
    g_verbCanonical["molest"] = "rape";
    g_verbSynonyms[V_RAPE].push_back("molest");
    g_verbLookup["read"] = V_READ;
    g_verbCanonical["read"] = "read";
    g_verbSynonyms[V_READ].push_back("read");
    g_verbLookup["skim"] = V_READ;
    g_verbCanonical["skim"] = "read";
    g_verbSynonyms[V_READ].push_back("skim");
    g_verbLookup["ring"] = V_RING;
    g_verbCanonical["ring"] = "ring";
    g_verbSynonyms[V_RING].push_back("ring");
    g_verbLookup["peal"] = V_RING;
    g_verbCanonical["peal"] = "ring";
    g_verbSynonyms[V_RING].push_back("peal");
    g_verbLookup["rub"] = V_RUB;
    g_verbCanonical["rub"] = "rub";
    g_verbSynonyms[V_RUB].push_back("rub");
    g_verbLookup["touch"] = V_RUB;
    g_verbCanonical["touch"] = "rub";
    g_verbSynonyms[V_RUB].push_back("touch");
    g_verbLookup["feel"] = V_RUB;
    g_verbCanonical["feel"] = "rub";
    g_verbSynonyms[V_RUB].push_back("feel");
    g_verbLookup["pat"] = V_RUB;
    g_verbCanonical["pat"] = "rub";
    g_verbSynonyms[V_RUB].push_back("pat");
    g_verbLookup["pet"] = V_RUB;
    g_verbCanonical["pet"] = "rub";
    g_verbSynonyms[V_RUB].push_back("pet");
    g_verbLookup["skip"] = V_SKIP;
    g_verbCanonical["skip"] = "skip";
    g_verbSynonyms[V_SKIP].push_back("skip");
    g_verbLookup["hop"] = V_SKIP;
    g_verbCanonical["hop"] = "skip";
    g_verbSynonyms[V_SKIP].push_back("hop");
    g_verbLookup["smell"] = V_SMELL;
    g_verbCanonical["smell"] = "smell";
    g_verbSynonyms[V_SMELL].push_back("smell");
    g_verbLookup["sniff"] = V_SMELL;
    g_verbCanonical["sniff"] = "smell";
    g_verbSynonyms[V_SMELL].push_back("sniff");
    g_verbLookup["swim"] = V_SWIM;
    g_verbCanonical["swim"] = "swim";
    g_verbSynonyms[V_SWIM].push_back("swim");
    g_verbLookup["bathe"] = V_SWIM;
    g_verbCanonical["bathe"] = "swim";
    g_verbSynonyms[V_SWIM].push_back("bathe");
    g_verbLookup["wade"] = V_SWIM;
    g_verbCanonical["wade"] = "swim";
    g_verbSynonyms[V_SWIM].push_back("wade");
    g_verbLookup["swing"] = V_SWING;
    g_verbCanonical["swing"] = "swing";
    g_verbSynonyms[V_SWING].push_back("swing");
    g_verbLookup["thrust"] = V_SWING;
    g_verbCanonical["thrust"] = "swing";
    g_verbSynonyms[V_SWING].push_back("thrust");
    g_verbLookup["take"] = V_TAKE;
    g_verbCanonical["take"] = "take";
    g_verbSynonyms[V_TAKE].push_back("take");
    g_verbLookup["get"] = V_TAKE;
    g_verbCanonical["get"] = "take";
    g_verbSynonyms[V_TAKE].push_back("get");
    g_verbLookup["hold"] = V_TAKE;
    g_verbCanonical["hold"] = "take";
    g_verbSynonyms[V_TAKE].push_back("hold");
    g_verbLookup["carry"] = V_TAKE;
    g_verbCanonical["carry"] = "take";
    g_verbSynonyms[V_TAKE].push_back("carry");
    g_verbLookup["remove"] = V_TAKE;
    g_verbCanonical["remove"] = "take";
    g_verbSynonyms[V_TAKE].push_back("remove");
    g_verbLookup["grab"] = V_TAKE;
    g_verbCanonical["grab"] = "take";
    g_verbSynonyms[V_TAKE].push_back("grab");
    g_verbLookup["catch"] = V_TAKE;
    g_verbCanonical["catch"] = "take";
    g_verbSynonyms[V_TAKE].push_back("catch");
    g_verbLookup["tell"] = V_TELL;
    g_verbCanonical["tell"] = "tell";
    g_verbSynonyms[V_TELL].push_back("tell");
    g_verbLookup["ask"] = V_TELL;
    g_verbCanonical["ask"] = "tell";
    g_verbSynonyms[V_TELL].push_back("ask");
    g_verbLookup["throw"] = V_THROW;
    g_verbCanonical["throw"] = "throw";
    g_verbSynonyms[V_THROW].push_back("throw");
    g_verbLookup["hurl"] = V_THROW;
    g_verbCanonical["hurl"] = "throw";
    g_verbSynonyms[V_THROW].push_back("hurl");
    g_verbLookup["chuck"] = V_THROW;
    g_verbCanonical["chuck"] = "throw";
    g_verbSynonyms[V_THROW].push_back("chuck");
    g_verbLookup["toss"] = V_THROW;
    g_verbCanonical["toss"] = "throw";
    g_verbSynonyms[V_THROW].push_back("toss");
    g_verbLookup["tie"] = V_TIE;
    g_verbCanonical["tie"] = "tie";
    g_verbSynonyms[V_TIE].push_back("tie");
    g_verbLookup["fasten"] = V_TIE;
    g_verbCanonical["fasten"] = "tie";
    g_verbSynonyms[V_TIE].push_back("fasten");
    g_verbLookup["secure"] = V_TIE;
    g_verbCanonical["secure"] = "tie";
    g_verbSynonyms[V_TIE].push_back("secure");
    g_verbLookup["attach"] = V_TIE;
    g_verbCanonical["attach"] = "tie";
    g_verbSynonyms[V_TIE].push_back("attach");
    g_verbLookup["treasure"] = V_TREASURE;
    g_verbCanonical["treasure"] = "treasure";
    g_verbSynonyms[V_TREASURE].push_back("treasure");
    g_verbLookup["temple"] = V_TREASURE;
    g_verbCanonical["temple"] = "treasure";
    g_verbSynonyms[V_TREASURE].push_back("temple");
    g_verbLookup["turn"] = V_TURN;
    g_verbCanonical["turn"] = "turn";
    g_verbSynonyms[V_TURN].push_back("turn");
    g_verbLookup["set"] = V_TURN;
    g_verbCanonical["set"] = "turn";
    g_verbSynonyms[V_TURN].push_back("set");
    g_verbLookup["flip"] = V_TURN;
    g_verbCanonical["flip"] = "turn";
    g_verbSynonyms[V_TURN].push_back("flip");
    g_verbLookup["shut"] = V_TURN;
    g_verbCanonical["shut"] = "turn";
    g_verbSynonyms[V_TURN].push_back("shut");
    g_verbLookup["untie"] = V_UNTIE;
    g_verbCanonical["untie"] = "untie";
    g_verbSynonyms[V_UNTIE].push_back("untie");
    g_verbLookup["free"] = V_UNTIE;
    g_verbCanonical["free"] = "untie";
    g_verbSynonyms[V_UNTIE].push_back("free");
    g_verbLookup["release"] = V_UNTIE;
    g_verbCanonical["release"] = "untie";
    g_verbSynonyms[V_UNTIE].push_back("release");
    g_verbLookup["unfasten"] = V_UNTIE;
    g_verbCanonical["unfasten"] = "untie";
    g_verbSynonyms[V_UNTIE].push_back("unfasten");
    g_verbLookup["unattach"] = V_UNTIE;
    g_verbCanonical["unattach"] = "untie";
    g_verbSynonyms[V_UNTIE].push_back("unattach");
    g_verbLookup["unhook"] = V_UNTIE;
    g_verbCanonical["unhook"] = "untie";
    g_verbSynonyms[V_UNTIE].push_back("unhook");
    g_verbLookup["wait"] = V_WAIT;
    g_verbCanonical["wait"] = "wait";
    g_verbSynonyms[V_WAIT].push_back("wait");
    g_verbLookup["z"] = V_WAIT;
    g_verbCanonical["z"] = "wait";
    g_verbSynonyms[V_WAIT].push_back("z");
    g_verbLookup["wake"] = V_WAKE;
    g_verbCanonical["wake"] = "wake";
    g_verbSynonyms[V_WAKE].push_back("wake");
    g_verbLookup["awake"] = V_WAKE;
    g_verbCanonical["awake"] = "wake";
    g_verbSynonyms[V_WAKE].push_back("awake");
    g_verbLookup["surprise"] = V_WAKE;
    g_verbCanonical["surprise"] = "wake";
    g_verbSynonyms[V_WAKE].push_back("surprise");
    g_verbLookup["startle"] = V_WAKE;
    g_verbCanonical["startle"] = "wake";
    g_verbSynonyms[V_WAKE].push_back("startle");
    g_verbLookup["walk"] = V_WALK;
    g_verbCanonical["walk"] = "walk";
    g_verbSynonyms[V_WALK].push_back("walk");
    g_verbLookup["go"] = V_WALK;
    g_verbCanonical["go"] = "walk";
    g_verbSynonyms[V_WALK].push_back("go");
    g_verbLookup["run"] = V_WALK;
    g_verbCanonical["run"] = "walk";
    g_verbSynonyms[V_WALK].push_back("run");
    g_verbLookup["proceed"] = V_WALK;
    g_verbCanonical["proceed"] = "walk";
    g_verbSynonyms[V_WALK].push_back("proceed");
    g_verbLookup["step"] = V_WALK;
    g_verbCanonical["step"] = "walk";
    g_verbSynonyms[V_WALK].push_back("step");
    g_verbLookup["wave"] = V_WAVE;
    g_verbCanonical["wave"] = "wave";
    g_verbSynonyms[V_WAVE].push_back("wave");
    g_verbLookup["brandish"] = V_WAVE;
    g_verbCanonical["brandish"] = "wave";
    g_verbSynonyms[V_WAVE].push_back("brandish");
    g_verbLookup["win"] = V_WIN;
    g_verbCanonical["win"] = "win";
    g_verbSynonyms[V_WIN].push_back("win");
    g_verbLookup["winnage"] = V_WIN;
    g_verbCanonical["winnage"] = "win";
    g_verbSynonyms[V_WIN].push_back("winnage");
    g_verbLookup["yell"] = V_YELL;
    g_verbCanonical["yell"] = "yell";
    g_verbSynonyms[V_YELL].push_back("yell");
    g_verbLookup["scream"] = V_YELL;
    g_verbCanonical["scream"] = "yell";
    g_verbSynonyms[V_YELL].push_back("scream");
    g_verbLookup["shout"] = V_YELL;
    g_verbCanonical["shout"] = "yell";
    g_verbSynonyms[V_YELL].push_back("shout");

    // 267 Syntaxes from zil/gsyntax.zil
    g_syntaxes.reserve(267);

    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:40";
        s.patternString = "VERBOSE";
        s.verb = "VERBOSE";
        s.verbId = V_VERBOSE;
        s.actionName = "V-VERBOSE";
        s.actionId = V_VERBOSE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "VERBOSE";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:42";
        s.patternString = "BRIEF";
        s.verb = "BRIEF";
        s.verbId = V_BRIEF;
        s.actionName = "V-BRIEF";
        s.actionId = V_BRIEF;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "BRIEF";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:44";
        s.patternString = "SUPER";
        s.verb = "SUPER";
        s.verbId = V_SUPER;
        s.actionName = "V-SUPER-BRIEF";
        s.actionId = V_SUPERBRIEF;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SUPER";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:47";
        s.patternString = "DIAGNOSE";
        s.verb = "DIAGNOSE";
        s.verbId = V_DIAGNOSE;
        s.actionName = "V-DIAGNOSE";
        s.actionId = V_DIAGNOSE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "DIAGNOSE";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:49";
        s.patternString = "INVENTORY";
        s.verb = "INVENTORY";
        s.verbId = V_INVENTORY;
        s.actionName = "V-INVENTORY";
        s.actionId = V_INVENTORY;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "INVENTORY";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:52";
        s.patternString = "QUIT";
        s.verb = "QUIT";
        s.verbId = V_QUIT;
        s.actionName = "V-QUIT";
        s.actionId = V_QUIT;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "QUIT";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:55";
        s.patternString = "RESTART";
        s.verb = "RESTART";
        s.verbId = V_RESTART;
        s.actionName = "V-RESTART";
        s.actionId = V_RESTART;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "RESTART";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:57";
        s.patternString = "RESTORE";
        s.verb = "RESTORE";
        s.verbId = V_RESTORE;
        s.actionName = "V-RESTORE";
        s.actionId = V_RESTORE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "RESTORE";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:59";
        s.patternString = "SAVE";
        s.verb = "SAVE";
        s.verbId = V_SAVE;
        s.actionName = "V-SAVE";
        s.actionId = V_SAVE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SAVE";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:61";
        s.patternString = "SCORE";
        s.verb = "SCORE";
        s.verbId = V_SCORE;
        s.actionName = "V-SCORE";
        s.actionId = V_SCORE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SCORE";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:63";
        s.patternString = "SCRIPT";
        s.verb = "SCRIPT";
        s.verbId = V_SCRIPT;
        s.actionName = "V-SCRIPT";
        s.actionId = V_SCRIPT;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SCRIPT";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:65";
        s.patternString = "UNSCRIPT";
        s.verb = "UNSCRIPT";
        s.verbId = V_UNSCRIPT;
        s.actionName = "V-UNSCRIPT";
        s.actionId = V_UNSCRIPT;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "UNSCRIPT";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:67";
        s.patternString = "VERSION";
        s.verb = "VERSION";
        s.verbId = V_VERSION;
        s.actionName = "V-VERSION";
        s.actionId = V_VERSION;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "VERSION";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:69";
        s.patternString = "$VERIFY";
        s.verb = "$VERIFY";
        s.verbId = V_VERIFY;
        s.actionName = "V-VERIFY";
        s.actionId = V_VERIFY;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "$VERIFY";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:71";
        s.patternString = "\\#RANDOM OBJECT";
        s.verb = "\\#RANDOM";
        s.verbId = V_RANDOM;
        s.actionName = "V-RANDOM";
        s.actionId = V_RANDOM;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "\\#RANDOM";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:73";
        s.patternString = "\\#COMMAND";
        s.verb = "\\#COMMAND";
        s.verbId = V_COMMAND;
        s.actionName = "V-COMMAND-FILE";
        s.actionId = V_COMMAND_FILE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "\\#COMMAND";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:75";
        s.patternString = "\\#RECORD";
        s.verb = "\\#RECORD";
        s.verbId = V_RECORD;
        s.actionName = "V-RECORD";
        s.actionId = V_RECORD;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "\\#RECORD";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:77";
        s.patternString = "\\#UNRECORD";
        s.verb = "\\#UNRECORD";
        s.verbId = V_UNRECORD;
        s.actionName = "V-UNRECORD";
        s.actionId = V_UNRECORD;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "\\#UNRECORD";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:83";
        s.patternString = "ACTIVATE OBJECT (FIND LIGHTBIT) (HELD CARRIED ON-GROUND IN-ROOM)";
        s.verb = "ACTIVATE";
        s.verbId = V_ACTIVATE;
        s.actionName = "V-LAMP-ON";
        s.actionId = V_LAMP_ON;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "ACTIVATE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::LIGHTBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:86";
        s.patternString = "ANSWER";
        s.verb = "ANSWER";
        s.verbId = V_ANSWER;
        s.actionName = "V-ANSWER";
        s.actionId = V_ANSWER;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "ANSWER";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:86";
        s.patternString = "ANSWER OBJECT";
        s.verb = "ANSWER";
        s.verbId = V_ANSWER;
        s.actionName = "V-REPLY";
        s.actionId = V_REPLY;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "ANSWER";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:90";
        s.patternString = "APPLY OBJECT TO OBJECT";
        s.verb = "APPLY";
        s.verbId = V_APPLY;
        s.actionName = "V-PUT";
        s.actionId = V_PUT;
        s.preactionName = "PRE-PUT";
        s.preactionId = V_PUT;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "APPLY";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "to";
            e.synonyms = {"to"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:93";
        s.patternString = "ATTACK OBJECT (FIND ACTORBIT) (ON-GROUND IN-ROOM) WITH OBJECT (FIND WEAPONBIT) (HELD CARRIED HAVE)";
        s.verb = "ATTACK";
        s.verbId = V_ATTACK;
        s.actionName = "V-ATTACK";
        s.actionId = V_ATTACK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "ATTACK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::WEAPONBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:100";
        s.patternString = "BACK";
        s.verb = "BACK";
        s.verbId = V_BACK;
        s.actionName = "V-BACK";
        s.actionId = V_BACK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "BACK";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:102";
        s.patternString = "BLAST";
        s.verb = "BLAST";
        s.verbId = V_BLAST;
        s.actionName = "V-BLAST";
        s.actionId = V_BLAST;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "BLAST";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:104";
        s.patternString = "BLOW OUT OBJECT";
        s.verb = "BLOW";
        s.verbId = V_BLOW;
        s.actionName = "V-LAMP-OFF";
        s.actionId = V_LAMP_OFF;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "BLOW";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "out";
            e.synonyms = {"out"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:104";
        s.patternString = "BLOW UP OBJECT WITH OBJECT (FIND TOOLBIT) (ON-GROUND IN-ROOM HELD CARRIED)";
        s.verb = "BLOW";
        s.verbId = V_BLOW;
        s.actionName = "V-INFLATE";
        s.actionId = V_INFLATE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "BLOW";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TOOLBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM | SH_HELD | SH_CARRIED;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:104";
        s.patternString = "BLOW UP OBJECT";
        s.verb = "BLOW";
        s.verbId = V_BLOW;
        s.actionName = "V-BLAST";
        s.actionId = V_BLAST;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "BLOW";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:104";
        s.patternString = "BLOW IN OBJECT";
        s.verb = "BLOW";
        s.verbId = V_BLOW;
        s.actionName = "V-BREATHE";
        s.actionId = V_BREATHE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "BLOW";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:110";
        s.patternString = "BOARD OBJECT (FIND VEHBIT) (ON-GROUND IN-ROOM)";
        s.verb = "BOARD";
        s.verbId = V_BOARD;
        s.actionName = "V-BOARD";
        s.actionId = V_BOARD;
        s.preactionName = "PRE-BOARD";
        s.preactionId = V_BOARD;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "BOARD";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::VEHBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:112";
        s.patternString = "BRUSH OBJECT (HELD CARRIED ON-GROUND IN-ROOM)";
        s.verb = "BRUSH";
        s.verbId = V_BRUSH;
        s.actionName = "V-BRUSH";
        s.actionId = V_BRUSH;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "BRUSH";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:112";
        s.patternString = "BRUSH OBJECT (HELD CARRIED ON-GROUND IN-ROOM) WITH OBJECT";
        s.verb = "BRUSH";
        s.verbId = V_BRUSH;
        s.actionName = "V-BRUSH";
        s.actionId = V_BRUSH;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "BRUSH";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:117";
        s.patternString = "BUG";
        s.verb = "BUG";
        s.verbId = V_BUG;
        s.actionName = "V-BUG";
        s.actionId = V_BUG;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "BUG";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:119";
        s.patternString = "BURN OBJECT (FIND BURNBIT) (HELD CARRIED ON-GROUND IN-ROOM) WITH OBJECT (FIND FLAMEBIT) (HELD CARRIED ON-GROUND IN-ROOM HAVE)";
        s.verb = "BURN";
        s.verbId = V_BURN;
        s.actionName = "V-BURN";
        s.actionId = V_BURN;
        s.preactionName = "PRE-BURN";
        s.preactionId = V_BURN;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "BURN";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::BURNBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::FLAMEBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:119";
        s.patternString = "BURN DOWN OBJECT (FIND BURNBIT) (HELD CARRIED ON-GROUND IN-ROOM) WITH OBJECT (FIND FLAMEBIT) (HELD CARRIED ON-GROUND IN-ROOM HAVE)";
        s.verb = "BURN";
        s.verbId = V_BURN;
        s.actionName = "V-BURN";
        s.actionId = V_BURN;
        s.preactionName = "PRE-BURN";
        s.preactionId = V_BURN;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "BURN";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "down";
            e.synonyms = {"down"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::BURNBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::FLAMEBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:127";
        s.patternString = "CHOMP";
        s.verb = "CHOMP";
        s.verbId = V_CHOMP;
        s.actionName = "V-CHOMP";
        s.actionId = V_CHOMP;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "CHOMP";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:130";
        s.patternString = "CLIMB UP OBJECT (FIND RMUNGBIT)";
        s.verb = "CLIMB";
        s.verbId = V_CLIMB;
        s.actionName = "V-CLIMB-UP";
        s.actionId = V_CLIMB_UP;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "CLIMB";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::RMUNGBIT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:130";
        s.patternString = "CLIMB UP OBJECT (FIND CLIMBBIT) (ON-GROUND IN-ROOM)";
        s.verb = "CLIMB";
        s.verbId = V_CLIMB;
        s.actionName = "V-CLIMB-UP";
        s.actionId = V_CLIMB_UP;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "CLIMB";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::CLIMBBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:130";
        s.patternString = "CLIMB DOWN OBJECT (FIND RMUNGBIT)";
        s.verb = "CLIMB";
        s.verbId = V_CLIMB;
        s.actionName = "V-CLIMB-DOWN";
        s.actionId = V_CLIMB_DOWN;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "CLIMB";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "down";
            e.synonyms = {"down"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::RMUNGBIT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:130";
        s.patternString = "CLIMB DOWN OBJECT (FIND CLIMBBIT) (ON-GROUND IN-ROOM)";
        s.verb = "CLIMB";
        s.verbId = V_CLIMB;
        s.actionName = "V-CLIMB-DOWN";
        s.actionId = V_CLIMB_DOWN;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "CLIMB";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "down";
            e.synonyms = {"down"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::CLIMBBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:130";
        s.patternString = "CLIMB OBJECT (FIND CLIMBBIT) (ON-GROUND IN-ROOM)";
        s.verb = "CLIMB";
        s.verbId = V_CLIMB;
        s.actionName = "V-CLIMB-FOO";
        s.actionId = V_CLIMB_FOO;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "CLIMB";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::CLIMBBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:131";
        s.patternString = "CLIMB IN OBJECT (FIND VEHBIT) (ON-GROUND IN-ROOM)";
        s.verb = "CLIMB";
        s.verbId = V_CLIMB;
        s.actionName = "V-BOARD";
        s.actionId = V_BOARD;
        s.preactionName = "PRE-BOARD";
        s.preactionId = V_BOARD;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "CLIMB";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::VEHBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:132";
        s.patternString = "CLIMB ON OBJECT (FIND VEHBIT) (ON-GROUND IN-ROOM)";
        s.verb = "CLIMB";
        s.verbId = V_CLIMB;
        s.actionName = "V-CLIMB-ON";
        s.actionId = V_CLIMB_ON;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "CLIMB";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::VEHBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:133";
        s.patternString = "CLIMB WITH OBJECT";
        s.verb = "CLIMB";
        s.verbId = V_CLIMB;
        s.actionName = "V-THROUGH";
        s.actionId = V_THROUGH;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "CLIMB";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:140";
        s.patternString = "CLOSE OBJECT (FIND DOORBIT) (HELD CARRIED ON-GROUND IN-ROOM)";
        s.verb = "CLOSE";
        s.verbId = V_CLOSE;
        s.actionName = "V-CLOSE";
        s.actionId = V_CLOSE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "CLOSE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::DOORBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:142";
        s.patternString = "COMMAND OBJECT (FIND ACTORBIT)";
        s.verb = "COMMAND";
        s.verbId = V_COMMAND;
        s.actionName = "V-COMMAND";
        s.actionId = V_COMMAND;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "COMMAND";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:144";
        s.patternString = "COUNT OBJECT";
        s.verb = "COUNT";
        s.verbId = V_COUNT;
        s.actionName = "V-COUNT";
        s.actionId = V_COUNT;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "COUNT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:146";
        s.patternString = "CROSS OBJECT";
        s.verb = "CROSS";
        s.verbId = V_CROSS;
        s.actionName = "V-CROSS";
        s.actionId = V_CROSS;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "CROSS";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:149";
        s.patternString = "CUT OBJECT WITH OBJECT (FIND WEAPONBIT) (CARRIED HELD)";
        s.verb = "CUT";
        s.verbId = V_CUT;
        s.actionName = "V-CUT";
        s.actionId = V_CUT;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "CUT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::WEAPONBIT;
            e.scopeFlags = SH_CARRIED | SH_HELD;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:152";
        s.patternString = "CURSE";
        s.verb = "CURSE";
        s.verbId = V_CURSE;
        s.actionName = "V-CURSES";
        s.actionId = V_CURSE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "CURSE";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:152";
        s.patternString = "CURSE OBJECT (FIND ACTORBIT)";
        s.verb = "CURSE";
        s.verbId = V_CURSE;
        s.actionName = "V-CURSES";
        s.actionId = V_CURSE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "CURSE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:156";
        s.patternString = "DEFLATE OBJECT";
        s.verb = "DEFLATE";
        s.verbId = V_DEFLATE;
        s.actionName = "V-DEFLATE";
        s.actionId = V_DEFLATE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "DEFLATE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:158";
        s.patternString = "DESTROY OBJECT (ON-GROUND IN-ROOM HELD CARRIED) WITH OBJECT (HELD CARRIED TAKE)";
        s.verb = "DESTROY";
        s.verbId = V_DESTROY;
        s.actionName = "V-MUNG";
        s.actionId = V_MUNG;
        s.preactionName = "PRE-MUNG";
        s.preactionId = V_MUNG;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "DESTROY";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM | SH_HELD | SH_CARRIED;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:158";
        s.patternString = "DESTROY DOWN OBJECT (ON-GROUND IN-ROOM HELD CARRIED) WITH OBJECT (HELD CARRIED TAKE)";
        s.verb = "DESTROY";
        s.verbId = V_DESTROY;
        s.actionName = "V-MUNG";
        s.actionId = V_MUNG;
        s.preactionName = "PRE-MUNG";
        s.preactionId = V_MUNG;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "DESTROY";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "down";
            e.synonyms = {"down"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM | SH_HELD | SH_CARRIED;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:158";
        s.patternString = "DESTROY IN OBJECT (ON-GROUND IN-ROOM HELD CARRIED)";
        s.verb = "DESTROY";
        s.verbId = V_DESTROY;
        s.actionName = "V-OPEN";
        s.actionId = V_OPEN;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "DESTROY";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM | SH_HELD | SH_CARRIED;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:165";
        s.patternString = "DIG IN OBJECT (ON-GROUND IN-ROOM)";
        s.verb = "DIG";
        s.verbId = V_DIG;
        s.actionName = "V-DIG";
        s.actionId = V_DIG;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "DIG";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:165";
        s.patternString = "DIG IN OBJECT (ON-GROUND IN-ROOM) WITH OBJECT (FIND TOOLBIT) (HELD CARRIED HAVE)";
        s.verb = "DIG";
        s.verbId = V_DIG;
        s.actionName = "V-DIG";
        s.actionId = V_DIG;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "DIG";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TOOLBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:165";
        s.patternString = "DIG OBJECT (ON-GROUND IN-ROOM) WITH OBJECT (FIND TOOLBIT) (HELD CARRIED HAVE)";
        s.verb = "DIG";
        s.verbId = V_DIG;
        s.actionName = "V-DIG";
        s.actionId = V_DIG;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "DIG";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TOOLBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:171";
        s.patternString = "DISEMBARK OBJECT (FIND VEHBIT) (ON-GROUND IN-ROOM)";
        s.verb = "DISEMBARK";
        s.verbId = V_DISEMBARK;
        s.actionName = "V-DISEMBARK";
        s.actionId = V_DISEMBARK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "DISEMBARK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::VEHBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:174";
        s.patternString = "DISENCHANT OBJECT";
        s.verb = "DISENCHANT";
        s.verbId = V_DISENCHANT;
        s.actionName = "V-DISENCHANT";
        s.actionId = V_DISENCHANT;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "DISENCHANT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:176";
        s.patternString = "DRINK OBJECT (FIND DRINKBIT) (HELD CARRIED ON-GROUND IN-ROOM)";
        s.verb = "DRINK";
        s.verbId = V_DRINK;
        s.actionName = "V-DRINK";
        s.actionId = V_DRINK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "DRINK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::DRINKBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:176";
        s.patternString = "DRINK FROM OBJECT (HELD CARRIED)";
        s.verb = "DRINK";
        s.verbId = V_DRINK;
        s.actionName = "V-DRINK-FROM";
        s.actionId = V_DRINK_FROM;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "DRINK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "from";
            e.synonyms = {"from"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:181";
        s.patternString = "DROP OBJECT (HELD MANY HAVE)";
        s.verb = "DROP";
        s.verbId = V_DROP;
        s.actionName = "V-DROP";
        s.actionId = V_DROP;
        s.preactionName = "PRE-DROP";
        s.preactionId = V_DROP;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "DROP";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD;
            e.have = true;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:181";
        s.patternString = "DROP OBJECT (HELD MANY HAVE) DOWN OBJECT";
        s.verb = "DROP";
        s.verbId = V_DROP;
        s.actionName = "V-PUT";
        s.actionId = V_PUT;
        s.preactionName = "PRE-PUT";
        s.preactionId = V_PUT;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "DROP";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD;
            e.have = true;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "down";
            e.synonyms = {"down"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:181";
        s.patternString = "DROP OBJECT (HELD MANY HAVE) IN OBJECT";
        s.verb = "DROP";
        s.verbId = V_DROP;
        s.actionName = "V-PUT";
        s.actionId = V_PUT;
        s.preactionName = "PRE-PUT";
        s.preactionId = V_PUT;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "DROP";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD;
            e.have = true;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:181";
        s.patternString = "DROP OBJECT (HELD MANY HAVE) ON OBJECT";
        s.verb = "DROP";
        s.verbId = V_DROP;
        s.actionName = "V-PUT-ON";
        s.actionId = V_PUT_ON;
        s.preactionName = "PRE-PUT";
        s.preactionId = V_PUT;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "DROP";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD;
            e.have = true;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:186";
        s.patternString = "EAT OBJECT (FIND FOODBIT) (HELD CARRIED ON-GROUND IN-ROOM TAKE)";
        s.verb = "EAT";
        s.verbId = V_EAT;
        s.actionName = "V-EAT";
        s.actionId = V_EAT;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "EAT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::FOODBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:190";
        s.patternString = "ECHO";
        s.verb = "ECHO";
        s.verbId = V_ECHO;
        s.actionName = "V-ECHO";
        s.actionId = V_ECHO;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "ECHO";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:192";
        s.patternString = "ENCHANT OBJECT (ON-GROUND IN-ROOM)";
        s.verb = "ENCHANT";
        s.verbId = V_ENCHANT;
        s.actionName = "V-ENCHANT";
        s.actionId = V_ENCHANT;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "ENCHANT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:194";
        s.patternString = "ENTER";
        s.verb = "ENTER";
        s.verbId = V_ENTER;
        s.actionName = "V-ENTER";
        s.actionId = V_ENTER;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "ENTER";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:194";
        s.patternString = "ENTER OBJECT";
        s.verb = "ENTER";
        s.verbId = V_ENTER;
        s.actionName = "V-THROUGH";
        s.actionId = V_THROUGH;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "ENTER";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:196";
        s.patternString = "EXIT";
        s.verb = "EXIT";
        s.verbId = V_EXIT;
        s.actionName = "V-EXIT";
        s.actionId = V_EXIT;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "EXIT";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:196";
        s.patternString = "EXIT OBJECT";
        s.verb = "EXIT";
        s.verbId = V_EXIT;
        s.actionName = "V-EXIT";
        s.actionId = V_EXIT;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "EXIT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:199";
        s.patternString = "EXAMINE OBJECT (MANY)";
        s.verb = "EXAMINE";
        s.verbId = V_EXAMINE;
        s.actionName = "V-EXAMINE";
        s.actionId = V_EXAMINE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "EXAMINE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:199";
        s.patternString = "EXAMINE IN OBJECT (HELD CARRIED IN-ROOM ON-GROUND MANY)";
        s.verb = "EXAMINE";
        s.verbId = V_EXAMINE;
        s.actionName = "V-LOOK-INSIDE";
        s.actionId = V_LOOK_INSIDE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "EXAMINE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_IN_ROOM | SH_ON_GROUND;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:199";
        s.patternString = "EXAMINE ON OBJECT (HELD CARRIED IN-ROOM ON-GROUND MANY)";
        s.verb = "EXAMINE";
        s.verbId = V_EXAMINE;
        s.actionName = "V-LOOK-INSIDE";
        s.actionId = V_LOOK_INSIDE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "EXAMINE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_IN_ROOM | SH_ON_GROUND;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:206";
        s.patternString = "EXORCISE OBJECT";
        s.verb = "EXORCISE";
        s.verbId = V_EXORCISE;
        s.actionName = "V-EXORCISE";
        s.actionId = V_EXORCISE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "EXORCISE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:206";
        s.patternString = "EXORCISE OUT OBJECT (FIND ACTORBIT)";
        s.verb = "EXORCISE";
        s.verbId = V_EXORCISE;
        s.actionName = "V-EXORCISE";
        s.actionId = V_EXORCISE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "EXORCISE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "out";
            e.synonyms = {"out"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:206";
        s.patternString = "EXORCISE AWAY OBJECT (FIND ACTORBIT)";
        s.verb = "EXORCISE";
        s.verbId = V_EXORCISE;
        s.actionName = "V-EXORCISE";
        s.actionId = V_EXORCISE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "EXORCISE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "away";
            e.synonyms = {"away"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:211";
        s.patternString = "EXTINGUISH OBJECT (FIND ONBIT) (HELD CARRIED ON-GROUND IN-ROOM TAKE HAVE)";
        s.verb = "EXTINGUISH";
        s.verbId = V_EXTINGUISH;
        s.actionName = "V-LAMP-OFF";
        s.actionId = V_LAMP_OFF;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "EXTINGUISH";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ONBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.have = true;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:215";
        s.patternString = "FILL OBJECT (FIND CONTBIT) (HELD CARRIED ON-GROUND IN-ROOM) WITH OBJECT";
        s.verb = "FILL";
        s.verbId = V_FILL;
        s.actionName = "V-FILL";
        s.actionId = V_FILL;
        s.preactionName = "PRE-FILL";
        s.preactionId = V_FILL;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "FILL";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::CONTBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:215";
        s.patternString = "FILL OBJECT (FIND CONTBIT) (HELD CARRIED ON-GROUND IN-ROOM)";
        s.verb = "FILL";
        s.verbId = V_FILL;
        s.actionName = "V-FILL";
        s.actionId = V_FILL;
        s.preactionName = "PRE-FILL";
        s.preactionId = V_FILL;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "FILL";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::CONTBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:220";
        s.patternString = "FIND OBJECT";
        s.verb = "FIND";
        s.verbId = V_FIND;
        s.actionName = "V-FIND";
        s.actionId = V_FIND;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "FIND";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:223";
        s.patternString = "FOLLOW";
        s.verb = "FOLLOW";
        s.verbId = V_FOLLOW;
        s.actionName = "V-FOLLOW";
        s.actionId = V_FOLLOW;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "FOLLOW";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:223";
        s.patternString = "FOLLOW OBJECT";
        s.verb = "FOLLOW";
        s.verbId = V_FOLLOW;
        s.actionName = "V-FOLLOW";
        s.actionId = V_FOLLOW;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "FOLLOW";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:227";
        s.patternString = "FROBOZZ";
        s.verb = "FROBOZZ";
        s.verbId = V_FROBOZZ;
        s.actionName = "V-FROBOZZ";
        s.actionId = V_FROBOZZ;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "FROBOZZ";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:229";
        s.patternString = "GIVE OBJECT (MANY HELD HAVE) TO OBJECT (FIND ACTORBIT) (ON-GROUND)";
        s.verb = "GIVE";
        s.verbId = V_GIVE;
        s.actionName = "V-GIVE";
        s.actionId = V_GIVE;
        s.preactionName = "PRE-GIVE";
        s.preactionId = V_GIVE;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "GIVE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD;
            e.have = true;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "to";
            e.synonyms = {"to"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:229";
        s.patternString = "GIVE OBJECT (FIND ACTORBIT) (ON-GROUND) OBJECT (MANY HELD HAVE)";
        s.verb = "GIVE";
        s.verbId = V_GIVE;
        s.actionName = "V-SGIVE";
        s.actionId = V_SGIVE;
        s.preactionName = "PRE-SGIVE";
        s.preactionId = V_SGIVE;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "GIVE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD;
            e.have = true;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:237";
        s.patternString = "HATCH OBJECT";
        s.verb = "HATCH";
        s.verbId = V_HATCH;
        s.actionName = "V-HATCH";
        s.actionId = V_HATCH;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "HATCH";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:239";
        s.patternString = "HELLO";
        s.verb = "HELLO";
        s.verbId = V_HELLO;
        s.actionName = "V-HELLO";
        s.actionId = V_HELLO;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "HELLO";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:239";
        s.patternString = "HELLO OBJECT";
        s.verb = "HELLO";
        s.verbId = V_HELLO;
        s.actionName = "V-HELLO";
        s.actionId = V_HELLO;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "HELLO";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:243";
        s.patternString = "INCANT";
        s.verb = "INCANT";
        s.verbId = V_INCANT;
        s.actionName = "V-INCANT";
        s.actionId = V_INCANT;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "INCANT";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:246";
        s.patternString = "INFLAT OBJECT WITH OBJECT (FIND TOOLBIT) (ON-GROUND IN-ROOM HELD CARRIED)";
        s.verb = "INFLAT";
        s.verbId = V_INFLAT;
        s.actionName = "V-INFLATE";
        s.actionId = V_INFLATE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "INFLAT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TOOLBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM | SH_HELD | SH_CARRIED;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:249";
        s.patternString = "JUMP";
        s.verb = "JUMP";
        s.verbId = V_JUMP;
        s.actionName = "V-LEAP";
        s.actionId = V_LEAP;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "JUMP";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:249";
        s.patternString = "JUMP OVER OBJECT";
        s.verb = "JUMP";
        s.verbId = V_JUMP;
        s.actionName = "V-LEAP";
        s.actionId = V_LEAP;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "JUMP";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "over";
            e.synonyms = {"over"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:249";
        s.patternString = "JUMP ACROSS OBJECT";
        s.verb = "JUMP";
        s.verbId = V_JUMP;
        s.actionName = "V-LEAP";
        s.actionId = V_LEAP;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "JUMP";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "across";
            e.synonyms = {"across"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:249";
        s.patternString = "JUMP IN OBJECT";
        s.verb = "JUMP";
        s.verbId = V_JUMP;
        s.actionName = "V-LEAP";
        s.actionId = V_LEAP;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "JUMP";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:249";
        s.patternString = "JUMP FROM OBJECT";
        s.verb = "JUMP";
        s.verbId = V_JUMP;
        s.actionName = "V-LEAP";
        s.actionId = V_LEAP;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "JUMP";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "from";
            e.synonyms = {"from"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:249";
        s.patternString = "JUMP OFF OBJECT";
        s.verb = "JUMP";
        s.verbId = V_JUMP;
        s.actionName = "V-LEAP";
        s.actionId = V_LEAP;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "JUMP";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "off";
            e.synonyms = {"off"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:257";
        s.patternString = "KICK OBJECT";
        s.verb = "KICK";
        s.verbId = V_KICK;
        s.actionName = "V-KICK";
        s.actionId = V_KICK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "KICK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:261";
        s.patternString = "KILL OBJECT (FIND ACTORBIT) (ON-GROUND IN-ROOM) WITH OBJECT (FIND WEAPONBIT) (HELD CARRIED HAVE)";
        s.verb = "KILL";
        s.verbId = V_KILL;
        s.actionName = "V-ATTACK";
        s.actionId = V_ATTACK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "KILL";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::WEAPONBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:268";
        s.patternString = "STAB OBJECT (FIND ACTORBIT) (ON-GROUND IN-ROOM)";
        s.verb = "STAB";
        s.verbId = V_STAB;
        s.actionName = "V-STAB";
        s.actionId = V_STAB;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "STAB";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:268";
        s.patternString = "STAB OBJECT (FIND ACTORBIT) (ON-GROUND IN-ROOM) WITH OBJECT (FIND WEAPONBIT) (HELD CARRIED HAVE)";
        s.verb = "STAB";
        s.verbId = V_STAB;
        s.actionName = "V-ATTACK";
        s.actionId = V_ATTACK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "STAB";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::WEAPONBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:272";
        s.patternString = "KISS OBJECT (FIND ACTORBIT) (ON-GROUND IN-ROOM)";
        s.verb = "KISS";
        s.verbId = V_KISS;
        s.actionName = "V-KISS";
        s.actionId = V_KISS;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "KISS";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:274";
        s.patternString = "KNOCK AT OBJECT";
        s.verb = "KNOCK";
        s.verbId = V_KNOCK;
        s.actionName = "V-KNOCK";
        s.actionId = V_KNOCK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "KNOCK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "at";
            e.synonyms = {"at"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:274";
        s.patternString = "KNOCK ON OBJECT";
        s.verb = "KNOCK";
        s.verbId = V_KNOCK;
        s.actionName = "V-KNOCK";
        s.actionId = V_KNOCK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "KNOCK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:274";
        s.patternString = "KNOCK DOWN OBJECT (FIND ACTORBIT) (ON-GROUND IN-ROOM)";
        s.verb = "KNOCK";
        s.verbId = V_KNOCK;
        s.actionName = "V-ATTACK";
        s.actionId = V_ATTACK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "KNOCK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "down";
            e.synonyms = {"down"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:279";
        s.patternString = "LAUNCH OBJECT (FIND VEHBIT)";
        s.verb = "LAUNCH";
        s.verbId = V_LAUNCH;
        s.actionName = "V-LAUNCH";
        s.actionId = V_LAUNCH;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LAUNCH";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::VEHBIT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:281";
        s.patternString = "LEAN ON OBJECT (HELD HAVE)";
        s.verb = "LEAN";
        s.verbId = V_LEAN;
        s.actionName = "V-LEAN-ON";
        s.actionId = V_LEAN_ON;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LEAN";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:283";
        s.patternString = "LEAVE";
        s.verb = "LEAVE";
        s.verbId = V_LEAVE;
        s.actionName = "V-LEAVE";
        s.actionId = V_LEAVE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LEAVE";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:283";
        s.patternString = "LEAVE OBJECT";
        s.verb = "LEAVE";
        s.verbId = V_LEAVE;
        s.actionName = "V-DROP";
        s.actionId = V_DROP;
        s.preactionName = "PRE-DROP";
        s.preactionId = V_DROP;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LEAVE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:286";
        s.patternString = "LIGHT OBJECT (FIND LIGHTBIT) (HELD CARRIED ON-GROUND IN-ROOM TAKE HAVE)";
        s.verb = "LIGHT";
        s.verbId = V_LIGHT;
        s.actionName = "V-LAMP-ON";
        s.actionId = V_LAMP_ON;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LIGHT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::LIGHTBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.have = true;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:286";
        s.patternString = "LIGHT OBJECT (FIND LIGHTBIT) (HELD CARRIED ON-GROUND IN-ROOM) WITH OBJECT (FIND FLAMEBIT) (HELD CARRIED TAKE HAVE)";
        s.verb = "LIGHT";
        s.verbId = V_LIGHT;
        s.actionName = "V-BURN";
        s.actionId = V_BURN;
        s.preactionName = "PRE-BURN";
        s.preactionId = V_BURN;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LIGHT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::LIGHTBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::FLAMEBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:291";
        s.patternString = "LISTEN TO OBJECT";
        s.verb = "LISTEN";
        s.verbId = V_LISTEN;
        s.actionName = "V-LISTEN";
        s.actionId = V_LISTEN;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LISTEN";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "to";
            e.synonyms = {"to"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:291";
        s.patternString = "LISTEN FOR OBJECT";
        s.verb = "LISTEN";
        s.verbId = V_LISTEN;
        s.actionName = "V-LISTEN";
        s.actionId = V_LISTEN;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LISTEN";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "for";
            e.synonyms = {"for"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:294";
        s.patternString = "LOCK OBJECT (ON-GROUND IN-ROOM) WITH OBJECT (FIND TOOLBIT) (HELD CARRIED ON-GROUND IN-ROOM TAKE)";
        s.verb = "LOCK";
        s.verbId = V_LOCK;
        s.actionName = "V-LOCK";
        s.actionId = V_LOCK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LOCK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TOOLBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:297";
        s.patternString = "LOOK";
        s.verb = "LOOK";
        s.verbId = V_LOOK;
        s.actionName = "V-LOOK";
        s.actionId = V_LOOK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LOOK";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:297";
        s.patternString = "LOOK AROUND OBJECT (FIND RMUNGBIT)";
        s.verb = "LOOK";
        s.verbId = V_LOOK;
        s.actionName = "V-LOOK";
        s.actionId = V_LOOK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LOOK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "around";
            e.synonyms = {"around"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::RMUNGBIT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:297";
        s.patternString = "LOOK UP OBJECT (FIND RMUNGBIT)";
        s.verb = "LOOK";
        s.verbId = V_LOOK;
        s.actionName = "V-LOOK";
        s.actionId = V_LOOK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LOOK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::RMUNGBIT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:297";
        s.patternString = "LOOK DOWN OBJECT (FIND RMUNGBIT)";
        s.verb = "LOOK";
        s.verbId = V_LOOK;
        s.actionName = "V-LOOK";
        s.actionId = V_LOOK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LOOK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "down";
            e.synonyms = {"down"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::RMUNGBIT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:297";
        s.patternString = "LOOK AT OBJECT (HELD CARRIED ON-GROUND IN-ROOM MANY)";
        s.verb = "LOOK";
        s.verbId = V_LOOK;
        s.actionName = "V-EXAMINE";
        s.actionId = V_EXAMINE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LOOK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "at";
            e.synonyms = {"at"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:298";
        s.patternString = "LOOK ON OBJECT";
        s.verb = "LOOK";
        s.verbId = V_LOOK;
        s.actionName = "V-LOOK-ON";
        s.actionId = V_LOOK_ON;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LOOK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:299";
        s.patternString = "LOOK WITH OBJECT (HELD CARRIED ON-GROUND IN-ROOM MANY)";
        s.verb = "LOOK";
        s.verbId = V_LOOK;
        s.actionName = "V-LOOK-INSIDE";
        s.actionId = V_LOOK_INSIDE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LOOK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:300";
        s.patternString = "LOOK UNDER OBJECT";
        s.verb = "LOOK";
        s.verbId = V_LOOK;
        s.actionName = "V-LOOK-UNDER";
        s.actionId = V_LOOK_UNDER;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LOOK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "under";
            e.synonyms = {"under", "underneath", "beneath", "below"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:301";
        s.patternString = "LOOK BEHIND OBJECT";
        s.verb = "LOOK";
        s.verbId = V_LOOK;
        s.actionName = "V-LOOK-BEHIND";
        s.actionId = V_LOOK_BEHIND;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LOOK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "behind";
            e.synonyms = {"behind"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:299";
        s.patternString = "LOOK IN OBJECT (HELD CARRIED ON-GROUND IN-ROOM MANY)";
        s.verb = "LOOK";
        s.verbId = V_LOOK;
        s.actionName = "V-LOOK-INSIDE";
        s.actionId = V_LOOK_INSIDE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LOOK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:304";
        s.patternString = "LOOK AT OBJECT (HELD CARRIED ON-GROUND IN-ROOM) WITH OBJECT";
        s.verb = "LOOK";
        s.verbId = V_LOOK;
        s.actionName = "V-READ";
        s.actionId = V_READ;
        s.preactionName = "PRE-READ";
        s.preactionId = V_READ;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LOOK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "at";
            e.synonyms = {"at"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:305";
        s.patternString = "LOOK FOR OBJECT";
        s.verb = "LOOK";
        s.verbId = V_LOOK;
        s.actionName = "V-FIND";
        s.actionId = V_FIND;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LOOK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "for";
            e.synonyms = {"for"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:312";
        s.patternString = "LOWER OBJECT";
        s.verb = "LOWER";
        s.verbId = V_LOWER;
        s.actionName = "V-LOWER";
        s.actionId = V_LOWER;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LOWER";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:314";
        s.patternString = "LUBRICATE OBJECT WITH OBJECT (HELD CARRIED)";
        s.verb = "LUBRICATE";
        s.verbId = V_LUBRICATE;
        s.actionName = "V-OIL";
        s.actionId = V_OIL;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "LUBRICATE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:317";
        s.patternString = "MAKE OBJECT";
        s.verb = "MAKE";
        s.verbId = V_MAKE;
        s.actionName = "V-MAKE";
        s.actionId = V_MAKE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "MAKE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:319";
        s.patternString = "MELT OBJECT WITH OBJECT (FIND FLAMEBIT) (HELD CARRIED ON-GROUND IN-ROOM)";
        s.verb = "MELT";
        s.verbId = V_MELT;
        s.actionName = "V-MELT";
        s.actionId = V_MELT;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "MELT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::FLAMEBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:323";
        s.patternString = "MOVE OBJECT (ON-GROUND IN-ROOM)";
        s.verb = "MOVE";
        s.verbId = V_MOVE;
        s.actionName = "V-MOVE";
        s.actionId = V_MOVE;
        s.preactionName = "PRE-MOVE";
        s.preactionId = V_MOVE;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "MOVE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:323";
        s.patternString = "MOVE OBJECT (ON-GROUND IN-ROOM) OBJECT";
        s.verb = "MOVE";
        s.verbId = V_MOVE;
        s.actionName = "V-PUSH-TO";
        s.actionId = V_PUSH_TO;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "MOVE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:323";
        s.patternString = "MOVE OBJECT (ON-GROUND IN-ROOM) TO OBJECT";
        s.verb = "MOVE";
        s.verbId = V_MOVE;
        s.actionName = "V-PUSH-TO";
        s.actionId = V_PUSH_TO;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "MOVE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "to";
            e.synonyms = {"to"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:323";
        s.patternString = "MOVE OBJECT (HELD MANY HAVE) IN OBJECT";
        s.verb = "MOVE";
        s.verbId = V_MOVE;
        s.actionName = "V-PUT";
        s.actionId = V_PUT;
        s.preactionName = "PRE-PUT";
        s.preactionId = V_PUT;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "MOVE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD;
            e.have = true;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:323";
        s.patternString = "MOVE OBJECT WITH OBJECT (FIND TOOLBIT)";
        s.verb = "MOVE";
        s.verbId = V_MOVE;
        s.actionName = "V-TURN";
        s.actionId = V_TURN;
        s.preactionName = "PRE-TURN";
        s.preactionId = V_TURN;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "MOVE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TOOLBIT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:328";
        s.patternString = "ROLL UP OBJECT (ON-GROUND IN-ROOM)";
        s.verb = "ROLL";
        s.verbId = V_ROLL;
        s.actionName = "V-MOVE";
        s.actionId = V_MOVE;
        s.preactionName = "PRE-MOVE";
        s.preactionId = V_MOVE;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "ROLL";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:328";
        s.patternString = "ROLL OBJECT (ON-GROUND IN-ROOM)";
        s.verb = "ROLL";
        s.verbId = V_ROLL;
        s.actionName = "V-MOVE";
        s.actionId = V_MOVE;
        s.preactionName = "PRE-MOVE";
        s.preactionId = V_MOVE;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "ROLL";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:331";
        s.patternString = "MUMBLE";
        s.verb = "MUMBLE";
        s.verbId = V_MUMBLE;
        s.actionName = "V-MUMBLE";
        s.actionId = V_MUMBLE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "MUMBLE";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:334";
        s.patternString = "ODYSSEUS";
        s.verb = "ODYSSEUS";
        s.verbId = V_ODYSSEUS;
        s.actionName = "V-ODYSSEUS";
        s.actionId = V_ODYSSEUS;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "ODYSSEUS";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:337";
        s.patternString = "OPEN OBJECT (FIND DOORBIT) (HELD CARRIED ON-GROUND IN-ROOM)";
        s.verb = "OPEN";
        s.verbId = V_OPEN;
        s.actionName = "V-OPEN";
        s.actionId = V_OPEN;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "OPEN";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::DOORBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:337";
        s.patternString = "OPEN UP OBJECT (FIND DOORBIT) (HELD CARRIED ON-GROUND IN-ROOM)";
        s.verb = "OPEN";
        s.verbId = V_OPEN;
        s.actionName = "V-OPEN";
        s.actionId = V_OPEN;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "OPEN";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::DOORBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:337";
        s.patternString = "OPEN OBJECT (FIND DOORBIT) (HELD CARRIED ON-GROUND IN-ROOM) WITH OBJECT (FIND TOOLBIT) (ON-GROUND IN-ROOM HELD CARRIED HAVE)";
        s.verb = "OPEN";
        s.verbId = V_OPEN;
        s.actionName = "V-OPEN";
        s.actionId = V_OPEN;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "OPEN";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::DOORBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TOOLBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM | SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:343";
        s.patternString = "PICK OBJECT";
        s.verb = "PICK";
        s.verbId = V_PICK;
        s.actionName = "V-PICK";
        s.actionId = V_PICK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PICK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:343";
        s.patternString = "PICK OBJECT WITH OBJECT";
        s.verb = "PICK";
        s.verbId = V_PICK;
        s.actionName = "V-PICK";
        s.actionId = V_PICK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PICK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:343";
        s.patternString = "PICK UP OBJECT (FIND TAKEBIT) (ON-GROUND MANY)";
        s.verb = "PICK";
        s.verbId = V_PICK;
        s.actionName = "V-TAKE";
        s.actionId = V_TAKE;
        s.preactionName = "PRE-TAKE";
        s.preactionId = V_TAKE;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PICK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TAKEBIT;
            e.scopeFlags = SH_ON_GROUND;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:347";
        s.patternString = "PLAY OBJECT";
        s.verb = "PLAY";
        s.verbId = V_PLAY;
        s.actionName = "V-PLAY";
        s.actionId = V_PLAY;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PLAY";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:349";
        s.patternString = "PLUG OBJECT WITH OBJECT";
        s.verb = "PLUG";
        s.verbId = V_PLUG;
        s.actionName = "V-PLUG";
        s.actionId = V_PLUG;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PLUG";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:352";
        s.patternString = "PLUGH";
        s.verb = "PLUGH";
        s.verbId = V_PLUGH;
        s.actionName = "V-ADVENT";
        s.actionId = V_PLUGH;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PLUGH";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:355";
        s.patternString = "POKE OBJECT (FIND ACTORBIT) (ON-GROUND IN-ROOM) WITH OBJECT (FIND WEAPONBIT) (HELD CARRIED HAVE)";
        s.verb = "POKE";
        s.verbId = V_POKE;
        s.actionName = "V-MUNG";
        s.actionId = V_MUNG;
        s.preactionName = "PRE-MUNG";
        s.preactionId = V_MUNG;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "POKE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::WEAPONBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:357";
        s.patternString = "PUNCTURE OBJECT (FIND ACTORBIT) (ON-GROUND IN-ROOM) WITH OBJECT (FIND WEAPONBIT) (HELD CARRIED HAVE)";
        s.verb = "PUNCTURE";
        s.verbId = V_PUNCTURE;
        s.actionName = "V-MUNG";
        s.actionId = V_MUNG;
        s.preactionName = "PRE-MUNG";
        s.preactionId = V_MUNG;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PUNCTURE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::WEAPONBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:360";
        s.patternString = "POUR OBJECT (HELD CARRIED)";
        s.verb = "POUR";
        s.verbId = V_POUR;
        s.actionName = "V-DROP";
        s.actionId = V_DROP;
        s.preactionName = "PRE-DROP";
        s.preactionId = V_DROP;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "POUR";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:360";
        s.patternString = "POUR OBJECT (HELD CARRIED) IN OBJECT";
        s.verb = "POUR";
        s.verbId = V_POUR;
        s.actionName = "V-DROP";
        s.actionId = V_DROP;
        s.preactionName = "PRE-DROP";
        s.preactionId = V_DROP;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "POUR";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:360";
        s.patternString = "POUR OBJECT (HELD CARRIED) ON OBJECT";
        s.verb = "POUR";
        s.verbId = V_POUR;
        s.actionName = "V-POUR-ON";
        s.actionId = V_POUR_ON;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "POUR";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:360";
        s.patternString = "POUR OBJECT (HELD CARRIED) FROM OBJECT";
        s.verb = "POUR";
        s.verbId = V_POUR;
        s.actionName = "V-DROP";
        s.actionId = V_DROP;
        s.preactionName = "PRE-DROP";
        s.preactionId = V_DROP;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "POUR";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "from";
            e.synonyms = {"from"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:366";
        s.patternString = "PRAY";
        s.verb = "PRAY";
        s.verbId = V_PRAY;
        s.actionName = "V-PRAY";
        s.actionId = V_PRAY;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PRAY";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:368";
        s.patternString = "PULL OBJECT (ON-GROUND IN-ROOM)";
        s.verb = "PULL";
        s.verbId = V_PULL;
        s.actionName = "V-MOVE";
        s.actionId = V_MOVE;
        s.preactionName = "PRE-MOVE";
        s.preactionId = V_MOVE;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PULL";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:368";
        s.patternString = "PULL ON OBJECT (ON-GROUND IN-ROOM)";
        s.verb = "PULL";
        s.verbId = V_PULL;
        s.actionName = "V-MOVE";
        s.actionId = V_MOVE;
        s.preactionName = "PRE-MOVE";
        s.preactionId = V_MOVE;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PULL";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:368";
        s.patternString = "PULL UP OBJECT (ON-GROUND IN-ROOM)";
        s.verb = "PULL";
        s.verbId = V_PULL;
        s.actionName = "V-MOVE";
        s.actionId = V_MOVE;
        s.preactionName = "PRE-MOVE";
        s.preactionId = V_MOVE;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PULL";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:373";
        s.patternString = "PUMP UP OBJECT";
        s.verb = "PUMP";
        s.verbId = V_PUMP;
        s.actionName = "V-PUMP";
        s.actionId = V_PUMP;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PUMP";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:373";
        s.patternString = "PUMP UP OBJECT WITH OBJECT";
        s.verb = "PUMP";
        s.verbId = V_PUMP;
        s.actionName = "V-PUMP";
        s.actionId = V_PUMP;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PUMP";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:376";
        s.patternString = "PUSH OBJECT (IN-ROOM ON-GROUND) OBJECT";
        s.verb = "PUSH";
        s.verbId = V_PUSH;
        s.actionName = "V-PUSH-TO";
        s.actionId = V_PUSH_TO;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PUSH";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_IN_ROOM | SH_ON_GROUND;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:376";
        s.patternString = "PUSH OBJECT (IN-ROOM ON-GROUND) TO OBJECT";
        s.verb = "PUSH";
        s.verbId = V_PUSH;
        s.actionName = "V-PUSH-TO";
        s.actionId = V_PUSH_TO;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PUSH";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_IN_ROOM | SH_ON_GROUND;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "to";
            e.synonyms = {"to"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:376";
        s.patternString = "PUSH OBJECT (IN-ROOM ON-GROUND MANY)";
        s.verb = "PUSH";
        s.verbId = V_PUSH;
        s.actionName = "V-PUSH";
        s.actionId = V_PUSH;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PUSH";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_IN_ROOM | SH_ON_GROUND;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:376";
        s.patternString = "PUSH ON OBJECT (IN-ROOM ON-GROUND MANY)";
        s.verb = "PUSH";
        s.verbId = V_PUSH;
        s.actionName = "V-PUSH";
        s.actionId = V_PUSH;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PUSH";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_IN_ROOM | SH_ON_GROUND;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:376";
        s.patternString = "PUSH OBJECT WITH OBJECT (FIND TOOLBIT)";
        s.verb = "PUSH";
        s.verbId = V_PUSH;
        s.actionName = "V-TURN";
        s.actionId = V_TURN;
        s.preactionName = "PRE-TURN";
        s.preactionId = V_TURN;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PUSH";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TOOLBIT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:377";
        s.patternString = "PUSH OBJECT UNDER OBJECT";
        s.verb = "PUSH";
        s.verbId = V_PUSH;
        s.actionName = "V-PUT-UNDER";
        s.actionId = V_PUT_UNDER;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PUSH";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "under";
            e.synonyms = {"under", "underneath", "beneath", "below"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:384";
        s.patternString = "PUT OBJECT (HELD MANY HAVE) IN OBJECT";
        s.verb = "PUT";
        s.verbId = V_PUT;
        s.actionName = "V-PUT";
        s.actionId = V_PUT;
        s.preactionName = "PRE-PUT";
        s.preactionId = V_PUT;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PUT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD;
            e.have = true;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:384";
        s.patternString = "PUT OBJECT (HELD MANY HAVE) ON OBJECT";
        s.verb = "PUT";
        s.verbId = V_PUT;
        s.actionName = "V-PUT-ON";
        s.actionId = V_PUT_ON;
        s.preactionName = "PRE-PUT";
        s.preactionId = V_PUT;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PUT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD;
            e.have = true;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:384";
        s.patternString = "PUT DOWN OBJECT (HELD MANY)";
        s.verb = "PUT";
        s.verbId = V_PUT;
        s.actionName = "V-DROP";
        s.actionId = V_DROP;
        s.preactionName = "PRE-DROP";
        s.preactionId = V_DROP;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PUT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "down";
            e.synonyms = {"down"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:384";
        s.patternString = "PUT OBJECT (HELD HAVE) UNDER OBJECT";
        s.verb = "PUT";
        s.verbId = V_PUT;
        s.actionName = "V-PUT-UNDER";
        s.actionId = V_PUT_UNDER;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PUT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "under";
            e.synonyms = {"under", "underneath", "beneath", "below"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:385";
        s.patternString = "PUT OUT OBJECT (FIND ONBIT) (HELD CARRIED ON-GROUND IN-ROOM TAKE HAVE)";
        s.verb = "PUT";
        s.verbId = V_PUT;
        s.actionName = "V-LAMP-OFF";
        s.actionId = V_LAMP_OFF;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PUT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "out";
            e.synonyms = {"out"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ONBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.have = true;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:386";
        s.patternString = "PUT ON OBJECT (IN-ROOM ON-GROUND CARRIED MANY)";
        s.verb = "PUT";
        s.verbId = V_PUT;
        s.actionName = "V-WEAR";
        s.actionId = V_WEAR;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PUT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_IN_ROOM | SH_ON_GROUND | SH_CARRIED;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:387";
        s.patternString = "PUT OBJECT (HELD MANY HAVE) BEHIND OBJECT";
        s.verb = "PUT";
        s.verbId = V_PUT;
        s.actionName = "V-PUT-BEHIND";
        s.actionId = V_PUT_BEHIND;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "PUT";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD;
            e.have = true;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "behind";
            e.synonyms = {"behind"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:394";
        s.patternString = "RAISE OBJECT";
        s.verb = "RAISE";
        s.verbId = V_RAISE;
        s.actionName = "V-RAISE";
        s.actionId = V_RAISE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "RAISE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:394";
        s.patternString = "RAISE UP OBJECT";
        s.verb = "RAISE";
        s.verbId = V_RAISE;
        s.actionName = "V-RAISE";
        s.actionId = V_RAISE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "RAISE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:398";
        s.patternString = "RAPE OBJECT (FIND ACTORBIT)";
        s.verb = "RAPE";
        s.verbId = V_RAPE;
        s.actionName = "V-RAPE";
        s.actionId = V_RAPE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "RAPE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:401";
        s.patternString = "READ OBJECT (FIND READBIT) (HELD CARRIED ON-GROUND IN-ROOM TAKE)";
        s.verb = "READ";
        s.verbId = V_READ;
        s.actionName = "V-READ";
        s.actionId = V_READ;
        s.preactionName = "PRE-READ";
        s.preactionId = V_READ;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "READ";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::READBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:401";
        s.patternString = "READ FROM OBJECT (FIND READBIT) (HELD CARRIED ON-GROUND IN-ROOM TAKE)";
        s.verb = "READ";
        s.verbId = V_READ;
        s.actionName = "V-READ";
        s.actionId = V_READ;
        s.preactionName = "PRE-READ";
        s.preactionId = V_READ;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "READ";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "from";
            e.synonyms = {"from"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::READBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:401";
        s.patternString = "READ OBJECT (FIND READBIT) (HELD CARRIED ON-GROUND IN-ROOM TAKE) WITH OBJECT";
        s.verb = "READ";
        s.verbId = V_READ;
        s.actionName = "V-READ";
        s.actionId = V_READ;
        s.preactionName = "PRE-READ";
        s.preactionId = V_READ;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "READ";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::READBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:405";
        s.patternString = "READ OBJECT (FIND READBIT) (HELD CARRIED ON-GROUND IN-ROOM TAKE) OBJECT";
        s.verb = "READ";
        s.verbId = V_READ;
        s.actionName = "V-READ-PAGE";
        s.actionId = V_READ_PAGE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "READ";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::READBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:411";
        s.patternString = "REPENT";
        s.verb = "REPENT";
        s.verbId = V_REPENT;
        s.actionName = "V-REPENT";
        s.actionId = V_REPENT;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "REPENT";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:413";
        s.patternString = "RING OBJECT (TAKE)";
        s.verb = "RING";
        s.verbId = V_RING;
        s.actionName = "V-RING";
        s.actionId = V_RING;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "RING";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:413";
        s.patternString = "RING OBJECT (TAKE) WITH OBJECT";
        s.verb = "RING";
        s.verbId = V_RING;
        s.actionName = "V-RING";
        s.actionId = V_RING;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "RING";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:417";
        s.patternString = "RUB OBJECT";
        s.verb = "RUB";
        s.verbId = V_RUB;
        s.actionName = "V-RUB";
        s.actionId = V_RUB;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "RUB";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:417";
        s.patternString = "RUB OBJECT WITH OBJECT";
        s.verb = "RUB";
        s.verbId = V_RUB;
        s.actionName = "V-RUB";
        s.actionId = V_RUB;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "RUB";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:421";
        s.patternString = "TALK TO OBJECT (FIND ACTORBIT) (IN-ROOM)";
        s.verb = "TALK";
        s.verbId = V_TALK;
        s.actionName = "V-TELL";
        s.actionId = V_TELL;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TALK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "to";
            e.synonyms = {"to"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:422";
        s.patternString = "SAY";
        s.verb = "SAY";
        s.verbId = V_SAY;
        s.actionName = "V-SAY";
        s.actionId = V_SAY;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SAY";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:424";
        s.patternString = "SEARCH OBJECT";
        s.verb = "SEARCH";
        s.verbId = V_SEARCH;
        s.actionName = "V-SEARCH";
        s.actionId = V_SEARCH;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SEARCH";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:424";
        s.patternString = "SEARCH IN OBJECT";
        s.verb = "SEARCH";
        s.verbId = V_SEARCH;
        s.actionName = "V-SEARCH";
        s.actionId = V_SEARCH;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SEARCH";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:424";
        s.patternString = "SEARCH FOR OBJECT";
        s.verb = "SEARCH";
        s.verbId = V_SEARCH;
        s.actionName = "V-FIND";
        s.actionId = V_FIND;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SEARCH";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "for";
            e.synonyms = {"for"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:428";
        s.patternString = "SEND FOR OBJECT";
        s.verb = "SEND";
        s.verbId = V_SEND;
        s.actionName = "V-SEND";
        s.actionId = V_SEND;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SEND";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "for";
            e.synonyms = {"for"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:430";
        s.patternString = "SHAKE OBJECT (HAVE)";
        s.verb = "SHAKE";
        s.verbId = V_SHAKE;
        s.actionName = "V-SHAKE";
        s.actionId = V_SHAKE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SHAKE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:432";
        s.patternString = "SKIP";
        s.verb = "SKIP";
        s.verbId = V_SKIP;
        s.actionName = "V-SKIP";
        s.actionId = V_SKIP;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SKIP";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:435";
        s.patternString = "SLIDE OBJECT UNDER OBJECT";
        s.verb = "SLIDE";
        s.verbId = V_SLIDE;
        s.actionName = "V-PUT-UNDER";
        s.actionId = V_PUT_UNDER;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SLIDE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "under";
            e.synonyms = {"under", "underneath", "beneath", "below"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:435";
        s.patternString = "SLIDE OBJECT (IN-ROOM ON-GROUND) OBJECT";
        s.verb = "SLIDE";
        s.verbId = V_SLIDE;
        s.actionName = "V-PUSH-TO";
        s.actionId = V_PUSH_TO;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SLIDE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_IN_ROOM | SH_ON_GROUND;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:435";
        s.patternString = "SLIDE OBJECT (IN-ROOM ON-GROUND) TO OBJECT";
        s.verb = "SLIDE";
        s.verbId = V_SLIDE;
        s.actionName = "V-PUSH-TO";
        s.actionId = V_PUSH_TO;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SLIDE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_IN_ROOM | SH_ON_GROUND;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "to";
            e.synonyms = {"to"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:439";
        s.patternString = "SMELL OBJECT";
        s.verb = "SMELL";
        s.verbId = V_SMELL;
        s.actionName = "V-SMELL";
        s.actionId = V_SMELL;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SMELL";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:442";
        s.patternString = "SPIN OBJECT";
        s.verb = "SPIN";
        s.verbId = V_SPIN;
        s.actionName = "V-SPIN";
        s.actionId = V_SPIN;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SPIN";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:444";
        s.patternString = "SPRAY OBJECT ON OBJECT";
        s.verb = "SPRAY";
        s.verbId = V_SPRAY;
        s.actionName = "V-SPRAY";
        s.actionId = V_SPRAY;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SPRAY";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:444";
        s.patternString = "SPRAY OBJECT WITH OBJECT";
        s.verb = "SPRAY";
        s.verbId = V_SPRAY;
        s.actionName = "V-SSPRAY";
        s.actionId = V_SSPRAY;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SPRAY";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:447";
        s.patternString = "SQUEEZE OBJECT";
        s.verb = "SQUEEZE";
        s.verbId = V_SQUEEZE;
        s.actionName = "V-SQUEEZE";
        s.actionId = V_SQUEEZE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SQUEEZE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:447";
        s.patternString = "SQUEEZE OBJECT ON OBJECT";
        s.verb = "SQUEEZE";
        s.verbId = V_SQUEEZE;
        s.actionName = "V-PUT";
        s.actionId = V_PUT;
        s.preactionName = "PRE-PUT";
        s.preactionId = V_PUT;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SQUEEZE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:450";
        s.patternString = "STAND";
        s.verb = "STAND";
        s.verbId = V_STAND;
        s.actionName = "V-STAND";
        s.actionId = V_STAND;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "STAND";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:450";
        s.patternString = "STAND UP OBJECT (FIND RMUNGBIT)";
        s.verb = "STAND";
        s.verbId = V_STAND;
        s.actionName = "V-STAND";
        s.actionId = V_STAND;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "STAND";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::RMUNGBIT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:453";
        s.patternString = "STAY";
        s.verb = "STAY";
        s.verbId = V_STAY;
        s.actionName = "V-STAY";
        s.actionId = V_STAY;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "STAY";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:455";
        s.patternString = "STRIKE OBJECT (FIND ACTORBIT) (ON-GROUND IN-ROOM) WITH OBJECT (FIND WEAPONBIT) (HELD CARRIED ON-GROUND IN-ROOM HAVE)";
        s.verb = "STRIKE";
        s.verbId = V_STRIKE;
        s.actionName = "V-ATTACK";
        s.actionId = V_ATTACK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "STRIKE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::WEAPONBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:455";
        s.patternString = "STRIKE OBJECT (ON-GROUND IN-ROOM HELD CARRIED)";
        s.verb = "STRIKE";
        s.verbId = V_STRIKE;
        s.actionName = "V-STRIKE";
        s.actionId = V_STRIKE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "STRIKE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM | SH_HELD | SH_CARRIED;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:459";
        s.patternString = "SWIM";
        s.verb = "SWIM";
        s.verbId = V_SWIM;
        s.actionName = "V-SWIM";
        s.actionId = V_SWIM;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SWIM";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:459";
        s.patternString = "SWIM IN OBJECT";
        s.verb = "SWIM";
        s.verbId = V_SWIM;
        s.actionName = "V-SWIM";
        s.actionId = V_SWIM;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SWIM";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:459";
        s.patternString = "SWIM ACROSS OBJECT";
        s.verb = "SWIM";
        s.verbId = V_SWIM;
        s.actionName = "V-SWIM";
        s.actionId = V_SWIM;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SWIM";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "across";
            e.synonyms = {"across"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:464";
        s.patternString = "SWING OBJECT (FIND WEAPONBIT) (HELD CARRIED HAVE)";
        s.verb = "SWING";
        s.verbId = V_SWING;
        s.actionName = "V-SWING";
        s.actionId = V_SWING;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SWING";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::WEAPONBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:464";
        s.patternString = "SWING OBJECT (FIND WEAPONBIT) (HELD CARRIED HAVE) AT OBJECT (FIND ACTORBIT) (ON-GROUND IN-ROOM)";
        s.verb = "SWING";
        s.verbId = V_SWING;
        s.actionName = "V-SWING";
        s.actionId = V_SWING;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "SWING";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::WEAPONBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "at";
            e.synonyms = {"at"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:469";
        s.patternString = "TAKE OBJECT (FIND TAKEBIT) (ON-GROUND IN-ROOM MANY)";
        s.verb = "TAKE";
        s.verbId = V_TAKE;
        s.actionName = "V-TAKE";
        s.actionId = V_TAKE;
        s.preactionName = "PRE-TAKE";
        s.preactionId = V_TAKE;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TAKE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TAKEBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:469";
        s.patternString = "TAKE IN OBJECT (FIND VEHBIT) (ON-GROUND IN-ROOM)";
        s.verb = "TAKE";
        s.verbId = V_TAKE;
        s.actionName = "V-BOARD";
        s.actionId = V_BOARD;
        s.preactionName = "PRE-BOARD";
        s.preactionId = V_BOARD;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TAKE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::VEHBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:469";
        s.patternString = "TAKE OUT OBJECT (FIND RMUNGBIT) (ON-GROUND IN-ROOM)";
        s.verb = "TAKE";
        s.verbId = V_TAKE;
        s.actionName = "V-DISEMBARK";
        s.actionId = V_DISEMBARK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TAKE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "out";
            e.synonyms = {"out"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::RMUNGBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:469";
        s.patternString = "TAKE ON OBJECT (FIND VEHBIT) (ON-GROUND IN-ROOM)";
        s.verb = "TAKE";
        s.verbId = V_TAKE;
        s.actionName = "V-CLIMB-ON";
        s.actionId = V_CLIMB_ON;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TAKE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::VEHBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:469";
        s.patternString = "TAKE UP OBJECT (FIND RMUNGBIT)";
        s.verb = "TAKE";
        s.verbId = V_TAKE;
        s.actionName = "V-STAND";
        s.actionId = V_STAND;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TAKE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::RMUNGBIT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:469";
        s.patternString = "TAKE OBJECT (FIND TAKEBIT) (CARRIED IN-ROOM MANY) OUT OBJECT";
        s.verb = "TAKE";
        s.verbId = V_TAKE;
        s.actionName = "V-TAKE";
        s.actionId = V_TAKE;
        s.preactionName = "PRE-TAKE";
        s.preactionId = V_TAKE;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TAKE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TAKEBIT;
            e.scopeFlags = SH_CARRIED | SH_IN_ROOM;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "out";
            e.synonyms = {"out"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:469";
        s.patternString = "TAKE OBJECT (FIND TAKEBIT) (CARRIED IN-ROOM MANY) OFF OBJECT";
        s.verb = "TAKE";
        s.verbId = V_TAKE;
        s.actionName = "V-TAKE";
        s.actionId = V_TAKE;
        s.preactionName = "PRE-TAKE";
        s.preactionId = V_TAKE;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TAKE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TAKEBIT;
            e.scopeFlags = SH_CARRIED | SH_IN_ROOM;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "off";
            e.synonyms = {"off"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:469";
        s.patternString = "TAKE OBJECT (FIND TAKEBIT) (IN-ROOM CARRIED MANY) FROM OBJECT";
        s.verb = "TAKE";
        s.verbId = V_TAKE;
        s.actionName = "V-TAKE";
        s.actionId = V_TAKE;
        s.preactionName = "PRE-TAKE";
        s.preactionId = V_TAKE;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TAKE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TAKEBIT;
            e.scopeFlags = SH_IN_ROOM | SH_CARRIED;
            e.many = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "from";
            e.synonyms = {"from"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:482";
        s.patternString = "TELL OBJECT (FIND ACTORBIT) (IN-ROOM)";
        s.verb = "TELL";
        s.verbId = V_TELL;
        s.actionName = "V-TELL";
        s.actionId = V_TELL;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TELL";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:482";
        s.patternString = "TELL OBJECT (FIND ACTORBIT) (IN-ROOM) ABOUT OBJECT";
        s.verb = "TELL";
        s.verbId = V_TELL;
        s.actionName = "V-TELL";
        s.actionId = V_TELL;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TELL";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "about";
            e.synonyms = {"about"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:486";
        s.patternString = "THROW OBJECT (HELD CARRIED HAVE) AT OBJECT (FIND ACTORBIT) (ON-GROUND IN-ROOM)";
        s.verb = "THROW";
        s.verbId = V_THROW;
        s.actionName = "V-THROW";
        s.actionId = V_THROW;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "THROW";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "at";
            e.synonyms = {"at"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:486";
        s.patternString = "THROW OBJECT (HELD CARRIED HAVE) WITH OBJECT (FIND ACTORBIT) (ON-GROUND IN-ROOM)";
        s.verb = "THROW";
        s.verbId = V_THROW;
        s.actionName = "V-THROW";
        s.actionId = V_THROW;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "THROW";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:486";
        s.patternString = "THROW OBJECT OBJECT";
        s.verb = "THROW";
        s.verbId = V_THROW;
        s.actionName = "V-OVERBOARD";
        s.actionId = V_OVERBOARD;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "THROW";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:488";
        s.patternString = "THROW OBJECT (HELD CARRIED HAVE) IN OBJECT";
        s.verb = "THROW";
        s.verbId = V_THROW;
        s.actionName = "V-PUT";
        s.actionId = V_PUT;
        s.preactionName = "PRE-PUT";
        s.preactionId = V_PUT;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "THROW";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:488";
        s.patternString = "THROW OBJECT (HELD CARRIED HAVE) ON OBJECT";
        s.verb = "THROW";
        s.verbId = V_THROW;
        s.actionName = "V-PUT-ON";
        s.actionId = V_PUT_ON;
        s.preactionName = "PRE-PUT";
        s.preactionId = V_PUT;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "THROW";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:490";
        s.patternString = "THROW OBJECT (HELD CARRIED HAVE) OFF OBJECT";
        s.verb = "THROW";
        s.verbId = V_THROW;
        s.actionName = "V-THROW-OFF";
        s.actionId = V_THROW_OFF;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "THROW";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "off";
            e.synonyms = {"off"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:490";
        s.patternString = "THROW OBJECT (HELD CARRIED HAVE) OVER OBJECT";
        s.verb = "THROW";
        s.verbId = V_THROW;
        s.actionName = "V-THROW-OFF";
        s.actionId = V_THROW_OFF;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "THROW";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "over";
            e.synonyms = {"over"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:497";
        s.patternString = "TIE OBJECT TO OBJECT";
        s.verb = "TIE";
        s.verbId = V_TIE;
        s.actionName = "V-TIE";
        s.actionId = V_TIE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TIE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "to";
            e.synonyms = {"to"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:497";
        s.patternString = "TIE UP OBJECT (FIND ACTORBIT) (ON-GROUND IN-ROOM) WITH OBJECT (FIND TOOLBIT) (ON-GROUND IN-ROOM HELD CARRIED HAVE)";
        s.verb = "TIE";
        s.verbId = V_TIE;
        s.actionName = "V-TIE-UP";
        s.actionId = V_TIE_UP;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TIE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TOOLBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM | SH_HELD | SH_CARRIED;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:502";
        s.patternString = "TREASURE";
        s.verb = "TREASURE";
        s.verbId = V_TREASURE;
        s.actionName = "V-TREASURE";
        s.actionId = V_TREASURE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TREASURE";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:505";
        s.patternString = "TURN OBJECT (FIND TURNBIT) (HELD CARRIED ON-GROUND IN-ROOM) WITH OBJECT (FIND RMUNGBIT)";
        s.verb = "TURN";
        s.verbId = V_TURN;
        s.actionName = "V-TURN";
        s.actionId = V_TURN;
        s.preactionName = "PRE-TURN";
        s.preactionId = V_TURN;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TURN";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TURNBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::RMUNGBIT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:505";
        s.patternString = "TURN ON OBJECT (FIND LIGHTBIT) (HELD CARRIED ON-GROUND IN-ROOM)";
        s.verb = "TURN";
        s.verbId = V_TURN;
        s.actionName = "V-LAMP-ON";
        s.actionId = V_LAMP_ON;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TURN";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::LIGHTBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:505";
        s.patternString = "TURN ON OBJECT WITH OBJECT (HAVE)";
        s.verb = "TURN";
        s.verbId = V_TURN;
        s.actionName = "V-LAMP-ON";
        s.actionId = V_LAMP_ON;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TURN";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.have = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:507";
        s.patternString = "TURN OFF OBJECT (FIND ONBIT) (HELD CARRIED ON-GROUND IN-ROOM TAKE HAVE)";
        s.verb = "TURN";
        s.verbId = V_TURN;
        s.actionName = "V-LAMP-OFF";
        s.actionId = V_LAMP_OFF;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TURN";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "off";
            e.synonyms = {"off"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ONBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.have = true;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:505";
        s.patternString = "TURN OBJECT (FIND TURNBIT) TO OBJECT";
        s.verb = "TURN";
        s.verbId = V_TURN;
        s.actionName = "V-TURN";
        s.actionId = V_TURN;
        s.preactionName = "PRE-TURN";
        s.preactionId = V_TURN;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TURN";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TURNBIT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "to";
            e.synonyms = {"to"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:505";
        s.patternString = "TURN OBJECT (FIND TURNBIT) FOR OBJECT";
        s.verb = "TURN";
        s.verbId = V_TURN;
        s.actionName = "V-TURN";
        s.actionId = V_TURN;
        s.preactionName = "PRE-TURN";
        s.preactionId = V_TURN;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "TURN";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TURNBIT;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "for";
            e.synonyms = {"for"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:516";
        s.patternString = "UNLOCK OBJECT (ON-GROUND IN-ROOM) WITH OBJECT (FIND TOOLBIT) (HELD CARRIED ON-GROUND IN-ROOM TAKE)";
        s.verb = "UNLOCK";
        s.verbId = V_UNLOCK;
        s.actionName = "V-UNLOCK";
        s.actionId = V_UNLOCK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "UNLOCK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::TOOLBIT;
            e.scopeFlags = SH_HELD | SH_CARRIED | SH_ON_GROUND | SH_IN_ROOM;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:519";
        s.patternString = "UNTIE OBJECT (ON-GROUND IN-ROOM HELD CARRIED)";
        s.verb = "UNTIE";
        s.verbId = V_UNTIE;
        s.actionName = "V-UNTIE";
        s.actionId = V_UNTIE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "UNTIE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM | SH_HELD | SH_CARRIED;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:519";
        s.patternString = "UNTIE OBJECT (ON-GROUND IN-ROOM HELD CARRIED) FROM OBJECT";
        s.verb = "UNTIE";
        s.verbId = V_UNTIE;
        s.actionName = "V-UNTIE";
        s.actionId = V_UNTIE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "UNTIE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM | SH_HELD | SH_CARRIED;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "from";
            e.synonyms = {"from"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:524";
        s.patternString = "WAIT";
        s.verb = "WAIT";
        s.verbId = V_WAIT;
        s.actionName = "V-WAIT";
        s.actionId = V_WAIT;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WAIT";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:527";
        s.patternString = "WAKE OBJECT (FIND ACTORBIT) (ON-GROUND IN-ROOM)";
        s.verb = "WAKE";
        s.verbId = V_WAKE;
        s.actionName = "V-ALARM";
        s.actionId = V_ALARM;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WAKE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:527";
        s.patternString = "WAKE UP OBJECT (FIND ACTORBIT) (ON-GROUND IN-ROOM)";
        s.verb = "WAKE";
        s.verbId = V_WAKE;
        s.actionName = "V-ALARM";
        s.actionId = V_ALARM;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WAKE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::ACTORBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:531";
        s.patternString = "WALK";
        s.verb = "WALK";
        s.verbId = V_WALK;
        s.actionName = "V-WALK-AROUND";
        s.actionId = V_WALK_AROUND;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WALK";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:531";
        s.patternString = "WALK OBJECT";
        s.verb = "WALK";
        s.verbId = V_WALK;
        s.actionName = "V-WALK";
        s.actionId = V_WALK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WALK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:531";
        s.patternString = "WALK AWAY OBJECT";
        s.verb = "WALK";
        s.verbId = V_WALK;
        s.actionName = "V-WALK";
        s.actionId = V_WALK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WALK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "away";
            e.synonyms = {"away"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:531";
        s.patternString = "WALK IN OBJECT";
        s.verb = "WALK";
        s.verbId = V_WALK;
        s.actionName = "V-THROUGH";
        s.actionId = V_THROUGH;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WALK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "in";
            e.synonyms = {"in", "inside", "into"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:531";
        s.patternString = "WALK WITH OBJECT";
        s.verb = "WALK";
        s.verbId = V_WALK;
        s.actionName = "V-THROUGH";
        s.actionId = V_THROUGH;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WALK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "with";
            e.synonyms = {"with", "using", "through", "thru"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:531";
        s.patternString = "WALK ON OBJECT";
        s.verb = "WALK";
        s.verbId = V_WALK;
        s.actionName = "V-THROUGH";
        s.actionId = V_THROUGH;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WALK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "on";
            e.synonyms = {"on", "onto"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:533";
        s.patternString = "WALK OVER OBJECT";
        s.verb = "WALK";
        s.verbId = V_WALK;
        s.actionName = "V-LEAP";
        s.actionId = V_LEAP;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WALK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "over";
            e.synonyms = {"over"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:534";
        s.patternString = "WALK TO OBJECT";
        s.verb = "WALK";
        s.verbId = V_WALK;
        s.actionName = "V-WALK-TO";
        s.actionId = V_WALK_TO;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WALK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "to";
            e.synonyms = {"to"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:531";
        s.patternString = "WALK AROUND OBJECT";
        s.verb = "WALK";
        s.verbId = V_WALK;
        s.actionName = "V-WALK-AROUND";
        s.actionId = V_WALK_AROUND;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WALK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "around";
            e.synonyms = {"around"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:536";
        s.patternString = "WALK UP OBJECT (FIND CLIMBBIT) (ON-GROUND IN-ROOM)";
        s.verb = "WALK";
        s.verbId = V_WALK;
        s.actionName = "V-CLIMB-UP";
        s.actionId = V_CLIMB_UP;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WALK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::CLIMBBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:537";
        s.patternString = "WALK DOWN OBJECT (FIND CLIMBBIT) (ON-GROUND IN-ROOM)";
        s.verb = "WALK";
        s.verbId = V_WALK;
        s.actionName = "V-CLIMB-DOWN";
        s.actionId = V_CLIMB_DOWN;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WALK";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "down";
            e.synonyms = {"down"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.findFlag = ObjectFlag::CLIMBBIT;
            e.scopeFlags = SH_ON_GROUND | SH_IN_ROOM;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:544";
        s.patternString = "WAVE OBJECT (HELD CARRIED TAKE HAVE)";
        s.verb = "WAVE";
        s.verbId = V_WAVE;
        s.actionName = "V-WAVE";
        s.actionId = V_WAVE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WAVE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:544";
        s.patternString = "WAVE OBJECT (HELD CARRIED TAKE HAVE) AT OBJECT";
        s.verb = "WAVE";
        s.verbId = V_WAVE;
        s.actionName = "V-WAVE";
        s.actionId = V_WAVE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WAVE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            e.scopeFlags = SH_HELD | SH_CARRIED;
            e.have = true;
            e.take = true;
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "at";
            e.synonyms = {"at"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:544";
        s.patternString = "WAVE AT OBJECT";
        s.verb = "WAVE";
        s.verbId = V_WAVE;
        s.actionName = "V-WAVE";
        s.actionId = V_WAVE;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WAVE";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "at";
            e.synonyms = {"at"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:549";
        s.patternString = "WEAR OBJECT";
        s.verb = "WEAR";
        s.verbId = V_WEAR;
        s.actionName = "V-WEAR";
        s.actionId = V_WEAR;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WEAR";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:551";
        s.patternString = "WIN";
        s.verb = "WIN";
        s.verbId = V_WIN;
        s.actionName = "V-WIN";
        s.actionId = V_WIN;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WIN";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:554";
        s.patternString = "WIND OBJECT";
        s.verb = "WIND";
        s.verbId = V_WIND;
        s.actionName = "V-WIND";
        s.actionId = V_WIND;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WIND";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:554";
        s.patternString = "WIND UP OBJECT";
        s.verb = "WIND";
        s.verbId = V_WIND;
        s.actionName = "V-WIND";
        s.actionId = V_WIND;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WIND";
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::PREPOSITION;
            e.value = "up";
            e.synonyms = {"up"};
            s.elements.push_back(std::move(e));
        }
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::OBJECT;
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:557";
        s.patternString = "WISH";
        s.verb = "WISH";
        s.verbId = V_WISH;
        s.actionName = "V-WISH";
        s.actionId = V_WISH;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "WISH";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:559";
        s.patternString = "YELL";
        s.verb = "YELL";
        s.verbId = V_YELL;
        s.actionName = "V-YELL";
        s.actionId = V_YELL;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "YELL";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }
    {
        ZilSyntax s;
        s.zilSource = "zil/gsyntax.zil:562";
        s.patternString = "ZORK";
        s.verb = "ZORK";
        s.verbId = V_ZORK;
        s.actionName = "V-ZORK";
        s.actionId = V_ZORK;
        s.preactionName = "";
        s.preactionId = std::nullopt;
        {
            SyntaxElement e;
            e.type = SyntaxElement::Type::VERB;
            e.value = "ZORK";
            s.elements.push_back(std::move(e));
        }
        g_syntaxes.push_back(std::move(s));
    }

    // Index syntaxes
    for (const auto& syn : g_syntaxes) {
        g_verbSyntaxMap[syn.actionId].push_back(&syn);
        std::string vLower = toLower(syn.verb);
        g_wordSyntaxMap[vLower].push_back(&syn);
    }
}

bool isBuzzWord(std::string_view word) {
    initialize();
    return g_buzzwordSet.contains(toLower(word));
}

const std::vector<std::string>& getBuzzWords() {
    initialize();
    return g_buzzwords;
}

bool isPreposition(std::string_view word) {
    initialize();
    return g_prepCanonical.contains(toLower(word));
}

std::string canonicalPreposition(std::string_view word) {
    initialize();
    auto it = g_prepCanonical.find(toLower(word));
    if (it != g_prepCanonical.end()) {
        return it->second;
    }
    return toLower(word);
}

const std::vector<std::string>& getPrepositionSynonyms(std::string_view canonicalPrep) {
    initialize();
    static const std::vector<std::string> emptyList;
    auto it = g_prepSynonyms.find(toLower(canonicalPrep));
    if (it != g_prepSynonyms.end()) {
        return it->second;
    }
    return emptyList;
}

bool isDirection(std::string_view word) {
    initialize();
    return g_dirCanonical.contains(toLower(word));
}

std::string canonicalDirection(std::string_view word) {
    initialize();
    auto it = g_dirCanonical.find(toLower(word));
    if (it != g_dirCanonical.end()) {
        return it->second;
    }
    return toLower(word);
}

const std::vector<std::string>& getDirectionSynonyms(std::string_view canonicalDir) {
    initialize();
    static const std::vector<std::string> emptyList;
    auto it = g_dirSynonyms.find(toLower(canonicalDir));
    if (it != g_dirSynonyms.end()) {
        return it->second;
    }
    return emptyList;
}

std::optional<VerbId> lookupVerb(std::string_view word) {
    initialize();
    auto it = g_verbLookup.find(toLower(word));
    if (it != g_verbLookup.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::string canonicalVerb(std::string_view word) {
    initialize();
    auto it = g_verbCanonical.find(toLower(word));
    if (it != g_verbCanonical.end()) {
        return it->second;
    }
    return toLower(word);
}

const std::vector<std::string>& getVerbSynonyms(VerbId verbId) {
    initialize();
    static const std::vector<std::string> emptyList;
    auto it = g_verbSynonyms.find(verbId);
    if (it != g_verbSynonyms.end()) {
        return it->second;
    }
    return emptyList;
}

const std::vector<ZilSyntax>& getAllSyntaxes() {
    initialize();
    return g_syntaxes;
}

std::vector<const ZilSyntax*> getSyntaxesForVerb(VerbId verbId) {
    initialize();
    auto it = g_verbSyntaxMap.find(verbId);
    if (it != g_verbSyntaxMap.end()) {
        return it->second;
    }
    return {};
}

std::vector<const ZilSyntax*> getSyntaxesForVerb(std::string_view verbWord) {
    initialize();
    auto it = g_wordSyntaxMap.find(toLower(verbWord));
    if (it != g_wordSyntaxMap.end()) {
        return it->second;
    }
    return {};
}

const ZilSyntax* matchSyntax(std::string_view verbWord,
                             const std::vector<std::string>& prepositions,
                             size_t objectCount) {
    initialize();
    auto list = getSyntaxesForVerb(verbWord);
    for (const auto* syn : list) {
        size_t objCount = 0;
        std::vector<std::string> preps;
        for (const auto& elem : syn->elements) {
            if (elem.type == SyntaxElement::Type::OBJECT) {
                objCount++;
            } else if (elem.type == SyntaxElement::Type::PREPOSITION) {
                preps.push_back(elem.value);
            }
        }
        if (objCount == objectCount && preps.size() == prepositions.size()) {
            bool match = true;
            for (size_t i = 0; i < preps.size(); ++i) {
                if (canonicalPreposition(preps[i]) != canonicalPreposition(prepositions[i])) {
                    match = false;
                    break;
                }
            }
            if (match) return syn;
        }
    }
    return nullptr;
}

void populateVerbRegistry(VerbRegistry& registry) {
    initialize();

    using ET = SyntaxPattern::ElementType;
    using Elem = SyntaxPattern::Element;

    for (const auto& syn : g_syntaxes) {
        std::vector<Elem> patElements;
        for (const auto& el : syn.elements) {
            if (el.type == SyntaxElement::Type::VERB) {
                patElements.emplace_back(ET::VERB);
            } else if (el.type == SyntaxElement::Type::PREPOSITION) {
                patElements.emplace_back(ET::PREPOSITION, el.synonyms);
            } else if (el.type == SyntaxElement::Type::OBJECT) {
                if (el.findFlag.has_value()) {
                    patElements.emplace_back(ET::OBJECT, *el.findFlag);
                } else {
                    patElements.emplace_back(ET::OBJECT);
                }
            }
        }
        registry.registerSyntax(syn.verbId, SyntaxPattern(syn.actionId, std::move(patElements)));
    }
}

} // namespace GSyntax
