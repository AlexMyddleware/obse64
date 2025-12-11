# OBSE64 File Writing Guide - Complete Tutorial

This guide explains how to write data to files from your OBSE64 plugin for Oblivion Remastered. Perfect for beginners!

---

## Table of Contents

1. [Why Write to Files?](#why-write-to-files)
2. [Understanding File Streams](#understanding-file-streams)
3. [Basic File Writing](#basic-file-writing)
4. [File Paths and Locations](#file-paths-and-locations)
5. [Writing Different Data Types](#writing-different-data-types)
6. [Formatting Output](#formatting-output)
7. [Append vs Overwrite](#append-vs-overwrite)
8. [Error Handling](#error-handling)
9. [Advanced Techniques](#advanced-techniques)
10. [Complete Examples](#complete-examples)

---

## Why Write to Files?

Writing to files from your OBSE plugin is essential for:

1. **Exporting Data**: Save player stats, inventory, quest progress, etc.
2. **Logging**: Debug your plugin by writing detailed logs
3. **Configuration**: Save plugin settings that persist between sessions
4. **Data Interchange**: Export data in formats other programs can read (CSV, JSON, XML)
5. **Backups**: Create backups of game state

**Example Use Cases:**
- Export player character stats to share online
- Log combat events for analysis
- Save custom plugin settings
- Create reports of mod compatibility
- Generate data files for external tools

---

## Understanding File Streams

### What is a File Stream?

A **file stream** is a C++ object that manages reading/writing to files. Think of it like a pipe:

```
Your Code → [File Stream] → File on Disk
```

### Three Types of File Streams

```cpp
#include <fstream>  // Include this header

// 1. ofstream - Output File Stream (WRITING)
std::ofstream outFile("data.txt");
outFile << "Hello World";

// 2. ifstream - Input File Stream (READING)
std::ifstream inFile("data.txt");
std::string text;
inFile >> text;

// 3. fstream - Both reading and writing
std::fstream file("data.txt", std::ios::in | std::ios::out);
```

**For this guide, we'll focus on `std::ofstream` (writing).**

---

## Basic File Writing

### Simplest Example

```cpp
#include <fstream>

bool Cmd_WriteHello_Execute(COMMAND_ARGS)
{
    // Create file stream (creates/overwrites "hello.txt")
    std::ofstream outFile("hello.txt");

    // Check if file opened successfully
    if (!outFile.is_open()) {
        Console_Print("ERROR: Could not create file!");
        return false;
    }

    // Write text to file
    outFile << "Hello World!";

    // Close file (also happens automatically)
    outFile.close();

    Console_Print("File written successfully!");
    return true;
}
```

**What Happens:**
1. `std::ofstream outFile("hello.txt")` - Creates object and opens file
2. File is created in the game's root directory
3. If file exists, it's **overwritten** (old contents deleted)
4. `<<` operator writes text to the file
5. File is closed automatically when `outFile` goes out of scope

### Step-by-Step Breakdown

```cpp
// STEP 1: Include the file stream library
#include <fstream>

// STEP 2: Create an output file stream
std::ofstream outFile("filename.txt");
//    ↑         ↑           ↑
//    |         |           └─ Filename (in game directory)
//    |         └─ Variable name (you choose this)
//    └─ Output file stream type

// STEP 3: Check if it opened
if (!outFile.is_open()) {
    // Handle error - file couldn't be created
    _MESSAGE("ERROR: File failed to open");
    return false;
}

// STEP 4: Write data using << operator
outFile << "Some text";
outFile << "\n";  // Newline
outFile << 42;    // Numbers work too!

// STEP 5: Close the file
outFile.close();  // Ensures data is written to disk
```

---

## File Paths and Locations

### Default Location

When you just specify a filename, it goes in the **game's root directory**:

```cpp
std::ofstream outFile("mydata.txt");
// Creates: C:\Program Files\Oblivion Remastered\mydata.txt
```

### Absolute Paths

You can specify the full path:

```cpp
std::ofstream outFile("C:\\MyDocuments\\OblivionData\\stats.txt");
//                      ↑
//                      Note: Double backslashes!
```

**Why double backslashes?**
- `\` is an escape character in C++
- `\\` means "a literal backslash"
- `\n` would mean newline, not backslash-n

### Relative Paths

Relative to the game directory:

```cpp
// Create in Data folder
std::ofstream outFile("Data\\MyPlugin\\export.txt");

// Create in parent directory
std::ofstream outFile("..\\export.txt");

// Create in subdirectory
std::ofstream outFile("Exports\\PlayerData.txt");
```

### Best Practices

```cpp
// GOOD: Organized in Data folder
std::ofstream outFile("Data\\OBSE\\Plugins\\MyPlugin\\export.txt");

// GOOD: Use your plugin's folder
std::ofstream outFile("Data\\MyPluginData\\stats.txt");

// BAD: Clutters game root directory
std::ofstream outFile("data1.txt");
std::ofstream outFile("data2.txt");
```

### Creating Directories

**Important:** File streams **don't** create directories!

```cpp
// This will FAIL if "MyFolder" doesn't exist:
std::ofstream outFile("MyFolder\\data.txt");

// You need to create the directory first (using Windows API):
#include <direct.h>  // For _mkdir
_mkdir("MyFolder");  // Create directory
std::ofstream outFile("MyFolder\\data.txt");  // Now this works!
```

**Creating nested directories:**

```cpp
#include <direct.h>

// Create each level separately
_mkdir("Data");
_mkdir("Data\\MyPlugin");
_mkdir("Data\\MyPlugin\\Exports");
std::ofstream outFile("Data\\MyPlugin\\Exports\\data.txt");
```

---

## Writing Different Data Types

### Writing Text

```cpp
std::ofstream outFile("text.txt");

// String literals
outFile << "Hello World";

// String variables
std::string name = "Player";
outFile << name;

// Character arrays
char* text = "Some text";
outFile << text;
```

### Writing Numbers

```cpp
std::ofstream outFile("numbers.txt");

// Integers
int health = 100;
outFile << health;           // Writes: 100

// Floating point
float strength = 50.5f;
outFile << strength;         // Writes: 50.5

// Double precision
double precise = 3.14159265;
outFile << precise;          // Writes: 3.14159
```

### Writing Multiple Values

```cpp
std::ofstream outFile("data.txt");

// Chain multiple values
outFile << "Health: " << 100 << "\n";
outFile << "Strength: " << 50.5 << "\n";

// Result in file:
// Health: 100
// Strength: 50.5
```

### Writing Special Characters

```cpp
std::ofstream outFile("special.txt");

outFile << "Line 1\n";        // \n = newline
outFile << "Tab\there\n";     // \t = tab
outFile << "Quote: \"Hi\"\n"; // \" = quote mark
outFile << "Backslash: \\\n"; // \\ = single backslash
```

---

## Formatting Output

### Adding Newlines

```cpp
std::ofstream outFile("lines.txt");

// Three ways to add newlines:

// 1. Using \n character
outFile << "Line 1\n";

// 2. Using std::endl (flushes buffer)
outFile << "Line 2" << std::endl;

// 3. Multiple writes
outFile << "Line 3";
outFile << "\n";
```

**Difference between `\n` and `std::endl`:**
- `\n` - Just adds newline character (faster)
- `std::endl` - Adds newline AND flushes buffer to disk (slower but safer)

### Number Formatting

```cpp
#include <iomanip>  // For formatting manipulators

std::ofstream outFile("formatted.txt");

// Set decimal precision
outFile << std::setprecision(2) << std::fixed;
outFile << 3.14159 << "\n";  // Writes: 3.14

// Set width (right-aligned)
outFile << std::setw(10) << 42 << "\n";  // Writes: "        42"

// Set width (left-aligned)
outFile << std::left << std::setw(10) << 42 << "\n";  // Writes: "42        "

// Hexadecimal
outFile << std::hex << 255 << "\n";  // Writes: ff

// Reset to decimal
outFile << std::dec << 255 << "\n";  // Writes: 255
```

### Creating Tables

```cpp
#include <iomanip>

std::ofstream outFile("table.txt");

// Table header
outFile << std::left;
outFile << std::setw(15) << "Name"
        << std::setw(10) << "Health"
        << std::setw(10) << "Magicka" << "\n";
outFile << "----------------------------------------\n";

// Table rows
outFile << std::setw(15) << "Player"
        << std::setw(10) << 100
        << std::setw(10) << 200 << "\n";
outFile << std::setw(15) << "Enemy"
        << std::setw(10) << 50
        << std::setw(10) << 100 << "\n";

// Result:
// Name           Health    Magicka
// ----------------------------------------
// Player         100       200
// Enemy          50        100
```

---

## Append vs Overwrite

### Overwrite Mode (Default)

```cpp
// Creates new file or OVERWRITES existing file
std::ofstream outFile("data.txt");
outFile << "New data";
// Previous contents are DELETED!
```

### Append Mode

```cpp
// Append to end of file (keeps existing contents)
std::ofstream outFile("data.txt", std::ios::app);
outFile << "Additional data\n";
// Adds to end of file without deleting previous contents
```

### Example: Logging

```cpp
bool LogEvent(const char* message)
{
    // Open in append mode - adds to existing log
    std::ofstream logFile("plugin_log.txt", std::ios::app);

    if (!logFile.is_open()) {
        return false;
    }

    // Add timestamp and message
    logFile << "[Event] " << message << "\n";
    logFile.close();

    return true;
}

// Usage:
LogEvent("Player entered combat");
LogEvent("Quest completed");
LogEvent("Item equipped");

// File contents:
// [Event] Player entered combat
// [Event] Quest completed
// [Event] Item equipped
```

### Other Open Modes

```cpp
#include <fstream>

// Truncate (default) - erase contents
std::ofstream file1("data.txt", std::ios::trunc);

// Append - add to end
std::ofstream file2("data.txt", std::ios::app);

// Binary mode - don't translate line endings
std::ofstream file3("data.bin", std::ios::binary);

// Combine modes with | (bitwise OR)
std::ofstream file4("data.txt", std::ios::app | std::ios::binary);
```

---

## Error Handling

### Checking if File Opened

```cpp
std::ofstream outFile("data.txt");

// Method 1: is_open()
if (!outFile.is_open()) {
    Console_Print("ERROR: Could not open file!");
    _MESSAGE("File open failed: data.txt");
    return false;
}

// Method 2: Boolean conversion
if (!outFile) {
    Console_Print("ERROR: File stream invalid!");
    return false;
}
```

### Common Errors

**Error: File won't open**
- Directory doesn't exist (create it first!)
- No write permission (try different location)
- File is locked by another program
- Invalid filename (illegal characters: `< > : " / \ | ? *`)

**Error: Data not in file**
- Forgot to close file (data in buffer, not written yet)
- File stream went out of scope before close
- Use `std::endl` or `flush()` to force write

### Robust Error Handling

```cpp
bool WriteDataSafely(const char* filename, const char* data)
{
    // Try to open file
    std::ofstream outFile(filename);

    // Check if opened
    if (!outFile.is_open()) {
        _MESSAGE("ERROR: Could not open file: %s", filename);
        Console_Print("Failed to create export file!");
        return false;
    }

    // Write data
    outFile << data;

    // Check if write succeeded
    if (outFile.fail()) {
        _MESSAGE("ERROR: Write operation failed!");
        Console_Print("Failed to write data!");
        outFile.close();
        return false;
    }

    // Explicitly close
    outFile.close();

    // Check if close succeeded
    if (outFile.fail()) {
        _MESSAGE("ERROR: Failed to close file!");
        return false;
    }

    _MESSAGE("File written successfully: %s", filename);
    return true;
}
```

---

## Advanced Techniques

### Binary File Writing

```cpp
// Write raw bytes (not text)
std::ofstream outFile("data.bin", std::ios::binary);

// Write a structure
struct PlayerData {
    int health;
    int magicka;
    float strength;
};

PlayerData data = { 100, 200, 50.5f };

// Write raw bytes
outFile.write(reinterpret_cast<char*>(&data), sizeof(data));
outFile.close();

// WARNING: Binary files are not human-readable!
// Only use if you need exact memory representation
```

### Writing CSV Files

```cpp
bool ExportToCSV(const char* filename)
{
    std::ofstream csv(filename);
    if (!csv.is_open()) return false;

    // Write header row
    csv << "Name,Health,Magicka,Strength\n";

    // Write data rows
    csv << "Player," << 100 << "," << 200 << "," << 50.5 << "\n";
    csv << "Enemy," << 75 << "," << 150 << "," << 35.0 << "\n";

    csv.close();
    return true;
}

// Creates file readable by Excel/Google Sheets:
// Name,Health,Magicka,Strength
// Player,100,200,50.5
// Enemy,75,150,35.0
```

### Writing JSON-like Format

```cpp
bool ExportToJSON(const char* filename)
{
    std::ofstream json(filename);
    if (!json.is_open()) return false;

    json << "{\n";
    json << "  \"player\": {\n";
    json << "    \"health\": 100,\n";
    json << "    \"magicka\": 200,\n";
    json << "    \"strength\": 50.5\n";
    json << "  }\n";
    json << "}\n";

    json.close();
    return true;
}

// Creates:
// {
//   "player": {
//     "health": 100,
//     "magicka": 200,
//     "strength": 50.5
//   }
// }
```

### Buffering and Flushing

```cpp
std::ofstream outFile("data.txt");

// Write to buffer (not disk yet)
outFile << "Line 1\n";
outFile << "Line 2\n";

// Flush buffer to disk (force write)
outFile.flush();

// Alternative: use std::endl (writes newline AND flushes)
outFile << "Line 3" << std::endl;  // Written immediately

// Close also flushes
outFile.close();
```

**When to flush:**
- Critical data that must be saved immediately
- Debugging (so you can see output even if program crashes)
- Before long operations
- Before reading the file elsewhere

---

## Complete Examples

### Example 1: Export Player Stats

```cpp
bool Cmd_ExportPlayerStats_Execute(COMMAND_ARGS)
{
    // Get player
    PlayerCharacter* player = *g_thePlayer;
    if (!player) {
        Console_Print("ERROR: Player not found!");
        return false;
    }

    // Create file
    std::ofstream outFile("Data\\PlayerStats.txt");
    if (!outFile.is_open()) {
        Console_Print("ERROR: Could not create export file!");
        return false;
    }

    // Write header
    outFile << "====================================\n";
    outFile << "PLAYER STATISTICS\n";
    outFile << "====================================\n\n";

    // Write attributes
    outFile << "ATTRIBUTES:\n";
    outFile << "  Strength:     " << player->GetActorValue(0) << "\n";
    outFile << "  Intelligence: " << player->GetActorValue(1) << "\n";
    outFile << "  Willpower:    " << player->GetActorValue(2) << "\n";
    outFile << "  Agility:      " << player->GetActorValue(3) << "\n";
    outFile << "  Speed:        " << player->GetActorValue(4) << "\n";
    outFile << "  Endurance:    " << player->GetActorValue(5) << "\n";
    outFile << "  Personality:  " << player->GetActorValue(6) << "\n";
    outFile << "  Luck:         " << player->GetActorValue(7) << "\n\n";

    // Write vital stats
    outFile << "VITAL STATS:\n";
    outFile << "  Health:  " << player->GetActorValue(8) << "\n";
    outFile << "  Magicka: " << player->GetActorValue(9) << "\n";
    outFile << "  Fatigue: " << player->GetActorValue(10) << "\n";

    // Close file
    outFile.close();

    Console_Print("Stats exported to Data\\PlayerStats.txt");
    return true;
}
```

### Example 2: Combat Logger

```cpp
// Global file stream (stays open)
static std::ofstream* g_combatLog = nullptr;

// Initialize logging
void InitCombatLog()
{
    g_combatLog = new std::ofstream("Data\\CombatLog.txt", std::ios::app);

    if (g_combatLog->is_open()) {
        *g_combatLog << "\n=== New Session ===\n";
        g_combatLog->flush();
        _MESSAGE("Combat log initialized");
    }
}

// Log combat event
void LogCombatEvent(const char* attacker, const char* target, float damage)
{
    if (!g_combatLog || !g_combatLog->is_open()) return;

    *g_combatLog << attacker << " dealt " << damage
                 << " damage to " << target << "\n";
    g_combatLog->flush();  // Write immediately
}

// Cleanup
void CloseCombatLog()
{
    if (g_combatLog) {
        g_combatLog->close();
        delete g_combatLog;
        g_combatLog = nullptr;
    }
}

// Usage in plugin:
extern "C" {
    __declspec(dllexport) bool OBSEPlugin_Load(const OBSEInterface* obse)
    {
        InitCombatLog();
        // ... rest of load code ...
        return true;
    }
}
```

### Example 3: Configuration Saver

```cpp
struct PluginConfig {
    bool enableLogging;
    float damageMultiplier;
    int maxEnemies;
};

bool SaveConfig(const PluginConfig& config, const char* filename)
{
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << "# My Plugin Configuration\n";
    file << "# Edit values below\n\n";

    file << "EnableLogging=" << (config.enableLogging ? "1" : "0") << "\n";
    file << "DamageMultiplier=" << config.damageMultiplier << "\n";
    file << "MaxEnemies=" << config.maxEnemies << "\n";

    file.close();
    return true;
}

// Usage:
PluginConfig myConfig = { true, 1.5f, 10 };
SaveConfig(myConfig, "Data\\MyPlugin.ini");

// Creates file:
// # My Plugin Configuration
// # Edit values below
//
// EnableLogging=1
// DamageMultiplier=1.5
// MaxEnemies=10
```

### Example 4: Timestamp Logger

```cpp
#include <ctime>  // For time functions

void LogWithTimestamp(const char* message)
{
    std::ofstream log("plugin_log.txt", std::ios::app);
    if (!log.is_open()) return;

    // Get current time
    time_t now = time(nullptr);
    char timeStr[26];
    ctime_s(timeStr, sizeof(timeStr), &now);

    // Remove newline from time string
    timeStr[24] = '\0';

    // Write with timestamp
    log << "[" << timeStr << "] " << message << "\n";
    log.close();
}

// Usage:
LogWithTimestamp("Plugin initialized");
LogWithTimestamp("Player entered combat");

// Creates:
// [Mon Dec 10 14:30:45 2025] Plugin initialized
// [Mon Dec 10 14:31:12 2025] Player entered combat
```

---

## Summary

### Quick Reference

```cpp
// Include header
#include <fstream>

// Basic write
std::ofstream out("file.txt");
out << "text";
out.close();

// Append mode
std::ofstream out("file.txt", std::ios::app);

// Check if opened
if (!out.is_open()) { /* error */ }

// Format numbers
#include <iomanip>
out << std::setprecision(2) << std::fixed << 3.14159;

// Flush to disk
out.flush();
out << "text" << std::endl;  // Also flushes
```

### Best Practices

1. **Always check if file opened** - Use `is_open()`
2. **Use meaningful filenames** - Not "data1.txt", "data2.txt"
3. **Organize in folders** - Use `Data\YourPlugin\` directory
4. **Close files** - Explicitly call `close()` or let destructor do it
5. **Handle errors** - Log failures with `_MESSAGE()`
6. **Flush critical data** - Use `flush()` or `std::endl`
7. **Use append for logs** - `std::ios::app` mode
8. **Comment your format** - Future you will thank you!

### Common Mistakes

❌ **Wrong:** `std::ofstream out("C:\MyFolder\file.txt");`
✅ **Right:** `std::ofstream out("C:\\MyFolder\\file.txt");`

❌ **Wrong:** Writing to non-existent directory
✅ **Right:** Create directory first with `_mkdir()`

❌ **Wrong:** Not checking if file opened
✅ **Right:** Always check `is_open()`

❌ **Wrong:** Using file stream after close
✅ **Right:** Don't use after `close()` called

---

## Next Steps

- Read [READING_FROM_FILES.md](READING_FROM_FILES.md) to learn file reading
- Study [FILE_IO_ANNOTATED_EXAMPLE.cpp.md](FILE_IO_ANNOTATED_EXAMPLE.cpp.md) for complete examples
- Experiment with different formats (CSV, JSON, INI)
- Build your own data export/logging system

Happy coding!
