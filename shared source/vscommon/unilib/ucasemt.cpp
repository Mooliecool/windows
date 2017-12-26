// ucasemt.cpp - Unicode Case conversion - min table version
//----------------------------------------------------------------------------
// Microsoft Confidential
// Copyright (C) 1995-1998 Microsoft Corporation.  All Rights Reserved.
//----------------------------------------------------------------------------
//
#include "pch.h"

static __forceinline bool IsOdd (WCHAR ch) { return  (ch & 1); }
static __forceinline bool IsEven(WCHAR ch) { return !(ch & 1); }

#define HI4(v) ((v) >> 4)   // NOT general-purpose: assumes v is byte size already
#define LO4(v) ((v) & 0xF)
static __forceinline bool IsBit(WCHAR ch, WCHAR base, const unsigned short * rgbits)
{
  unsigned short x = (ch - base);
  unsigned short v = rgbits[HI4(x)];
  return (v & (1 << LO4(x)));
}

//=====================================================================

// to Lowercase
static const unsigned short rgbitsLo_01[] = {
//              // FEDC BA98 7654 3210  
/* 0 */ 0x5555, // 0101 0101 0101 0101 
/* 1 */ 0x5555, // 0101 0101 0101 0101 
/* 2 */ 0x5555, // 0101 0101 0101 0101 
/* 3 */ 0xAA54, // 1010 1010 0101 0100 
/* 4 */ 0x54AA, // 0101 0100 1010 1010 
/* 5 */ 0x5555, // 0101 0101 0101 0101 
/* 6 */ 0x5555, // 0101 0101 0101 0101 
/* 7 */ 0x2A55, // 0010 1010 0101 0101 
};

static const unsigned char rgDxLo_01[] = {
//        0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F    
/* 8 */   0, 210,   1,   0,   1,   0, 206,   1,   0, 205, 205,   1,   0,   0,  79, 202, 
/* 9 */ 203,   1,   0, 205, 207,   0, 211, 209,   1,   0,   0,   0, 211, 213,   0, 214, 
/* A */   1,   0,   1,   0,   1,   0, 218,   1,   0, 218,   0,   0,   1,   0, 218,   1, 
/* B */   0, 217, 217,   1,   0,   1,   0, 219,   1,   0,   0,   0,   1,   0,   0,   0, 
/* C */   0,   0,   0,   0,   2,   1,   0,   2,   1,   0,   2,   1,   0,   1,   0,   1, 
/* D */   0,   1,   0,   1,   0,   1,   0,   1,   0,   1,   0,   1,   0,   0,   1,   0, 
/* E */   1,   0,   1,   0,   1,   0,   1,   0,   1,   0,   1,   0,   1,   0,   1,   0, 
/* F */   0,   2,   1,   0,   1,   0,   0,   0,   1,   0,   1,   0,   1,   0,   1,   0
};

static const char rgDxLo_03[] = { 38, 0, 37, 37, 37, 0, 64, 0, 63, 63, 0 };

static const unsigned short rgbitsLo_04[] = {
//                   FEDC BA98 7654 3210            
/* 6 */    0x5555, //0101 0101 0101 0101            
/* 7 */    0x5555, //0101 0101 0101 0101            
/* 8 */    0x5001, //0101 0000 0000 0001            
/* 9 */    0x5555, //0101 0101 0101 0101            
/* A */    0x5555, //0101 0101 0101 0101            
/* B */    0x5555, //0101 0101 0101 0101            
/* C */    0x088A, //0000 1000 1000 1010            
/* D */    0x5555, //0101 0101 0101 0101            
/* E */    0x5555, //0101 0101 0101 0101            
/* F */    0x0115  //0000 0001 0001 0101            
};

