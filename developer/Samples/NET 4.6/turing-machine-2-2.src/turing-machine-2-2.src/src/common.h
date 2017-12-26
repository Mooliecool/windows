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
//  FILE     : common.h
//
//  DESCRIPTION :
//         Common function etc (Declaration)
//
// ##############################################################




// ###############
#ifndef _COMMON_H
#define _COMMON_H

#include <cassert>
#include <ctime>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iterator>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;


#include "macro.h"

// ===================
unsigned int is_udec (char * const str_i);
unsigned int is_dec (char * const str_i);
unsigned int is_hex (char * const str_i);


// ===================
template <typename T>
string to_string (
	const T& value_i, 
	int width_i = -1, 
	char fill_i = ' ', 
	bool align_i = true, 		// true - left
	const string& prefix_i = string ()
	)
{
string		ret_string_value;
ostringstream	tmp_osstr;

        // =================================
        tmp_osstr << prefix_i;
        if (width_i > 0)
        {
                tmp_osstr << setw (width_i);
                tmp_osstr << setfill (fill_i);
        }
        tmp_osstr << (align_i ? left : right) << value_i;

        // =================================
        ret_string_value = tmp_osstr.str();
        // =================================
        return ret_string_value;

} // string to_string (T value_i)


#endif	// _COMMON_H
