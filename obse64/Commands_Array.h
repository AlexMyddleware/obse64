#pragma once

#include "GameScript.h"

// Command info declarations
extern CommandInfo kCommandInfo_ar_Size;
extern CommandInfo kCommandInfo_ar_Construct;

// Array access function (for getting array elements by index)
// This is used internally by the array indexing system
u32 GetArrayID(double arrayResult);
bool GetArrayElement(u32 arrayID, u32 index, std::string& outValue);
