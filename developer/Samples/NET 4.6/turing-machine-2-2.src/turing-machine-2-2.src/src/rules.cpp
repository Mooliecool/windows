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
//  FILE     : rules.cpp
//
//  DESCRIPTION :
//         Classes CurSituation & NextSituation (Implementation)
//
// ##############################################################




// ###############
#include "rules.h"

// =========
// =========
// Constructor-0
CurSituation::CurSituation ()
{
} 

// =========
// Constructor-1
CurSituation::CurSituation (const state_t& cur_state_i, const vector<symbol_t>& cur_symbols_i)
	:
	cur_state_ (cur_state_i),
	cur_symbols_ (cur_symbols_i)
{
} 

// =========
// Destructor
CurSituation::~CurSituation ()
{
} 

// =========
state_t CurSituation::get_state () const
{
  return cur_state_;
} 

// =========
symbol_t	CurSituation::get_symbol (size_t index_i) const
{
  assert (index_i < cur_symbols_.size());
  return cur_symbols_[index_i];
}

// =========
size_t	CurSituation::get_total_symbols () const
{
  return cur_symbols_.size();
}

// =========
string	CurSituation::getstr_symbols (size_t max_symbol_size_i) const
{
ostringstream oss;
  for (size_t i = 0; i < cur_symbols_.size(); i++)
  {
    oss << setw(max_symbol_size_i) << left << to_string (cur_symbols_[i]).c_str() << " ";
  }
  //copy (cur_symbols_.begin (), cur_symbols_.end (), ostream_iterator<symbol_t> (oss, " "));
  return oss.str();
}



// =========
bool operator< (const CurSituation& inst1_i, const CurSituation& inst2_i)
{
  if (inst1_i.cur_state_ < inst2_i.cur_state_)	return true;
  if (inst1_i.cur_state_ > inst2_i.cur_state_)	return false;

  assert (inst1_i.cur_symbols_.size() == inst2_i.cur_symbols_.size()); 
const size_t the_size = inst1_i.cur_symbols_.size();
  for (size_t i = 0; i < the_size; i++)
  {
    if (inst1_i.cur_symbols_[i] < inst2_i.cur_symbols_[i])
    {
      return true;
    }
    if (inst1_i.cur_symbols_[i] > inst2_i.cur_symbols_[i])
    {
      return false;
    }

  }
  return false;
}



// =========
// =========
// Constructor-0
NextSituation::NextSituation ()
{
} 

// =========
// Constructor-1
NextSituation::NextSituation (const state_t& next_state_i, const vector<SymbolAndShift>& next_symbols_and_shifts_i)
	:
	next_state_ (next_state_i),
	next_symbols_and_shifts_ (next_symbols_and_shifts_i)
{
} 

// =========
// Destructor
NextSituation::~NextSituation ()
{
} 

// =========
state_t NextSituation::get_state () const
{
  return next_state_;
} 

// =========
symbol_t	NextSituation::get_symbol (size_t index_i) const
{
  assert (index_i < next_symbols_and_shifts_.size());
  return next_symbols_and_shifts_[index_i].symbol_;
}

// =========
size_t	NextSituation::get_total_symbols () const
{
  return next_symbols_and_shifts_.size();
}

// =========
shift_t	NextSituation::get_shift (size_t index_i) const
{
  assert (index_i < next_symbols_and_shifts_.size());
  return next_symbols_and_shifts_[index_i].shift_;
}

// =========
string	NextSituation::getstr_symbols_and_shifts(size_t max_symbol_size_i) const
{
ostringstream oss;
  for (size_t i = 0; i < next_symbols_and_shifts_.size(); i++)
  {
    oss << "(" << setw (max_symbol_size_i) << left << to_string (next_symbols_and_shifts_[i].symbol_).c_str() << ", " << next_symbols_and_shifts_[i].shift_ << ") ";
  }
  return oss.str();
}

