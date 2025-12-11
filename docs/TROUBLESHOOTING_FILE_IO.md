# Varla File I/O Troubleshooting Log

## Session Date: 2025-12-11

### Goal
Implement file I/O functionality for OBSE64 (Oblivion Remastered) to enable the Varla save/load plugin.

### Commands Implemented

#### 1. **PrintC**
- **Purpose**: Print to console AND write to registered log files
- **Syntax**: `PrintC "format string" [args...]`
- **Shortname**: `printc`
- **Status**: ⚠️ Prints to console but NOT writing to files

#### 2. **RegisterLog**
- **Purpose**: Register a log file for reading/writing
- **Syntax**: `RegisterLog "logname" mode` (mode: 0=read, 1=write)
- **Status**: ✅ Working - creates log file at correct path

#### 3. **ReadFromLog**
- **Purpose**: Read all lines from a log file into an array
- **Syntax**: `let array = ReadFromLog "logname"`
- **Status**: ⚠️ Not tested yet

#### 4. **UnregisterLog**
- **Purpose**: Close and unregister a log file
- **Syntax**: `UnregisterLog "logname" [flush] [saveIndex]`
- **Status**: ✅ Working - no errors

#### 5. **VarlaWriteToFile** (Alternative)
- **Purpose**: Simple file write without registration
- **Syntax**: `VarlaWriteToFile "filename" "content"`
- **Status**: ❓ Not tested yet

#### 6. **VarlaReadFromFile** (Alternative)
- **Syntax**: `let array = VarlaReadFromFile "filename"`
- **Status**: ❓ Not tested yet

---

## Issues Encountered & Fixes Applied

### Issue 1: Wrong Directory Path
**Problem**: Log files were being created in `My Games\Oblivion\` instead of `My Games\Oblivion Remastered\`

**Fix**: Changed `GetLogDirectory()` in Commands_FileIO.cpp line 36:
```cpp
// Before
std::string logPath = std::string(path) + "\\My Games\\Oblivion\\";

// After
std::string logPath = std::string(path) + "\\My Games\\Oblivion Remastered\\";
```

**Result**: ✅ Log file now created in correct location

---

### Issue 2: ofstream Not Copyable in std::map
**Problem**: `std::ofstream` cannot be copied, causing issues when stored in `std::map`

**Fix**: Changed to use `std::unique_ptr<std::ofstream>` in Commands_FileIO.cpp:
```cpp
// Before
struct LogFile {
    std::ofstream writeStream;
    ...
};

// After
struct LogFile {
    std::unique_ptr<std::ofstream> writeStream;
    ...
};
```

**Result**: ✅ Compiles correctly, but still not writing to files

---

### Issue 3: PrintC Not Writing to Log Files
**Problem**: `PrintC` prints to console but log file remains empty, even though `RegisterLog` creates the file

**Symptoms**:
- ✅ `RegisterLog "test" 1` outputs: `RegisterLog: Registered 'test' at C:\...\test.log`
- ✅ File `test.log` is created
- ✅ `PrintC "Hello from Varla!"` outputs: `Hello from Varla`
- ❌ But `test.log` remains empty (0 bytes)
- ❌ Debug messages from PrintC are NOT appearing in console

**Attempted Fixes**:
1. Added diagnostic messages to show:
   - Number of registered logs
   - Log properties (isOpen, mode, hasStream, streamOpen)
   - Write confirmation

**Current Status**:
Debug messages are NOT appearing in console output, which suggests:
- PrintC may be calling a different function (name collision?)
- Our command may not be registered properly
- Console might be using cached DLL

---

## Test Commands Used

### Basic Test Sequence
```
RegisterLog "test" 1
PrintC "Hello from Varla!"
UnregisterLog "test" 0 0
```

**Expected Output**:
```
RegisterLog: Registered 'test' at C:\Users\...\test.log
[DEBUG: PrintC checking 1 registered log(s)]
[DEBUG: Log 'test' - isOpen=1, mode=1, hasStream=1, streamOpen=1]
Hello from Varla!
[DEBUG: Successfully wrote to log 'test']
[PrintC: wrote to 1 log(s)]
```

**Actual Output**:
```
RegisterLog: Registered 'test' at C:\Users\...\test.log
Hello from Varla!
```

**Note**: Debug messages completely missing!

---

## Files Modified

### Core Implementation Files
1. **Commands_FileIO.cpp** - Main file I/O command implementations
2. **Commands_FileIO.h** - Header for file I/O commands
3. **Commands_Array.cpp** - Basic array support (ar_Size, ar_Construct)
4. **Commands_Array.h** - Array command headers
5. **ArrayTypes.h** - Shared array type definitions
6. **VarlaPlugin.cpp** - Plugin interface (created but not used in final implementation)
7. **CommandTable.cpp** - Registered commands in AddScriptCommands()
8. **obse64.def** - Exported AddScriptCommand for plugin support

### Build Configuration
1. **CMakeLists.txt** - Added new source files to build
2. Output DLL: `obse64_1_512_105.dll`
3. Installation path: `C:\Steam\steamapps\common\Oblivion Remastered\OblivionRemastered\Binaries\Win64\`

---

## Next Steps for Tomorrow

### Priority 1: Verify PrintC is Actually Being Called
**Possible Issues**:
1. Name collision with existing command
2. Command not properly registered
3. Game using cached DLL

**Tests to Try**:
```
# Test if our command is registered
help printc

