# OBSE64 Feature Verification Report

**Date**: December 12, 2025
**Purpose**: Verify which OBSE v4 features are actually implemented in OBSE64

---

## Methodology: How We Know What's Missing

To verify if a feature exists in OBSE64, we check:

1. **Command Implementation**: Search for the command in `Commands_*.cpp` files
2. **Parameter Types Used**: Check if `kParamType_StringVar` or other advanced types are actually used
3. **Script Parser**: Look for loop/control flow keywords in parser
4. **Function Exports**: Verify function exists and is registered in `CommandTable.cpp`

---

## ❌ CONFIRMED MISSING: string_var System

### Evidence 1: kParamType_StringVar Defined But Never Used

**File**: `GameScript.h:58`
```cpp
kParamType_StringVar = 0x01,  // OBSE extension - defined in enum
```

**Search Result**: Grep for `kParamType_StringVar` in all `.cpp` files
```
Result: ZERO matches in implementation files
```

**What this means**: The enum exists as a placeholder from original OBSE, but no command actually uses it.

### Evidence 2: All Commands Only Use Float Parameters

**File**: `Commands_Console.cpp:31-39`
```cpp
ParamInfo kParams_StringFormat[10] = {
    {"string", kParamType_String, 0},   // Format string only
    {"float", kParamType_Float, 1},     // All args are floats!
    {"float", kParamType_Float, 1},
    {"float", kParamType_Float, 1},
    {"float", kParamType_Float, 1},
    {"float", kParamType_Float, 1},
    {"float", kParamType_Float, 1},
    {"float", kParamType_Float, 1},
    {"float", kParamType_Float, 1},
    {"float", kParamType_Float, 1}
};
```

**File**: `Commands_FileIO.cpp:251-260` (LogPrint)
```cpp
// Same pattern - only floats accepted as arguments
```

**Conclusion**: No commands accept `kParamType_StringVar` parameters.

### Evidence 3: No String Variable Functions

**Search**: `sv_Construct`, `sv_Destruct`, `sv_Split`, etc.
```
Result: ZERO matches in obse64 directory
```

These functions don't exist at all.

**Verdict**: ❌ **string_var is NOT implemented**

---

## ❌ CONFIRMED MISSING: %z Format Specifier

### Evidence 1: Only Float Formats Documented

**File**: `Commands_Console.cpp:9`
```cpp
// Format notation is the same as MessageBox,
// so you can use %#.#f %g %e %%
```

**Supported formats**: `%f`, `%g`, `%e`, `%%` (float formats only)
**NOT supported**: `%z` (string format)

### Evidence 2: All %z References Are in Documentation/Scripts

**Search Results** for `%z`:
```
✅ docs/scripts/varla_export_script.txt  - Script wanting to use it
✅ docs/VARLA_IMPLEMENTATION_PLAN.md     - Our analysis
✅ xbyak/xbyak_util.h:1108               - Unrelated (x86 assembler lib)
❌ NO matches in obse64/*.cpp            - Not implemented!
```

The xbyak match is just: `fprintf(fp_, "%llx %zx %s%s", ...)` - this is `%zu` (standard C++ for size_t), NOT the OBSE `%z` for strings!

### Evidence 3: sprintf_s Call Shows Only Float Support

**File**: `Commands_FileIO.cpp:58`
```cpp
sprintf_s(buffer, sizeof(buffer), fmtstring, f0, f1, f2, f3, f4, f5, f6, f7, f8);
```

All variables are `float f0, f1, f2...` - can't pass strings to sprintf this way.

**Verdict**: ❌ **%z format is NOT implemented**

---

## ❌ CONFIRMED MISSING: foreach Loops

### Evidence: Search for Loop Keywords

**Search**: `foreach`, `ForEach`, `loop` in `obse64/*.cpp`
```
Result: ZERO files found
```

**What we'd need to see**:
- Script parser recognizing `foreach` keyword
- Loop instruction generation
- Iterator implementation

**None of this exists.**

**Verdict**: ❌ **foreach loops NOT implemented**

---

## ❌ CONFIRMED MISSING: SaveIP/RestoreIP (While Loops)

### Evidence: Search for IP Manipulation

**Search**: `SaveIP`, `RestoreIP` in `obse64/*.cpp`
```
Result: ZERO files found
```

**What we'd need to see**:
- Instruction pointer save/restore commands
- Jump table management
- Label system for goto

**None of this exists.**

**Verdict**: ❌ **SaveIP/RestoreIP NOT implemented**

---

## ❌ CONFIRMED MISSING: Array Indexing Syntax

### Evidence: No Parser Support

The script compiler would need to parse:
```oblivion
let item := myArray[5]
```

**Search**: Look for array index parsing, `operator[]` handlers
```
Result: Basic array storage exists, but NO indexing syntax
```

**What we have**:
- ✅ `ar_Construct` - creates arrays
- ✅ `ar_Size` - gets array size
- ❌ Array element access by index
- ❌ Array iteration

