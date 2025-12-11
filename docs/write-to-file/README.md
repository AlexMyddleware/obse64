# File I/O Documentation for OBSE64 Plugins

This directory contains comprehensive documentation for reading and writing files in OBSE64 plugins for **Oblivion Remastered**.

---

## 📚 Documentation Files

### 1. [WRITING_TO_FILES.md](WRITING_TO_FILES.md)
**Complete guide to writing files**

Learn how to:
- Write text and data to files
- Create configuration files
- Export player data
- Handle different file formats (TXT, CSV, JSON, INI)
- Format output properly
- Handle errors gracefully

**Start here if you want to:**
- Export game data to files
- Create log files
- Save plugin settings
- Generate reports

---

### 2. [READING_FROM_FILES.md](READING_FROM_FILES.md)
**Complete guide to reading files**

Learn how to:
- Read text and data from files
- Parse configuration files
- Load saved data
- Handle different file formats
- Parse line by line
- Handle missing files gracefully

**Start here if you want to:**
- Load plugin configuration
- Import character data
- Read blacklists/whitelists
- Parse user settings

---

### 3. [FILE_IO_ANNOTATED_EXAMPLE.cpp.md](FILE_IO_ANNOTATED_EXAMPLE.cpp.md)
**Complete, fully-annotated example plugin**

A working OBSE64 plugin with:
- Every line explained in detail
- Configuration file I/O
- Character stats export/import
- Item blacklist system
- Error handling examples
- Console commands
- Data flow diagrams

**Start here if you want to:**
- See a complete working example
- Understand how everything fits together
- Learn by example
- Copy-paste working code

---

## 🚀 Quick Start

### For Absolute Beginners

1. Read the main **[BEGINNERS_GUIDE.md](../BEGINNERS_GUIDE.md)** first (in the parent directory)
2. Then read **[WRITING_TO_FILES.md](WRITING_TO_FILES.md)**
3. Then read **[READING_FROM_FILES.md](READING_FROM_FILES.md)**
4. Finally, study **[FILE_IO_ANNOTATED_EXAMPLE.cpp.md](FILE_IO_ANNOTATED_EXAMPLE.cpp.md)**

### For Experienced Programmers

1. Skim **[FILE_IO_ANNOTATED_EXAMPLE.cpp.md](FILE_IO_ANNOTATED_EXAMPLE.cpp.md)** to see the full picture
2. Use **[WRITING_TO_FILES.md](WRITING_TO_FILES.md)** and **[READING_FROM_FILES.md](READING_FROM_FILES.md)** as reference

---

## 📖 What You'll Learn

### Writing Files
- Using `std::ofstream` (output file stream)
- Creating and overwriting files
- Appending to existing files
- Writing different data types
- Formatting output
- Error handling

### Reading Files
- Using `std::ifstream` (input file stream)
- Reading line by line
- Parsing different formats
- Handling missing files
- Converting strings to numbers
- Error handling

### Practical Examples
- Export/import player stats
- Save/load plugin configuration
- Create item blacklists
- Implement logging systems
- Parse INI files
- Read CSV data

---

## 🎯 Common Use Cases

