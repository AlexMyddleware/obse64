/*
 * OBSE64 File I/O Example Plugin
 *
 * This plugin demonstrates how to:
 * - Write data to files
 * - Read data from files
 * - Parse configuration files
 * - Handle errors gracefully
 * - Import/Export player data
 *
 * EVERY LINE IS EXPLAINED - Perfect for beginners!
 */

// ============================================================================
// INCLUDES - Tell the compiler what code we're using
// ============================================================================

// OBSE64 Plugin API headers
#include "obse64/PluginAPI.h"      // Main plugin interface
#include "obse64/GameScript.h"     // Script/command structures
#include "obse64/GameConsole.h"    // Console_Print function
#include "obse64/GameForms.h"      // Game forms
#include "obse64/GameObjects.h"    // PlayerCharacter and other objects

// Standard C++ file I/O
// <fstream> provides:
//   - std::ofstream: Output file stream (for writing)
//   - std::ifstream: Input file stream (for reading)
//   - std::fstream:  Both input and output
#include <fstream>

// Standard C++ string manipulation
// <string> provides:
//   - std::string: Dynamic string class
//   - string.c_str(): Convert to C-style char*
//   - string.substr(): Extract substring
#include <string>

// Standard C++ string streams
// <sstream> provides:
//   - std::stringstream: String as a stream for parsing
//   - std::istringstream: Input string stream
//   - std::ostringstream: Output string stream
#include <sstream>

// Standard C++ vector (dynamic array)
// <vector> provides:
//   - std::vector<T>: Dynamic array that grows/shrinks
//   - push_back(): Add element to end
//   - size(): Get number of elements
#include <vector>

// Standard C++ algorithm utilities
// <algorithm> provides:
//   - std::find(): Search for element
//   - std::sort(): Sort elements
//   - std::remove(): Remove elements
#include <algorithm>

// Windows API for directory creation
// <direct.h> provides:
//   - _mkdir(): Create directory
#include <direct.h>

// ============================================================================
// DATA STRUCTURES
// ============================================================================

/*
 * PluginConfig
 *
 * PURPOSE: Store plugin configuration settings
 *
 * This structure holds all the settings that can be loaded from
 * the configuration file and saved back to it.
 */
struct PluginConfig {
    // Whether to enable debug logging
    bool enableDebug;

    // How often to auto-save (in seconds)
    float autoSaveInterval;

    // Maximum number of log entries to keep
    int maxLogEntries;

    // Custom player name override
    std::string customName;

    // Constructor - sets default values
    PluginConfig()
        : enableDebug(false)           // Default: debug off
        , autoSaveInterval(60.0f)      // Default: save every 60 seconds
        , maxLogEntries(1000)          // Default: keep 1000 entries
        , customName("Player")         // Default: "Player"
    {
        // All member variables are initialized above
        // This is called "member initializer list"
    }
};

/*
 * CharacterStats
 *
 * PURPOSE: Store character statistics for import/export
 *
 * This structure can be written to a file and read back later
 * to save/restore character stats.
 */
struct CharacterStats {
    // Attributes
    float strength;
    float intelligence;
    float willpower;
    float agility;
    float speed;
    float endurance;
    float personality;
    float luck;

    // Vital stats
    float health;
    float magicka;
    float fatigue;

    // Additional info
    int level;
    std::string characterName;
};

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

// Plugin handle (unique ID assigned by OBSE)
static PluginHandle g_pluginHandle = kPluginHandle_Invalid;

// Messaging interface (for receiving OBSE messages)
static OBSEMessagingInterface* g_messagingInterface = nullptr;

// Plugin configuration (loaded from file)
static PluginConfig g_config;

// Item blacklist (loaded from file)
static std::vector<std::string> g_itemBlacklist;

// ============================================================================
// HELPER FUNCTIONS - FILE PATHS
// ============================================================================

/*
 * GetPluginDirectory
 *
 * PURPOSE: Get the path to our plugin's data directory
 *
 * RETURNS: Path string like "Data\\OBSE\\Plugins\\FileIOExample\\"
 *
 * WHY THIS FUNCTION?
 * - Centralizes path management
 * - Makes it easy to change base directory
 * - Ensures consistent paths throughout plugin
 */
