# Varla File I/O Plugin Implementation

## Overview

This implementation adds file I/O capabilities to OBSE64, similar to the original OBSE "Conscribe" plugin. It allows scripts to write data to log files and read them back, enabling save/load functionality for character data.

## Implemented Commands

### PrintC
**Syntax:** `PrintC "format string" [arg1] [arg2] ...`
**Shortname:** `printc`

Prints a formatted string to both the console and any registered log files in write mode. This is an enhanced version of `PrintToConsole` that supports file output.

**Example:**
```
PrintC "Player level: %g" playerLevel
PrintC "Attribute: %z = %g" attributeName attributeValue
```

### RegisterLog
**Syntax:** `RegisterLog "logname" mode`

Registers a log file for reading or writing.
- **logname**: Name of the log file (without extension)
- **mode**: 0 = read mode, 1 = write/append mode

Log files are created in: `My Documents\My Games\Oblivion\`

**Example:**
```
RegisterLog "varla-test" 1  ; Open for writing
```

### ReadFromLog
**Syntax:** `let array = ReadFromLog "logname"`

Reads all lines from a registered log file and returns them as an array. The array can be accessed using `ar_Size` and array indexing.

**Example:**
```
let logData := ReadFromLog "varla-test"
let numLines := ar_Size logData
```

### UnregisterLog
**Syntax:** `UnregisterLog "logname" [flush] [saveIndex]`

Closes and unregisters a log file.
- **flush**: Whether to flush the file (typically 0)
- **saveIndex**: Whether to save index (typically 0)

**Example:**
```
UnregisterLog "varla-test" 0 0
```

## Array Support Commands

### ar_Size
**Syntax:** `let size = ar_Size array`

Returns the number of elements in an array.

### ar_Construct
**Syntax:** `let array = ar_Construct "type"`

Creates a new array. Type can be "array", "map", or "stringmap".

## Current Limitations

⚠️ **Important**: This is a **basic implementation** with the following limitations:

1. **Limited Array Support**: Full OBSE array functionality (array indexing syntax `array[index]`, `ForEach` loops, etc.) requires deeper integration with the scripting engine that is not yet implemented.

2. **Array Indexing**: While `ReadFromLog` returns an array ID and `ar_Size` works, the script syntax `array[index]` is not fully supported in OBSE64 yet. This means the import script will need modifications or additional work on the script parser.

3. **Array Functions**: Only `ar_Size` and `ar_Construct` are implemented. Other OBSE array functions like:
   - `ar_Erase`
   - `ar_Copy`
   - `ar_Insert`
   - `sv_*` (string variable functions)

   ...are not yet implemented.

4. **String Variables**: The scripts use OBSE string variables (`string_var`), which also require full implementation in the scripting engine.

## File Structure

The implementation consists of:

- **Commands_FileIO.cpp/h** - File I/O command implementations
- **Commands_Array.cpp/h** - Basic array command implementations
- **ArrayTypes.h** - Shared array type definitions
- **CommandTable.cpp** - Updated to register new commands

## Building

The project uses CMake. The new files have been added to `obse64/CMakeLists.txt` and will be automatically included in the build:

```bash
cmake --build build --config Release
```

## Next Steps

To fully support the Varla import/export scripts, the following work is needed:

1. **Script Parser Enhancement**: Implement array indexing syntax `array[index]` in the script parser
2. **String Variable System**: Implement full OBSE string variable (`string_var`) support
3. **Additional Array Functions**: Implement remaining array manipulation functions
4. **ForEach Loop Support**: Implement the `ForEach` loop construct used in the import script
5. **Additional String Functions**: Implement `sv_Split`, `sv_Replace`, `sv_Erase`, `sv_Construct`, `sv_Destruct`

## Testing

Currently, you can test the basic file I/O functionality:

```
scn TestFileIO

Begin GameMode
    RegisterLog "test" 1
    PrintC "Hello from OBSE64!"
    PrintC "Test value: %g" 123.45
    UnregisterLog "test" 0 0
End
```

This will create a file at `My Documents\My Games\Oblivion\test.log` with the printed output.

## Compatibility

This implementation is designed to be compatible with the original OBSE Conscribe plugin API, making it easier to port existing scripts once full array and string variable support is added.
