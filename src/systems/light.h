#pragma once

// Light and darkness system (Requirement 50)
// Tracks room lighting and light sources to determine if player can see
class LightSystem {
public:
    // Check if a room is lit (Requirement 50)
    // Returns true if room has natural light or contains light sources
    static bool isRoomLit(class ZObject* room);
    
    // Check if player has a light source (Requirement 50)
    // Returns true if player inventory or current room has lit light sources
    static bool hasLightSource();
    
    // Update lighting state after light source changes (Requirement 50)
    // Should be called when lights are turned on/off or moved
    static void updateLighting();
    
    // Check for grue attack in darkness (Requirement 51)
    // Warns player about grue and potentially kills them
    static void checkGrue();
    
    // Reset light system state (for testing and new games)
    static void reset();
    
private:
    static int darknessTurns_;      // Turns spent in darkness
    static bool warnedAboutGrue_;   // Has player been warned?
};