std::string GetPluginDirectory()
{
    // Our plugin stores data in: Data\OBSE\Plugins\FileIOExample\
    // The double backslashes become single backslashes in the actual string
    return "Data\\OBSE\\Plugins\\FileIOExample\\";
}

/*
 * EnsureDirectoryExists
 *
 * PURPOSE: Create directory if it doesn't exist
 *
 * PARAMETERS:
 *   path - Directory path to create
 *
 * RETURNS: true if directory exists or was created successfully
 *
 * WHY THIS FUNCTION?
 * - File streams don't create directories automatically
 * - We need to create the directory before writing files to it
 * - Prevents "file not found" errors when directory is missing
 */
bool EnsureDirectoryExists(const char* path)
{
    // _mkdir() creates a directory
    // Returns:
    //   0 on success
    //   -1 if directory already exists (this is OK!)
    //   -1 on other errors (no permission, invalid path, etc.)
    //
    // Note: _mkdir only creates one level at a time
    // For nested directories, you need multiple calls

    int result = _mkdir(path);

    // Even if result is -1, the directory might already exist
    // We'll check by trying to create a test file
    // (There's no simple cross-platform "does directory exist" function)

    if (result == 0) {
        // Successfully created
        _MESSAGE("Created directory: %s", path);
        return true;
    }

    // If _mkdir failed, directory might already exist (which is fine)
    // We'll just assume it exists and try to use it
    // File operations will fail gracefully if it doesn't
    return true;
}

// ============================================================================
// HELPER FUNCTIONS - STRING MANIPULATION
// ============================================================================

/*
 * TrimWhitespace
 *
 * PURPOSE: Remove leading and trailing whitespace from a string
 *
 * PARAMETERS:
 *   str - String to trim
 *
 * RETURNS: Trimmed string
 *
 * WHY THIS FUNCTION?
 * - Users might add extra spaces in config files
 * - Makes parsing more robust
 * - "  value  " becomes "value"
 */
std::string TrimWhitespace(const std::string& str)
{
    // Find first non-whitespace character
    // Whitespace: space, tab, newline, etc.
    size_t start = str.find_first_not_of(" \t\r\n");

    // If string is all whitespace, return empty string
    if (start == std::string::npos) {
        return "";
    }

    // Find last non-whitespace character
    size_t end = str.find_last_not_of(" \t\r\n");

    // Extract substring from first to last non-whitespace character
    // substr(start, length)
    return str.substr(start, end - start + 1);
}

/*
 * StringToBool
 *
 * PURPOSE: Convert string to boolean value
 *
 * PARAMETERS:
 *   str - String like "true", "1", "yes", "on"
 *
 * RETURNS: true or false
 *
 * WHY THIS FUNCTION?
 * - Config files store booleans as text
 * - Supports multiple formats (true/1/yes/on)
 * - Case-insensitive
 */
bool StringToBool(const std::string& str)
{
    // Convert to lowercase for comparison
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    // Check for various "true" representations
    return (lower == "true" ||
            lower == "1" ||
            lower == "yes" ||
            lower == "on");
}

// ============================================================================
// CONFIGURATION FILE I/O
// ============================================================================

/*
 * SaveConfig
 *
 * PURPOSE: Save plugin configuration to file
 *
 * PARAMETERS:
 *   filename - Path to config file
 *   config - Configuration structure to save
 *
 * RETURNS: true if saved successfully
 *
 * FILE FORMAT:
 *   # Comment lines start with #
 *   Key=Value
 *   Key=Value
 */
