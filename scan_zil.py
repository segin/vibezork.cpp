import os
import re

ZIL_DIR = "zil"

def scan_objects_and_rooms():
    objects = []
    rooms = []
    
    # Simple regex for top-level definitions. 
    # Valid ZIL definition: <OBJECT NAME (FLAGS ...) ...>
    # Note: ZIL syntax is LISP-like. It creates parsing challenges with regex.
    # We will grab the line starting with <OBJECT or <ROOM and subsequent lines until > ?
    # No, easy grab: <OBJECT name ...
    
    obj_pattern = re.compile(r'<OBJECT\s+([^\s\)]+)', re.IGNORECASE)
    room_pattern = re.compile(r'<ROOM\s+([^\s\)]+)', re.IGNORECASE)
    
    # We will iterate all files, but mostly 1dungeon.zil
    for filename in os.listdir(ZIL_DIR):
        if not filename.endswith(".zil"): continue
        path = os.path.join(ZIL_DIR, filename)
        
        with open(path, 'r', encoding='latin-1') as f:
            content = f.read()
            
            # Find all objects
            for match in obj_pattern.finditer(content):
                objects.append(match.group(1))
                
            # Find all rooms
            for match in room_pattern.finditer(content):
                rooms.append(match.group(1))
                
    return sorted(list(set(objects))), sorted(list(set(rooms)))

def scan_routines():
    routines = []
    pattern = re.compile(r'<ROUTINE\s+([^\s\)]+)', re.IGNORECASE)
    
    for filename in os.listdir(ZIL_DIR):
        if not filename.endswith(".zil"): continue
        path = os.path.join(ZIL_DIR, filename)
        
        with open(path, 'r', encoding='latin-1') as f:
            content = f.read()
            for match in pattern.finditer(content):
                routines.append((match.group(1), filename))
    
    return sorted(routines)

def scan_verbs():
    verbs = []
    # <SYNTAX ... = V-VERB>
    # <VERB-SYNONYM ...>
    pass # Todo, complex.
    # Just grab all V-xxx
    return []

def main():
    objs, rooms = scan_objects_and_rooms()
    routines = scan_routines()
    
    print(f"Found {len(objs)} Objects.")
    print(f"Found {len(rooms)} Rooms.")
    print(f"Found {len(routines)} Routines.")
    
    # Write Objects
    with open("zil_objects_data.txt", "w") as f:
        for o in objs:
            f.write(f"| {o} | | | |\n")

    # Write Rooms
    with open("zil_rooms_data.txt", "w") as f:
        for r in rooms:
            f.write(f"| {r} | | | |\n")
            
    # Write Routines
    with open("zil_systems_data.txt", "w") as f:
        for r, src in routines:
            f.write(f"| {r} | {src} | | |\n")

if __name__ == "__main__":
    main()