**Verdict**: ❌ **Array indexing syntax NOT implemented**

---

## ✅ CONFIRMED WORKING: What We Actually Have

### File I/O Commands ✅

| Command | Status | Evidence |
|---------|--------|----------|
| LogPrint | ✅ Working | `Commands_FileIO.cpp:49-96`, tested in-game |
| RegisterLog | ✅ Working | `Commands_FileIO.cpp:104-155`, tested in-game |
| UnregisterLog | ✅ Working | `Commands_FileIO.cpp:220-246`, tested in-game |
| ReadFromLog | ✅ Working | `Commands_FileIO.cpp:163-211`, implemented |
| VarlaWriteToFile | ✅ Working | `Commands_FileIO.cpp:332-366`, tested in-game |
| VarlaReadFromFile | ✅ Working | `Commands_FileIO.cpp:375-420`, implemented |

**Test Evidence**: `C:\Users\hotar\Documents\My Games\Oblivion Remastered\test.log` exists with content "Hello from LogPrint!"

### Basic Array Support ✅

| Command | Status | Evidence |
|---------|--------|----------|
| ar_Construct | ✅ Working | `Commands_Array.cpp:29-49` |
| ar_Size | ✅ Working | `Commands_Array.cpp:14-27` |

### Vanilla Parameter Types ✅

| Type | Status | Usage |
|------|--------|-------|
| kParamType_String | ✅ Working | Literal strings only |
| kParamType_Float | ✅ Working | All numeric values |
| kParamType_Integer | ✅ Working | Integer values |
| kParamType_FormID | ✅ Working | Game object references |

---

## Missing OBSE v4 Functions

**Search Method**: Look for function implementations in `Commands_*.cpp`

### Quest Functions
- ❌ GetCompletedQuests - NOT FOUND
- ❌ GetCurrentQuests - NOT FOUND
- ❌ GetEditorID - NOT FOUND

### Spell Functions
- ❌ GetSpellCount - NOT FOUND
- ❌ GetNthSpell - NOT FOUND
- ❌ GetMagicEffectName - NOT FOUND

### Faction Functions
- ❌ GetNumFactions - NOT FOUND
- ❌ GetNthFaction - NOT FOUND

### String Functions
- ❌ sv_Construct - NOT FOUND
- ❌ sv_Destruct - NOT FOUND
- ❌ sv_Split - NOT FOUND
- ❌ sv_Replace - NOT FOUND

### Inventory Functions
- ❌ foreach iterator - NOT FOUND

### Name Functions
- ❌ GetName - NOT FOUND (would need string_var anyway)

---

## Summary: Why Original Varla Scripts Won't Work

The `varla_export_script.txt` uses:

| Feature | Line Numbers | Implemented? | Workaround? |
|---------|--------------|--------------|-------------|
| string_var | 10-16, 73-76, 83-89 | ❌ NO | Must use float IDs only |
| %z format | 83, 85, 87, 89 | ❌ NO | Use numeric formats |
| foreach loops | 235, 328 | ❌ NO | Hardcode sequences |
| SaveIP/RestoreIP | 177, 197, 257 | ❌ NO | Unroll loops |
| Array indexing | 119, 179, 309 | ❌ NO | Sequential processing |
| GetName | 73-76, 186 | ❌ NO | Skip names entirely |
| GetEditorID | 310, 318, 332 | ❌ NO | Use FormIDs |
| GetCompletedQuests | 304 | ❌ NO | Skip quest export |
| GetNthSpell | 179 | ❌ NO | Skip spell export |

**Result**: Cannot run original scripts without major simplification.

---

## How to Verify Any Feature

### Step 1: Search for Command Implementation
```bash
grep -r "CommandName" C:\webdev\obse64\obse64\Commands_*.cpp
```

### Step 2: Check Command Registration
```bash
grep "CommandName" C:\webdev\obse64\obse64\CommandTable.cpp
```

### Step 3: Test In-Game
```
Open console > Type: help CommandName
```

If it shows "Command not found" → NOT implemented
If it shows command info → Might be implemented (test it!)

### Step 4: Check Parameter Types
Look at the ParamInfo array - does it use:
- ✅ `kParamType_String` - Works (literals only)
- ✅ `kParamType_Float` - Works
- ❌ `kParamType_StringVar` - Doesn't exist in practice
- ❌ `kParamType_Array` - Partially exists (ar_Size works, indexing doesn't)

---

## Conclusion

**OBSE64 for Oblivion Remastered is a MINIMAL implementation**

✅ **What works**:
- Basic file I/O (our implementation)
- Simple arrays (construct, size)
- Float/string literal parameters
- Vanilla Oblivion script commands

❌ **What's missing**:
- String variables (string_var)
- String format (%z)
- Loop constructs (foreach, SaveIP/RestoreIP)
- Array indexing syntax
- Most OBSE v4 functions (GetName, GetEditorID, etc.)
- Advanced data structures

**Recommendation**: Use simplified scripts that work within these constraints.