bool SaveConfig(const char* filename, const PluginConfig& config)
{
    _MESSAGE("Saving config to: %s", filename);

    // ========================================================================
    // STEP 1: Open file for writing
    // ========================================================================

    // std::ofstream = Output File Stream
    // Opens file for writing (creates if doesn't exist, overwrites if exists)
    std::ofstream outFile(filename);

    // Check if file opened successfully
    if (!outFile.is_open()) {
        _MESSAGE("ERROR: Could not open config file for writing");
        return false;
    }

    // ========================================================================
    // STEP 2: Write header comment
    // ========================================================================

    // << is the stream insertion operator
    // It writes data to the stream (file)
    // \n is newline character
    outFile << "# File I/O Example Plugin Configuration\n";
    outFile << "# Edit values below to customize behavior\n";
    outFile << "#\n";
    outFile << "# Boolean values: true/false or 1/0\n";
    outFile << "# Numbers: integers or decimals\n";
    outFile << "# Strings: any text\n";
    outFile << "\n";

    // ========================================================================
    // STEP 3: Write configuration values
    // ========================================================================

    // Boolean: convert true/false to 1/0
    outFile << "EnableDebug=" << (config.enableDebug ? "1" : "0") << "\n";
    //          ↑              ↑                                      ↑
    //          Key            Value (ternary operator)               Newline
    //
    // Ternary operator: condition ? value_if_true : value_if_false
    // If enableDebug is true, writes "1", else writes "0"

    // Float: writes decimal number
    outFile << "AutoSaveInterval=" << config.autoSaveInterval << "\n";

    // Integer: writes whole number
    outFile << "MaxLogEntries=" << config.maxLogEntries << "\n";

    // String: writes text
    outFile << "CustomName=" << config.customName << "\n";

    // ========================================================================
    // STEP 4: Close file and report success
    // ========================================================================

    // Close the file
    // This ensures all data is flushed from buffer to disk
    outFile.close();

    _MESSAGE("Config saved successfully");
    return true;
}

/*
 * LoadConfig
 *
 * PURPOSE: Load plugin configuration from file
 *
 * PARAMETERS:
 *   filename - Path to config file
 *   config - Configuration structure to fill (output parameter)
 *
 * RETURNS: true if loaded successfully (false = use defaults)
 *
 * HOW IT WORKS:
 *   1. Open file for reading
 *   2. Read line by line
 *   3. Skip comments and empty lines
 *   4. Parse Key=Value pairs
 *   5. Update config structure
 */
bool LoadConfig(const char* filename, PluginConfig& config)
{
    _MESSAGE("Loading config from: %s", filename);

    // ========================================================================
    // STEP 1: Open file for reading
    // ========================================================================

    // std::ifstream = Input File Stream
    // Opens file for reading (file must exist!)
    std::ifstream inFile(filename);

    // Check if file opened successfully
    if (!inFile.is_open()) {
        _MESSAGE("Config file not found, using defaults");
        return false;  // Not an error - just use default config
    }

    // ========================================================================
    // STEP 2: Read and parse line by line
    // ========================================================================

    std::string line;  // Variable to hold each line

    // std::getline(stream, string) reads one line from stream
    // Returns false when end of file is reached
    // Perfect for reading config files line by line
    while (std::getline(inFile, line)) {
        //    ↑          ↑       ↑
        //    |          |       └─ Variable to store line in
        //    |          └─ File stream to read from
        //    └─ Function that reads until newline

        // --------------------------------------------------------------------
        // Skip empty lines
        // --------------------------------------------------------------------

        // Trim whitespace first (removes leading/trailing spaces)
        line = TrimWhitespace(line);

        // If line is empty after trimming, skip it
        if (line.empty()) {
            continue;  // Skip to next iteration of while loop
        }

        // --------------------------------------------------------------------
        // Skip comment lines
        // --------------------------------------------------------------------

        // Comment lines start with # or ;
        if (line[0] == '#' || line[0] == ';') {
            //  ↑          ↑
            //  |          └─ Check if first character is ;
            //  └─ line[0] is the first character (index 0)
            continue;
        }

        // --------------------------------------------------------------------
        // Parse Key=Value pair
        // --------------------------------------------------------------------

        // Find the = character
        // find() returns position of character, or std::string::npos if not found
        size_t equalPos = line.find('=');

        // If no = found, skip this line (malformed)
        if (equalPos == std::string::npos) {
            _MESSAGE("WARNING: Skipping malformed line: %s", line.c_str());
            continue;
        }

        // Split line into key and value
        // substr(start, length) extracts substring
        std::string key = line.substr(0, equalPos);          // Everything before =
        std::string value = line.substr(equalPos + 1);       // Everything after =
        //                                 ↑
        //                                 +1 to skip the = itself

        // Trim whitespace from key and value
        // This handles "Key = Value" or "Key=Value" equally
        key = TrimWhitespace(key);
        value = TrimWhitespace(value);

        // --------------------------------------------------------------------
        // Update config based on key
        // --------------------------------------------------------------------

        // Compare key to known configuration options
        if (key == "EnableDebug") {
            config.enableDebug = StringToBool(value);
            _MESSAGE("  EnableDebug = %s", config.enableDebug ? "true" : "false");
        }
        else if (key == "AutoSaveInterval") {
            // std::stof = String TO Float
            // Converts string like "60.5" to float 60.5
            config.autoSaveInterval = std::stof(value);
            _MESSAGE("  AutoSaveInterval = %.1f", config.autoSaveInterval);
        }
        else if (key == "MaxLogEntries") {
            // std::stoi = String TO Integer
            // Converts string like "1000" to int 1000
            config.maxLogEntries = std::stoi(value);
            _MESSAGE("  MaxLogEntries = %d", config.maxLogEntries);
        }
        else if (key == "CustomName") {
            config.customName = value;
            _MESSAGE("  CustomName = %s", config.customName.c_str());
        }
        else {
            // Unknown key - log warning but continue
            _MESSAGE("WARNING: Unknown config key: %s", key.c_str());
        }
    }

    // ========================================================================
    // STEP 3: Close file and report success
    // ========================================================================

    inFile.close();
    _MESSAGE("Config loaded successfully");
    return true;
}

