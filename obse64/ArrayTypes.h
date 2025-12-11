#pragma once

#include <vector>
#include <map>
#include <string>
#include "obse64_common/Types.h"

// Simple array storage for OBSE arrays
// In a full OBSE implementation, this would be more sophisticated
struct OBSEArray {
	std::vector<std::string> lines;
	std::map<std::string, std::string> stringMap;
	std::map<u32, std::string> intMap;
};

// Global array storage
extern std::map<u32, OBSEArray> g_arrayStorage;
extern u32 g_nextArrayID;
