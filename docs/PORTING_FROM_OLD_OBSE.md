# Porting OBSE Features from old-obse to OBSE64

**Date**: December 12, 2025
**Source**: `c:\webdev\obse64\old-obse\xOBSE\obse\obse`
**Target**: `c:\webdev\obse64\obse64`

---

## What We Found in Old OBSE ✅

The old OBSE source contains **complete implementations** of all missing features needed for Varla!

### 1. String Variable System ✅

**Files**:
- `StringVar.h` - Class definition
- `StringVar.cpp` - Core implementation
- `Commands_String.h` - String command declarations
- `Commands_String.cpp` - String commands (sv_Construct, sv_Set, etc.)

**Key Components**:

```cpp
// StringVar.h:20
class StringVar
{
    std::wstring data;
    UInt8 owningModIndex;
    std::unique_ptr<const char[]> multibyte_ptr;
    size_t multibyte_len;
    bool modified;
public:
    StringVar(const char* in_data, UInt32 in_refID);
    void Set(const char* newString);
    SInt32 Compare(char* rhs, bool caseSensitive);
    void Insert(const char* subString, UInt32 insertionPos);
    UInt32 Find(char* subString, UInt32 startPos, UInt32 numChars, bool bCaseSensitive = false);
    UInt32 Replace(char* toReplace, const char* replaceWith, ...);
    // ... etc
};

// StringVar.h:56
class StringVarMap : public VarMap<StringVar>
{
public:
    void Save(OBSESerializationInterface* intfc);
    void Load(OBSESerializationInterface* intfc);
    void Clean();
    UInt32 Add(UInt8 varModIndex, const char* data, bool bTemp = false);
};

extern StringVarMap g_StringMap;
```

**String Commands** (Commands_String.cpp):
- `sv_Construct` - Create string variable
- `sv_Set` - Set string value
- `sv_Destruct` - Destroy string
- `sv_Compare` - Compare strings
- `sv_Find` - Find substring
- `sv_Replace` - Replace substring
- `sv_Insert` - Insert substring
- `sv_Erase` - Delete substring
- `sv_ToUpper` / `sv_ToLower` - Case conversion
- Plus ~30 more string functions!

---

### 2. %z Format Specifier ✅

**File**: `GameAPI.cpp:754-764`

```cpp
case 'z':
case 'Z':    // string variable
{
    fmtString.erase(strIdx, 2);
    double strID = 0;
    if (!args.Arg(args.kArgType_Float, &strID))
        return false;

    const char* toInsert = StringFromStringVar(strID);

    if (toInsert && toInsert[0])
    {
        fmtString.insert(strIdx, toInsert);
        strIdx += strlen(toInsert);
    }
    break;
}
```

**How it works**:
1. String variables are stored in `g_StringMap` with numeric IDs
2. When `%z` is encountered, the ID is extracted as a float
3. `StringFromStringVar(strID)` retrieves the string from the map
4. String is inserted into the format string

**Related Functions**:
- `StringFromStringVar()` - Get string from ID
- `ExtractFormatStringArgs()` - Handles format string parsing

---

### 3. Loop System ✅

**Files**:
- `Loops.h` - Loop class definitions
- `Loops.cpp` - Loop implementations

**Loop Types** (Loops.h:14-117):