// ============================================================================
// CHARACTER STATS I/O
// ============================================================================

/*
 * ExportCharacterStats
 *
 * PURPOSE: Export current player stats to a file
 *
 * PARAMETERS:
 *   filename - Path to export file
 *   player - Pointer to player character
 *
 * RETURNS: true if exported successfully
 *
 * FILE FORMAT: Human-readable text file with stats
 */
bool ExportCharacterStats(const char* filename, PlayerCharacter* player)
{
    _MESSAGE("Exporting character stats to: %s", filename);

    // ========================================================================
    // STEP 1: Validate player pointer
    // ========================================================================

    if (!player) {
        _MESSAGE("ERROR: Player pointer is null!");
        return false;
    }

    // ========================================================================
    // STEP 2: Gather stats from player
    // ========================================================================

    // Create a CharacterStats structure to hold the data
    CharacterStats stats;

    // Read attributes using GetActorValue()
    // Actor values are identified by numeric IDs
    stats.strength = player->GetActorValue(0);      // kActorVal_Strength
    stats.intelligence = player->GetActorValue(1);  // kActorVal_Intelligence
    stats.willpower = player->GetActorValue(2);     // kActorVal_Willpower
    stats.agility = player->GetActorValue(3);       // kActorVal_Agility
    stats.speed = player->GetActorValue(4);         // kActorVal_Speed
    stats.endurance = player->GetActorValue(5);     // kActorVal_Endurance
    stats.personality = player->GetActorValue(6);   // kActorVal_Personality
    stats.luck = player->GetActorValue(7);          // kActorVal_Luck

    // Read vital stats
    stats.health = player->GetActorValue(8);        // kActorVal_Health
    stats.magicka = player->GetActorValue(9);       // kActorVal_Magicka
    stats.fatigue = player->GetActorValue(10);      // kActorVal_Fatigue

    // Get player level
    // GetLevel() is a function on Actor class
    stats.level = player->GetLevel();

    // Get character name
    // GetName() returns const char*, we convert to std::string
    const char* name = player->GetName();
    stats.characterName = name ? name : "Unknown";  // Handle null name

    // ========================================================================
    // STEP 3: Open file for writing
    // ========================================================================

    std::ofstream outFile(filename);

    if (!outFile.is_open()) {
        _MESSAGE("ERROR: Could not create export file");
        return false;
    }

    // ========================================================================
    // STEP 4: Write stats to file in readable format
    // ========================================================================

    // Include iomanip for formatting
    #include <iomanip>

    // Write header
    outFile << "========================================\n";
    outFile << "CHARACTER STATS EXPORT\n";
    outFile << "========================================\n\n";

    // Write character info
    outFile << "Character Name: " << stats.characterName << "\n";
    outFile << "Level: " << stats.level << "\n\n";

    // Write attributes
    outFile << "ATTRIBUTES:\n";

    // Use std::fixed and std::setprecision for consistent decimal formatting
    outFile << std::fixed << std::setprecision(1);
    //         ↑              ↑
    //         |              └─ Show 1 decimal place (50.0 instead of 50)
    //         └─ Use fixed-point notation (not scientific)

    outFile << "  Strength:     " << stats.strength << "\n";
    outFile << "  Intelligence: " << stats.intelligence << "\n";
    outFile << "  Willpower:    " << stats.willpower << "\n";
    outFile << "  Agility:      " << stats.agility << "\n";
    outFile << "  Speed:        " << stats.speed << "\n";
    outFile << "  Endurance:    " << stats.endurance << "\n";
    outFile << "  Personality:  " << stats.personality << "\n";
    outFile << "  Luck:         " << stats.luck << "\n\n";

    // Write vital stats
    outFile << "VITAL STATS:\n";
    outFile << "  Health:  " << stats.health << "\n";
    outFile << "  Magicka: " << stats.magicka << "\n";
    outFile << "  Fatigue: " << stats.fatigue << "\n";

    // ========================================================================
    // STEP 5: Close file and report success
    // ========================================================================

    outFile.close();
    _MESSAGE("Character stats exported successfully");
    return true;
}

