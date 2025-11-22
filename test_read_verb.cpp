#include "src/core/object.h"
#include "src/core/globals.h"
#include "src/world/world.h"
#include "src/world/objects.h"
#include "src/verbs/verbs.h"
#include <iostream>

int main() {
    auto& g = Globals::instance();
    
    // Initialize the world
    initializeWorld();
    
    // Get the leaflet from the mailbox
    ZObject* leaflet = g.getObject(ObjectIds::ADVERTISEMENT);
    if (!leaflet) {
        std::cerr << "ERROR: Leaflet not found!" << std::endl;
        return 1;
    }
    
    std::cout << "Testing READ verb with LEAFLET..." << std::endl;
    std::cout << "Leaflet description: " << leaflet->getDesc() << std::endl;
    std::cout << "Has READBIT: " << (leaflet->hasFlag(ObjectFlag::READBIT) ? "YES" : "NO") << std::endl;
    std::cout << "Has text: " << (leaflet->hasText() ? "YES" : "NO") << std::endl;
    std::cout << std::endl;
    
    // Set up globals for READ verb
    g.prso = leaflet;
    g.prsa = V_READ;
    
    // Call READ verb
    std::cout << "Calling vRead():" << std::endl;
    std::cout << "---" << std::endl;
    bool result = Verbs::vRead();
    std::cout << "---" << std::endl;
    std::cout << "Result: " << (result ? "TRUE" : "FALSE") << std::endl;
    std::cout << std::endl;
    
    // Test with BOOK
    ZObject* book = g.getObject(ObjectIds::BOOK);
    if (book) {
        std::cout << "Testing READ verb with BOOK..." << std::endl;
        g.prso = book;
        std::cout << "Calling vRead():" << std::endl;
        std::cout << "---" << std::endl;
        Verbs::vRead();
        std::cout << "---" << std::endl;
    }
    
    // Test with non-readable object
    ZObject* mailbox = g.getObject(OBJ_MAILBOX);
    if (mailbox) {
        std::cout << std::endl;
        std::cout << "Testing READ verb with non-readable MAILBOX..." << std::endl;
        g.prso = mailbox;
        std::cout << "Calling vRead():" << std::endl;
        std::cout << "---" << std::endl;
        Verbs::vRead();
        std::cout << "---" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "All tests completed successfully!" << std::endl;
    return 0;
}
