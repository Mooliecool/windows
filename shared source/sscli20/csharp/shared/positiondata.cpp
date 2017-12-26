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

#include "pch.h"

#include "posdata.h"

////////////////////////////////////////////////////////////////////////////////

POSDATA::POSDATA() : iChar(CHARMASK), iUserByte(0xff), iLine(LINEMASK), iUserBits(0xf)
{
}

////////////////////////////////////////////////////////////////////////////////

POSDATA::POSDATA (long i, long c) : iChar(c), iUserByte(0), iLine(i), iUserBits(0)
{
}

////////////////////////////////////////////////////////////////////////////////

bool POSDATA::Adjust (const POSDATA &posOld, const POSDATA &posNew)
{
    ASSERT(!(this->IsUninitialized() || posOld.IsUninitialized() || posNew.IsUninitialized()));

    // Nothing to adjust if the change is below us, or isn't really a change
    if (posOld.iLine > iLine || posOld == posNew)
        return false;

    if (posOld.iLine == iLine)
    {
        // The old position is on the same line as us.  If the
        // char position is before us, update it.
        if (posOld.iChar < iChar)
        {
            iChar += (posNew.iChar - posOld.iChar);
            iLine = posNew.iLine;
            return true;
        }

        return false;
    }

    // The line must be above us, so just update our line
    iLine += (posNew.iLine - posOld.iLine);
    return (posNew.iLine - posOld.iLine) ? true : false;
}

////////////////////////////////////////////////////////////////////////////////

long POSDATA::Compare (const POSDATA &p) const
{
    ASSERT(!(this->IsUninitialized() || p.IsUninitialized()));

    if (this->iLine == p.iLine)
        return this->iChar - p.iChar;

    return this->iLine - p.iLine;
}

////////////////////////////////////////////////////////////////////////////////

bool POSDATA::operator < (const POSDATA &p) const
{
    return Compare (p) < 0;
}

////////////////////////////////////////////////////////////////////////////////

bool POSDATA::operator > (const POSDATA &p) const
{
    return Compare (p) > 0;
}

////////////////////////////////////////////////////////////////////////////////

bool POSDATA::operator <= (const POSDATA &p) const
{
    return Compare (p) <= 0;
}

////////////////////////////////////////////////////////////////////////////////

bool POSDATA::operator >= (const POSDATA &p) const
{
    return Compare (p) >= 0;
}

////////////////////////////////////////////////////////////////////////////////

bool POSDATA::operator == (const POSDATA &p) const
{
    return this->iLine == p.iLine && this->iChar == p.iChar;
}

////////////////////////////////////////////////////////////////////////////////

bool POSDATA::operator != (const POSDATA &p) const
{
    return this->iLine != p.iLine || this->iChar != p.iChar;
}

////////////////////////////////////////////////////////////////////////////////

void POSDATA::SetUninitialized()
{
    iChar = CHARMASK; iLine = LINEMASK;
}

////////////////////////////////////////////////////////////////////////////////

bool POSDATA::IsUninitialized () const
{
    return ((iChar & CHARMASK) == CHARMASK) && ((iLine & LINEMASK) == LINEMASK);
}

////////////////////////////////////////////////////////////////////////////////

bool POSDATA::IsZero () const
{
    return iLine == 0 && iChar == 0;
}