/*
 * ImportCharacterStats
 *
 * PURPOSE: Import player stats from a file
 *
 * PARAMETERS:
 *   filename - Path to import file
 *   player - Pointer to player character
 *
 * RETURNS: true if imported successfully
 *
 * HOW IT WORKS:
 *   1. Read file line by line
 *   2. Parse "StatName: Value" format
 *   3. Set actor values on player
 */
bool ImportCharacterStats(const char* filename, PlayerCharacter* player)
{
    _MESSAGE("Importing character stats from: %s", filename);

    // ========================================================================
    // STEP 1: Validate player pointer
    // ========================================================================

    if (!player) {
        _MESSAGE("ERROR: Player pointer is null!");
        return false;
    }

    // ========================================================================
    // STEP 2: Open file for reading
    // ========================================================================

    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        _MESSAGE("ERROR: Import file not found");
        return false;
    }

    // ========================================================================
    // STEP 3: Read and parse file
    // ========================================================================

    std::string line;
    int statsImported = 0;

    while (std::getline(inFile, line)) {
        // Trim whitespace
        line = TrimWhitespace(line);

        // Skip empty lines, headers, and decorative lines
        if (line.empty() || line[0] == '=' || line[0] == '#') {
            continue;
        }

        // Skip section headers (lines with only letters and colons)
        // We're looking for "Name: Value" format
        if (line.find(':') == std::string::npos) {
            continue;
        }

        // Parse "StatName: Value" format
        size_t colonPos = line.find(':');
        std::string statName = TrimWhitespace(line.substr(0, colonPos));
        std::string valueStr = TrimWhitespace(line.substr(colonPos + 1));

        // Skip if value is empty
        if (valueStr.empty()) continue;

        // Convert value to float
        float value;
        try {
            value = std::stof(valueStr);
        }
        catch (...) {
            // std::stof throws exception if conversion fails
            _MESSAGE("WARNING: Could not parse value: %s", valueStr.c_str());
            continue;
        }

        // Map stat name to actor value ID
        int actorValueID = -1;

        if (statName == "Strength") actorValueID = 0;
        else if (statName == "Intelligence") actorValueID = 1;
        else if (statName == "Willpower") actorValueID = 2;
        else if (statName == "Agility") actorValueID = 3;
        else if (statName == "Speed") actorValueID = 4;
        else if (statName == "Endurance") actorValueID = 5;
        else if (statName == "Personality") actorValueID = 6;
        else if (statName == "Luck") actorValueID = 7;
        else if (statName == "Health") actorValueID = 8;
        else if (statName == "Magicka") actorValueID = 9;
        else if (statName == "Fatigue") actorValueID = 10;

        // If we found a matching stat, set it
        if (actorValueID >= 0) {
            // SetActorValue modifies the actor's stat
            // Note: This function may not exist on all Actor classes
            // You might need to use ModActorValue instead
            player->SetActorValue(actorValueID, value);
            _MESSAGE("  Set %s to %.1f", statName.c_str(), value);
            statsImported++;
        }
    }

    // ========================================================================
    // STEP 4: Close file and report success
    // ========================================================================

    inFile.close();
    _MESSAGE("Imported %d stats successfully", statsImported);
    return (statsImported > 0);
}

