#ifndef __TYPES_H
#define __TYPES_H

typedef unsigned short umode_t;

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
typedef __signed__ long long __s64;
typedef unsigned long long __u64;
#endif

typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

typedef unsigned int uint;

typedef volatile unsigned char	vuchar;
typedef volatile unsigned long	vulong;
typedef volatile unsigned short	vushort;

typedef long time_t;
typedef long suseconds_t;

#include <stddef.h>

#endif
