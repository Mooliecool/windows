// ==============================================================
//
//  Copyright (c) 2002-2004 Alex Vinokur.
//
//  ------------------------------------------------------------
//  This file is part of C++ Simulator of a Nondeterministic Turing Machine.
//
//  C++ Simulator of a Nondeterministic Turing Machine is free software; 
//  you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License,
//  or (at your option) any later version.
//
//  C++ Simulator of a Nondeterministic Turing Machine is distributed in the hope 
//  that it will be useful, but WITHOUT ANY WARRANTY; 
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with C++ Simulator of a Nondeterministic Turing Machine; 
//  if not, write to the Free Software Foundation, Inc., 
//  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  ------------------------------------------------------------
// 
//  mailto:alexvn@connect.to
//  http://up.to/alexv
//
// ==============================================================


// ##############################################################
//
//  SOFTWARE : Nondeterministic Turing Machine (C++ Simulator)
//  FILE     : macro.h
//
//  DESCRIPTION :
//         Various macro
//
// ##############################################################



// ###############
#ifndef _MACRO_H
#define _MACRO_H

#include "version.h"

#define	COUT	cout << "[ " << setw(12) << left << __FILE__ << ", " << setw (3) << right << __LINE__ << " ] "
#define	FATAL_MSG(t)	cout << endl; COUT << "FATAL ERROR : " << t << endl
#define	ERROR_MSG(t)	cout << endl; COUT << "ERROR       : " << t << endl
#define	WARNING_MSG(t)	COUT << "WARNING     : " << t << endl
#define	DIAGNOSTIC_MSG(t)	cout << endl; COUT << "----------- : " << t << endl
#define	INFO_MSG(t)	cout << string (20, '#') << endl; cout << "INFO : " << t << endl; cout << string (20, '#') << endl

#define	IF_NOT_EMPTY(mg, wd, ch) \
		if (!mg.empty()) \
		{ \
		cout << "\t" << string (wd, ch) << " " << mg << " " << string (wd, ch) << endl; \
		}

#define MIN_VALUE(x,y)  ((x) < (y) ? (x) : (y))
#define MAX_VALUE(x,y)  ((x) > (y) ? (x) : (y))

#define ASSERT(x)       if (!(x)) {cerr << endl << endl << "\t=== BUG IN PROGRAM ===" << endl;}; assert (x)

#endif	// _MACRO_H