// ============================================================================
// ITEM BLACKLIST I/O
// ============================================================================

/*
 * LoadItemBlacklist
 *
 * PURPOSE: Load list of blacklisted item names from file
 *
 * PARAMETERS:
 *   filename - Path to blacklist file
 *
 * RETURNS: true if loaded successfully
 *
 * FILE FORMAT: One item name per line
 *   # Comments start with #
 *   Iron Sword
 *   Steel Dagger
 *   Daedric Armor
 */
bool LoadItemBlacklist(const char* filename)
{
    _MESSAGE("Loading item blacklist from: %s", filename);

    // Clear existing blacklist
    g_itemBlacklist.clear();

    // ========================================================================
    // STEP 1: Open file for reading
    // ========================================================================

    std::ifstream inFile(filename);

    if (!inFile.is_open()) {
        _MESSAGE("Blacklist file not found (this is OK if not using one)");
        return false;
    }

    // ========================================================================
    // STEP 2: Read line by line
    // ========================================================================

    std::string line;

    while (std::getline(inFile, line)) {
        // Trim whitespace
        line = TrimWhitespace(line);

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Add to blacklist
        g_itemBlacklist.push_back(line);
        _MESSAGE("  Blacklisted: %s", line.c_str());
    }

    // ========================================================================
    // STEP 3: Close file and report success
    // ========================================================================

    inFile.close();
    _MESSAGE("Loaded %d blacklisted items", g_itemBlacklist.size());
    return true;
}

/*
 * IsItemBlacklisted
 *
 * PURPOSE: Check if an item is on the blacklist
 *
 * PARAMETERS:
 *   itemName - Name of item to check
 *
 * RETURNS: true if item is blacklisted
 */
bool IsItemBlacklisted(const char* itemName)
{
    // Convert to std::string for comparison
    std::string name(itemName);

    // std::find searches vector for matching element
    // Returns iterator pointing to element, or end() if not found
    auto it = std::find(g_itemBlacklist.begin(), g_itemBlacklist.end(), name);
    //   ↑                     ↑                        ↑                 ↑
    //   |                     |                        |                 └─ Value to find
    //   |                     |                        └─ End of search range
    //   |                     └─ Start of search range
    //   └─ Iterator to found element (or end())

    // If iterator is not at end(), we found the item
    return (it != g_itemBlacklist.end());
}

// ============================================================================
// LOGGING SYSTEM
// ============================================================================

/*
 * AppendToLog
 *
 * PURPOSE: Append a message to the log file
 *
 * PARAMETERS:
 *   filename - Path to log file
 *   message - Message to append
 *
 * RETURNS: true if logged successfully
 *
 * NOTE: Uses append mode (std::ios::app) to add to end of file
 */
bool AppendToLog(const char* filename, const char* message)
{
    // Open in APPEND mode
    // std::ios::app = append (don't overwrite existing content)
    std::ofstream logFile(filename, std::ios::app);
    //                              ↑
    //                              Second parameter = open mode

    if (!logFile.is_open()) {
        _MESSAGE("ERROR: Could not open log file for writing");
        return false;
    }

    // Write message with newline
    logFile << message << "\n";

    // Flush to ensure it's written immediately
    // Important for logs (in case of crash, we want the log saved)
    logFile.flush();
    //      ↑
    //      Force write buffer to disk NOW (don't wait)

    logFile.close();
    return true;
}

// ============================================================================
// CONSOLE COMMANDS
// ============================================================================

/*
 * Cmd_ExportStats_Execute
 *
 * PURPOSE: Console command to export player stats
 *
 * USAGE: ExportStats
 */
bool Cmd_ExportStats_Execute(COMMAND_ARGS)
{
    _MESSAGE("ExportStats command executed");

    // Get player
    PlayerCharacter* player = *g_thePlayer;
    if (!player) {
        Console_Print("ERROR: Player not found!");
        return false;
    }

    // Build file path
    std::string path = GetPluginDirectory() + "PlayerStats.txt";

    // Ensure directory exists
    EnsureDirectoryExists(GetPluginDirectory().c_str());

    // Export stats
    if (ExportCharacterStats(path.c_str(), player)) {
        Console_Print("Stats exported to: %s", path.c_str());
        return true;
    } else {
        Console_Print("Failed to export stats!");
        return false;
    }
}

