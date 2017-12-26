// utable.h - UniLib table access functions, 
// Private to UniLib -- do not use these directly.

#pragma once

BYTE WINAPI RLELookup (const BYTE * pbData, BYTE index);
BYTE WINAPI LookupPropRLE (BYTE **prgTable, WCHAR ch);
BYTE WINAPI LookupProp    (BYTE **prgTable, WCHAR ch);

