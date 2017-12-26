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
//  FILE     : tape.h
//
//  DESCRIPTION :
//         Class Tape (Definition)
//
// ##############################################################




// ###############
#ifndef _TAPE_H
#define _TAPE_H

#include "defs.h"


// =========
#define LEFT_SHIFT	"L"
#define NO_SHIFT	"N"
#define RIGHT_SHIFT	"R"

#define HEAD_START_POSITION_DEFAULT		0
#define HEAD_START_POSITION_POINTER_REDERVED_SYMBOL	"-"

// =========
enum SemiTapeType { LEFT_SEMI_TAPE, RIGHT_SEMI_TAPE };
struct ActualLocation
{
  SemiTapeType actual_semi_tape_type_;
  ulong        actual_position_;
};

// =========
class Tape
{
friend class NondeterministicTuringMachine;
  private :
    // ------ Private Data ------
    const vector<symbol_t>	empty_symbols_alphabet_;
    const vector<symbol_t>	internal_alphabet_;
    const vector<symbol_t>	input_alphabet_;
    
    vector<symbol_t>		right_semi_tape_;
    vector<symbol_t>		left_semi_tape_;
    long			logical_position_;
    bool			check_results_;
    size_t			max_symbol_size_;

    map<long, ulong>		cell_visits_;

    // ------ Private Methods ------
    void	clear_it();
    vector<symbol_t>	get_full_alphabet() const;

    void set_input (const vector<symbol_t>& input_words_i, long init_pos_i);

    void	shift_position (shift_t shift_i);
    void	right_shift();
    void	left_shift();
    symbol_t	get_scanned_symbol () const;
    void	set_symbol (const symbol_t& symbol_i);
    ActualLocation	get_actual_location () const;
    ulong	get_actual_position () const;
    bool	actual_semi_tape_is_right () const;
    bool	actual_semi_tape_is_left () const;
    vector<symbol_t>	get_output_word () const;

    void	set_max_symbol_size_ ();

    bool	is_input_symbol (const symbol_t& symbol_i) const;
    bool	is_internal_symbol (const symbol_t& symbol_i) const;
    bool	is_empty_symbol (const symbol_t& symbol_i) const;
    bool	is_valid_symbol (const symbol_t& symbol_i) const;
    bool	is_valid_shift (shift_t shift_i) const;

    bool	check_alphabet () const;
    void	show_alphabet (const string& msg_i = string()) const;
    void	show_tape (const string& msg_i = string()) const;
    void	show_right_tape (const string& msg_i = string()) const;
    void	show_left_tape (const string& msg_i = string()) const;
    void	visual_cleaning ();
    void	visual_cleaning_right_semi_tape ();
    void	visual_cleaning_left_semi_tape ();

  public :
   Tape () {}
    Tape (
	const vector<symbol_t>& empty_symbols_alphabet_i, 
	const vector<symbol_t>& internal_alphabet_i,
	const vector<symbol_t>& input_alphabet_i,
	const string& msg_i = string()
	);
    ~Tape ();  

};

#endif	// _TAPE_H
