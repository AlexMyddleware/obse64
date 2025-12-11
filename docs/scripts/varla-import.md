scn VarlaMinimalImport

; ===== VARIABLE DECLARATIONS =====

;mod loading
short modIterator
short numberOfLoadedPlugins
string_var modName
array_var modByIndex

;parsing mod id in items
string_var itemId
string_var idPrefix
string_var idBase
short modIndex
string_var modNameParsing

; Core arrays and strings
array_var ar_LogData
array_var ar_Entry
string_var log_name
string_var str_current_line
string_var parse_mode

; Item-related variables
ref item_ref
int item_quantity
int item_formID
string_var item_formID_str

; Spell-related variables
ref spell_ref
int spell_formID
string_var spell_formID_str

; Attribute variables
string_var attr_name
float attr_value

; Skill variables
string_var skill_name
float skill_value

; Level variable
float player_level

; Misc stat variables
short misc_stat_index
short misc_stat_value
short misc_stat_state  ; 0=none, 1=expecting_index, 2=expecting_value

; Fame/Infamy/Bounty variables
short fame_value
short infamy_value
short bounty_value
short fame_infamy_bounty_state  ; 0=none, 1=expecting_fame, 2=expecting_infamy, 3=expecting_bounty

; Faction variables
ref faction_ref
short faction_rank
int faction_formID
string_var faction_formID_str

; Quest variables
string_var quest_editor_id
ref quest_ref
int quest_stage_val
short process_quest

; Loop control variables
short line_iterator
short field_count
short section_active