```cpp
// Base class
class Loop
{
public:
    virtual bool Update(COMMAND_ARGS) = 0;
};

// While loop implementation
class WhileLoop : public Loop
{
    UInt32 m_exprOffset;  // offset of test expression
public:
    WhileLoop(UInt32 exprOffset);
    virtual bool Update(COMMAND_ARGS);
};

// ForEach base class
class ForEachLoop : public Loop
{
public:
    virtual bool Update(COMMAND_ARGS) = 0;
    virtual bool IsEmpty() = 0;
};

// Array iteration
class ArrayIterLoop : public ForEachLoop
{
    ArrayID m_srcID;
    ArrayID m_iterID;
    ArrayKey m_curKey;
    ScriptEventList::Var* m_iterVar;
    UInt8 m_modIndex;
public:
    ArrayIterLoop(const ForEachContext* context, UInt8 modIndex);
    virtual bool Update(COMMAND_ARGS);
};

// String iteration
class StringIterLoop : public ForEachLoop
{
    std::string m_src;
    UInt32 m_curIndex;
    UInt32 m_iterID;
public:
    StringIterLoop(const ForEachContext* context);
    virtual bool Update(COMMAND_ARGS);
};

// Container/inventory iteration
class ContainerIterLoop : public ForEachLoop
{
    InventoryReference* m_invRef;
    ScriptEventList::Var* m_refVar;
    UInt32 m_iterIndex;
    std::vector<IRefData> m_elements;
public:
    ContainerIterLoop(const ForEachContext* context);
    virtual bool Update(COMMAND_ARGS);
};

// Loop manager
class LoopManager
{
    std::stack<LoopInfo> m_loops;
public:
    static LoopManager* GetSingleton();
    void Add(Loop* loop, ScriptExecutionState* state, UInt32 startOffset, UInt32 endOffset, COMMAND_ARGS);
    bool Break(ScriptExecutionState* state, COMMAND_ARGS);
    bool Continue(ScriptExecutionState* state, COMMAND_ARGS);
};
```

**How it works**:
- Script compiler generates loop structures
- LoopManager maintains a stack of active loops
- Each loop type handles its own iteration logic
- SaveIP/RestoreIP handled by LoopManager

---

### 4. Quest Functions ✅

**File**: `Commands_Quest.cpp:86-95`

```cpp
static bool Cmd_GetCurrentQuests_Execute(COMMAND_ARGS)
{
    *result = GetQuestList_Execute(QuestStatePredicate(TESQuest::kQuestFlag_Active), scriptObj);
    return true;
}

static bool Cmd_GetCompletedQuests_Execute(COMMAND_ARGS)
{
    *result = GetQuestList_Execute(QuestStatePredicate(TESQuest::kQuestFlag_Completed), scriptObj);
    return true;
}
```

**Available**:
- `GetCurrentQuests` - Returns array of active quests
- `GetCompletedQuests` - Returns array of completed quests
- Both return `kRetnType_Array`

---

### 5. Spell Functions ✅

**File**: `Commands_Actor.cpp:94-127`

```cpp
static bool Cmd_GetSpellCount_Execute(COMMAND_ARGS)
{
    *result = 0;
    UInt32 spellCount = 0;
    // ... get spell list
    *result = (double)spellCount;
    return true;
}

static bool Cmd_GetNthSpell_Execute(COMMAND_ARGS)
{
    UInt32* refResult = (UInt32*)result;
    *refResult = 0;

    // ... get spell list
    TESForm* spellForm = spellList->GetNthSpell(whichSpell);
    if (spellForm) {
        *refResult = spellForm->refID;
    }
    return true;
}
```

**Available**:
- `GetSpellCount` - Returns number of spells
- `GetNthSpell` - Returns spell at index N
- Supports iteration over player's spell list

---

### 6. Name/EditorID Functions ✅

**File**: `Commands_String.cpp:380-385`

```cpp
static bool Cmd_GetName_Execute(COMMAND_ARGS)
{
    TESForm* form = NULL;
    const char* name = "";

    if (ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &form))
    {
        TESFullName* fullName = DYNAMIC_CAST(form, TESForm, TESFullName);
        if (fullName)
            name = fullName->name.m_data;
    }

    AssignToStringVar(PASS_COMMAND_ARGS, name);
    return true;
}
```

**File**: `Commands_MiscForms.cpp:153-157`

```cpp
static bool Cmd_GetEditorID_Execute(COMMAND_ARGS)
{
    char buf[0x200] = "00000000";
    const char* idStr = buf;

    TESForm* form = NULL;
    if (ExtractArgs(EXTRACT_ARGS, &form) && form)
    {
        idStr = form->GetEditorID();
        if (!idStr) idStr = buf;
    }

    AssignToStringVar(PASS_COMMAND_ARGS, idStr);
    return true;
}
```

