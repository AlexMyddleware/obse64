#pragma once

#include "GameForms.h"

// Minimal Script class for OBSE64
// Based on old OBSE Script.h, simplified for initial implementation
// Will be expanded as more functionality is ported
//
// Note: This class just provides type information. The actual implementation
// exists in the game executable, so we don't provide constructor/destructor.

class Script : public TESForm
{
public:
	// Members will be added as needed
	// For now, we inherit GetModIndex() from TESForm
};
