import os

def filter_routines():
    with open("zil_systems_data.txt", "r") as f:
        lines = f.readlines()
        
    verbs = []
    actions = []
    systems = []
    
    for line in lines:
        if not line.strip(): continue
        # Format: | ROUTINE | FILE | | |
        parts = line.split('|')
        if len(parts) < 3: continue
        
        routine = parts[1].strip()
        file_src = parts[2].strip()
        
        if routine.startswith("V-"):
            verbs.append(line)
        elif routine.endswith("-F") or routine.endswith("-FCN") or routine.endswith("-FUNCTION"):
            actions.append(line)
        else:
            systems.append(line)
            
    # Write Actions
    with open("zil_actions.md", "w") as f:
        f.write("# ZIL Actions Chart\nScanned from `zil` files. Focused on Object Action Routines (*-F, *-FCN, *-FUNCTION).\n\n")
        f.write("| Action Routine | File | Description |\n")
        f.write("|----------------|------|-------------|\n")
        f.writelines(actions)

    # Write Verbs
    with open("zil_verbs.md", "w") as f:
        f.write("# ZIL Verbs Chart\nScanned from `zil` files. Focused on Verb Handlers (V-*).\n\n")
        f.write("| Verb Routine | File | Syntax Pattern (Pending) | Special Uses |\n")
        f.write("|--------------|------|--------------------------|--------------|\n")
        f.writelines(verbs)

    # Write Systems
    with open("zil_systems.md", "w") as f:
        f.write("# ZIL Systems Chart\nScanned from `zil` files. (Generic ROUTINES).\n\n")
        f.write("| System / Routine | File | Purpose | Logic |\n")
        f.write("|------------------|------|---------|-------|\n")
        f.writelines(systems)

if __name__ == "__main__":
    filter_routines()