/*
 * Cmd_ImportStats_Execute
 *
 * PURPOSE: Console command to import player stats
 *
 * USAGE: ImportStats
 *
 * WARNING: This will overwrite current player stats!
 */
bool Cmd_ImportStats_Execute(COMMAND_ARGS)
{
    _MESSAGE("ImportStats command executed");

    // Get player
    PlayerCharacter* player = *g_thePlayer;
    if (!player) {
        Console_Print("ERROR: Player not found!");
        return false;
    }

    // Build file path
    std::string path = GetPluginDirectory() + "PlayerStats.txt";

    // Import stats
    if (ImportCharacterStats(path.c_str(), player)) {
        Console_Print("Stats imported from: %s", path.c_str());
        return true;
    } else {
        Console_Print("Failed to import stats! File may not exist.");
        return false;
    }
}

/*
 * Cmd_SaveConfig_Execute
 *
 * PURPOSE: Console command to save current configuration
 *
 * USAGE: SaveConfig
 */
bool Cmd_SaveConfig_Execute(COMMAND_ARGS)
{
    _MESSAGE("SaveConfig command executed");

    // Build file path
    std::string path = GetPluginDirectory() + "Config.ini";

    // Ensure directory exists
    EnsureDirectoryExists(GetPluginDirectory().c_str());

    // Save configuration
    if (SaveConfig(path.c_str(), g_config)) {
        Console_Print("Config saved to: %s", path.c_str());
        return true;
    } else {
        Console_Print("Failed to save config!");
        return false;
    }
}

/*
 * Cmd_ReloadConfig_Execute
 *
 * PURPOSE: Console command to reload configuration from file
 *
 * USAGE: ReloadConfig
 */
bool Cmd_ReloadConfig_Execute(COMMAND_ARGS)
{
    _MESSAGE("ReloadConfig command executed");

    // Build file path
    std::string path = GetPluginDirectory() + "Config.ini";

    // Load configuration
    if (LoadConfig(path.c_str(), g_config)) {
        Console_Print("Config reloaded from: %s", path.c_str());
        Console_Print("Debug mode: %s", g_config.enableDebug ? "ON" : "OFF");
        return true;
    } else {
        Console_Print("Failed to reload config (file may not exist)");
        return false;
    }
}

// ============================================================================
// COMMAND DEFINITIONS
// ============================================================================

CommandInfo kCommandInfo_ExportStats = {
    "ExportStats",              // Full name
    "exs",                      // Short name
    0,                          // Opcode (auto-assign)
    "exports player stats to file",  // Help text
    0,                          // needsParent
    0,                          // numParams
    nullptr,                    // params
    Cmd_ExportStats_Execute     // execute function
};

CommandInfo kCommandInfo_ImportStats = {
    "ImportStats",
    "ims",
    0,
    "imports player stats from file",
    0, 0, nullptr,
    Cmd_ImportStats_Execute
};

CommandInfo kCommandInfo_SaveConfig = {
    "SaveConfig",
    "scfg",
    0,
    "saves plugin configuration to file",
    0, 0, nullptr,
    Cmd_SaveConfig_Execute
};

CommandInfo kCommandInfo_ReloadConfig = {
    "ReloadConfig",
    "rcfg",
    0,
    "reloads plugin configuration from file",
    0, 0, nullptr,
    Cmd_ReloadConfig_Execute
};

// ============================================================================
// MESSAGE HANDLING
// ============================================================================

