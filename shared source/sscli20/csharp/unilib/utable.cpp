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

// UTable.cpp - UniLib property table routines
//
#include "pch.h"

/*
 The UniLib tables are based on 2D tables indexed by hi/lo byte of the WCHAR.

 The tables are optimized as follows:

 Complete rows that contain the same value are optimized by setting the pointer 
 for that row to the value itself. In Windows, byte values and pointers are 
 always distinguishable.

 Duplicate row data is removed, so multiple entries can point to the same row.

 Some of the tables contain rows that are run-length-encoded.

 RLE row format:
 BYTE cb, data...
 max cb is 256 bytes
 if cb is 0, then the row is uncompressed and can be indexed directly

*/

//----------------------------------------------------------------------------
// RLELookup - look up a byte from an RLE-compressed row  
//
// To lookup, we don't decompress the row and index it.
// Instead, we walk the compressed data until we find the node
// that the index resides in.
//
BYTE WINAPI RLELookup (const BYTE * pbData, BYTE index)
{
  int cb = (char)*pbData++; // get row length
  if (0 == cb)              // if row is uncompressed...
    return pbData[index];   // ...just index it

  // 'vcb' == 'virtual cb'
  int vcbLast = 0;
  cb = (char)*pbData++;
  int vcb     = (cb >= 0 ? cb : -cb);
  while (index >= vcb)
  {
    if (cb >= 0) pbData++; else pbData += (-cb);
    vcbLast = vcb;
    cb = (char)*pbData++;
    vcb = vcb + (cb >= 0 ? cb : -cb);
  }
  return (cb >= 0) ? *pbData : pbData[index - vcbLast];
}

//----------------------------------------------------------------------------
// LookupPropRLE - look up a byte in a hi/lo table of RLE rows
//
BYTE WINAPI LookupPropRLE (BYTE **prgTable, WCHAR ch)
{
  BYTE * p = prgTable[ch>>8];
  if ((INT_PTR)p & ~0xff)
  {
    if (*p++)
      return RLELookup(p, ch & 0xff);
    else
      return p[ch & 0xff];
  }
  return (BYTE) ((INT_PTR) p);
}

//----------------------------------------------------------------------------
// LookupProp - look up a byte in a hi/lo table
//
BYTE WINAPI LookupProp (BYTE **prgTable, WCHAR ch)
{
  BYTE * p = prgTable[ch>>8];
  if ((INT_PTR)p > 0xff)
    return p[(BYTE)ch];
  return (BYTE) ((INT_PTR) p);
}
