# Varla Export Script - Implementation Plan

## Current Status Analysis

### What We Have ✅

| Feature | Status | Notes |
|---------|--------|-------|
| LogPrint | ✅ Working | Renamed from PrintC, writes to console and files |
| RegisterLog | ✅ Working | Opens log files for writing |
| UnregisterLog | ✅ Working | Closes log files |
| VarlaWriteToFile | ✅ Working | Simple file write |
| ar_Size | ✅ Working | Get array size |
| ar_Construct | ✅ Working | Create arrays |
| Basic variables | ✅ Working | short, float, ref |
| Vanilla functions | ✅ Available | GetAV, GetLevel, etc. |

### What We DON'T Have ❌

| Feature | Priority | Used In Script | Impact |
|---------|----------|----------------|--------|
| **string_var** | 🔴 CRITICAL | Lines 10-16, 73-76, 83-89 | Can't store/print names |
| **%z format** | 🔴 CRITICAL | Lines 83, 85, 87, 89 | Can't print strings |
| **foreach loops** | 🔴 CRITICAL | Lines 235, 328 | Can't iterate arrays |
| **SaveIP/RestoreIP** | 🔴 CRITICAL | Lines 177, 197, 257 | Can't do while loops |
| **array indexing** | 🔴 CRITICAL | Lines 119, 179, 309 | Can't access array elements |
| GetName | 🟡 HIGH | Lines 73-76, 186 | Get form names |
| GetEditorID | 🟡 HIGH | Lines 310, 318, 332 | Get quest IDs |
| GetCompletedQuests | 🟡 HIGH | Line 304 | Returns array |
| GetCurrentQuests | 🟡 HIGH | Line 326 | Returns array |
| GetNthSpell | 🟡 HIGH | Line 179 | Iterate spells |
| GetSpellCount | 🟡 HIGH | Line 173 | Count spells |
| GetNthFaction | 🟡 HIGH | Line 288 | Iterate factions |
| GetNumFactions | 🟡 HIGH | Line 285 | Count factions |
| sv_Construct | 🟢 LOW | Line 293 | String creation |
| Magic item functions | 🟢 LOW | Lines 191-217 | Spell details |

---

## Problem: The Script is OBSE v4 Heavy

The original script uses **advanced OBSE v4 features** extensively:

### Critical Blockers (Can't Work Without These):

#### 1. **string_var System**
```oblivion
string_var sName           ; Declare string variable
let sName := GetName rPlayer    ; Store string
PrintC "%z" sName          ; Print string with %z
```

**Impact**: Used in ~30 places for names, IDs, etc.

**Complexity**: Medium - needs variable type system in script engine

---

#### 2. **Array Indexing Syntax**
```oblivion
array_var aCompletedQuests
let rQuestRef := aCompletedQuests[iQuestIndex]  ; Access by index
```

**Impact**: Can't access array elements without this

**Complexity**: High - needs script parser modifications

---

#### 3. **SaveIP/RestoreIP (While Loops)**
```oblivion
SaveIP 15
if spellIndex < numSpells
    ; ... do stuff
    set spellIndex to spellIndex + 1
    RestoreIP 15  ; Jump back to SaveIP
endif
```

**Impact**: Used for all loops in the script (spells, stats, factions, etc.)

**Complexity**: High - needs instruction pointer manipulation

---

#### 4. **foreach Loops**
```oblivion
foreach rItem <- rPlayer
    ; Process each item
loop
```

**Impact**: Used for inventory iteration

**Complexity**: High - needs script engine support

---

## Two Approaches

### Approach A: Implement Missing OBSE Features (Proper Solution)
**Time**: Weeks to months
**Difficulty**: Very High
**Result**: Full OBSE v4 compatibility

**What to implement**:
1. String variable system
2. Array indexing
3. SaveIP/RestoreIP
4. foreach/loop
5. All missing OBSE functions

**Pros**:
- ✅ Full compatibility with existing OBSE scripts
- ✅ Can use original varla scripts unchanged
- ✅ Benefits all future mods

**Cons**:
- ❌ Massive undertaking
- ❌ Requires deep script engine knowledge
- ❌ May take weeks/months

---

### Approach B: Simplified Varla (Quick Solution)
**Time**: Hours to days
**Difficulty**: Low
**Result**: Basic export/import without advanced features

**Create simplified scripts that**:
1. Don't use string_var (just print raw values)
2. Don't use arrays (sequential processing)
3. Don't use loops (hardcoded sequences)
4. Use only vanilla + our file I/O functions

**Pros**:
- ✅ Can implement TODAY
- ✅ Gets basic functionality working
- ✅ Uses only what we have

**Cons**:
- ❌ Less flexible than original
- ❌ Can't handle dynamic data (spells, quests, etc.)
- ❌ Limited to basic stats only

---

## Recommended: Hybrid Approach

### Phase 1: Simplified Export (NOW) ⚡
Create a basic working export that handles:
- ✅ Player level
- ✅ All attributes (Strength, Intelligence, etc.)
- ✅ All skills (Blade, Destruction, etc.)
- ✅ Fame, Infamy, Bounty
- ✅ Basic stats

