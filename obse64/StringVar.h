#pragma once

#include "VarMap.h"
#include "GameScript.h"
#include "Script.h"
#include <string>
#include <memory>
#include <tuple>

// String variable system ported from old OBSE
// Simplified for OBSE64 - serialization support removed for initial implementation

class StringVar
{
	std::wstring data;
	UInt8		owningModIndex;
	std::unique_ptr<const char[]> multibyte_ptr;
	size_t	multibyte_len;
	bool modified;
public:
	StringVar(const char* in_data, UInt32 in_refID);

	void		Set(const char* newString);
	SInt32		Compare(char* rhs, bool caseSensitive);
	void		Insert(const char* subString, UInt32 insertionPos);
	UInt32		Find(char* subString, UInt32 startPos, UInt32 numChars, bool bCaseSensitive = false);	//returns position of substring
	UInt32		Count(char* subString, UInt32 startPos, UInt32 numChars, bool bCaseSensitive = false);
	UInt32		Replace(char* toReplace, const char* replaceWith, UInt32 startPos, UInt32 numChars, bool bCaseSensitive, UInt32 numToReplace = -1);	//returns num replaced
	void		Erase(UInt32 startPos, UInt32 numChars);
	std::string	SubString(UInt32 startPos, UInt32 numChars);
	double*		ToFloat(UInt32 startPos, UInt32 numChars);
	char		At(UInt32 charPos);
	static UInt32	GetCharType(char ch);

	std::string String();
	const std::tuple<const char*, const UInt16>	GetCString();
	UInt32		GetLength();
	UInt8		GetOwningModIndex();
};

enum {
	kCharType_Alphabetic	= 1 << 0,
	kCharType_Digit			= 1 << 1,
	kCharType_Punctuation	= 1 << 2,
	kCharType_Printable		= 1 << 3,
	kCharType_Uppercase		= 1 << 4,
};

class StringVarMap : public VarMap<StringVar>
{
public:
	// Serialization functions removed for initial OBSE64 implementation
	// void Save(OBSESerializationInterface* intfc);
	// void Load(OBSESerializationInterface* intfc);
	void Clean();

	UInt32 Add(UInt8 varModIndex, const char* data, bool bTemp = false);
};

extern StringVarMap g_StringMap;

// Forward declarations
struct ParamInfo;
class TESObjectREFR;
struct ScriptEventList;

bool AssignToStringVar(ParamInfo * paramInfo, void * arg1, TESObjectREFR * thisObj, TESObjectREFR* contObj, Script * scriptObj, ScriptEventList * eventList, double * result, UInt32 * opcodeOffsetPtr, const char* newValue);

namespace PluginAPI
{
	const char* GetString(UInt32 stringID);
	const char* GetStringWithSize(UInt32 stringID, UInt32* len);
	void SetString(UInt32 stringID, const char* newVal);
	UInt32 CreateString(const char* strVal, void* owningScript);
}
