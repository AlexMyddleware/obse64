#include "PluginAPI.h"
#include "Hooks_Script.h"
#include "Commands_FileIO.h"
#include "Commands_Array.h"
#include "obse64_common/obse64_version.h"
#include <cstring>

static PluginHandle	g_pluginHandle = kPluginHandle_Invalid;

// Plugin version information
extern "C" {
	__declspec(dllexport) OBSEPluginVersionData OBSEPlugin_Version =
	{
		OBSEPluginVersionData::kVersion,

		1,									// plugin version
		"Varla",							// plugin name
		"OBSE64 Team",						// author

		0,									// not address independent (we don't patch memory)
		OBSEPluginVersionData::kStructureIndependence_NoStructs,	// we don't use game structures directly
		{ RUNTIME_VERSION, 0 },				// compatible with current runtime version

		0,									// works with any OBSE version
		0,									// reserved
		0,									// reserved
		{ 0 }								// reserved array
	};
}

// Plugin load callback
extern "C" {
	__declspec(dllexport) bool OBSEPlugin_Load(const OBSEInterface * obse)
	{
		// Get plugin handle
		g_pluginHandle = obse->GetPluginHandle();

		// Register commands
		AddScriptCommand(kCommandInfo_PrintC);
		AddScriptCommand(kCommandInfo_RegisterLog);
		AddScriptCommand(kCommandInfo_ReadFromLog);
		AddScriptCommand(kCommandInfo_UnregisterLog);

		// Varla module commands
		AddScriptCommand(kCommandInfo_VarlaWriteToFile);
		AddScriptCommand(kCommandInfo_VarlaReadFromFile);

		// Array commands
		AddScriptCommand(kCommandInfo_ar_Size);
		AddScriptCommand(kCommandInfo_ar_Construct);

		return true;
	}
}
