import os
import re

# Paths
ZIL_DIR = "zil"
SRC_DIR = "src"

# Output File
OUTPUT_FILE = "comprehensive_checklist.md"

def scan_zil_entities():
    # Reuse logic from scan_zil.py but more robust if needed, 
    # or just read the generated text files if they exist/are reliable.
    # We'll re-scan for simplicity and self-containment.
    
    objects = set()
    rooms = set()
    verbs = set()
    actions = set() # Object-specific functions
    
    # 1. Objects and Rooms from 1dungeon.zil (and others)
    obj_pattern = re.compile(r'<OBJECT\s+([^\s\)]+)', re.IGNORECASE)
    room_pattern = re.compile(r'<ROOM\s+([^\s\)]+)', re.IGNORECASE)
    routine_pattern = re.compile(r'<ROUTINE\s+([^\s\)]+)', re.IGNORECASE)
    
    for filename in os.listdir(ZIL_DIR):
        if not filename.endswith(".zil"): continue
        path = os.path.join(ZIL_DIR, filename)
        with open(path, 'r', encoding='latin-1') as f:
            content = f.read()
            
            for m in obj_pattern.finditer(content): objects.add(m.group(1))
            for m in room_pattern.finditer(content): rooms.add(m.group(1))
            
            for m in routine_pattern.finditer(content):
                name = m.group(1)
                if name.startswith("V-"):
                    verbs.add(name)
                elif name.endswith("-F") or name.endswith("-FCN") or name.endswith("-FUNCTION"):
                    actions.add(name)
                # Ignore generic systems for now, or add to separate list?
                
    return sorted(list(objects)), sorted(list(rooms)), sorted(list(verbs)), sorted(list(actions))

def scan_cpp_entities():
    # Scan C++ code for implemented IDs and Functions
    implemented_objects = set()
    implemented_rooms = set()
    implemented_verbs = set()
    implemented_actions = set()
    
    # 1. Scan ObjectIds and RoomIds in header files? 
    # Or usage in world_init / rooms.cpp
    
    # Heuristic: verify if "ObjectIds::NAME" or "RoomIds::NAME" exists or macro usage
    # Actually, simpler: grep definitions.
    
    # Objects (src/world/objects.h or .cpp)
    # Rooms (src/world/rooms.h or .cpp)
    
    for root, dirs, files in os.walk(SRC_DIR):
        for file in files:
            if not file.endswith(".cpp") and not file.endswith(".h"): continue
            path = os.path.join(root, file)
            
            with open(path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                
                # Objects: Look for createObject matches or ObjectIds definitions
                # Matches: createObject({ .id = ObjectIds::EGG ...
                # Or simply: constexpr ObjectId EGG = ...
                
                # We'll just look for occurrences of the ZIL name (normalized)
                # ZIL: EGG -> C++: EGG
                # ZIL: DECK-NINE-F -> C++: DECK_NINE_F
                
                pass
                
    return implemented_objects, implemented_rooms, implemented_verbs, implemented_actions

def normalize(name):
    # PRE-BOARD -> PRE_BOARD
    return name.replace("-", "_").upper()

def fuzzy_check(zil_list, cpp_content_map):
    # Returns list of (zil_name, is_implemented)
    results = []
    
    # Create a massive string of all C++ code?
    # Or list of identifiers?
    
    # Let's read all C++ content into one big buffer for easy checking (project is small enough)
    full_cpp_content = ""
    for root, dirs, files in os.walk(SRC_DIR):
        for file in files:
            if file.endswith(".cpp") or file.endswith(".h"):
                 with open(os.path.join(root, file), 'r', encoding='utf-8', errors='ignore') as f:
                     full_cpp_content += f.read()
                     
    full_cpp_content = full_cpp_content.upper()
    
    for item in zil_list:
        norm = normalize(item)
        
        # Heuristics
        # Objects: Look for "ObjectIds::NORM" or "ObjectIds::... NORM"
        # Since we might not have exact namespace prefix everywhere, checking specific patterns is verifying.
        
        # Check 1: Exact ID definition
        # "constexpr ObjectId " + norm
        # "constexpr ObjectId " + norm + " ="
        
        # Check 2: Action function
        # "bool " + norm + "()"
        # "bool " + item.replace("-FUNCTION", "Action").replace("-FCN", "Action") ... logic?
        
        # Let's just check if the NORM string exists in C++ mainly? 
        # No, that's too loose. comments might trigger it.
        
        # Refined Object Check:
        found = False
        if "OBJECTIDS::" + norm in full_cpp_content: found = True
        elif "ROOMIDS::" + norm in full_cpp_content: found = True
        elif "V_" + norm.replace("V_", "") in full_cpp_content: found = True # Verb enum
        elif norm.replace("_FCN", "ACTION").replace("_FUNCTION", "ACTION") in full_cpp_content: found = True # Action mapping logic
        
        # Special case for Actions: ZIL "CYCLOPS-FCN" -> C++ "cyclopsAction" (camelCase)
        # normalize produces CYCLOPS_FCN.
        # We need camelCase converter?
        
        if not found:
             # Try camelCase for actions
             base = item.lower()
             if base.endswith("-fcn"): base = base[:-4]
             if base.endswith("-function"): base = base[:-9]
             if base.endswith("-f"): base = base[:-2]
             
             # "cyclops" -> "cyclopsAction"
             # "blue-button" -> "blueButtonAction"
             
             parts = base.split('-')
             camel = parts[0] + ''.join(x.title() for x in parts[1:]) + "Action"
             
             if camel.upper() in full_cpp_content: found = True
             
             # Try just camelCase without Action suffix?
             camel_plain = parts[0] + ''.join(x.title() for x in parts[1:])
             if "BOOL " + camel_plain.upper() in full_cpp_content: found = True
        
        results.append((item, found))
        
    return results

def main():
    objects, rooms, verbs, actions = scan_zil_entities()
    
    # Scan C++
    # We do a fuzzy check against the full codebase
    
    obj_results = fuzzy_check(objects, None)
    room_results = fuzzy_check(rooms, None)
    verb_results = fuzzy_check(verbs, None)
    action_results = fuzzy_check(actions, None)
    
    # Generate Checklist
    with open(OUTPUT_FILE, "w") as f:
        f.write("# Comprehensive Implementation Checklist\n")
        f.write("Generated from ZIL vs C++ Gap Analysis.\n\n")
        
        f.write("## Objects\n")
        for name, done in obj_results:
            mark = "x" if done else " "
            f.write(f"- [{mark}] {name}\n")
            
        f.write("\n## Rooms\n")
        for name, done in room_results:
            mark = "x" if done else " "
            f.write(f"- [{mark}] {name}\n")
            
        f.write("\n## Verbs (V-*)\n")
        for name, done in verb_results:
            mark = "x" if done else " "
            f.write(f"- [{mark}] {name}\n")
            
        f.write("\n## Actions (*-FCN)\n")
        for name, done in action_results:
            mark = "x" if done else " "
            f.write(f"- [{mark}] {name}\n")

if __name__ == "__main__":
    main()
