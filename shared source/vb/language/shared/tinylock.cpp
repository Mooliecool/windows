//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Smallest possible implementation of an exclusive lock.
//
//-------------------------------------------------------------------------------------------------

#include "StdAfx.h"

LONG CTinyLock::m_iSpins = 0;
LONG CTinyLock::m_iSleeps = 0;
LONG CTinyLock::m_iSpinStart = -1;

