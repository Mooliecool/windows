// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
// ===========================================================================
// File: smallstructs.cpp
// 
// tests marshalling of longs and small structures
// ===========================================================================

#include "windows.h"
#include "stdlib.h"

struct Struct1 {
    unsigned char a;
};

struct Struct2 {
    unsigned char a;
    unsigned char b;
};

struct Struct3 {
    unsigned char a;
    unsigned char b;
    unsigned char c;
};

struct Struct4 {
    unsigned char a;
    unsigned char b;
    unsigned char c;
    unsigned char d;
};

struct Struct5 {
    unsigned char a;
    unsigned char b;
    unsigned char c;
    unsigned char d;
    unsigned char e;
};

struct Struct8 {
    unsigned char a;
    unsigned char b;
    unsigned char c;
    unsigned char d;
    unsigned char e;
    unsigned char f;
    unsigned char g;
    unsigned char h;
};

extern "C" UINT8 __stdcall ByteTest(UINT8 (__stdcall *callback)(UINT8 b, UINT8 * _b), UINT8 b, UINT8 * _b)
{
    b = (UINT8)(b + 123456789);
    *_b = (UINT8)(*_b + 123456789);
    return (UINT8)(123456789 + callback(b, _b));
}

extern "C" INT8 __stdcall SByteTest(INT8 b, INT8 (__stdcall *callback)(INT8 b, INT8 * _b), INT8 * _b)
{
    b = (INT8)(b + 123456789);
    *_b = (INT8)(*_b + 123456789);
    return (INT8)(123456789 + callback(b, _b));
}

extern "C" INT16 __stdcall ShortTest(INT16 s, INT16 * _s, INT16 (__stdcall *callback)(INT16 s, INT16 * _s))
{
    s = (INT16)(s + 123456789);
    *_s = (INT16)(*_s + 123456789);
    return (INT16)(123456789 + callback(s, _s));
}

extern "C" UINT16 __stdcall UShortTest(UINT16 (__stdcall *callback)(UINT16 s, UINT16 * _s), UINT16 s, UINT16 * _s)
{
    s = (UINT16)(s + 123456789);
    *_s = (UINT16)(*_s + 123456789);
    return (UINT16)(123456789 + callback(s, _s));
}

extern "C" INT64 __stdcall LongTest(INT64 l, INT64 (__stdcall *callback)(INT64 l, INT64 * _l), INT64 * _l)
{
    l = l + 123456789;
    *_l = *_l + 123456789;
    return 123456789 + callback(l, _l);
}

extern "C" Struct1 StructTest1(Struct1 s, Struct1 *_s)
{
    Struct1 ret;
    ret.a = 2*s.a + 5*_s->a + 104;
    return ret;
}

extern "C" Struct2 StructTest2(Struct2 s, Struct2 *_s)
{
    Struct2 ret;
    ret.a = 2*s.a + 5*_s->a + 104;
    ret.b = 2*s.b + 5*_s->b + 108;
    return ret;
}

extern "C" Struct3 StructTest3(Struct3 s, Struct3 *_s)
{
    Struct3 ret;
    ret.a = 2*s.a + 5*_s->a + 104;
    ret.b = 2*s.b + 5*_s->b + 108;
    ret.c = 2*s.c + 5*_s->c + 112;
    return ret;
}

extern "C" Struct4 StructTest4(Struct4 s, Struct4 *_s)
{
    Struct4 ret;
    ret.a = 2*s.a + 5*_s->a + 104;
    ret.b = 2*s.b + 5*_s->b + 108;
    ret.c = 2*s.c + 5*_s->c + 112;
    ret.d = 2*s.d + 5*_s->d + 116;
    return ret;
}

extern "C" Struct5 StructTest5(Struct5 s, Struct5 *_s)
{
    Struct5 ret;
    ret.a = 2*s.a + 5*_s->a + 104;
    ret.b = 2*s.b + 5*_s->b + 108;
    ret.c = 2*s.c + 5*_s->c + 112;
    ret.d = 2*s.d + 5*_s->d + 116;
    ret.e = 2*s.e + 5*_s->e + 120;
    return ret;
}

extern "C" Struct8 StructTest8(Struct8 s, Struct8 *_s)
{
    Struct8 ret;
    ret.a = 2*s.a + 5*_s->a + 104;
    ret.b = 2*s.b + 5*_s->b + 108;
    ret.c = 2*s.c + 5*_s->c + 112;
    ret.d = 2*s.d + 5*_s->d + 116;
    ret.e = 2*s.e + 5*_s->e + 120;
    ret.f = 2*s.f + 5*_s->f + 124;
    ret.g = 2*s.g + 5*_s->g + 128;
    ret.h = 2*s.h + 5*_s->h + 132;
    return ret;
}
