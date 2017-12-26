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
//  nmtime.h - defines DOS packed date and time types
//
// Purpose:
//    This file defines the DOS packed date and time types.


#define MASK4   0xf     // 4 bit mask
#define MASK5   0x1f    // 5 bit mask
#define MASK6   0x3f    // 6 bit mask
#define MASK7   0x7f    // 7 bit mask

#define DAYLOC      0   // day value starts in bit 0
#define MONTHLOC    5   // month value starts in bit 5
#define YEARLOC     9   // year value starts in bit 9

#define SECLOC      0   // seconds value starts in bit 0
#define MINLOC      5   // minutes value starts in bit 5
#define HOURLOC     11  // hours value starts in bit 11

#define DOS_DAY(dword)      (((dword) >> DAYLOC) & MASK5)
#define DOS_MONTH(dword)    (((dword) >> MONTHLOC) & MASK4)
#define DOS_YEAR(dword)     (((dword) >> YEARLOC) & MASK7)

#define DOS_HOUR(tword) (((tword) >> HOURLOC) & MASK5)
#define DOS_MIN(tword)  (((tword) >> MINLOC) & MASK6)
#define DOS_SEC(tword)  (((tword) >> SECLOC) & MASK5)

extern time_t CDECL _dostotime_t(int, int, int, int, int, int);

#define XTIME(d,t)  _dostotime_t(DOS_YEAR(d),                      \
            DOS_MONTH(d),                          \
            DOS_DAY(d),                        \
            DOS_HOUR(t),                           \
            DOS_MIN(t),                        \
            DOS_SEC(t)*2)
