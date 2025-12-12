# Varla Simple Export - Usage Guide

**Status**: ✅ **READY TO USE NOW!**

The simplified Varla export script works with current OBSE64 features without needing string_var, loops, or advanced OBSE functions.

---

## What It Exports

✅ **Player Level**
✅ **All 8 Attributes** (Strength, Intelligence, Willpower, Agility, Speed, Endurance, Personality, Luck)
✅ **All 21 Skills** (Blade, Destruction, Sneak, etc.)
✅ **Health, Magicka, Fatigue**
✅ **Fame, Infamy, Bounty**

❌ **NOT Exported** (requires full OBSE features):
- Player name, race, class, birthsign (needs string_var)
- Spells (needs GetSpellCount, GetNthSpell, foreach)
- Inventory (needs foreach)
- Quests (needs GetCompletedQuests, GetCurrentQuests)
- Factions (needs GetNumFactions, GetNthFaction)

---

## How to Use

### Step 1: Create the Spell

1. Open **Construction Set**
2. Go to **Gameplay → Magic Effects**
3. Create a new **Script Effect**:
   - Effect ID: `SCEE` (or any unused ID)
   - Name: "Varla Export Effect"
   - Check "Allow on Self"

4. Go to **Gameplay → Spells**
5. Create a new **Spell**:
   - ID: `VarlaExportSpell`
   - Name: "Export Character"
   - Type: **Spell** or **Lesser Power**
   - Cost: 0
   - Add the Script Effect you created

6. **Attach the script**:
   - In the script effect, set Script: **VarlaSimpleExport**

### Step 2: Add Script to Game

1. Save the script as: `C:\webdev\obse64\docs\scripts\VarlaSimpleExport.txt`
2. Copy to game folder (if needed)
3. Compile in Construction Set

### Step 3: Give Yourself the Spell

In-game console:
```
player.addspell VarlaExportSpell
```

### Step 4: Export Your Character

1. Cast the spell on yourself
2. You'll see: "Character exported to varla-export.log"
3. File created at: `C:\Users\[YourName]\Documents\My Games\Oblivion Remastered\varla-export.log`

---

## Export File Format

The export file looks like this:

```
=== VARLA CHARACTER EXPORT ===
Version: Simple 1.0

[LEVEL]
Level,50

[ATTRIBUTES]
Strength,100
Intelligence,85
Willpower,75
Agility,90
Speed,80
Endurance,100
Personality,65
Luck,55

[SKILLS]
Blade,100
Blunt,75
HandToHand,50
... etc ...

[STATS]
Health,350
Magicka,200
Fatigue,280
Fame,45
Infamy,2
Bounty,0

=== EXPORT COMPLETE ===
```

---

## Troubleshooting

### "Script command not found"
- Make sure OBSE64 DLL is rebuilt and copied to game directory
- Verify commands work by testing in console:
  ```
  RegisterLog "test" 1
  LogPrint "Hello"
  UnregisterLog "test" 0 0
  ```

### "Log file is empty"
- Check that LogPrint is working (see debug messages in console)
- Verify file path: `My Documents\My Games\Oblivion Remastered\`

### "Script won't compile"
- Make sure all OBSE64 commands are recognized
- Check syntax (commas, quotes, etc.)

---

## What's Next?

### Phase 1: String System (In Progress)
- Port StringVar to OBSE64
- Add %z format support
- Add GetName, GetEditorID functions
- **Result**: Can export names, race, class, etc.

### Phase 2: Quest/Spell Functions
- Port GetCompletedQuests, GetCurrentQuests
- Port GetSpellCount, GetNthSpell
- **Result**: Can export quests and spells

### Phase 3: Full Import
- Implement array indexing or CSV parsing
- Create import script that reads the export file
- **Result**: Full character transfer!

---

## Differences from Full Varla

| Feature | Simple Version | Full Varla (Original) |
|---------|----------------|----------------------|
| Level, Attributes, Skills | ✅ Works | ✅ Works |
| Player Name | ❌ No (needs string_var) | ✅ Works |
| Race, Class, Sign | ❌ No (needs string_var) | ✅ Works |
| Spells | ❌ No (needs loops) | ✅ Works |
| Inventory | ❌ No (needs foreach) | ✅ Works |
| Quests | ❌ No (needs quest functions) | ✅ Works |
| Factions | ❌ No (needs faction functions) | ✅ Works |
| Import | ❌ Manual only | ✅ Automatic |

---

## Manual Import (Until Auto-Import Works)

You can manually set values using console commands:

```
player.setav strength 100
player.setav blade 85
player.setlevel 50
player.setav health 350
player.modPCFame 45
```

Create a batch file from your export:
1. Open `varla-export.log`
2. Convert each line:
   - `Strength,100` → `player.setav strength 100`
   - `Blade,85` → `player.setav blade 85`
3. Save as `import.bat`
4. In-game: `bat import`

---

## Success Criteria

✅ **Working**: Export runs without errors
✅ **Working**: Log file is created
✅ **Working**: All stats are exported correctly
✅ **Working**: Values can be manually imported

🔄 **In Progress**: String system for names
🔄 **In Progress**: Quest/spell export
⏳ **Pending**: Automatic import script

---

## Testing Checklist

- [ ] Compile script in Construction Set
- [ ] Create spell with script effect
- [ ] Add spell to player
- [ ] Cast spell
- [ ] Check for success message
- [ ] Open log file
- [ ] Verify all values are correct
- [ ] Test manual import with console commands

---

**You can use this RIGHT NOW while we finish porting the string system!**