static const unsigned char rgiDxLo_1f[] = {
//      0  1  2  3  4  5  6  7    8    9    A    B    C    D    E    F    
/* 0 */ 0, 0, 0, 0, 0, 0, 0, 0,   8,   8,   8,   8,   8,   8,   8,   8, 
/* 1 */ 0, 0, 0, 0, 0, 0, 0, 0,   8,   8,   8,   8,   8,   8,   0,   0, 
/* 2 */ 0, 0, 0, 0, 0, 0, 0, 0,   8,   8,   8,   8,   8,   8,   8,   8, 
/* 3 */ 0, 0, 0, 0, 0, 0, 0, 0,   8,   8,   8,   8,   8,   8,   8,   8, 
/* 4 */ 0, 0, 0, 0, 0, 0, 0, 0,   8,   8,   8,   8,   8,   8,   0,   0, 
/* 5 */ 0, 0, 0, 0, 0, 0, 0, 0,   0,   8,   0,   8,   0,   8,   0,   8, 
/* 6 */ 0, 0, 0, 0, 0, 0, 0, 0,   8,   8,   8,   8,   8,   8,   8,   8, 
/* 7 */ 0, 0, 0, 0, 0, 0, 0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 
/* 8 */ 0, 0, 0, 0, 0, 0, 0, 0,   8,   8,   8,   8,   8,   8,   8,   8, 
/* 9 */ 0, 0, 0, 0, 0, 0, 0, 0,   8,   8,   8,   8,   8,   8,   8,   8, 
/* A */ 0, 0, 0, 0, 0, 0, 0, 0,   8,   8,   8,   8,   8,   8,   8,   8, 
/* B */ 0, 0, 0, 0, 0, 0, 0, 0,   8,   8,  74,  74,   9,   0,   0,   0, 
/* C */ 0, 0, 0, 0, 0, 0, 0, 0,  86,  86,  86,  86,   9,   0,   0,   0, 
/* D */ 0, 0, 0, 0, 0, 0, 0, 0,   8,   8, 100, 100,   0,   0,   0,   0, 
/* E */ 0, 0, 0, 0, 0, 0, 0, 0,   8,   8, 112, 112,   7,   0,   0,   0, 
/* F */ 0, 0, 0, 0, 0, 0, 0, 0, 128, 128, 126, 126,   9,   0,   0,   0
};

WCHAR WINAPI LowerCaseComplete(WCHAR ch)
{
  switch((BYTE)(ch >> 8))
  {
  case 0x00:
    if      (ch <  0x0041) {}
    else if (ch <= 0x005A) ch += 32;
    else if (ch <  0x00C0) {}
    else if (ch <= 0x00DE) { if (ch != 0x00D7) ch += 32; }
    break;

  case 0x01:
    if (ch < 0x0180)
    {
      if (IsBit(ch, 0x0100, rgbitsLo_01)) ch++;
      else if (ch == 0x0130) ch = 0x0069;
      else if (ch == 0x0178) ch = 0x00FF;
    }
    else if (ch == 0x01F6) ch = 0x0195;
    else if (ch == 0x01F7) ch = 0x01BF;
    else ch += rgDxLo_01[ch - 0x0180];
    break;

  case 0x02: 
    if ((ch < 0x0233) && IsEven(ch) && (ch != 0x0220)) ch++;
    break;

  case 0x03:
    if (ch > 0x0385) 
    {
      if      (ch < 0x0391) ch += rgDxLo_03[ch - 0x0386];
      else if (ch < 0x03AC) { if (ch != 0x03A2) ch += 32; }
      else if (ch < 0x03DA) {}
      else if ((ch < 0x03EF) && IsEven(ch)) ch++;
    }
    break;

  case 0x04: 
    if (ch < 0x0460)
    {
      if (ch < 0x0430)
      {
        if (ch < 0x0410) ch += 80; else ch += 32;
      }
    }
    else if (IsBit(ch, 0x0460, rgbitsLo_04)) 
      ch++;
    break;

  case 0x05: 
    if (IN_RANGE(ch, 0x0531, 0x0556)) ch += 48;
    break;

  case 0x1e: 
    if (IsEven(ch))
    {
      if ((ch <= 0x1E94) || IN_RANGE(ch, 0x1EA0, 0x1EF8)) ch++;
    }
    break;

  case 0x1f: ch -= rgiDxLo_1f[(BYTE)ch]; break;

  case 0x21: 
    if (ch < 0x2160) 
    {
      if      (ch == 0x2126) ch = 0x03C9;
      else if (ch == 0x212A) ch = 0x006B;
      else if (ch == 0x212B) ch = 0x00E5;
    }
    else if (ch >= 0x2170) {}
    else ch += 16;
    break;

  case 0x24: 
    if (IN_RANGE(ch, 0x24B6, 0x24CF)) ch += 26;
    break;

  case 0xff: 
    if (IN_RANGE(ch, 0xFF21, 0xFF3A)) ch += 32;
    break;
  }
  return ch;
};