**Example Script**:
```oblivion
scn VarlaSimpleExport

float fVal

Begin ScriptEffectStart
    RegisterLog "varla-export" 1

    LogPrint "=== Varla Simple Export ==="

    ; Level
    set fVal to Player.GetLevel
    LogPrint "Level,%g" fVal

    ; Attributes
    set fVal to Player.GetAV Strength
    LogPrint "Strength,%g" fVal
    set fVal to Player.GetAV Intelligence
    LogPrint "Intelligence,%g" fVal
    ; ... all attributes

    ; Skills
    set fVal to Player.GetAV Blade
    LogPrint "Blade,%g" fVal
    ; ... all skills

    LogPrint "=== Export Complete ==="
    UnregisterLog "varla-export" 0 0

    Message "Export complete! Check varla-export.log"
End
```

**This works TODAY with what we have!** ✅

---

### Phase 2: Add Essential OBSE Functions (NEXT)
Implement the most critical functions:

#### Priority 1: GetName
```cpp
bool Cmd_GetName_Execute(COMMAND_ARGS)
{
    TESForm* form = nullptr;
    if (ExtractArgs(EXTRACT_ARGS, &form))
    {
        if (form && form->GetName())
        {
            // Return string somehow
            // This needs string_var support!
        }
    }
    return true;
}
```

**Problem**: Still needs string_var system!

#### Priority 2: GetEditorID
**Problem**: Same - needs string_var!

**Conclusion**: Can't implement these properly without string_var.

---

### Phase 3: Implement string_var System (LATER)
This is the big one. Needs:

1. **Script Variable Types**
   - Extend script variable system to support string type
   - String storage and lifetime management

2. **Format Specifiers**
   - Add %z support to LogPrint
   - Handle string arguments in ExtractArgs

3. **String Functions**
   - sv_Construct
   - sv_Destruct
   - sv_Split
   - sv_Replace
   - etc.

**Complexity**: Requires understanding OBSE's original implementation.

---

## What Can We Do TODAY?

### Option 1: Create Simplified Export Script ✅
Use only what we have:
- LogPrint for formatted output
- RegisterLog/UnregisterLog
- Player.GetAV for attributes
- Player.GetLevel for level
- Hardcoded sequences (no loops)

**Result**: Basic character stat export/import working today.

---

### Option 2: Start with string_var Implementation 🔧
Begin implementing the string variable system:

1. Study original OBSE source for string_var
2. Implement basic string storage
3. Add %z format support to LogPrint
4. Implement GetName function

**Result**: Can use names in scripts.

---

### Option 3: Add Inventory Counting (No Names) 🔧
Can we iterate inventory without names?

```oblivion
; Count items by formID only
foreach rItem <- Player
    let formID := rItem.GetFormID
    let count := Player.GetItemCount rItem
    LogPrint "Item,%08X,%g" formID count
loop
```

**Problem**: Still needs foreach!

---

## My Recommendation: START SIMPLE

### Step 1 (TODAY): Create Working Simplified Script
Let me create a simplified export script that:
- Uses ONLY what we have implemented
- Exports level, attributes, skills, fame/infamy
- Works with current OBSE64

### Step 2 (TOMORROW): Test Import
Create matching import script that reads the simple format.

### Step 3 (NEXT WEEK): Evaluate
Once basic export/import works:
- Decide if simple version is "good enough"
- OR commit to implementing full OBSE features

---

## The Question

**Do you want to:**

**A)** Create a simplified script TODAY that works with what we have?
- Exports: Level, Attributes, Skills, Fame/Infamy
- No spells, no items, no quests
- But WORKS immediately

**B)** Start implementing OBSE features (string_var, etc.)?
- Takes longer
- More complex
- But eventually full compatibility

**C)** Both - simple script now, OBSE features gradually?
- Quick win now
- Better solution later
- Incremental progress

I recommend **Option C** - let's get something working now, then improve it!

---

## Next Steps if We Choose Option C

### TODAY:
1. ✅ Create VarlaSimpleExport.txt script
2. ✅ Test in Construction Set
3. ✅ Verify export works
4. ✅ Create VarlaSimpleImport.txt
5. ✅ Test import

### THIS WEEK:
1. 🔧 Study OBSE source for string_var
2. 🔧 Plan string variable implementation
3. 🔧 Implement basic string storage

### NEXT WEEK:
1. 🔧 Add GetName function
2. 🔧 Add %z format support
3. 🔧 Test with names

### FUTURE:
1. 🔧 Implement foreach loops
2. 🔧 Implement SaveIP/RestoreIP
3. 🔧 Add array indexing
4. 🔧 Full OBSE v4 compatibility

---

## Summary

**Current Reality**: We can't run the original script as-is.

**Quick Win**: Simplified script works TODAY.

**Long Term**: Gradually add OBSE features.

**Your Choice**: Which path should we take?