void MessageHandler(OBSEMessagingInterface::Message* msg)
{
    switch (msg->type) {
        case OBSEMessagingInterface::kMessage_PostLoad:
            _MESSAGE("OBSE PostLoad message received");
            break;

        case OBSEMessagingInterface::kMessage_DataLoaded:
            _MESSAGE("OBSE DataLoaded message received");

            // This is a good time to load our config and blacklist
            {
                std::string pluginDir = GetPluginDirectory();
                EnsureDirectoryExists(pluginDir.c_str());

                // Load configuration
                std::string configPath = pluginDir + "Config.ini";
                if (!LoadConfig(configPath.c_str(), g_config)) {
                    // Config doesn't exist, create default one
                    SaveConfig(configPath.c_str(), g_config);
                    _MESSAGE("Created default config file");
                }

                // Load blacklist
                std::string blacklistPath = pluginDir + "ItemBlacklist.txt";
                LoadItemBlacklist(blacklistPath.c_str());
            }
            break;

        default:
            break;
    }
}

// ============================================================================
// VERSION DECLARATION
// ============================================================================

extern "C" {
    __declspec(dllexport) OBSEPluginVersionData OBSEPlugin_Version = {
        OBSEPluginVersionData::kVersion,
        1,                              // Plugin version
        "File I/O Example Plugin",      // Plugin name
        "Your Name",                    // Author
        0, 0,                           // Address/structure independence
        { RUNTIME_VERSION_1_512_105, 0 },  // Compatible versions
        0, 0, 0, { 0 }                  // Reserved
    };
}

// ============================================================================
// PLUGIN LOAD FUNCTION
// ============================================================================

extern "C" {
    __declspec(dllexport) bool OBSEPlugin_Load(const OBSEInterface* obse)
    {
        _MESSAGE("File I/O Example Plugin loading...");

        // Get plugin handle
        g_pluginHandle = obse->GetPluginHandle();

        // Get messaging interface
        g_messagingInterface = (OBSEMessagingInterface*)obse->QueryInterface(kInterface_Messaging);

        if (g_messagingInterface) {
            // Register for OBSE messages
            g_messagingInterface->RegisterListener(g_pluginHandle, "OBSE", MessageHandler);
        }

        // Register commands
        AddScriptCommand(&kCommandInfo_ExportStats);
        AddScriptCommand(&kCommandInfo_ImportStats);
        AddScriptCommand(&kCommandInfo_SaveConfig);
        AddScriptCommand(&kCommandInfo_ReloadConfig);

        _MESSAGE("File I/O Example Plugin loaded successfully!");
        _MESSAGE("Commands registered:");
        _MESSAGE("  ExportStats - Export player stats to file");
        _MESSAGE("  ImportStats - Import player stats from file");
        _MESSAGE("  SaveConfig - Save configuration");
        _MESSAGE("  ReloadConfig - Reload configuration");

        return true;
    }
}

// ============================================================================
// DATA FLOW SUMMARY - FILE WRITING
// ============================================================================

/*
 * WRITING TO A FILE - Step by Step:
 *
 * 1. Create std::ofstream object with filename
 *    std::ofstream outFile("data.txt");
 *
 * 2. Check if file opened successfully
 *    if (!outFile.is_open()) return false;
 *
 * 3. Write data using << operator
 *    outFile << "Text" << 123 << "\n";
 *
 * 4. Close file (happens automatically or call close())
 *    outFile.close();
 *
 * MEMORY FLOW:
 *
 *   Your Code
 *       ↓
 *   [String/Number Data]
 *       ↓
 *   outFile << data
 *       ↓
 *   [File Stream Buffer]
 *       ↓
 *   Flush/Close
 *       ↓
 *   [File on Disk]
 */

// ============================================================================
// DATA FLOW SUMMARY - FILE READING
// ============================================================================

/*
 * READING FROM A FILE - Step by Step:
 *
 * 1. Create std::ifstream object with filename
 *    std::ifstream inFile("data.txt");
 *
 * 2. Check if file opened successfully
 *    if (!inFile.is_open()) return false;
 *
 * 3. Read data using >> operator or getline()
 *    inFile >> number;                    // Read number
 *    std::getline(inFile, line);          // Read line
 *
 * 4. Close file (happens automatically or call close())
 *    inFile.close();
 *
 * MEMORY FLOW:
 *
 *   [File on Disk]
 *       ↓
 *   inFile.read()
 *       ↓
 *   [File Stream Buffer]
 *       ↓
 *   >> or getline()
 *       ↓
 *   [Your Variables]
 *       ↓
 *   Your Code
 */

// ============================================================================
// END OF FILE
// ============================================================================