//=====================================================================

// to Uppercase
static const unsigned short rgbitsUp_01[] = {
//                  FEDC BA98 7654 3210  
/* 0 */  0xAAAA, // 1010 1010 1010 1010
/* 1 */  0xAAAA, // 1010 1010 1010 1010
/* 2 */  0xAAAA, // 1010 1010 1010 1010
/* 3 */  0x54A8, // 0101 0100 1010 1000
/* 4 */  0xA955, // 1010 1001 0101 0101
/* 5 */  0xAAAA, // 1010 1010 1010 1010
/* 6 */  0xAAAA, // 1010 1010 1010 1010
/* 7 */  0x54AA, // 0101 0100 1010 1010
/* 8 */  0x1128, // 0001 0001 0010 1000
/* 9 */  0x0204, // 0000 0010 0000 0100
/* A */  0x212A, // 0010 0001 0010 1010
/* B */  0x2251  // 0010 0010 0101 0001
};

const unsigned char rgiDxUp_01[] = {
//        0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F    
/* C */   0,   0,   0,   0,   0,   1,   2,   0,   1,   2,   0,   1,   2,   0,   1,   0, 
/* D */   1,   0,   1,   0,   1,   0,   1,   0,   1,   0,   1,   0,   1,  79,   0,   1, 
/* E */   0,   1,   0,   1,   0,   1,   0,   1,   0,   1,   0,   1,   0,   1,   0,   1, 
/* F */   0,   0,   1,   2,   0,   1,   0,   0,   0,   1,   0,   1,   0,   1,   0,   1
};

const unsigned char rgiDxUp_02[] = {
//         0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F    
/* 0 */    0,    1,    0,    1,    0,    1,    0,    1,    0,    1,    0,    1,    0,    1,    0,    1, 
/* 1 */    0,    1,    0,    1,    0,    1,    0,    1,    0,    1,    0,    1,    0,    1,    0,    1, 
/* 2 */    0,    0,    0,    1,    0,    1,    0,    1,    0,    1,    0,    1,    0,    1,    0,    1, 
/* 3 */    0,    1,    0,    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
/* 4 */    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
/* 5 */    0,    0,    0,  210,  206,    0,  205,  205,    0,  202,    0,  203,    0,    0,    0,    0, 
/* 6 */  205,    0,    0,  207,    0,    0,    0,    0,  209,  211,    0,    0,    0,    0,    0,  211, 
/* 7 */    0,    0,  213,    0,    0,  214,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
/* 8 */  218,    0,    0,  218,    0,    0,    0,    0,  218,    0,  217,  217,    0,    0,    0,    0, 
/* 9 */    0,    0,  219
};

const char rgiDxUp_03[] = {
//         0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F    
/* A */                                                                           38,   37,   37,   37, 
/* B */    0,   32,   32,   32,   32,   32,   32,   32,   32,   32,   32,   32,   32,   32,   32,   32, 
/* C */   32,   32,   31,   32,   32,   32,   32,   32,   32,   32,   32,   32,   64,   63,   63,    0, 
/* D */   62,   57,    0,    0,    0,   47,   54,    0,    0,    0,    0,    1,    0,    1,    0,    1, 
/* E */    0,    1,    0,    1,    0,    1,    0,    1,    0,    1,    0,    1,    0,    1,    0,    1, 
/* F */   86,   80,   79,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0
};

const unsigned short rgbitsUp_0460[] = {
//                   FEDC BA98 7654 3210    
/* 6 */    0xAAAA, //1010 1010 1010 1010, 
/* 7 */    0xAAAA, //1010 1010 1010 1010, 
/* 8 */    0xA002, //1010 0000 0000 0010, 
/* 9 */    0xAAAA, //1010 1010 1010 1010, 
/* A */    0xAAAA, //1010 1010 1010 1010, 
/* B */    0xAAAA, //1010 1010 1010 1010, 
/* C */    0x1114, //0001 0001 0001 0100, 
/* D */    0xAAAA, //1010 1010 1010 1010, 
/* E */    0xAAAA, //1010 1010 1010 1010, 
/* F */    0x022A  //0000 0010 0010 1010
};

