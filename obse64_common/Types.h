#pragma once

#include <intrin.h>

typedef unsigned __int8		u8;
typedef unsigned __int16	u16;
typedef unsigned __int32	u32;
typedef unsigned __int64	u64;
typedef signed __int8		s8;
typedef signed __int16		s16;
typedef signed __int32		s32;
typedef signed __int64		s64;
typedef float				f32;
typedef double				f64;
typedef u64					uint;

typedef u8	unk8;
typedef u16	unk16;
typedef u32	unk32;
typedef u64	unk64;

// Legacy OBSE type names for compatibility
typedef u8		UInt8;
typedef u16		UInt16;
typedef u32		UInt32;
typedef u64		UInt64;
typedef s8		SInt8;
typedef s16		SInt16;
typedef s32		SInt32;
typedef s64		SInt64;

inline u16 swap16(u16 a)
{
	return _byteswap_ushort(a);
}

inline u32 swap32(u32 a)
{
	return _byteswap_ulong(a);
}

inline u64 swap64(u64 a)
{
	return _byteswap_uint64(a);
}
