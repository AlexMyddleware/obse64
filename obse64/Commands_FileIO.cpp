#include "Commands_FileIO.h"
#include "ArrayTypes.h"
#include "GameConsole.h"
#include "GameScript.h"
#include "obse64_common/Log.h"
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <shlobj.h>

// Log management structures
struct LogFile {
	std::string name;
	std::string fullPath;
	std::ofstream writeStream;
	int mode; // 0 = read, 1 = write/append
	bool isOpen;

	LogFile() : mode(0), isOpen(false) {}
};

// Global state for log management
static std::map<std::string, LogFile> g_registeredLogs;

// Array storage (defined in ArrayTypes.h, implemented here)
std::map<u32, OBSEArray> g_arrayStorage;
u32 g_nextArrayID = 1;

// Helper function to get the log directory path
static std::string GetLogDirectory()
{
	char path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, path)))
	{
		std::string logPath = std::string(path) + "\\My Games\\Oblivion\\";
		CreateDirectoryA(logPath.c_str(), NULL);
		return logPath;
	}
	return "";
}

/* PrintC - Print formatted string to console and to registered log file (if any)
 * syntax: PrintC fmtstring [num1] [num2] ...
 * shortname: printc
 *
 * Enhanced version of PrintToConsole that also writes to a registered log file
 */
bool Cmd_PrintC_Execute(COMMAND_ARGS)
{
	char fmtstring[BUFSIZ];
	float f0 = 0, f1 = 0, f2 = 0, f3 = 0, f4 = 0, f5 = 0, f6 = 0, f7 = 0, f8 = 0;

	if (ExtractArgs(EXTRACT_ARGS, &fmtstring, &f0, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8))
	{
		// Format the string
		char buffer[BUFSIZ * 2];
		sprintf_s(buffer, sizeof(buffer), fmtstring, f0, f1, f2, f3, f4, f5, f6, f7, f8);

		// Print to console
		Console_Print("%s", buffer);

		// Write to all registered logs in write mode
		for (auto& pair : g_registeredLogs)
		{
			LogFile& log = pair.second;
			if (log.isOpen && log.mode == 1 && log.writeStream.is_open())
			{
				log.writeStream << buffer << std::endl;
				log.writeStream.flush();
			}
		}
	}

	return true;
}

/* RegisterLog - Register a log file for reading or writing
 * syntax: RegisterLog "logname" mode
 *
 * Registers a log file. Mode: 0 = read, 1 = write/append
 * Log files are created in My Documents\My Games\Oblivion\
 */
bool Cmd_RegisterLog_Execute(COMMAND_ARGS)
{
	char logName[256];
	u32 mode = 0;

	if (ExtractArgs(EXTRACT_ARGS, &logName, &mode))
	{
		std::string logDir = GetLogDirectory();
		if (logDir.empty())
		{
			Console_Print("RegisterLog: Failed to get log directory");
			return true;
		}

		std::string fullPath = logDir + std::string(logName) + ".log";

		// Check if already registered
		auto it = g_registeredLogs.find(logName);
		if (it != g_registeredLogs.end())
		{
			// Close existing log
			if (it->second.writeStream.is_open())
				it->second.writeStream.close();
			g_registeredLogs.erase(it);
		}

		// Register new log
		LogFile& log = g_registeredLogs[logName];
		log.name = logName;
		log.fullPath = fullPath;
		log.mode = mode;
		log.isOpen = true;

		if (mode == 1) // Write mode
		{
			// Open in append mode
			log.writeStream.open(fullPath, std::ios::out | std::ios::app);
			if (!log.writeStream.is_open())
			{
				Console_Print("RegisterLog: Failed to open log file for writing: %s", fullPath.c_str());
				log.isOpen = false;
			}
		}
		// For read mode, we'll open the file when ReadFromLog is called

		#if _DEBUG
		Console_Print("RegisterLog: Registered '%s' (mode %d) at %s", logName, mode, fullPath.c_str());
		#endif
	}

	return true;
}