const unsigned char rgiDxUp_1f[] = {
//        0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F    
/* 0 */   8,   8,   8,   8,   8,   8,   8,   8,   0,   0,   0,   0,   0,   0,   0,   0, 
/* 1 */   8,   8,   8,   8,   8,   8,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
/* 2 */   8,   8,   8,   8,   8,   8,   8,   8,   0,   0,   0,   0,   0,   0,   0,   0, 
/* 3 */   8,   8,   8,   8,   8,   8,   8,   8,   0,   0,   0,   0,   0,   0,   0,   0, 
/* 4 */   8,   8,   8,   8,   8,   8,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
/* 5 */   0,   8,   0,   8,   0,   8,   0,   8,   0,   0,   0,   0,   0,   0,   0,   0, 
/* 6 */   8,   8,   8,   8,   8,   8,   8,   8,   0,   0,   0,   0,   0,   0,   0,   0, 
/* 7 */  74,  74,  86,  86,  86,  86, 100, 100, 128, 128, 112, 112, 126, 126,   0,   0, 
/* 8 */   8,   8,   8,   8,   8,   8,   8,   8,   0,   0,   0,   0,   0,   0,   0,   0, 
/* 9 */   8,   8,   8,   8,   8,   8,   8,   8,   0,   0,   0,   0,   0,   0,   0,   0, 
/* A */   8,   8,   8,   8,   8,   8,   8,   8,   0,   0,   0,   0,   0,   0,   0,   0, 
/* B */   8,   8,   0,   9,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
/* C */   0,   0,   0,   9,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
/* D */   8,   8,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
/* E */   8,   8,   0,   0,   0,   7,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
/* F */   0,   0,   0,   9,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

WCHAR WINAPI UpperCase(WCHAR ch)
{
  switch((BYTE)(ch >> 8))
  {
  case 0x00:
    if      (ch <  0x0061) {}
    else if (ch <= 0x007A) ch -= 32;
    else if (ch <  0x00E0) 
    {      
      if (ch == 0x00B5) ch = 0x039C;
    }
    else
    {
      if      (ch == 0x00F7) {}
      else if (ch == 0x00FF) ch = 0x0178; 
      else ch -= 32; 
    }
    break;

  case 0x01: 
    if (ch < 0x01C0)
    {
      if (IsBit(ch, 0x0100, rgbitsUp_01)) ch--;
      else if (ch == 0x0131) ch = 0x0049;
      else if (ch == 0x017F) ch = 0x0053;
      else if (ch == 0x0195) ch = 0x01F6;
      else if (ch == 0x01BF) ch = 0x01F7;
    }
    else ch -= rgiDxUp_01[ch - 0x01C0]; break;

  case 0x02: 
    if (ch < 0x0293) ch -= rgiDxUp_02[ch - 0x0200];
    break;

  case 0x03:
    if (ch < 0x03AC) { if (ch == 0x0345) ch = 0x0399; }
    else ch -= rgiDxUp_03[ch - 0x03AC];
    break;

  case 0x04: //rgiDxUp_04;
    if (ch < 0x0460)
    {
      if      (ch < 0x0430) {}
      else if (ch < 0x0450) ch -= 32;
      else ch -= 80;
    }
    else if (IsBit(ch, 0x0460, rgbitsUp_0460)) 
      ch--;
    break;

  case 0x05: if (IN_RANGE(ch, 0x0561, 0x0586)) ch -= 48; break;

  case 0x1e:
    if (IsOdd(ch)) // in this block, only odd chars have a mapping 
    {
      if      (ch <= 0x1E95) ch--;
      else if (IN_RANGE(ch, 0x1EA1, 0x1EF9)) ch--;
      else if (ch == 0x1E9B) ch = 0x1E60;
    }
    break;

  case 0x1f: 
    if (ch == 0x1FBE) ch = 0x0399;
    else ch += rgiDxUp_1f[(BYTE)ch];
    break;

  case 0x21: if (IN_RANGE(ch, 0x2170, 0x217F)) ch -= 16; break;
  case 0x24: if (IN_RANGE(ch, 0x24D0, 0x24E9)) ch -= 26; break;
  case 0xff: if (IN_RANGE(ch, 0xFF41, 0xFF5A)) ch -= 32; break;
  }
  return ch;
};


WCHAR WINAPI FileSystemUpperCase(WCHAR ch)
{
    switch((BYTE)(ch >> 8))
    {
        case 0x00:
            if (ch != 0xB5) ch = UpperCase(ch);
            break;

        case 0x01:
            if ((ch == 0x131) || (ch == 0x17F) || (ch == 0x195) || (ch == 0x1BF) || (ch == 0x1C5) || (ch == 0x1C8) || (ch == 0x1CB) || (ch == 0x1F2) || (ch == 0x1F9)) {}
            else ch = UpperCase(ch);
            break;

        case 0x02:
            if ((ch == 0x219) || (ch == 0x21B) || (ch == 0x21D) || (ch == 0x21F) || (ch == 0x223) || (ch == 0x225) || (ch == 0x227) || (ch == 0x229) || (ch == 0x22B) || (ch == 0x22D) || (ch == 0x22F) || (ch == 0x231) || (ch == 0x233) || (ch == 0x280)) {}
            else ch = UpperCase(ch);
            break;
                
        case 0x03:
            if ((ch == 0x345) || (ch == 0x3D0) || (ch == 0x3D1) || (ch == 0x3D5) || (ch == 0x3D6) || (ch == 0x3DB) || (ch == 0x3DD) || (ch == 0x3DF) || (ch == 0x3E1) || IN_RANGE(ch, 0x3F0, 0x3F2)) {}
            else ch = UpperCase(ch);
            break;

        case 0x04:
            if ((ch == 0x450) || (ch == 0x45D) || (ch == 0x48D) || (ch == 0x48F) || (ch == 0x4ED)) {}
            else ch = UpperCase(ch);
            break;

        case 0x1E:
            if (ch != 0x1E9B) ch = UpperCase(ch);
            break;

        case 0x1F:
            if (IN_RANGE(ch, 0x1F80, 0x1F87) || IN_RANGE(ch, 0x1F90, 0x1F97) || IN_RANGE(ch, 0x1FA0, 0x1FA7) || (ch == 0x1FB3) || (ch == 0x1FBE) || (ch == 0x1FC3) || (ch == 0x1FF3)) {}
            else ch = UpperCase(ch);
            break;

        default:
            ch = UpperCase(ch);
    }

    return ch;
}

WCHAR WINAPI FileSystemLowerCase(WCHAR ch)
{
    switch((BYTE)(ch >> 8))
    {
        case 0x01:
            if ((ch == 0x130) || (ch == 0x1A6) || (ch == 0x1C5) || (ch == 0x1C8) || (ch == 0x1CB) || (ch == 0x1F2) || IN_RANGE(ch, 0x1F6, 0x1F8)) {}
            else ch = LowerCase(ch);
            break;

        case 0x02:
            if ((ch == 0x218) || (ch == 0x21A) || (ch == 0x21C) || (ch == 0x21E) || (ch == 0x222) || (ch == 0x224) || (ch == 0x226) || (ch == 0x228) || (ch == 0x22A) || (ch == 0x22C) || (ch == 0x22E) || (ch == 0x230) || (ch == 0x232)) {}
            else ch = LowerCase(ch);
            break;

        case 0x03:
            if ((ch == 0x3DA) || (ch == 0x3DC) || (ch == 0x3DE) || (ch == 0x3E0)) {}
            else ch = LowerCase(ch);
            break;

        case 0x04:
            if ((ch == 0x400) || (ch == 0x40D) || (ch == 0x48C) || (ch == 0x48E) || (ch == 0x4EC)) {}
            else ch = LowerCase(ch);
            break;

        case 0x1F:
            if (IN_RANGE(ch, 0x1F88, 0x1F8F) || IN_RANGE(ch, 0x1F98, 0x1F9F) || IN_RANGE(ch, 0x1FA8, 0x1FAF) || (ch == 0x1FBC) || (ch == 0x1FCC) || (ch == 0x1FFC)) {}
            else ch = LowerCase(ch);
            break;

        case 0x21:
            if (ch == 0x2126 || (ch == 0x212A) || (ch == 0x212B)) {}
            else ch = LowerCase(ch);
            break;

        default:
            ch = LowerCase(ch);
    }
            
    return ch;
}
                
