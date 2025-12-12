#include "StringVar.h"
#include "GameConsole.h"
#include "GameForms.h"
#include "GameScript.h"
#include <algorithm>
#include <cctype>

// Ported from old OBSE for OBSE64
// Simplified: Removed serialization support for initial implementation

// Helper function for case-insensitive character comparison
inline bool ci_equal(wchar_t ch1, wchar_t ch2)
{
	return towlower(ch1) == towlower(ch2);
}

// Wide string conversion helpers
wchar_t* ConvertToWideString(const char* string) {
	int sizeMultibyteBuffer = MultiByteToWideChar(CP_ACP, 0, string, -1, nullptr, 0);
	wchar_t* widestring = new wchar_t[sizeMultibyteBuffer];
	MultiByteToWideChar(CP_ACP, 0, string, -1, widestring, sizeMultibyteBuffer);
	return widestring;
}

std::tuple<char*, UInt32> ConvertToMultibyteString(const wchar_t* widestring) {
	int sizeMultibyteBuffer = WideCharToMultiByte(CP_ACP, 0, widestring, -1, nullptr, 0, nullptr, nullptr);
	char* string = new char[sizeMultibyteBuffer];
	WideCharToMultiByte(CP_ACP, 0, widestring, -1, string, sizeMultibyteBuffer, nullptr, nullptr);
	return { string, sizeMultibyteBuffer };
}

// StringVar implementation

StringVar::StringVar(const char* in_data, UInt32 in_refID)
	: owningModIndex(in_refID >> 24), modified(false), multibyte_len(0)
{
	wchar_t* widestring = ConvertToWideString(in_data);
	data = std::wstring(widestring);
	delete[] widestring;
}

std::string StringVar::String() {
	auto [string, size] = ConvertToMultibyteString(data.c_str());
	std::string result(string);
	delete[] string;
	return result;
}

const std::tuple<const char*, const UInt16> StringVar::GetCString()
{
	if (modified == true || multibyte_ptr.get() == nullptr) {
		auto [string, size] = ConvertToMultibyteString(data.c_str());
		multibyte_ptr = std::unique_ptr<const char[]>(string);
		multibyte_len = size;
		modified = false;
	}
	return { multibyte_ptr.get(), (UInt16)multibyte_len };
}

void StringVar::Set(const char* newString)
{
	wchar_t* widestring = ConvertToWideString(newString);
	data = std::wstring(widestring);
	delete[] widestring;
	modified = true;
}

SInt32 StringVar::Compare(char* rhs, bool caseSensitive)
{
	SInt32 cmp = 0;
	wchar_t* widecmp = ConvertToWideString(rhs);

	if (!caseSensitive)
		cmp = _wcsicmp(data.c_str(), widecmp);
	else
		cmp = wcscmp(data.c_str(), widecmp);

	delete[] widecmp;

	if (cmp > 0)
		return -1;
	else if (cmp < 0)
		return 1;
	else
		return 0;
}

void StringVar::Insert(const char* subString, UInt32 insertionPos)
{
	wchar_t* wide = ConvertToWideString(subString);
	if (insertionPos < GetLength())
		data.insert(insertionPos, wide);
	else if (insertionPos == GetLength())
		data.append(wide);
	delete[] wide;
	modified = true;
}

#pragma warning(disable : 4996)	// disable checked iterator warning for std::transform with char*
UInt32 StringVar::Find(char* subString, UInt32 startPos, UInt32 numChars, bool bCaseSensitive)
{
	UInt32 pos = std::wstring::npos;

	if (numChars + startPos >= GetLength())
		numChars = GetLength() - startPos;

	if (startPos < GetLength())
	{
		std::wstring source = data.substr(startPos, numChars);
		wchar_t* wide = ConvertToWideString(subString);
		if (!bCaseSensitive)
		{
			std::transform(source.begin(), source.end(), source.begin(), towlower);
			std::transform(wide, wide + wcslen(wide), wide, towlower);
		}

		pos = source.find(wide);
		if (pos != std::wstring::npos)
			pos += startPos;
		delete[] wide;
	}

	return pos;
}

