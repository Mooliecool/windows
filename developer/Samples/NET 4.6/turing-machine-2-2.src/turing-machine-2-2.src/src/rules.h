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
//  FILE     : rules.h
//
//  DESCRIPTION :
//         Classes CurSituation & NextSituation (Definition)
//
// ##############################################################




// ###############
#ifndef _RULES_H
#define _RULES_H

#include "defs.h"


// =========
class CurSituation
{
friend bool operator< (const CurSituation& inst1_i, const CurSituation& inst2_i);
  private :
    const state_t		cur_state_;
    const vector<symbol_t>	cur_symbols_;	// cur_symbols_.size() == number of tapes

  public :
    CurSituation ();
    CurSituation (const state_t& cur_state_i, const vector<symbol_t>& cur_symbols_i);
    ~CurSituation ();

   state_t	get_state () const;
   symbol_t	get_symbol (size_t index_i) const;
   size_t	get_total_symbols () const;
   string	getstr_symbols (size_t max_symbol_size_i = 0) const;
};

// =========
class SymbolAndShift 
{ 
friend class NextSituation;
  private :
    symbol_t symbol_; 
    shift_t shift_; 

  public :
    SymbolAndShift (const symbol_t& symbol_i, shift_t shift_i) : symbol_ (symbol_i), shift_ (shift_i) {}
    ~SymbolAndShift () {} 
};

// =========
class NextSituation
{
  private :
    state_t			next_state_;
    vector<SymbolAndShift>	next_symbols_and_shifts_;	// next_symbol_and_shift_.size() == number of tapes

  public :
    NextSituation ();
    NextSituation (const state_t& next_state_i, const vector<SymbolAndShift>& next_symbols_and_shifts_i);
    ~NextSituation ();	

   state_t	get_state () const;
   symbol_t	get_symbol (size_t index_i) const;
   shift_t	get_shift (size_t index_i) const;
   size_t	get_total_symbols () const;
   string	getstr_symbols_and_shifts(size_t max_symbol_size_i = 0) const;
};



#endif	// _RULES_H
