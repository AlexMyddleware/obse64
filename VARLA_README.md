# Varla Module for Oblivion Remastered

The Varla module provides simple file I/O functions for OBSE scripts in Oblivion Remastered, replacing the old Conscribe dependency.

## Building

### Simple Build (no auto-copy)
```powershell
.\build-varla.ps1
```
Output: `build\obse64\Release\Varla.dll`

### Build with Auto-Copy
```powershell
.\build-varla.ps1 -AutoCopy -OblivionPath "C:\Program Files (x86)\Steam\steamapps\common\Oblivion"
```

### Debug Build
```powershell
.\build-varla.ps1 -Config Debug
```

## Installation

Copy `Varla.dll` to your Oblivion Remastered directory (where Oblivion.exe is located).

## Functions

### VarlaWriteToFile
Writes or appends content to a file.

**Syntax:**
```
VarlaWriteToFile "filename" "content"
```

**Example:**
```
VarlaWriteToFile "save-data.txt" "Item 0x00000F 5"
VarlaWriteToFile "save-data.txt" "Spell 0x0012AB"
```

**Notes:**
- Files are stored in `Documents\My Games\Oblivion\`
- Automatically creates the file if it doesn't exist
- Appends content (doesn't overwrite)
- Adds a newline after each write

### VarlaReadFromFile
Reads all lines from a file into an array.

**Syntax:**
```
let array_var = VarlaReadFromFile "filename"
```

**Example:**
```
array_var ar_LogData
short lineCount

let ar_LogData := VarlaReadFromFile "save-data.txt"
let lineCount := ar_Size ar_LogData

; Access individual lines
let firstLine := ar_LogData[0]
let secondLine := ar_LogData[1]
```

**Notes:**
- Returns an array compatible with `ar_Size` and array indexing `[]`
- Returns 0 if file doesn't exist or can't be opened
- Files are read from `Documents\My Games\Oblivion\`

## Migration from Conscribe

### Old way (Conscribe):
```
RegisterLog $log_name 1
PrintC "some text"  ; writes to log
let ar_LogData := ReadFromLog $log_name
UnregisterLog $log_name 0 0
```

### New way (Varla):
```
VarlaWriteToFile "myfile.txt" "some text"
let ar_LogData := VarlaReadFromFile "myfile.txt"
```

## Compatibility

- **Oblivion Remastered**: ✅ Supported
- **Original Oblivion**: ❌ Not supported (use Conscribe instead)

## File Locations

All files are stored in:
```
C:\Users\[YourUsername]\Documents\My Games\Oblivion\
```

## Example Script

```
scn VarlaExportScript

string_var filename
array_var itemData
short i

Begin ScriptEffectStart
    let filename := "player-export.txt"

    ; Write player data
    VarlaWriteToFile $filename "Character Level 25"
    VarlaWriteToFile $filename "Attribute Strength 85"
    VarlaWriteToFile $filename "Item 0x00000F 10"

    PrintC "Data exported to %z" filename

    ; Read it back
    let itemData := VarlaReadFromFile $filename
    let i := ar_Size itemData
    PrintC "Read %g lines" i

    sv_Destruct filename
End
```

## Additional Functions

The module also includes the original OBSE64 functions:
- `PrintC` - Print to console (also writes to registered logs)
- `RegisterLog` - Register a log for reading/writing
- `ReadFromLog` - Read from registered log
- `UnregisterLog` - Unregister a log

These are kept for backwards compatibility with existing scripts.