Begin ScriptEffectStart
    PrintC "=== VARLA IMPORT SCRIPT START ==="

    ;loading the plugins in order to get the form ids dynamically
    PrintC"Loading the plugins"
    let modIterator := 0
    set numberOfLoadedPlugins to GetNumLoadedMods
    let modByIndex := ar_Construct Map
    while modIterator < numberOfLoadedPlugins
        ; PrintC "this is the moditerator"
        ; PrintC "%x2"modIterator
        ; PrintC "this is the modname"
        let modName := GetNthModName modIterator
        ; PrintC "%z" modname
        let modByIndex[ GetModIndex $modName ] := modName
        set modIterator to modIterator + 1
    loop
    sv_destruct modName

    ; Register and read the log
    let log_name := "varla-test"
    PrintC "Log name set to: %z" log_name

    RegisterLog $log_name 1
    PrintC "RegisterLog called"

    ; Read all lines from the log
    let ar_LogData := ReadFromLog $log_name
    PrintC "ReadFromLog called"

    let line_iterator := ar_Size ar_LogData
    PrintC "Array size (number of lines): %.0f" line_iterator

    ; Initialize parsing mode
    let parse_mode := "NONE"
    let section_active := 0
    let fame_infamy_bounty_state := 0
    let misc_stat_state := 0

    ; ===== MAIN PARSING LOOP =====
    While line_iterator > 0
        let line_iterator -= 1
        ; PrintC "--- Processing line index: %.0f ---" line_iterator

        let str_current_line := ar_LogData[line_iterator]
        ; PrintC "Raw line data: %z" str_current_line

        ; Split line by space to get fields
        let ar_Entry := sv_Split $str_current_line " "
        let field_count := ar_Size ar_Entry

        if field_count == 0
            continue
        endif

        ; Get the first field to determine line type
        let parse_mode := ar_Entry[0]
        let process_quest := 0

        ; ===== LINE TYPE PARSING =====

        ; === ITEMS ===
        if eval (parse_mode == "Item") && (field_count >= 3)
            let item_formID_str := ar_Entry[1]
            let item_quantity := ToNumber ar_Entry[2]
            let item_formID := ToNumber $item_formID_str 1

            ;==========SECTION TO PARSE THE MOD ID==============================
                let itemId := item_formID_str
                sv_Replace "0X|" itemId
                ; split fixed-width: first 2 hex digits = mod index, rest = base id
                let idPrefix := itemId
                sv_Erase idPrefix 2
                let idBase := itemId
                sv_Erase idBase 0 2
                
                let modIndex := ToNumber $idPrefix 1 ; hex -> number
                
                if modIndex == 255
                    PrintC "Dynamic formID (custom spell or the like),  Skip."
                else
                    let modName := modByIndex[modIndex]
                    if modName
                        ; GetFormFromMod wants the base id WITHOUT the two-digit index
                        let item_ref := GetFormFromMod $modName $idBase
                        if IsFormValid item_ref
                            player.additem item_ref item_quantity
                        else
                            PrintC "Form not found in %z: %z" modName itemId
                        endif
                    else
                        PrintC "No plugin loaded with index %x2" modIndex
                    endif
                endif
                
                sv_destruct itemId
                sv_destruct idPrefix
                sv_destruct idBase
                sv_destruct modName

        ; === SPELLS ===
        elseif eval (parse_mode == "Spell") && (field_count >= 2)
            let spell_formID_str := ar_Entry[1]
            let spell_formID := ToNumber $spell_formID_str 1

            ;==========SECTION TO PARSE THE MOD ID==============================
            let itemId := spell_formID_str
            sv_Replace "0X|" itemId
            let idPrefix := itemId
            sv_Erase idPrefix 2
            let idBase := itemId
            sv_Erase idBase 0 2

            let modIndex := ToNumber $idPrefix 1 ; hex -> number

            if modIndex == 255
                PrintC "Dynamic formID (custom spell or the like),  Skip."
            else
                let modName := modByIndex[modIndex]
                if modName
                    let spell_ref := GetFormFromMod $modName $idBase
                    if IsFormValid spell_ref
                        player.addspell spell_ref
                    else
                        PrintC "Form not found in %z: %z" modName itemId
                    endif
                else
                    PrintC "No plugin loaded with index %x2" modIndex
                endif
            endif

            sv_destruct itemId
            sv_destruct idPrefix
            sv_destruct idBase
            sv_destruct modName

        ; === REMOVE SPELLS ===
        elseif eval (parse_mode == "RemoveSpell") && (field_count >= 2)
            let spell_formID_str := ar_Entry[1]
            let spell_formID := ToNumber $spell_formID_str 1

            ;==========SECTION TO PARSE THE MOD ID==============================
            let itemId := spell_formID_str
            sv_Replace "0X|" itemId
            let idPrefix := itemId
            sv_Erase idPrefix 2
            let idBase := itemId
            sv_Erase idBase 0 2

            let modIndex := ToNumber $idPrefix 1 ; hex -> number

            if modIndex == 255
                PrintC "Dynamic formID (custom spell or the like),  Skip."
            else
                let modName := modByIndex[modIndex]
                if modName
                    let spell_ref := GetFormFromMod $modName $idBase
                    if IsFormValid spell_ref
                        player.removespell spell_ref
                    else
                        PrintC "Form not found in %z: %z" modName itemId
                    endif
                else
                    PrintC "No plugin loaded with index %x2" modIndex
                endif
            endif

            sv_destruct itemId
            sv_destruct idPrefix
            sv_destruct idBase
            sv_destruct modName

        ; === CHARACTER LEVEL ===
        elseif eval (parse_mode == "Character") && (field_count >= 3)
            if eval (ar_Entry[1] == "Level")
                let player_level := ToNumber ar_Entry[2]
                PrintC "Setting player level to: %.0f" player_level
                player.setLevel player_level
            endif

        ; === ATTRIBUTES ===
        elseif eval (parse_mode == "Attribute") && (field_count >= 3)
            let attr_name := ar_Entry[1]
            let attr_value := ToNumber ar_Entry[2]
            PrintC "Setting attribute %z to %.0f" attr_name attr_value

            if eval (attr_name == "Strength")
                player.SetAV Strength  attr_value
            elseif eval (attr_name == "Willpower")
                player.SetAV Willpower  attr_value
            elseif eval (attr_name == "Intelligence")
                player.SetAV Intelligence  attr_value
            elseif eval (attr_name == "Endurance")
                player.SetAV Endurance  attr_value
            elseif eval (attr_name == "Speed")
                player.SetAV Speed  attr_value
            elseif eval (attr_name == "Agility")
                player.SetAV Agility  attr_value
            elseif eval (attr_name == "Personality")
                player.SetAV Personality  attr_value
            elseif eval (attr_name == "Luck")
                player.SetAV Luck  attr_value
            endif

        ; === SKILLS ===
        elseif eval (parse_mode == "Skill") && (field_count >= 3)
            let skill_name := ar_Entry[1]
            let skill_value := ToNumber ar_Entry[2]
            PrintC "Setting skill %z to %.0f" skill_name skill_value

            ; Combat skills
            if eval (skill_name == "Armorer")
                player.SetAV Armorer  skill_value
            elseif eval (skill_name == "Athletics")
                player.SetAV Athletics  skill_value
            elseif eval (skill_name == "Blade")
                player.SetAV Blade  skill_value
            elseif eval (skill_name == "Block")
                player.SetAV Block  skill_value
            elseif eval (skill_name == "Blunt")
                player.SetAV Blunt  skill_value
            elseif eval (skill_name == "HandToHand")
                player.SetAV HandToHand  skill_value
            elseif eval (skill_name == "HeavyArmor")
                player.SetAV HeavyArmor  skill_value
            ; Magic skills
            elseif eval (skill_name == "Alchemy")
                player.SetAV Alchemy  skill_value
            elseif eval (skill_name == "Alteration")
                player.SetAV Alteration  skill_value
            elseif eval (skill_name == "Conjuration")
                player.SetAV Conjuration  skill_value
            elseif eval (skill_name == "Destruction")
                player.SetAV Destruction  skill_value
            elseif eval (skill_name == "Illusion")
                player.SetAV Illusion  skill_value
            elseif eval (skill_name == "Mysticism")
                player.SetAV Mysticism  skill_value
            elseif eval (skill_name == "Restoration")
                player.SetAV Restoration  skill_value
            ; Stealth skills
            elseif eval (skill_name == "Acrobatics")
                player.SetAV Acrobatics  skill_value
            elseif eval (skill_name == "LightArmor")
                player.SetAV LightArmor  skill_value
            elseif eval (skill_name == "Marksman")
                player.SetAV Marksman  skill_value
            elseif eval (skill_name == "Mercantile")
                player.SetAV Mercantile  skill_value
            elseif eval (skill_name == "Security")
                player.SetAV Security  skill_value
            elseif eval (skill_name == "Sneak")
                player.SetAV Sneak  skill_value
            elseif eval (skill_name == "Speechcraft")
                player.SetAV Speechcraft  skill_value
            endif

        ; === FAME/INFAMY/BOUNTY ===
        elseif eval (parse_mode == "Fame") && (field_count >= 2)
            let fame_value := ToNumber ar_Entry[1]
            SetPCFame fame_value
        elseif eval (parse_mode == "Infamy") && (field_count >= 2)
            let infamy_value := ToNumber ar_Entry[1]
            SetPCInfamy infamy_value
        elseif eval (parse_mode == "Bounty") && (field_count >= 2)
            let bounty_value := ToNumber ar_Entry[1]
            player.SetCrimeGold bounty_value

        ; === PCMISCSTAT ===
        elseif eval (parse_mode == "PCMiscStat") && (field_count >= 3)
            let misc_stat_index := ToNumber ar_Entry[1]
            let misc_stat_value := ToNumber ar_Entry[2]
            ; ModPCMiscStat misc_stat_index misc_stat_value

        ; === FACTIONS ===
        elseif eval (parse_mode == "Faction") && (field_count >= 3)
            let faction_formID_str := ar_Entry[1]
            let faction_rank := ToNumber ar_Entry[2]
            let faction_formID := ToNumber $faction_formID_str 1

            ;==========SECTION TO PARSE THE MOD ID==============================
            let itemId := faction_formID_str
            sv_Replace "0X|" itemId
            let idPrefix := itemId
            sv_Erase idPrefix 2
            let idBase := itemId
            sv_Erase idBase 0 2

            let modIndex := ToNumber $idPrefix 1 ; hex -> number

            if modIndex == 255
                PrintC "Dynamic formID (custom faction), Skip."
            else
                let modName := modByIndex[modIndex]
                if modName
                    let faction_ref := GetFormFromMod $modName $idBase
                    if IsFormValid faction_ref
                        player.SetFactionRank faction_ref faction_rank
                    else
                        PrintC "Form not found in %z: %z" modName itemId
                    endif
                else
                    PrintC "No plugin loaded with index %x2" modIndex
                endif
            endif

            sv_destruct itemId
            sv_destruct idPrefix
            sv_destruct idBase
            sv_destruct modName

        ; === QUESTS (COMPLETED, CURRENT, & ACTIVE) ===
        ; We merge all 3 types here to reuse the quest list mapping
        elseif eval (parse_mode == "CompletedQuest") || (parse_mode == "CurrentQuest") || (parse_mode == "ActiveQuest")
            let quest_editor_id := ar_Entry[1]
            let process_quest := 1
            
            if eval (parse_mode == "CompletedQuest")
                let quest_stage_val := 100
            elseif eval (parse_mode == "CurrentQuest") && (field_count >= 3)
                let quest_stage_val := ToNumber ar_Entry[2]
                PrintC "Processing CurrentQuest: %z Stage %.0f" quest_editor_id quest_stage_val
            elseif eval (parse_mode == "ActiveQuest")
                ; ActiveQuest in log implies it is running. We set stage 0 to ensure it starts.
                ; Note: If 'CurrentQuest' follows in the log for the same quest, it will override this with the correct stage.
                let quest_stage_val := 0
                PrintC "Processing ActiveQuest: %z (Defaulting to stage 0)" quest_editor_id
            else
                ; Invalid format (e.g. CurrentQuest missing value)
                let process_quest := 0
            endif

            ; === QUEST LOOKUP LIST ===
            if process_quest == 1
                if eval (quest_editor_id == "NDPilgrim")
                    SetStage NDPilgrim quest_stage_val
                elseif eval (quest_editor_id == "ND01")
                    SetStage ND01 quest_stage_val
                elseif eval (quest_editor_id == "ND02")
                    SetStage ND02 quest_stage_val
                elseif eval (quest_editor_id == "ND03")
                    SetStage ND03 quest_stage_val
                elseif eval (quest_editor_id == "ND04")
                    SetStage ND04 quest_stage_val
                elseif eval (quest_editor_id == "ND05")
                    SetStage ND05 quest_stage_val
                elseif eval (quest_editor_id == "ND06")
                    SetStage ND06 quest_stage_val
                elseif eval (quest_editor_id == "ND07")
                    SetStage ND07 quest_stage_val
                elseif eval (quest_editor_id == "ND08")
                    SetStage ND08 quest_stage_val
                elseif eval (quest_editor_id == "ND09")
                    SetStage ND09 quest_stage_val
                elseif eval (quest_editor_id == "ND10")
                    SetStage ND10 quest_stage_val
                elseif eval (quest_editor_id == "DLCFrostcragSpire")
                    SetStage DLCFrostcragSpire quest_stage_val
                elseif eval (quest_editor_id == "DLCBattlehornCastle")
                    SetStage DLCBattlehornCastle quest_stage_val
                elseif eval (quest_editor_id == "DLC06ThievesDen")
                    SetStage DLC06ThievesDen quest_stage_val
                elseif eval (quest_editor_id == "DL9MehrunesQuest")
                    SetStage DL9MehrunesQuest quest_stage_val
                elseif eval (quest_editor_id == "DLCDeepscorn")
                    SetStage DLCDeepscorn quest_stage_val
                elseif eval (quest_editor_id == "DLCOrrery")
                    SetStage DLCOrrery quest_stage_val
                elseif eval (quest_editor_id == "DLCHorseArmor")
                    SetStage DLCHorseArmor quest_stage_val
                elseif eval (quest_editor_id == "MS18")
                    SetStage MS18 quest_stage_val
                elseif eval (quest_editor_id == "MS17")
                    SetStage MS17 quest_stage_val
                elseif eval (quest_editor_id == "MS48")
                    SetStage MS48 quest_stage_val
                elseif eval (quest_editor_id == "MG05Fingers")
                    SetStage MG05Fingers quest_stage_val
                elseif eval (quest_editor_id == "Dark01Knife")
                    SetStage Dark01Knife quest_stage_val
                elseif eval (quest_editor_id == "Dark01KnifeFIN")
                    SetStage Dark01KnifeFIN quest_stage_val
                elseif eval (quest_editor_id == "MS16")
                    SetStage MS16 quest_stage_val
                elseif eval (quest_editor_id == "Dark02Watery")
                    SetStage Dark02Watery quest_stage_val
                elseif eval (quest_editor_id == "Dark02WateryFIN")
                    SetStage Dark02WateryFIN quest_stage_val
                elseif eval (quest_editor_id == "Dark03Accidents")
                    SetStage Dark03Accidents quest_stage_val
                elseif eval (quest_editor_id == "Dark03AccidentsFIN")
                    SetStage Dark03AccidentsFIN quest_stage_val
                elseif eval (quest_editor_id == "Dark04Execution")
                    SetStage Dark04Execution quest_stage_val
                elseif eval (quest_editor_id == "Dark04ExecutionFIN")
                    SetStage Dark04ExecutionFIN quest_stage_val
                elseif eval (quest_editor_id == "FGD01Default")
                    SetStage FGD01Default quest_stage_val
                elseif eval (quest_editor_id == "FGD00JoinFG")
                    SetStage FGD00JoinFG quest_stage_val
                elseif eval (quest_editor_id == "FGD02DandD")
                    SetStage FGD02DandD quest_stage_val
                elseif eval (quest_editor_id == "Dark05Assassinated")
                    SetStage Dark05Assassinated quest_stage_val
                elseif eval (quest_editor_id == "Dark05AssassinatedFIN")
                    SetStage Dark05AssassinatedFIN quest_stage_val
                elseif eval (quest_editor_id == "Dark06Wanderer")
                    SetStage Dark06Wanderer quest_stage_val
                elseif eval (quest_editor_id == "Dark06WandererFIN")
                    SetStage Dark06WandererFIN quest_stage_val
                elseif eval (quest_editor_id == "Dark07Medicine")
                    SetStage Dark07Medicine quest_stage_val
                elseif eval (quest_editor_id == "Dark07MedicineFIN")
                    SetStage Dark07MedicineFIN quest_stage_val
                elseif eval (quest_editor_id == "Dark08Whodunit")
                    SetStage Dark08Whodunit quest_stage_val
                elseif eval (quest_editor_id == "Dark08WhodunitFIN")
                    SetStage Dark08WhodunitFIN quest_stage_val
                elseif eval (quest_editor_id == "Dark09Retirement")
                    SetStage Dark09Retirement quest_stage_val
                elseif eval (quest_editor_id == "Dark09RetirementFIN")
                    SetStage Dark09RetirementFIN quest_stage_val
                elseif eval (quest_editor_id == "Charactergen")
                    SetStage Charactergen quest_stage_val
                elseif eval (quest_editor_id == "MS45")
                    SetStage MS45 quest_stage_val
                elseif eval (quest_editor_id == "FGC06Courier")
                    SetStage FGC06Courier quest_stage_val
                elseif eval (quest_editor_id == "MQ16")
                    SetStage MQ16 quest_stage_val
                elseif eval (quest_editor_id == "MG05A")
                    SetStage MG05A quest_stage_val
                elseif eval (quest_editor_id == "MS38")
                    SetStage MS38 quest_stage_val
                elseif eval (quest_editor_id == "MS21")
                    SetStage MS21 quest_stage_val
                elseif eval (quest_editor_id == "MS22")
                    SetStage MS22 quest_stage_val
                elseif eval (quest_editor_id == "MS16A")
                    SetStage MS16A quest_stage_val
                elseif eval (quest_editor_id == "MS49")
                    SetStage MS49 quest_stage_val
                elseif eval (quest_editor_id == "MS05")
                    SetStage MS05 quest_stage_val
                elseif eval (quest_editor_id == "MG00Join")
                    SetStage MG00Join quest_stage_val
                elseif eval (quest_editor_id == "FGD03Viranus")
                    SetStage FGD03Viranus quest_stage_val
                elseif eval (quest_editor_id == "MG01Destruct")
                    SetStage MG01Destruct quest_stage_val
                elseif eval (quest_editor_id == "FGD04Defector")
                    SetStage FGD04Defector quest_stage_val
                elseif eval (quest_editor_id == "FGD05Oreyn")
                    SetStage FGD05Oreyn quest_stage_val
                elseif eval (quest_editor_id == "FGD06DeadViranus")
                    SetStage FGD06DeadViranus quest_stage_val
                elseif eval (quest_editor_id == "MG02Alter")
                    SetStage MG02Alter quest_stage_val
                elseif eval (quest_editor_id == "MG03Illusion")
                    SetStage MG03Illusion quest_stage_val
                elseif eval (quest_editor_id == "MS27")
                    SetStage MS27 quest_stage_val
                elseif eval (quest_editor_id == "MG04Restore")
                    SetStage MG04Restore quest_stage_val
                elseif eval (quest_editor_id == "MS47")
                    SetStage MS47 quest_stage_val
                elseif eval (quest_editor_id == "Dark10Sanctuary")
                    SetStage Dark10Sanctuary quest_stage_val
                elseif eval (quest_editor_id == "Dark12Harem")
                    SetStage Dark12Harem quest_stage_val
                elseif eval (quest_editor_id == "Dark13Justice")
                    SetStage Dark13Justice quest_stage_val
                elseif eval (quest_editor_id == "Dark14Honor")
                    SetStage Dark14Honor quest_stage_val
                elseif eval (quest_editor_id == "Dark15Coldest")
                    SetStage Dark15Coldest quest_stage_val
                elseif eval (quest_editor_id == "Dark16Kiss")
                    SetStage Dark16Kiss quest_stage_val
                elseif eval (quest_editor_id == "Dark11Kin")
                    SetStage Dark11Kin quest_stage_val
                elseif eval (quest_editor_id == "Dark17Following")
                    SetStage Dark17Following quest_stage_val
                elseif eval (quest_editor_id == "Dark18Mother")
                    SetStage Dark18Mother quest_stage_val
                elseif eval (quest_editor_id == "Dark10SanctuaryFIN")
                    SetStage Dark10SanctuaryFIN quest_stage_val
                elseif eval (quest_editor_id == "MS46")
                    SetStage MS46 quest_stage_val
                elseif eval (quest_editor_id == "Dark10SpecialWizard")
                    SetStage Dark10SpecialWizard quest_stage_val
                elseif eval (quest_editor_id == "MG06Bruma")
                    SetStage MG06Bruma quest_stage_val
                elseif eval (quest_editor_id == "MS51")
                    SetStage MS51 quest_stage_val
                elseif eval (quest_editor_id == "MG07Leyawiin")
                    SetStage MG07Leyawiin quest_stage_val
                elseif eval (quest_editor_id == "TG02taxes")
                    SetStage TG02taxes quest_stage_val
                elseif eval (quest_editor_id == "MG08MagesStaff")
                    SetStage MG08MagesStaff quest_stage_val
                elseif eval (quest_editor_id == "TG03Elven")
                    SetStage TG03Elven quest_stage_val
                elseif eval (quest_editor_id == "MSShadowscale")
                    SetStage MSShadowscale quest_stage_val
                elseif eval (quest_editor_id == "TG00FindThievesGuild")
                    SetStage TG00FindThievesGuild quest_stage_val
                elseif eval (quest_editor_id == "TG01BestThief")
                    SetStage TG01BestThief quest_stage_val
                elseif eval (quest_editor_id == "MG09Motives")
                    SetStage MG09Motives quest_stage_val
                elseif eval (quest_editor_id == "FGD07Kidnap")
                    SetStage FGD07Kidnap quest_stage_val
                elseif eval (quest_editor_id == "FGD08Infiltration")
                    SetStage FGD08Infiltration quest_stage_val
                elseif eval (quest_editor_id == "FGD09Hist")
                    SetStage FGD09Hist quest_stage_val
                elseif eval (quest_editor_id == "FGC01Rats")
                    SetStage FGC01Rats quest_stage_val
                elseif eval (quest_editor_id == "FGC02Protect")
                    SetStage FGC02Protect quest_stage_val
                elseif eval (quest_editor_id == "FGC03Thieves")
                    SetStage FGC03Thieves quest_stage_val
                elseif eval (quest_editor_id == "FGC04Scholar")
                    SetStage FGC04Scholar quest_stage_val
                elseif eval (quest_editor_id == "FGC05Stone")
                    SetStage FGC05Stone quest_stage_val
                elseif eval (quest_editor_id == "FGC07Heirloom")
                    SetStage FGC07Heirloom quest_stage_val
                elseif eval (quest_editor_id == "FGC08Prison")
                    SetStage FGC08Prison quest_stage_val
                elseif eval (quest_editor_id == "FGC09Ogres")
                    SetStage FGC09Ogres quest_stage_val
                elseif eval (quest_editor_id == "FGC10Swamp")
                    SetStage FGC10Swamp quest_stage_val
                elseif eval (quest_editor_id == "TG04Mistake")
                    SetStage TG04Mistake quest_stage_val
                elseif eval (quest_editor_id == "MG10Vahtacen")
                    SetStage MG10Vahtacen quest_stage_val
                elseif eval (quest_editor_id == "MG11NecroMoon")
                    SetStage MG11NecroMoon quest_stage_val
                elseif eval (quest_editor_id == "TG05Misdirection")
                    SetStage TG05Misdirection quest_stage_val
                elseif eval (quest_editor_id == "TG06Atonement")
                    SetStage TG06Atonement quest_stage_val
                elseif eval (quest_editor_id == "TG07Lex")
                    SetStage TG07Lex quest_stage_val
                elseif eval (quest_editor_id == "TG08Blind")
                    SetStage TG08Blind quest_stage_val
                elseif eval (quest_editor_id == "TG09Arrow")
                    SetStage TG09Arrow quest_stage_val
                elseif eval (quest_editor_id == "TG10Boots")
                    SetStage TG10Boots quest_stage_val
                elseif eval (quest_editor_id == "TG11Heist")
                    SetStage TG11Heist quest_stage_val
                elseif eval (quest_editor_id == "MS02")
                    SetStage MS02 quest_stage_val
                elseif eval (quest_editor_id == "FGC03Flagon")
                    SetStage FGC03Flagon quest_stage_val
                elseif eval (quest_editor_id == "MG12Gate")
                    SetStage MG12Gate quest_stage_val
                elseif eval (quest_editor_id == "MG13Information")
                    SetStage MG13Information quest_stage_val
                elseif eval (quest_editor_id == "MG14Plot")
                    SetStage MG14Plot quest_stage_val
                elseif eval (quest_editor_id == "MG15Helm")
                    SetStage MG15Helm quest_stage_val
                elseif eval (quest_editor_id == "MG16Amulet")
                    SetStage MG16Amulet quest_stage_val
                elseif eval (quest_editor_id == "MG17Ambush")
                    SetStage MG17Ambush quest_stage_val
                elseif eval (quest_editor_id == "MG18KingofWorms")
                    SetStage MG18KingofWorms quest_stage_val
                elseif eval (quest_editor_id == "MS31")
                    SetStage MS31 quest_stage_val
                elseif eval (quest_editor_id == "MQ15")
                    SetStage MQ15 quest_stage_val
                elseif eval (quest_editor_id == "DAAzura")
                    SetStage DAAzura quest_stage_val
                elseif eval (quest_editor_id == "DABoethia")
                    SetStage DABoethia quest_stage_val
                elseif eval (quest_editor_id == "DAClavicusVile")
                    SetStage DAClavicusVile quest_stage_val
                elseif eval (quest_editor_id == "DAHircine")
                    SetStage DAHircine quest_stage_val
                elseif eval (quest_editor_id == "DAMalacath")
                    SetStage DAMalacath quest_stage_val
                elseif eval (quest_editor_id == "DAMephala")
                    SetStage DAMephala quest_stage_val
                elseif eval (quest_editor_id == "DAMeridia")
                    SetStage DAMeridia quest_stage_val
                elseif eval (quest_editor_id == "DANamira")
                    SetStage DANamira quest_stage_val
                elseif eval (quest_editor_id == "DANocturnal")
                    SetStage DANocturnal quest_stage_val
                elseif eval (quest_editor_id == "DAPeryite")
                    SetStage DAPeryite quest_stage_val
                elseif eval (quest_editor_id == "DASanguine")
                    SetStage DASanguine quest_stage_val
                elseif eval (quest_editor_id == "DASheogorath")
                    SetStage DASheogorath quest_stage_val
                elseif eval (quest_editor_id == "DAVaermina")
                    SetStage DAVaermina quest_stage_val
                elseif eval (quest_editor_id == "DAHermaeusMora")
                    SetStage DAHermaeusMora quest_stage_val
                elseif eval (quest_editor_id == "DAMolagBal")
                    SetStage DAMolagBal quest_stage_val
                elseif eval (quest_editor_id == "MS14")
                    SetStage MS14 quest_stage_val
                elseif eval (quest_editor_id == "MS12")
                    SetStage MS12 quest_stage_val
                elseif eval (quest_editor_id == "MS09")
                    SetStage MS09 quest_stage_val
                elseif eval (quest_editor_id == "MS10")
                    SetStage MS10 quest_stage_val
                elseif eval (quest_editor_id == "MS11")
                    SetStage MS11 quest_stage_val
                elseif eval (quest_editor_id == "MS23")
                    SetStage MS23 quest_stage_val
                elseif eval (quest_editor_id == "MQ01")
                    SetStage MQ01 quest_stage_val
                elseif eval (quest_editor_id == "MQ02")
                    SetStage MQ02 quest_stage_val
                elseif eval (quest_editor_id == "MQ03")
                    SetStage MQ03 quest_stage_val
                elseif eval (quest_editor_id == "MQ04")
                    SetStage MQ04 quest_stage_val
                elseif eval (quest_editor_id == "MQ05")
                    SetStage MQ05 quest_stage_val
                elseif eval (quest_editor_id == "MQ06")
                    SetStage MQ06 quest_stage_val
                elseif eval (quest_editor_id == "MQ07")
                    SetStage MQ07 quest_stage_val
                elseif eval (quest_editor_id == "MQ08")
                    SetStage MQ08 quest_stage_val
                elseif eval (quest_editor_id == "MQ09")
                    SetStage MQ09 quest_stage_val
                elseif eval (quest_editor_id == "MQ10")
                    SetStage MQ10 quest_stage_val
                elseif eval (quest_editor_id == "MQ11")
                    SetStage MQ11 quest_stage_val
                elseif eval (quest_editor_id == "MQ12")
                    SetStage MQ12 quest_stage_val
                elseif eval (quest_editor_id == "MQ13")
                    SetStage MQ13 quest_stage_val
                elseif eval (quest_editor_id == "MQ14")
                    SetStage MQ14 quest_stage_val
                elseif eval (quest_editor_id == "TGStolenGoods")
                    SetStage TGStolenGoods quest_stage_val
                elseif eval (quest_editor_id == "MS08")
                    SetStage MS08 quest_stage_val
                elseif eval (quest_editor_id == "MS37")
                    SetStage MS37 quest_stage_val
                elseif eval (quest_editor_id == "MGExpulsion01")
                    SetStage MGExpulsion01 quest_stage_val
                elseif eval (quest_editor_id == "MGExpulsion02")
                    SetStage MGExpulsion02 quest_stage_val
                elseif eval (quest_editor_id == "DarkExile")
                    SetStage DarkExile quest_stage_val
                elseif eval (quest_editor_id == "DarkExile2")
                    SetStage DarkExile2 quest_stage_val
                elseif eval (quest_editor_id == "DarkExile3")
                    SetStage DarkExile3 quest_stage_val
                elseif eval (quest_editor_id == "MS29")
                    SetStage MS29 quest_stage_val
                elseif eval (quest_editor_id == "FG00Expulsion")
                    SetStage FG00Expulsion quest_stage_val
                elseif eval (quest_editor_id == "MS13")
                    SetStage MS13 quest_stage_val
                elseif eval (quest_editor_id == "MS06")
                    SetStage MS06 quest_stage_val
                elseif eval (quest_editor_id == "MS26")
                    SetStage MS26 quest_stage_val
                elseif eval (quest_editor_id == "MS52")
                    SetStage MS52 quest_stage_val
                elseif eval (quest_editor_id == "MS04")
                    SetStage MS04 quest_stage_val
                elseif eval (quest_editor_id == "TrainingMarksman")
                    SetStage TrainingMarksman quest_stage_val
                elseif eval (quest_editor_id == "MS40")
                    SetStage MS40 quest_stage_val
                elseif eval (quest_editor_id == "DarkVamp")
                    SetStage DarkVamp quest_stage_val
                elseif eval (quest_editor_id == "MS39")
                    SetStage MS39 quest_stage_val
                elseif eval (quest_editor_id == "MS43")
                    SetStage MS43 quest_stage_val
                elseif eval (quest_editor_id == "HouseLeyawiin")
                    SetStage HouseLeyawiin quest_stage_val
                elseif eval (quest_editor_id == "HouseBravil")
                    SetStage HouseBravil quest_stage_val
                elseif eval (quest_editor_id == "HouseBruma")
                    SetStage HouseBruma quest_stage_val
                elseif eval (quest_editor_id == "HouseChorrol")
                    SetStage HouseChorrol quest_stage_val
                elseif eval (quest_editor_id == "HouseCheydinhal")
                    SetStage HouseCheydinhal quest_stage_val
                elseif eval (quest_editor_id == "HouseSkingrad")
                    SetStage HouseSkingrad quest_stage_val
                elseif eval (quest_editor_id == "HouseImperialCity")
                    SetStage HouseImperialCity quest_stage_val
                elseif eval (quest_editor_id == "MS91")
                    SetStage MS91 quest_stage_val
                elseif eval (quest_editor_id == "MS92")
                    SetStage MS92 quest_stage_val
                elseif eval (quest_editor_id == "MS93")
                    SetStage MS93 quest_stage_val
                elseif eval (quest_editor_id == "MS94")
                    SetStage MS94 quest_stage_val
                elseif eval (quest_editor_id == "MG19Alchemy")
                    SetStage MG19Alchemy quest_stage_val
                elseif eval (quest_editor_id == "HouseSkingradQuest")
                    SetStage HouseSkingradQuest quest_stage_val
                elseif eval (quest_editor_id == "HouseServant")
                    SetStage HouseServant quest_stage_val
                elseif eval (quest_editor_id == "SQ01")
                    SetStage SQ01 quest_stage_val
                elseif eval (quest_editor_id == "SQ02")
                    SetStage SQ02 quest_stage_val
                elseif eval (quest_editor_id == "SQ03")
                    SetStage SQ03 quest_stage_val
                elseif eval (quest_editor_id == "SQ04")
                    SetStage SQ04 quest_stage_val
                elseif eval (quest_editor_id == "SQ05")
                    SetStage SQ05 quest_stage_val
                elseif eval (quest_editor_id == "SQ06")
                    SetStage SQ06 quest_stage_val
                elseif eval (quest_editor_id == "SQ07")
                    SetStage SQ07 quest_stage_val
                elseif eval (quest_editor_id == "SQ08")
                    SetStage SQ08 quest_stage_val
                elseif eval (quest_editor_id == "SQ09")
                    SetStage SQ09 quest_stage_val
                elseif eval (quest_editor_id == "SQ10")
                    SetStage SQ10 quest_stage_val
                elseif eval (quest_editor_id == "TrainingDestruction")
                    SetStage TrainingDestruction quest_stage_val
                elseif eval (quest_editor_id == "TrainingHeavyArmor")
                    SetStage TrainingHeavyArmor quest_stage_val
                elseif eval (quest_editor_id == "TrainingAlchemy")
                    SetStage TrainingAlchemy quest_stage_val
                elseif eval (quest_editor_id == "TrainingAcrobatics")
                    SetStage TrainingAcrobatics quest_stage_val
                elseif eval (quest_editor_id == "TrainingAlteration")
                    SetStage TrainingAlteration quest_stage_val
                elseif eval (quest_editor_id == "TrainingArmorer")
                    SetStage TrainingArmorer quest_stage_val
                elseif eval (quest_editor_id == "TrainingAthletics")
                    SetStage TrainingAthletics quest_stage_val
                elseif eval (quest_editor_id == "TrainingBlade")
                    SetStage TrainingBlade quest_stage_val
                elseif eval (quest_editor_id == "TrainingBlock")
                    SetStage TrainingBlock quest_stage_val
                elseif eval (quest_editor_id == "TrainingBlunt")
                    SetStage TrainingBlunt quest_stage_val
                elseif eval (quest_editor_id == "TrainingConjuration")
                    SetStage TrainingConjuration quest_stage_val
                elseif eval (quest_editor_id == "TrainingHandtoHand")
                    SetStage TrainingHandtoHand quest_stage_val
                elseif eval (quest_editor_id == "TrainingIllusion")
                    SetStage TrainingIllusion quest_stage_val
                elseif eval (quest_editor_id == "TrainingLightArmor")
                    SetStage TrainingLightArmor quest_stage_val
                elseif eval (quest_editor_id == "TrainingMercantile")
                    SetStage TrainingMercantile quest_stage_val
                elseif eval (quest_editor_id == "TrainingMysticism")
                    SetStage TrainingMysticism quest_stage_val
                elseif eval (quest_editor_id == "TrainingRestoration")
                    SetStage TrainingRestoration quest_stage_val
                elseif eval (quest_editor_id == "TrainingSecurity")
                    SetStage TrainingSecurity quest_stage_val
                elseif eval (quest_editor_id == "TrainingSneak")
                    SetStage TrainingSneak quest_stage_val
                elseif eval (quest_editor_id == "TrainingSpeechcraft")
                    SetStage TrainingSpeechcraft quest_stage_val
                elseif eval (quest_editor_id == "MQDragonArmor")
                    SetStage MQDragonArmor quest_stage_val
                elseif eval (quest_editor_id == "TG04BloodPrice")
                    SetStage TG04BloodPrice quest_stage_val
                elseif eval (quest_editor_id == "TG05BloodPrice")
                    SetStage TG05BloodPrice quest_stage_val
                elseif eval (quest_editor_id == "TG06BloodPrice")
                    SetStage TG06BloodPrice quest_stage_val
                elseif eval (quest_editor_id == "TG02BloodPrice")
                    SetStage TG02BloodPrice quest_stage_val
                elseif eval (quest_editor_id == "TG07BloodPrice")
                    SetStage TG07BloodPrice quest_stage_val
                elseif eval (quest_editor_id == "TG01BloodPrice")
                    SetStage TG01BloodPrice quest_stage_val
                elseif eval (quest_editor_id == "TG08BloodPrice")
                    SetStage TG08BloodPrice quest_stage_val
                elseif eval (quest_editor_id == "TG09BloodPrice")
                    SetStage TG09BloodPrice quest_stage_val
                elseif eval (quest_editor_id == "TG03BloodPrice")
                    SetStage TG03BloodPrice quest_stage_val
                elseif eval (quest_editor_id == "TG10BloodPrice")
                    SetStage TG10BloodPrice quest_stage_val
                elseif eval (quest_editor_id == "SE02")
                    SetStage SE02 quest_stage_val
                elseif eval (quest_editor_id == "SE32")
                    SetStage SE32 quest_stage_val
                elseif eval (quest_editor_id == "SE06")
                    SetStage SE06 quest_stage_val
                elseif eval (quest_editor_id == "SE04")
                    SetStage SE04 quest_stage_val
                elseif eval (quest_editor_id == "SE01Door")
                    SetStage SE01Door quest_stage_val
                elseif eval (quest_editor_id == "SE05")
                    SetStage SE05 quest_stage_val
                elseif eval (quest_editor_id == "SE10")
                    SetStage SE10 quest_stage_val
                elseif eval (quest_editor_id == "SE09")
                    SetStage SE09 quest_stage_val
                elseif eval (quest_editor_id == "SE11")
                    SetStage SE11 quest_stage_val
                elseif eval (quest_editor_id == "SE08")
                    SetStage SE08 quest_stage_val
                elseif eval (quest_editor_id == "SE03")
                    SetStage SE03 quest_stage_val
                elseif eval (quest_editor_id == "SE07A")
                    SetStage SE07A quest_stage_val
                elseif eval (quest_editor_id == "SE07B")
                    SetStage SE07B quest_stage_val
                elseif eval (quest_editor_id == "SE11a")
                    SetStage SE11a quest_stage_val
                elseif eval (quest_editor_id == "SE11b")
                    SetStage SE11b quest_stage_val
                elseif eval (quest_editor_id == "SE12")
                    SetStage SE12 quest_stage_val
                elseif eval (quest_editor_id == "SE13")
                    SetStage SE13 quest_stage_val
                elseif eval (quest_editor_id == "SE03A")
                    SetStage SE03A quest_stage_val
                elseif eval (quest_editor_id == "SE14")
                    SetStage SE14 quest_stage_val
                elseif eval (quest_editor_id == "SE39")
                    SetStage SE39 quest_stage_val
                elseif eval (quest_editor_id == "SE35")
                    SetStage SE35 quest_stage_val
                elseif eval (quest_editor_id == "SE37")
                    SetStage SE37 quest_stage_val
                elseif eval (quest_editor_id == "SE04Shell")
                    SetStage SE04Shell quest_stage_val
                elseif eval (quest_editor_id == "SE36")
                    SetStage SE36 quest_stage_val
                elseif eval (quest_editor_id == "SEObelisks")
                    SetStage SEObelisks quest_stage_val
                elseif eval (quest_editor_id == "SE34")
                    SetStage SE34 quest_stage_val
                elseif eval (quest_editor_id == "SE41")
                    SetStage SE41 quest_stage_val
                elseif eval (quest_editor_id == "SE43")
                    SetStage SE43 quest_stage_val
                elseif eval (quest_editor_id == "SE42")
                    SetStage SE42 quest_stage_val
                elseif eval (quest_editor_id == "SE44")
                    SetStage SE44 quest_stage_val
                elseif eval (quest_editor_id == "SE45")
                    SetStage SE45 quest_stage_val
                elseif eval (quest_editor_id == "SE40")
                    SetStage SE40 quest_stage_val
                elseif eval (quest_editor_id == "SE46")
                    SetStage SE46 quest_stage_val
                elseif eval (quest_editor_id == "aaaFillingInForms")
                    PrintC "Skipping custom/mod quest: %z" quest_editor_id
                else
                    PrintC "Quest not in vanilla list: %z" quest_editor_id
                endif
            endif
        endif
    Loop

    PrintC "=== PARSING LOOP COMPLETE ==="

    ; Cleanup
    UnregisterLog $log_name 0 0
    PrintC "UnregisterLog called"

    ; Destruct string variables
    sv_Destruct log_name str_current_line parse_mode
    sv_Destruct item_formID_str spell_formID_str
    sv_Destruct attr_name skill_name
    sv_Destruct faction_formID_str quest_editor_id

    PrintC "=== VARLA IMPORT SCRIPT END ==="
End