# OBSE64 File Reading Guide - Complete Tutorial

This guide explains how to read data from files in your OBSE64 plugin for Oblivion Remastered. Perfect for beginners!

---

## Table of Contents

1. [Why Read from Files?](#why-read-from-files)
2. [Understanding Input Streams](#understanding-input-streams)
3. [Basic File Reading](#basic-file-reading)
4. [Reading Line by Line](#reading-line-by-line)
5. [Reading Different Data Types](#reading-different-data-types)
6. [Parsing File Formats](#parsing-file-formats)
7. [Error Handling](#error-handling)
8. [Advanced Techniques](#advanced-techniques)
9. [Complete Examples](#complete-examples)

---

## Why Read from Files?

Reading from files in your OBSE plugin enables:

1. **Configuration**: Load plugin settings from .ini or .cfg files
2. **Data Import**: Load player data, mod lists, blacklists/whitelists
3. **Saved State**: Restore plugin state from previous session
4. **User Input**: Let users customize behavior via text files
5. **Interoperability**: Read data exported by other tools

**Example Use Cases:**
- Load configuration from MyPlugin.ini
- Import character presets from text files
- Read item blacklists
- Load custom spawn locations from CSV
- Parse mod compatibility lists

---

## Understanding Input Streams

### What is an Input File Stream?

An **input file stream** reads data from a file into your program:

```
File on Disk → [File Stream] → Your Code
```

### The ifstream Class

```cpp
#include <fstream>  // Required header

// Create input file stream
std::ifstream inFile("data.txt");

// Read from file
std::string text;
inFile >> text;

// Close when done
inFile.close();
```

### Stream States

File streams have states that tell you what's happening:

```cpp
std::ifstream inFile("data.txt");

// Check various states:
inFile.is_open()   // Is file open?
inFile.good()      // Everything OK?
inFile.eof()       // End of file reached?
inFile.fail()      // Read/write operation failed?
inFile.bad()       // Fatal stream error?
```

---

## Basic File Reading

### Simplest Example

```cpp
#include <fstream>
#include <string>

bool Cmd_ReadFile_Execute(COMMAND_ARGS)
{
    // Open file for reading
    std::ifstream inFile("data.txt");

    // Check if file opened
    if (!inFile.is_open()) {
        Console_Print("ERROR: Could not open file!");
        return false;
    }

    // Read one word
    std::string word;
    inFile >> word;

    // Print it
    Console_Print("Read: %s", word.c_str());

    // Close file
    inFile.close();

    return true;
}
```

**What Happens:**
1. `std::ifstream inFile("data.txt")` - Opens file for reading
2. File must exist (won't be created like ofstream)
3. `>>` operator reads data from file
4. Reads up to whitespace (space, tab, newline)
5. File is closed

### Reading Multiple Values

```cpp
std::ifstream inFile("numbers.txt");
if (!inFile.is_open()) return false;

// File contains: 10 20 30
int a, b, c;
inFile >> a >> b >> c;

Console_Print("Values: %d, %d, %d", a, b, c);  // Values: 10, 20, 30
```

### Reading Entire File at Once

```cpp
#include <sstream>  // For stringstream

std::ifstream inFile("data.txt");
if (!inFile.is_open()) return false;

// Read entire file into string
std::stringstream buffer;
buffer << inFile.rdbuf();  // rdbuf() = read buffer
std::string contents = buffer.str();

Console_Print("File contents: %s", contents.c_str());
```

---

## Reading Line by Line

### Using getline()

```cpp
#include <fstream>
#include <string>

bool ReadFileLineByLine(const char* filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        _MESSAGE("ERROR: Could not open %s", filename);
        return false;
    }

    std::string line;
    int lineNumber = 1;

    // Read each line
    while (std::getline(inFile, line)) {
        // Process this line
        _MESSAGE("Line %d: %s", lineNumber, line.c_str());
        lineNumber++;
    }

    inFile.close();
    return true;
}
```

**How getline() works:**
- `std::getline(inFile, line)` reads until newline character
- Stores result in `line` (without the newline)
- Returns `false` when end of file reached
- Perfect for configuration files, logs, CSV files

### Reading Line by Line with Line Numbers

```cpp
void PrintFileWithNumbers(const char* filename)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open()) return;

    std::string line;
    int lineNum = 1;

    while (std::getline(inFile, line)) {
        Console_Print("%3d: %s", lineNum, line.c_str());
        lineNum++;
    }

    inFile.close();
}
```

### Skipping Empty Lines

```cpp
std::ifstream inFile("data.txt");
std::string line;

while (std::getline(inFile, line)) {
    // Skip empty lines
    if (line.empty()) continue;

    // Skip lines that are only whitespace
    if (line.find_first_not_of(" \t") == std::string::npos) continue;

    // Process non-empty line
    Console_Print("%s", line.c_str());
}
```

### Skipping Comments

```cpp
std::ifstream inFile("config.txt");
std::string line;

while (std::getline(inFile, line)) {
    // Skip comment lines (starting with #)
    if (line.empty() || line[0] == '#') continue;

    // Skip comment lines (starting with //)
    if (line.size() >= 2 && line[0] == '/' && line[1] == '/') continue;

    // Process non-comment line
    ProcessConfigLine(line);
}
```

---

## Reading Different Data Types

### Reading Integers

```cpp
std::ifstream inFile("numbers.txt");

int value;
while (inFile >> value) {
    Console_Print("Read integer: %d", value);
}

// File: 10 20 30 40
// Output:
// Read integer: 10
// Read integer: 20
// Read integer: 30
// Read integer: 40
```

### Reading Floating Point

```cpp
std::ifstream inFile("decimals.txt");

float value;
while (inFile >> value) {
    Console_Print("Read float: %.2f", value);
}
```

### Reading Mixed Types

```cpp
// File format: Name Age Height
// Example: John 25 5.9

std::ifstream inFile("people.txt");
std::string name;
int age;
float height;

while (inFile >> name >> age >> height) {
    Console_Print("%s is %d years old and %.1f feet tall",
                  name.c_str(), age, height);
}
```

### Reading Strings with Spaces

```cpp
// Problem: >> stops at whitespace
std::ifstream inFile("names.txt");
std::string name;
inFile >> name;  // If file has "John Smith", only reads "John"

// Solution: Use getline()
std::getline(inFile, name);  // Reads whole line "John Smith"
```

### Reading Character by Character

```cpp
std::ifstream inFile("data.txt");

char ch;
while (inFile.get(ch)) {  // Read one character
    Console_Print("Character: %c", ch);
}
```

---

## Parsing File Formats

### INI/Config Files

Example config file:
```ini
# MyPlugin Configuration
EnableLogging=1
DamageMultiplier=1.5
MaxEnemies=10
PlayerName=Hero
```

Parser code:
```cpp
struct Config {
    bool enableLogging;
    float damageMultiplier;
    int maxEnemies;
    std::string playerName;
};

bool LoadConfig(const char* filename, Config& config)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        _MESSAGE("ERROR: Could not open config file");
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;

        // Find the = sign
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;  // No = found

        // Split into key and value
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // Parse based on key
        if (key == "EnableLogging") {
            config.enableLogging = (value == "1" || value == "true");
        }
        else if (key == "DamageMultiplier") {
            config.damageMultiplier = std::stof(value);  // string to float
        }
        else if (key == "MaxEnemies") {
            config.maxEnemies = std::stoi(value);  // string to int
        }
        else if (key == "PlayerName") {
            config.playerName = value;
        }
    }

    file.close();
    _MESSAGE("Config loaded successfully");
    return true;
}

// Usage:
Config myConfig;
LoadConfig("Data\\MyPlugin.ini", myConfig);
Console_Print("Damage multiplier: %.1f", myConfig.damageMultiplier);
```

### CSV Files

Example CSV file:
```csv
Name,Health,Magicka,Level
Player,100,200,10
Enemy1,75,150,8
Enemy2,50,100,5
```

Parser code:
```cpp
#include <sstream>
#include <vector>

struct CharacterData {
    std::string name;
    int health;
    int magicka;
    int level;
};

std::vector<CharacterData> LoadCSV(const char* filename)
{
    std::vector<CharacterData> characters;
    std::ifstream file(filename);

    if (!file.is_open()) {
        _MESSAGE("ERROR: Could not open CSV file");
        return characters;
    }

    std::string line;

    // Skip header line
    std::getline(file, line);

    // Read data lines
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        CharacterData character;

        // Read name (until comma)
        std::getline(ss, character.name, ',');

        // Read health
        std::getline(ss, cell, ',');
        character.health = std::stoi(cell);

        // Read magicka
        std::getline(ss, cell, ',');
        character.magicka = std::stoi(cell);

        // Read level
        std::getline(ss, cell, ',');
        character.level = std::stoi(cell);

        characters.push_back(character);
    }

    file.close();
    _MESSAGE("Loaded %d characters from CSV", characters.size());
    return characters;
}

// Usage:
std::vector<CharacterData> chars = LoadCSV("Data\\Characters.csv");
for (const auto& c : chars) {
    Console_Print("%s: HP=%d, MP=%d, Lvl=%d",
                  c.name.c_str(), c.health, c.magicka, c.level);
}
```

### Simple List Files

Example file (one item per line):
```
Sword
Shield
Potion
Scroll
```

Parser code:
```cpp
std::vector<std::string> LoadList(const char* filename)
{
    std::vector<std::string> items;
    std::ifstream file(filename);

    if (!file.is_open()) {
        _MESSAGE("ERROR: Could not open list file");
        return items;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines
        if (line.empty()) continue;

        // Trim whitespace and add to list
        items.push_back(line);
    }

    file.close();
    _MESSAGE("Loaded %d items", items.size());
    return items;
}

// Usage:
std::vector<std::string> blacklist = LoadList("Data\\ItemBlacklist.txt");

bool IsBlacklisted(const std::string& itemName)
{
    for (const auto& item : blacklist) {
        if (item == itemName) return true;
    }
    return false;
}
```

---

## Error Handling

### Checking if File Exists

```cpp
#include <fstream>

bool FileExists(const char* filename)
{
    std::ifstream file(filename);
    return file.good();  // Returns true if file opened successfully
}

// Usage:
if (!FileExists("config.ini")) {
    Console_Print("Config file not found! Using defaults.");
    // Create default config...
}
```

### Checking Stream State

```cpp
std::ifstream inFile("data.txt");

// Check if opened
if (!inFile.is_open()) {
    _MESSAGE("ERROR: File could not be opened");
    return false;
}

// Try to read
int value;
inFile >> value;

// Check if read succeeded
if (inFile.fail()) {
    _MESSAGE("ERROR: Failed to read integer");
    return false;
}

// Check if at end of file
if (inFile.eof()) {
    _MESSAGE("Reached end of file");
}
```

### Robust File Reading

```cpp
bool ReadConfigSafely(const char* filename, int& value)
{
    // Check if file exists
    std::ifstream file(filename);
    if (!file.is_open()) {
        _MESSAGE("ERROR: Config file not found: %s", filename);
        return false;
    }

    // Try to read value
    if (!(file >> value)) {
        _MESSAGE("ERROR: Could not parse integer from config");
        file.close();
        return false;
    }

    // Success
    file.close();
    _MESSAGE("Successfully read value: %d", value);
    return true;
}

// Usage with fallback:
int maxItems = 100;  // Default value
if (!ReadConfigSafely("Data\\MaxItems.txt", maxItems)) {
    Console_Print("Using default: %d", maxItems);
}
```

### Handling Missing Files Gracefully

```cpp
struct PluginConfig {
    int maxEnemies = 10;           // Default values
    float damageMultiplier = 1.0f;
    bool enableLogging = false;
};

PluginConfig LoadConfigOrDefault(const char* filename)
{
    PluginConfig config;  // Has default values

    std::ifstream file(filename);
    if (!file.is_open()) {
        _MESSAGE("Config file not found, using defaults");
        return config;  // Return defaults
    }

    // Try to load config
    std::string line;
    while (std::getline(file, line)) {
        // Parse config (example simplified)
        if (line.find("MaxEnemies=") == 0) {
            config.maxEnemies = std::stoi(line.substr(11));
        }
        // ... more parsing ...
    }

    file.close();
    _MESSAGE("Config loaded from file");
    return config;
}
```

---

## Advanced Techniques

### Reading Binary Files

```cpp
// Read raw bytes from binary file
std::ifstream file("data.bin", std::ios::binary);

if (!file.is_open()) return false;

// Read a structure
struct PlayerData {
    int health;
    int magicka;
    float strength;
};

PlayerData data;
file.read(reinterpret_cast<char*>(&data), sizeof(data));

Console_Print("Health: %d, Magicka: %d, Strength: %.1f",
              data.health, data.magicka, data.strength);

file.close();
```

### Getting File Size

```cpp
#include <fstream>

size_t GetFileSize(const char* filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    //                                                      ↑
    //                                                  "at end" - opens at end

    if (!file.is_open()) return 0;

    size_t size = file.tellg();  // Get current position = file size
    file.close();

    return size;
}

// Usage:
size_t size = GetFileSize("data.txt");
Console_Print("File size: %zu bytes", size);
```

### Reading File into Buffer

```cpp
char* ReadEntireFile(const char* filename, size_t& outSize)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        outSize = 0;
        return nullptr;
    }

    // Get file size
    outSize = file.tellg();
    file.seekg(0, std::ios::beg);  // Go back to beginning

    // Allocate buffer
    char* buffer = new char[outSize + 1];

    // Read entire file
    file.read(buffer, outSize);
    buffer[outSize] = '\0';  // Null-terminate

    file.close();
    return buffer;
}

// Usage:
size_t size;
char* data = ReadEntireFile("data.txt", size);
if (data) {
    Console_Print("File contents: %s", data);
    delete[] data;  // Don't forget to free!
}
```

### Parsing Key-Value with Sections

Example file:
```ini
[Graphics]
Width=1920
Height=1080

[Gameplay]
Difficulty=Hard
EnableCheats=0
```

Parser:
```cpp
void LoadINI(const char* filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) return;

    std::string currentSection;
    std::string line;

    while (std::getline(file, line)) {
        // Skip empty and comment lines
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        // Check for section header [SectionName]
        if (line[0] == '[') {
            size_t end = line.find(']');
            if (end != std::string::npos) {
                currentSection = line.substr(1, end - 1);
                _MESSAGE("Section: %s", currentSection.c_str());
                continue;
            }
        }

        // Parse key=value
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        _MESSAGE("[%s] %s = %s",
                 currentSection.c_str(), key.c_str(), value.c_str());

        // Process based on section and key...
    }

    file.close();
}
```

### Reading Specific Line Number

```cpp
std::string GetLineNumber(const char* filename, int lineNum)
{
    std::ifstream file(filename);
    if (!file.is_open()) return "";

    std::string line;
    int current = 1;

    while (std::getline(file, line)) {
        if (current == lineNum) {
            file.close();
            return line;
        }
        current++;
    }

    file.close();
    return "";  // Line not found
}

// Usage:
std::string line5 = GetLineNumber("data.txt", 5);
Console_Print("Line 5: %s", line5.c_str());
```

---

## Complete Examples

### Example 1: Load Plugin Configuration

```cpp
struct PluginSettings {
    bool enableDebug;
    int maxLogSize;
    float updateInterval;
    std::string playerName;
};

PluginSettings g_settings;

bool LoadSettings()
{
    std::ifstream file("Data\\OBSE\\Plugins\\MyPlugin.ini");

    if (!file.is_open()) {
        _MESSAGE("Settings file not found, using defaults");
        g_settings.enableDebug = false;
        g_settings.maxLogSize = 1000;
        g_settings.updateInterval = 1.0f;
        g_settings.playerName = "Player";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        // Find = separator
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // Parse settings
        if (key == "EnableDebug") {
            g_settings.enableDebug = (value == "1" || value == "true");
        }
        else if (key == "MaxLogSize") {
            g_settings.maxLogSize = std::stoi(value);
        }
        else if (key == "UpdateInterval") {
            g_settings.updateInterval = std::stof(value);
        }
        else if (key == "PlayerName") {
            g_settings.playerName = value;
        }
    }

    file.close();
    _MESSAGE("Settings loaded successfully");
    return true;
}

// Call in OBSEPlugin_Load:
extern "C" {
    __declspec(dllexport) bool OBSEPlugin_Load(const OBSEInterface* obse)
    {
        LoadSettings();
        _MESSAGE("Debug mode: %s", g_settings.enableDebug ? "ON" : "OFF");
        return true;
    }
}
```

### Example 2: Load Item Blacklist

```cpp
#include <vector>
#include <algorithm>

std::vector<std::string> g_itemBlacklist;

bool LoadItemBlacklist()
{
    std::ifstream file("Data\\OBSE\\Plugins\\ItemBlacklist.txt");

    if (!file.is_open()) {
        _MESSAGE("No blacklist file found");
        return false;
    }

    g_itemBlacklist.clear();
    std::string line;

    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;

        // Trim whitespace
        size_t start = line.find_first_not_of(" \t");
        size_t end = line.find_last_not_of(" \t");

        if (start != std::string::npos) {
            std::string itemName = line.substr(start, end - start + 1);
            g_itemBlacklist.push_back(itemName);
        }
    }

    file.close();
    _MESSAGE("Loaded %d blacklisted items", g_itemBlacklist.size());
    return true;
}

bool IsItemBlacklisted(const char* itemName)
{
    std::string name(itemName);
    return std::find(g_itemBlacklist.begin(),
                     g_itemBlacklist.end(),
                     name) != g_itemBlacklist.end();
}

// Usage:
if (IsItemBlacklisted("Daedric Sword")) {
    Console_Print("This item is blacklisted!");
}
```

### Example 3: Import Character Stats

```cpp
bool Cmd_ImportStats_Execute(COMMAND_ARGS)
{
    PlayerCharacter* player = *g_thePlayer;
    if (!player) {
        Console_Print("ERROR: Player not found!");
        return false;
    }

    std::ifstream file("Data\\ImportedStats.txt");
    if (!file.is_open()) {
        Console_Print("ERROR: Import file not found!");
        return false;
    }

    std::string line;
    int statsLoaded = 0;

    while (std::getline(file, line)) {
        // Skip empty lines and headers
        if (line.empty() || line[0] == '=' || line[0] == '#') continue;

        // Expected format: "Strength: 50"
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string statName = line.substr(0, colonPos);
        std::string valueStr = line.substr(colonPos + 1);

        // Trim whitespace
        size_t start = valueStr.find_first_not_of(" \t");
        if (start == std::string::npos) continue;
        valueStr = valueStr.substr(start);

        float value = std::stof(valueStr);

        // Map stat name to ID and set value
        int actorValueID = -1;
        if (statName.find("Strength") != std::string::npos) actorValueID = 0;
        else if (statName.find("Intelligence") != std::string::npos) actorValueID = 1;
        else if (statName.find("Willpower") != std::string::npos) actorValueID = 2;
        // ... more stats ...

        if (actorValueID >= 0) {
            player->SetActorValue(actorValueID, value);
            statsLoaded++;
            _MESSAGE("Set %s to %.1f", statName.c_str(), value);
        }
    }

    file.close();
    Console_Print("Imported %d stats successfully!", statsLoaded);
    return true;
}
```

### Example 4: Read Last Line of Log

```cpp
std::string GetLastLogEntry(const char* filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) return "";

    std::string lastLine;
    std::string currentLine;

    // Read all lines, keeping track of last non-empty one
    while (std::getline(file, currentLine)) {
        if (!currentLine.empty()) {
            lastLine = currentLine;
        }
    }

    file.close();
    return lastLine;
}

// Usage:
bool Cmd_ShowLastLog_Execute(COMMAND_ARGS)
{
    std::string last = GetLastLogEntry("Data\\PluginLog.txt");
    if (!last.empty()) {
        Console_Print("Last log: %s", last.c_str());
    } else {
        Console_Print("Log is empty or not found");
    }
    return true;
}
```

---

## Summary

### Quick Reference

```cpp
// Include header
#include <fstream>
#include <string>

// Open file
std::ifstream in("file.txt");

// Check if opened
if (!in.is_open()) { /* error */ }

// Read word by word
std::string word;
while (in >> word) { /* process word */ }

// Read line by line
std::string line;
while (std::getline(in, line)) { /* process line */ }

// Read entire file
std::stringstream buffer;
buffer << in.rdbuf();
std::string contents = buffer.str();

// Close file
in.close();
```

### Best Practices

1. **Always check if file opened** - File might not exist
2. **Use getline() for text files** - Easier to parse line by line
3. **Skip empty lines and comments** - Makes parsing robust
4. **Provide defaults** - Don't crash if config file missing
5. **Validate input** - Check data types match expected
6. **Log errors** - Use `_MESSAGE()` for debugging
7. **Close files** - Release file handle when done
8. **Trim whitespace** - Users add extra spaces

### Common Mistakes

❌ **Wrong:** Assuming file exists without checking
✅ **Right:** Check `is_open()` before reading

❌ **Wrong:** Using `>>` to read strings with spaces
✅ **Right:** Use `std::getline()` instead

❌ **Wrong:** Not checking if read succeeded
✅ **Right:** Check `file.fail()` or test return value

❌ **Wrong:** Crashing when config file missing
✅ **Right:** Use default values as fallback

---

## Next Steps

- Read [WRITING_TO_FILES.md](WRITING_TO_FILES.md) to learn file writing
- Study [FILE_IO_ANNOTATED_EXAMPLE.cpp.md](FILE_IO_ANNOTATED_EXAMPLE.cpp.md)
- Build configuration system for your plugin
- Create import/export commands
- Experiment with different file formats

Happy coding!