UInt32 StringVar::Count(char* subString, UInt32 startPos, UInt32 numChars, bool bCaseSensitive)
{
	if (numChars + startPos >= GetLength())
		numChars = GetLength() - startPos;

	if (startPos >= GetLength())
		return 0;

	std::wstring source = data.substr(startPos, numChars);

	UInt32 subStringLen = strlen(subString);
	if (!subStringLen)
		return 0;

	wchar_t* wide = ConvertToWideString(subString);
	subStringLen = wcslen(wide);
	if (!bCaseSensitive)
	{
		std::transform(source.begin(), source.end(), source.begin(), towlower);
		std::transform(wide, wide + subStringLen, wide, towlower);
	}

	UInt32 strIdx = 0;
	UInt32 count = 0;
	while (strIdx < GetLength() && ((strIdx = source.find(wide, strIdx)) != std::wstring::npos))
	{
		count++;
		strIdx += subStringLen;
	}
	delete[] wide;
	return count;
}
#pragma warning(default : 4996)

UInt32 StringVar::GetLength()
{
	return data.length();
}

UInt32 StringVar::Replace(char* toReplace, const char* replaceWith, UInt32 startPos, UInt32 numChars, bool bCaseSensitive, UInt32 numToReplace)
{
	// calc length of substring
	if (startPos >= GetLength())
		return 0;
	else if (numChars + startPos > GetLength())
		numChars = GetLength() - startPos;

	UInt32 numReplaced = 0;
	wchar_t* toReplaceWide = ConvertToWideString(toReplace);
	wchar_t* replaceWithWide = ConvertToWideString(replaceWith);

	UInt32 replacementLen = wcslen(replaceWithWide);
	UInt32 toReplaceLen = wcslen(toReplaceWide);

	// create substring
	std::wstring srcStr = data.substr(startPos, numChars);

	// remove substring from original string
	data.erase(startPos, numChars);

	UInt32 strIdx = 0;
	while (numReplaced < numToReplace)
	{
		if (bCaseSensitive)
		{
			strIdx = srcStr.find(toReplaceWide, strIdx);
			if (strIdx == std::wstring::npos)
				break;
		}
		else
		{
			std::wstring strToReplace = toReplaceWide;
			std::wstring::iterator iter = std::search(srcStr.begin() + strIdx, srcStr.end(), strToReplace.begin(), strToReplace.end(), ci_equal);
			if (iter != srcStr.end())
				strIdx = iter - srcStr.begin();
			else
				break;
		}

		numReplaced++;
		srcStr.erase(strIdx, toReplaceLen);
		if (strIdx == srcStr.length())
		{
			srcStr.append(replaceWithWide);
			break;	// reached end of string so all done
		}
		else
		{
			srcStr.insert(strIdx, replaceWithWide);
			strIdx += replacementLen;
		}
	}

	// paste altered string back into original string
	if (startPos == GetLength())
		data.append(srcStr);
	else
		data.insert(startPos, srcStr);

	delete[] replaceWithWide;
	delete[] toReplaceWide;
	modified = true;

	return numReplaced;
}

void StringVar::Erase(UInt32 startPos, UInt32 numChars)
{
	if (numChars + startPos >= GetLength())
		numChars = GetLength() - startPos;

	if (startPos < GetLength())
		data.erase(startPos, numChars);
	modified = true;
}

std::string StringVar::SubString(UInt32 startPos, UInt32 numChars)
{
	if (numChars + startPos >= GetLength())
		numChars = GetLength() - startPos;

	if (startPos < GetLength()) {
		std::wstring sub = data.substr(startPos, numChars);
		auto [string, _] = ConvertToMultibyteString(sub.data());
		std::string result(string);
		delete[] string;
		return result;
	}
	else
		return "";
}