### Export Player Data
```cpp
std::ofstream outFile("PlayerStats.txt");
outFile << "Health: " << player->GetActorValue(8) << "\n";
outFile << "Level: " << player->GetLevel() << "\n";
outFile.close();
```
See: [WRITING_TO_FILES.md - Complete Examples](WRITING_TO_FILES.md#complete-examples)

### Load Configuration
```cpp
std::ifstream inFile("Config.ini");
std::string line;
while (std::getline(inFile, line)) {
    // Parse Key=Value
    size_t pos = line.find('=');
    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);
    // Use key and value...
}
inFile.close();
```
See: [READING_FROM_FILES.md - Parsing File Formats](READING_FROM_FILES.md#parsing-file-formats)

### Append to Log
```cpp
std::ofstream logFile("plugin.log", std::ios::app);
logFile << "Event: Player entered combat\n";
logFile.close();
```
See: [WRITING_TO_FILES.md - Append vs Overwrite](WRITING_TO_FILES.md#append-vs-overwrite)

---

## 🔧 Required Headers

All file I/O operations require these C++ headers:

```cpp
#include <fstream>   // std::ofstream, std::ifstream
#include <string>    // std::string
#include <sstream>   // std::stringstream (for parsing)
```

Optional but useful:

```cpp
#include <iomanip>   // Formatting (setprecision, setw, etc.)
#include <vector>    // Dynamic arrays
#include <algorithm> // find, sort, etc.
```

---

## ⚠️ Important Notes

### For Oblivion Remastered

These guides are specifically for **Oblivion Remastered** using OBSE64 (64-bit), not the original Oblivion. The concepts are the same, but:

- File paths may differ
- Game version checks are different
- Some APIs may have changed

### File Locations

**Recommended directory structure:**
```
Oblivion Remastered/
├── Data/
│   ├── OBSE/
│   │   ├── Plugins/
│   │   │   ├── YourPlugin.dll
│   │   │   └── YourPlugin/
│   │   │       ├── Config.ini
│   │   │       ├── Data.txt
│   │   │       └── Logs/
│   │   │           └── plugin.log
```

### Error Handling

**Always check if files opened successfully:**

```cpp
std::ofstream file("data.txt");
if (!file.is_open()) {
    _MESSAGE("ERROR: Could not open file!");
    return false;
}
```

**Use default values if config file missing:**

```cpp
if (!LoadConfig("config.ini", myConfig)) {
    // Use defaults - don't crash!
    myConfig = GetDefaultConfig();
}
```

---

## 💡 Tips and Best Practices

1. **Always check file operations** - Files might not exist, or you might not have permission
2. **Use meaningful filenames** - Not "data1.txt", use "PlayerStats.txt"
3. **Organize in folders** - Put your files in `Data\OBSE\Plugins\YourPlugin\`
4. **Create directories first** - Use `_mkdir()` before creating files in them
5. **Close files explicitly** - Or let destructors do it automatically
6. **Flush important data** - Use `flush()` or `std::endl` for logs
7. **Handle errors gracefully** - Don't crash if a file is missing
8. **Log everything** - Use `_MESSAGE()` to track what's happening

---

## 🤔 FAQ

**Q: Why use double backslashes in paths?**
A: In C++, `\` is an escape character. `\\` means "a literal backslash". So `"C:\\Data"` becomes `C:\Data`.

**Q: What's the difference between `\n` and `std::endl`?**
A: `\n` just adds a newline. `std::endl` adds a newline AND flushes the buffer (forces immediate write to disk).

**Q: How do I read a file with spaces in the name?**
A: Just use quotes: `std::ifstream file("My File.txt");`

**Q: Can I create directories from my plugin?**
A: Yes, use `_mkdir("DirectoryName")` from `<direct.h>`. Create parent directories first!

**Q: What if my file is huge?**
A: Read it line by line instead of all at once. Use `std::getline()` in a while loop.

**Q: How do I know if a file exists?**
```cpp
std::ifstream file("test.txt");
bool exists = file.good();
```

---

## 📚 Related Documentation

- **[../BEGINNERS_GUIDE.md](../BEGINNERS_GUIDE.md)** - Start here if you're new to OBSE64 plugins
- **[../EXAMPLE_PLUGIN_ANNOTATED.cpp.md](../EXAMPLE_PLUGIN_ANNOTATED.cpp.md)** - Console commands and basic plugin structure

---

## 🎓 Learning Path

**Complete beginner:**
1. Read `../BEGINNERS_GUIDE.md` (C++ basics, OBSE64 concepts)
2. Read `WRITING_TO_FILES.md` (how to write files)
3. Read `READING_FROM_FILES.md` (how to read files)
4. Study `FILE_IO_ANNOTATED_EXAMPLE.cpp.md` (complete example)
5. Build your own plugin!

**Experienced coder:**
1. Skim `FILE_IO_ANNOTATED_EXAMPLE.cpp.md` (see the full picture)
2. Use the other guides as reference when needed

---

## ✅ You're Ready When...

You can answer these questions:

- [ ] How do I write text to a file?
- [ ] How do I append to a file without overwriting it?
- [ ] How do I read a file line by line?
- [ ] How do I parse a "Key=Value" configuration file?
- [ ] How do I check if a file opened successfully?
- [ ] What's the difference between `ofstream` and `ifstream`?
- [ ] How do I convert a string to a number?
- [ ] Where should I put my plugin's data files?

If you can answer these, you're ready to build your own file I/O system!

---

## 🆘 Getting Help

If you're stuck:

1. **Check the examples** - All three guides have complete working examples
2. **Read the error messages** - `_MESSAGE()` logs go to `obse64.log`
3. **Use the community** - OBSE/Oblivion modding forums and Discord
4. **Debug with logs** - Add `_MESSAGE()` everywhere to see what's happening

---

## 🎉 Ready to Start?

Pick a guide and dive in:

- **[WRITING_TO_FILES.md](WRITING_TO_FILES.md)** - Learn to write files
- **[READING_FROM_FILES.md](READING_FROM_FILES.md)** - Learn to read files
- **[FILE_IO_ANNOTATED_EXAMPLE.cpp.md](FILE_IO_ANNOTATED_EXAMPLE.cpp.md)** - See complete example

Happy coding! 🚀