**Available**:
- `GetName` - Returns form name as string_var
- `GetEditorID` - Returns editor ID as string_var
- Both use `AssignToStringVar()` to return results

---

## Porting Strategy

### Phase 1: Core String System (CRITICAL)

Everything depends on this. Port in order:

1. **StringVar.h** → `obse64/StringVar.h`
   - Copy class definition
   - May need to update for 64-bit pointers

2. **StringVar.cpp** → `obse64/StringVar.cpp`
   - Copy implementation
   - Update any 32-bit specific code

3. **Modify GameAPI.cpp**
   - Add %z format handling to `ExtractFormatStringArgs()`
   - Add `StringFromStringVar()` helper

4. **Commands_String.cpp** → `obse64/Commands_String.cpp`
   - Copy all string commands
   - Register in CommandTable.cpp

**Estimated Effort**: Medium (1-2 days)
**Complexity**: Medium - mostly copy/paste with minor 64-bit adjustments
**Dependencies**: None

---

### Phase 2: GetName/GetEditorID (HIGH PRIORITY)

Once string_var works:

1. **Port GetName** from `Commands_String.cpp:380`
   - Requires string_var system (Phase 1)
   - Requires `TESFullName` class (check if exists in OBSE64)

2. **Port GetEditorID** from `Commands_MiscForms.cpp:153`
   - Requires string_var system (Phase 1)
   - Uses `form->GetEditorID()` method

**Estimated Effort**: Low (few hours)
**Complexity**: Low - simple functions once string_var exists
**Dependencies**: Phase 1 complete

---

### Phase 3: Quest Functions (MEDIUM PRIORITY)

1. **Port GetCompletedQuests/GetCurrentQuests**
   - From `Commands_Quest.cpp:86-95`
   - Returns array of quest references
   - Needs array system (already partially exists)

**Estimated Effort**: Low (few hours)
**Complexity**: Low - straightforward array return
**Dependencies**: Working array system (mostly done)

---

### Phase 4: Spell Functions (MEDIUM PRIORITY)

1. **Port GetSpellCount/GetNthSpell**
   - From `Commands_Actor.cpp:94-127`
   - Uses `TESSpellList::GetNthSpell()` method
   - Check if `TESSpellList` exists in OBSE64

**Estimated Effort**: Low (few hours)
**Complexity**: Low - depends on game class availability
**Dependencies**: None for basic implementation

---

### Phase 5: Loop System (COMPLEX)

This is the most complex feature:

1. **Port Loop Classes**
   - `Loops.h` → `obse64/Loops.h`
   - `Loops.cpp` → `obse64/Loops.cpp`
   - WhileLoop, ForEachLoop, ArrayIterLoop, etc.

2. **Modify Script Compiler**
   - Add `foreach`/`loop` keyword parsing
   - Add `while` loop support
   - Generate loop bytecode

3. **Integrate LoopManager**
   - Handle loop stack management
   - Implement Break/Continue commands

**Estimated Effort**: High (several days to a week)
**Complexity**: Very High - requires script compiler modifications
**Dependencies**: Understanding of OBSE64 script compilation

**Alternative**: Skip loops, use simplified scripts without iteration

---

## Recommended Porting Order

### Quick Win Path (Get Varla Working Soon):

**Week 1**: String System + Basic Functions
1. Port StringVar.h/cpp
2. Port %z format support
3. Port GetName/GetEditorID
4. Port sv_Construct and basic string commands

**Week 2**: Quest/Spell Functions
1. Port GetCompletedQuests/GetCurrentQuests
2. Port GetSpellCount/GetNthSpell
3. Test with simplified Varla script

**Result**: Can export character data with names, quests, spells!

**Later (Optional)**: Loop System
- Only if you want full foreach/while support
- Complex but enables advanced scripts

---

### Full Compatibility Path:

**Phase 1-4**: String, names, quests, spells (2-3 weeks)
**Phase 5**: Loop system (1-2 weeks)
**Testing**: Full varla script compatibility (1 week)

**Result**: Full OBSE v4 compatibility for Varla

---