UInt8 StringVar::GetOwningModIndex()
{
	return owningModIndex;
}

UInt32 StringVar::GetCharType(char ch)
{
	UInt32 charType = 0;
	if (isalpha(ch))
		charType |= kCharType_Alphabetic;
	if (isdigit(ch))
		charType |= kCharType_Digit;
	if (ispunct(ch))
		charType |= kCharType_Punctuation;
	if (isprint(ch))
		charType |= kCharType_Printable;
	if (isupper(ch))
		charType |= kCharType_Uppercase;

	return charType;
}

char StringVar::At(UInt32 charPos)
{
	if (charPos < GetLength())
		return (char)data[charPos];  // Note: This may lose unicode data
	else
		return -1;
}

double* StringVar::ToFloat(UInt32 startPos, UInt32 numChars)
{
	// Not implemented in simplified version
	static double result = 0.0;
	std::string sub = SubString(startPos, numChars);
	result = atof(sub.c_str());
	return &result;
}

// StringVarMap implementation

UInt32 StringVarMap::Add(UInt8 varModIndex, const char* data, bool bTemp)
{
	UInt32 varID = GetUnusedID();
	Insert(varID, new StringVar(data, varModIndex << 24));
	if (bTemp)
		MarkTemporary(varID, true);

	return varID;
}

void StringVarMap::Clean()		// clean up any temporary vars
{
	if (m_state) {
		while (m_state->tempVars.size())
		{
			UInt32 idToDelete = *(m_state->tempVars.begin());
			Delete(idToDelete);
		}
	}
}

StringVarMap g_StringMap;

// Simplified AssignToStringVar for OBSE64
// Removed dependency on ExpressionEvaluator and ExtractSetStatementVar
bool AssignToStringVar(ParamInfo * paramInfo, void * arg1, TESObjectREFR * thisObj, TESObjectREFR* contObj, Script * scriptObj, ScriptEventList * eventList, double * result, UInt32 * opcodeOffsetPtr, const char* newValue)
{
	double strID = 0;
	UInt8 modIndex = 0;
	bool bTemp = false;  // Simplified: assume not in expression mode
	StringVar* strVar = NULL;

	UInt32 len = (newValue) ? strlen(newValue) : 0;
	const UInt32 kMaxMessageLength = 0x4000;
	if (!newValue || len >= kMaxMessageLength)		//if null pointer or too long, assign an empty string
		newValue = "";

	// Simplified: For now, always create new string vars
	// TODO: Implement proper variable extraction when script system is more complete
	if (!modIndex && scriptObj)
		modIndex = scriptObj->GetModIndex();

	if (!modIndex)
		modIndex = 0xFF;  // Use last mod index as fallback

	strID = g_StringMap.Add(modIndex, newValue, bTemp);

	*result = strID;

#if _DEBUG
	Console_Print("Assigned string >> \"%s\" (ID: %d)", newValue, (int)strID);
#endif

	return true;
}

// Plugin API implementation

namespace PluginAPI
{
	const char* GetString(UInt32 stringID)
	{
		StringVar* var = g_StringMap.Get(stringID);
		if (var)
			return std::get<0>(var->GetCString());
		else
			return NULL;
	}

	const char* GetStringWithSize(UInt32 stringID, UInt32* size)
	{
		StringVar* var = g_StringMap.Get(stringID);
		if (var) {
			auto [string, len] = var->GetCString();
			if (size) *size = len;
			return string;
		}
		else
			return NULL;
	}

	void SetString(UInt32 stringID, const char* newVal)
	{
		StringVar* var = g_StringMap.Get(stringID);
		if (var)
			var->Set(newVal);
	}

	UInt32 CreateString(const char* strVal, void* owningScript)
	{
		Script* script = (Script*)owningScript;
		if (script)
			return g_StringMap.Add(script->GetModIndex(), strVal);
		else
			return 0;
	}
}
