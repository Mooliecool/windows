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
#ifndef _COMDATETIME_H_
#define _COMDATETIME_H_

#include <oleauto.h>
#include "fcall.h"

#include <pshpack1.h>

class COMDateTime {
    static const INT64 TicksPerMillisecond;
    static const INT64 TicksPerSecond;
    static const INT64 TicksPerMinute;
    static const INT64 TicksPerHour;
    static const INT64 TicksPerDay;

    static const INT64 MillisPerSecond;
	static const INT64 MillisPerDay;

    static const int DaysPer4Years;
    static const int DaysPer100Years;
    static const int DaysPer400Years;
    // Number of days from 1/1/0001 to 1/1/10000
    static const int DaysTo10000;

	static const int DaysTo1899;

	static const INT64 DoubleDateOffset;
	static const INT64 OADateMinAsTicks;  // in ticks
	static const double OADateMinAsDouble;
	static const double OADateMaxAsDouble;

	static const INT64 MaxTicks;
	static const INT64 MaxMillis;

	static MethodTable *g_pDateTimeMethodTable;

public:

	// Native util functions for other classes.
	static INT64 DoubleDateToTicks(const double d);  // From OleAut Date
	static double TicksToDoubleDate(const INT64 ticks);

	static FORCEINLINE MethodTable * GetMethodTable()		
	{
		WRAPPER_CONTRACT;
		if (g_pDateTimeMethodTable == NULL)
			g_pDateTimeMethodTable = (&g_Mscorlib)->GetClass(CLASS__DATE_TIME);
		return g_pDateTimeMethodTable;
	}
};

#include <poppack.h>

#endif // _COMDATETIME_H_