# Try lowercase version
printc "test message"

# Try the shortname explicitly
printc "test message"

# Test alternative command
VarlaWriteToFile "test.log" "Direct write test"
```

### Priority 2: Check Command Registration
- Verify commands appear in command list
- Check if PrintC/printc is shadowed by another command
- Try renaming to unique name like "VarlaPrintC"

### Priority 3: Add More Diagnostics
- Add debug message at the very start of Cmd_PrintC_Execute
- Print message when command is registered in CommandTable
- Check OBSE log for any command registration messages

### Priority 4: Alternative Approach
If PrintC continues to fail, test the simpler commands:
- Try `VarlaWriteToFile` which doesn't use registration
- If that works, we know the issue is specific to RegisterLog/PrintC interaction
- If that also fails, issue might be with file permissions or path

---

## Code Locations

### Key Functions
- `Cmd_PrintC_Execute()` - Commands_FileIO.cpp:49
- `Cmd_RegisterLog_Execute()` - Commands_FileIO.cpp:84
- `GetLogDirectory()` - Commands_FileIO.cpp:31
- Command registration - CommandTable.cpp:115-121

### Important Variables
- `g_registeredLogs` - Map of registered log files (Commands_FileIO.cpp:24)
- `g_arrayStorage` - Map of arrays for ReadFromLog (Commands_FileIO.cpp:27)

---

## Diagnostic Messages Added

Current debug output in Cmd_PrintC_Execute():
```cpp
Console_Print("[DEBUG: PrintC checking %d registered log(s)]", (int)g_registeredLogs.size());
Console_Print("[DEBUG: Log '%s' - isOpen=%d, mode=%d, hasStream=%d, streamOpen=%d]", ...);
Console_Print("[DEBUG: Successfully wrote to log '%s']", log.name.c_str());
Console_Print("[PrintC: wrote to 0 logs - check debug output above]");
```

**⚠️ ISSUE**: These messages are NOT appearing in console, indicating the function may not be executing at all!

---

## Working Theory

The most likely explanation for why debug messages don't appear:

1. **PrintC command collision**: There may be another PrintC/printc command that takes precedence
2. **DLL caching**: Game might be using cached version despite file replacement
3. **Command registration order**: Our command may be registered but overwritten by another

**Evidence**:
- RegisterLog messages appear correctly (✅)
- PrintC base message appears ("Hello from Varla!") (✅)
- But NO debug messages from our PrintC implementation (❌)
- This suggests a different PrintC is being called

---

## Summary

### What's Working ✅
- OBSE64 builds successfully with new commands
- Commands are registered in CommandTable
- RegisterLog creates log files at correct path
- Basic console output works

### What's Not Working ❌
- PrintC does not write to registered log files
- Debug messages from PrintC don't appear
- Log files remain empty despite being created

### Most Likely Issue
PrintC command name collision - another command (possibly the original PrintToConsole aliased as PrintC) is taking precedence over our implementation.

### Recommended Next Action
Rename PrintC to something unique like "VarlaPrint" or "LogPrint" to avoid any possible name collision, then test again.