/* ReadFromLog - Read all lines from a registered log file
 * syntax: let array = ReadFromLog "logname"
 *
 * Reads all lines from a log file and returns them as an array
 * The array can be accessed with ar_Size and indexed with []
 */
bool Cmd_ReadFromLog_Execute(COMMAND_ARGS)
{
	char logName[256];

	if (ExtractArgs(EXTRACT_ARGS, &logName))
	{
		// Check if log is registered
		auto it = g_registeredLogs.find(logName);
		if (it == g_registeredLogs.end())
		{
			Console_Print("ReadFromLog: Log '%s' not registered", logName);
			*result = 0;
			return true;
		}

		LogFile& log = it->second;

		// Read the file
		std::ifstream file(log.fullPath);
		if (!file.is_open())
		{
			Console_Print("ReadFromLog: Failed to open log file: %s", log.fullPath.c_str());
			*result = 0;
			return true;
		}

		// Create array and read lines
		u32 arrayID = g_nextArrayID++;
		OBSEArray& arr = g_arrayStorage[arrayID];

		std::string line;
		while (std::getline(file, line))
		{
			arr.lines.push_back(line);
		}
		file.close();

		#if _DEBUG
		Console_Print("ReadFromLog: Read %d lines from '%s'", (int)arr.lines.size(), logName);
		#endif

		// Return array ID
		// NOTE: This is a simplified implementation. In a full OBSE implementation,
		// this would integrate with the proper array variable system
		*result = arrayID;
	}

	return true;
}

/* UnregisterLog - Unregister a log file
 * syntax: UnregisterLog "logname" flush saveIndex
 *
 * Closes and unregisters a log file
 * flush: whether to flush the file (typically 0)
 * saveIndex: whether to save index (typically 0)
 */
bool Cmd_UnregisterLog_Execute(COMMAND_ARGS)
{
	char logName[256];
	u32 flush = 0;
	u32 saveIndex = 0;

	if (ExtractArgs(EXTRACT_ARGS, &logName, &flush, &saveIndex))
	{
		auto it = g_registeredLogs.find(logName);
		if (it != g_registeredLogs.end())
		{
			if (it->second.writeStream.is_open())
			{
				if (flush)
					it->second.writeStream.flush();
				it->second.writeStream.close();
			}
			g_registeredLogs.erase(it);

			#if _DEBUG
			Console_Print("UnregisterLog: Unregistered '%s'", logName);
			#endif
		}
	}

	return true;
}

// Parameter definitions
static ParamInfo kParams_PrintC[10] =
{
	{"string", kParamType_String, 0},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1},
	{"float", kParamType_Float, 1}
};

static ParamInfo kParams_RegisterLog[2] =
{
	{"logName", kParamType_String, 0},
	{"mode", kParamType_Integer, 0}
};

static ParamInfo kParams_ReadFromLog[1] =
{
	{"logName", kParamType_String, 0}
};

static ParamInfo kParams_UnregisterLog[3] =
{
	{"logName", kParamType_String, 0},
	{"flush", kParamType_Integer, 1},
	{"saveIndex", kParamType_Integer, 1}
};

// Command info structures
CommandInfo kCommandInfo_PrintC =
{
	"PrintC", "printc",
	0,
	"Print formatted string to console and log file",
	0,
	10, kParams_PrintC,
	Cmd_PrintC_Execute
};

CommandInfo kCommandInfo_RegisterLog =
{
	"RegisterLog", "",
	0,
	"Register a log file for reading or writing",
	0,
	2, kParams_RegisterLog,
	Cmd_RegisterLog_Execute
};

CommandInfo kCommandInfo_ReadFromLog =
{
	"ReadFromLog", "",
	0,
	"Read all lines from a registered log file",
	0,
	1, kParams_ReadFromLog,
	Cmd_ReadFromLog_Execute
};

CommandInfo kCommandInfo_UnregisterLog =
{
	"UnregisterLog", "",
	0,
	"Unregister a log file",
	0,
	3, kParams_UnregisterLog,
	Cmd_UnregisterLog_Execute
};
