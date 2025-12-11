# How to Test Varla Plugin In-Game

## Step 1: Verify Plugin Installation

Make sure `Varla.dll` is in the correct location:
```
C:\Steam\steamapps\common\Oblivion Remastered\OblivionRemastered\Binaries\Win64\OBSE\Plugins\Varla.dll
```

## Step 2: Create the ESP with Construction Set

1. **Open the Oblivion Construction Set**

2. **Create a new script:**
   - Go to `Gameplay` → `Edit Scripts...`
   - Click `New` to create a new script
   - Name it: `VarlaExportWorking`
   - Set the script type to: `Magic Effect Script`
   - Copy the contents from `docs\scripts\varla_export_working.txt`
   - Click `Save` and close the script editor

3. **Create a custom magic effect:**
   - Go to `Gameplay` → `Magic Effects...`
   - Right-click in the list and select `New`
   - Fill in the details:
     - **Effect Name:** `Varla Export`
     - **Effect ID:** `VEXP` (4 characters)
     - **Icon:** Choose any icon
     - **Effect Archetype:** Script
     - **Script:** Select `VarlaExportWorking` from dropdown
     - **Casting Type:** Fire and Forget
     - **Delivery:** Self
   - Click `OK`

4. **Create a spell that uses this effect:**
   - Go to `Gameplay` → `Spells...`
   - Right-click and select `New`
   - Fill in the details:
     - **ID:** `VarlaExportSpell`
     - **Name:** `Varla Export`
     - **Type:** Spell
     - **Level:** Novice
     - **Cost:** 0
     - **Auto-Calc:** Unchecked
   - In the effects list, click `Add`
     - Select your `Varla Export` effect
     - Duration: 0
     - Magnitude: 0
     - Click `OK`
   - Click `OK` to save the spell

5. **Save your plugin:**
   - Go to `File` → `Save`
   - Name it: `VarlaTest.esp`
   - Click `Save`

## Step 3: Test In-Game

1. **Launch Oblivion through OBSE:**
   - Make sure you launch through the OBSE loader (usually `obse_loader.exe`)
   - For Oblivion Remastered, this might be integrated into the launcher

2. **Load your save and enable the plugin:**
   - In the Data Files menu, make sure `VarlaTest.esp` is checked

3. **Add the spell to your character:**
   - Open the console with `~` key
   - Type: `player.addspell VarlaExportSpell`
   - Press Enter
   - Close console with `~`

4. **Cast the spell:**
   - Open your magic menu
   - Find "Varla Export" under your spells
   - Cast it (it will cast instantly on self)
   - You should see a message: "Varla export complete!"

5. **Check the output file:**
   - Navigate to: `My Documents\My Games\Oblivion\`
   - Open `varla-export.log`
   - You should see your character's stats formatted like:
     ```
     === Varla Export ===
     Player Info
     Level
     15
     Attributes
     Strength,50
     Intelligence,60
     ...
     ```

## Troubleshooting

### Plugin Not Loading
- Check OBSE is actually running (use console command `GetOBSEVersion`)
- Verify the DLL is in the correct Plugins folder
- Check obse.log for any error messages

### Script Errors
- Open the console in-game and look for error messages
- Make sure the script compiles without errors in the Construction Set
- Verify all script commands are typed correctly

### No Output File
- Make sure the log directory exists: `My Documents\My Games\Oblivion\`
- Check Windows permissions on the directory
- Look for debug messages in the console (if built in debug mode)

## Alternative: Quick Test via Console

You can also test directly from console without creating a spell:

1. Open console with `~`
2. Type these commands one by one:
   ```
   RegisterLog "test" 1
   PrintC "Hello from Varla!"
   PrintC "Player level: %g" player.getlevel
   UnregisterLog "test" 0 0
   ```
3. Check `My Documents\My Games\Oblivion\test.log`

## Next Steps

Once basic export works, you can:
1. Test the read functionality with `VarlaReadFromFile` or `ReadFromLog`
2. Adapt the original varla_export_script.txt (requires implementing string_var and more OBSE functions)
3. Create import scripts once array indexing is fully supported