## File Porting Checklist

### Files to Copy:

```
FROM: c:\webdev\obse64\old-obse\xOBSE\obse\obse\
TO:   c:\webdev\obse64\obse64\

Core String System:
☐ StringVar.h
☐ StringVar.cpp
☐ Commands_String.h
☐ Commands_String.cpp
☐ VarMap.h (dependency for StringVarMap)

Loop System (optional):
☐ Loops.h
☐ Loops.cpp

Quest Functions:
☐ Commands_Quest.cpp (GetCompletedQuests, GetCurrentQuests sections)
☐ Commands_Quest.h

Actor/Spell Functions:
☐ Commands_Actor.cpp (GetSpellCount, GetNthSpell sections)

Misc Functions:
☐ Commands_MiscForms.cpp (GetEditorID section)
```

### Files to Modify:

```
In: c:\webdev\obse64\obse64\

☐ GameAPI.cpp - Add %z format handling
☐ CommandTable.cpp - Register new commands
☐ CMakeLists.txt - Add new source files
```

---

## 64-bit Considerations

When porting from 32-bit old OBSE to 64-bit OBSE64:

### Pointer Sizes
```cpp
// OLD (32-bit):
UInt32* refResult = (UInt32*)result;

// NEW (64-bit): May need:
UInt64* refResult = (UInt64*)result;
```

### Alignment
- 64-bit platforms have different alignment requirements
- `std::unique_ptr` already 64-bit safe
- Check any pointer arithmetic

### FormIDs
- Game uses 32-bit FormIDs even on 64-bit
- But pointers to forms are 64-bit
- Be careful with conversions

---

## Testing Strategy

### Test 1: String System
```oblivion
scn TestStringVar
string_var sTest

Begin GameMode
    let sTest := sv_Construct "Hello World"
    PrintC "%z" sTest
    sv_Destruct sTest
End
```

**Expected**: Console shows "Hello World"

### Test 2: GetName
```oblivion
scn TestGetName
string_var sName

Begin GameMode
    let sName := Player.GetName
    PrintC "Player name: %z" sName
End
```

**Expected**: Console shows "Player name: [your character name]"

### Test 3: Quest Export
```oblivion
scn TestQuestExport
array_var aQuests
int i

Begin GameMode
    let aQuests := GetCompletedQuests
    let i := ar_Size aQuests
    PrintC "Completed quests: %g" i
End
```

**Expected**: Console shows number of completed quests

---

## Quick Start: Port String System NOW

Want to get started immediately? Here's the minimal set:

### Step 1: Copy Core Files
```bash
cp old-obse/xOBSE/obse/obse/StringVar.h obse64/
cp old-obse/xOBSE/obse/obse/StringVar.cpp obse64/
```

### Step 2: Add to CMakeLists.txt
```cmake
source_group(
    ${PROJECT_NAME}/string
    FILES
        StringVar.cpp
        StringVar.h
)
```

### Step 3: Check Dependencies
- Look for `VarMap.h` in old OBSE
- May need to copy additional utility files
- Check for `Serialization.h` dependencies

### Step 4: Fix 64-bit Issues
- Update any `UInt32` pointers to `UInt64` where needed
- Test compilation

### Step 5: Integrate %z Support
- Modify `GameAPI.cpp` to handle `case 'z':`
- Add `StringFromStringVar()` function

---

## Conclusion

**Yes, having the old OBSE source helps IMMENSELY!**

✅ **All missing features are implemented**
✅ **Code is available and can be ported**
✅ **Main work is adapting 32-bit → 64-bit**

**Recommendation**:
- Start with **String System** (Phase 1) - this is the foundation
- Then add **GetName/GetEditorID** (Phase 2) - quick win
- Add **Quest/Spell functions** (Phase 3-4) as needed
- **Skip loops** initially - use simplified scripts

**Timeline**:
- Basic string support: 1-2 days
- Names and basic functions: 1-2 days
- Full Varla compatibility (minus loops): 1-2 weeks
- Full loops support: +1-2 weeks (optional)

Want me to start porting the StringVar system now?
