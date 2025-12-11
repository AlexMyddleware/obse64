#include "Commands_Array.h"
#include "ArrayTypes.h"
#include "GameConsole.h"
#include "GameScript.h"
#include <vector>
#include <map>
#include <string>

/* ar_Size - Get the size of an array
 * syntax: let size = ar_Size array
 *
 * Returns the number of elements in an array
 */
bool Cmd_ar_Size_Execute(COMMAND_ARGS)
{
	// In OBSE, array variables are passed as special parameters
	// For this simplified implementation, we expect the array ID as a double
	double arrayID = 0;

	if (ExtractArgs(EXTRACT_ARGS, &arrayID))
	{
		u32 id = (u32)arrayID;
		auto it = g_arrayStorage.find(id);
		if (it != g_arrayStorage.end())
		{
			*result = (double)it->second.lines.size();
		}
		else
		{
			*result = 0;
		}
	}

	return true;
}

/* ar_Construct - Construct a new array
 * syntax: let array = ar_Construct arrayType
 *
 * Creates a new array. arrayType can be "array", "map", or "stringmap"
 * For this implementation, we support Map type
 */
bool Cmd_ar_Construct_Execute(COMMAND_ARGS)
{
	char arrayType[64] = "array";

	ExtractArgs(EXTRACT_ARGS, &arrayType);

	// Create a new empty array
	// In a full implementation, this would create different array types
	// For now, we just create a simple array
	extern u32 g_nextArrayID;
	u32 arrayID = g_nextArrayID++;

	// Initialize empty array
	g_arrayStorage[arrayID];

	*result = (double)arrayID;

	return true;
}

// Helper functions for array access
u32 GetArrayID(double arrayResult)
{
	return (u32)arrayResult;
}

bool GetArrayElement(u32 arrayID, u32 index, std::string& outValue)
{
	auto it = g_arrayStorage.find(arrayID);
	if (it != g_arrayStorage.end() && index < it->second.lines.size())
	{
		outValue = it->second.lines[index];
		return true;
	}
	return false;
}

// Parameter definitions
static ParamInfo kParams_ar_Size[1] =
{
	{"array", kParamType_Float, 0} // Simplified - should be kParamType_Array
};

static ParamInfo kParams_ar_Construct[1] =
{
	{"arrayType", kParamType_String, 1}
};

// Command info structures
CommandInfo kCommandInfo_ar_Size =
{
	"ar_Size", "",
	0,
	"Get the size of an array",
	0,
	1, kParams_ar_Size,
	Cmd_ar_Size_Execute
};

CommandInfo kCommandInfo_ar_Construct =
{
	"ar_Construct", "",
	0,
	"Construct a new array",
	0,
	1, kParams_ar_Construct,
	Cmd_ar_Construct_Execute
};
