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
//  FILE     : common.cpp
//
//  DESCRIPTION :
//         Common function etc (Implementation)
//
// ##############################################################



// ###############
#include "common.h"


// ==================================
// ---------------------------------
// Is a string an unsigned decimal?
// ---------------------------------
unsigned int is_udec (char * const str_i)
{
  char *p = &str_i[0];
  while (*p) if (!isdigit (*p++)) return 0;
  return str_i[0];
}

// ==================================
// ---------------------------------
// Is a string a signed decimal?
// ---------------------------------
unsigned int is_dec (char * const str_i)
{
  return (is_udec (&str_i[(str_i[0] == '-') || (str_i[0] == '+')]));
}

// ==================================
// ---------------------------------
// Is a string an hexadecimal?
// ---------------------------------
unsigned int is_hex (char * const str_i)
{
  int start_pos = ((str_i[0] == '0') && (str_i[1] == 'x')) ? 2 : 0;
  char *p = &str_i[start_pos];
  while (*p) if (!isxdigit (*p++)) return 0;
  return str_i[start_pos];
}

