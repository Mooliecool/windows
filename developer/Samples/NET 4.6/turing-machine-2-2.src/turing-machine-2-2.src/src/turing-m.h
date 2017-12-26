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
//  FILE     : turing-m.h
//
//  DESCRIPTION :
//         Class NondeterministicTuringMachine (Definition)
//
// ##############################################################




// ###############
#ifndef _TURING_M_H
#define _TURING_M_H


#include "rules.h"
#include "tape.h"


typedef map<CurSituation, vector<NextSituation> >	Transitions_t;
typedef map<size_t, Tape>				Tapes_t;


// =========
class MachineStatus
{
friend class NondeterministicTuringMachine;
  private :
    vector<int>				the_branch_;
    size_t				the_configuration_no_;
    Transitions_t::const_iterator	the_transition_;
    int					the_transition_subno_;
    state_t				the_state_;
    Tapes_t				the_tapes_;

    static size_t			next_id_to_be_processed_s;

  public :
    MachineStatus ();
    ~MachineStatus ();
    

};

// =========
class NondeterministicTuringMachine
{
  private :
    // ------ Private Data ------
    bool			is_actually_nondeterministic_;
    const vector<vector<string>	> descr_;
    const vector<state_t>	initial_states_;
    const vector<state_t>	halting_states_;
    const vector<state_t>	internal_states_;
    const Transitions_t		transitions_;
    bool			check_results_;

    vector<MachineStatus>	cur_status_;
    size_t			cur_input_set_no_;
    Tapes_t			init_tapes_;
    vector<uint>  		transition_statistics_;

    size_t	max_state_size_;
    size_t	max_symbol_size_;

    static size_t	counter_s;
    const size_t	serial_no_;

    size_t	input_size_;
    ulong	output_size_;
    ulong	space_used_;
    ulong	time_used_;

    // ------ Private Methods ------
    void	clear_it ();
    void	set_input (
			const vector<vector<symbol_t> >& input_words_i, 
			const vector<long>&              init_pos_i
			);
    bool	perform_step (
			const state_t&                 state_i, 
			const vector<symbol_t>&        scanned_symbols_i, 
			Transitions_t::const_iterator& find_iter_o,
			const string&                  msg_i = string());

    vector<state_t>	get_all_states () const;
    string		getstr_id_info (bool detail_flag_i = false) const;

    size_t&		get_cur_configuration ();
    size_t		get_processed_configuration () const;

    Tapes_t&		get_cur_tapes ();
    Tapes_t		get_processed_tapes () const;

    state_t&		get_cur_state ();
    state_t		get_processed_state () const;


    Transitions_t::const_iterator&	get_cur_transition();
    Transitions_t::const_iterator	get_processed_transition() const;

    int&		get_cur_transition_subno();
    int			get_processed_transition_subno() const;

    vector<int>		get_cur_branch () const;
    string		getstr_cur_branch () const;

    vector<int>		get_processed_branch () const;
    string		getstr_processed_branch () const;


    bool		get_next_id_to_be_processed  (size_t& id_o) const;

    ulong		get_output_size ();
    ulong		get_space_used ();

    bool	is_initial_state (const state_t& state_i) const;
    bool	is_halting_state (const state_t& state_i) const;
    bool	is_internal_state (const state_t& state_i) const;
    bool	is_valid_state (const state_t& state_i) const;

    void 	set_max_state_size();
    void	set_max_symbol_size();

    bool	check_states () const;
    bool	check_alphabets () const;
    bool	check_transition () const;
    void	detect_if_is_actually_nondterministic ();
    bool	check_input_words (
			vector<vector<symbol_t> >& input_words_io, 
			vector<long>& init_pos_o
			) const;

    void	show_descr (const string& msg_i = string()) const;
    void	show_states (const string& msg_i = string()) const;
    void	show_alphabet (const Tapes_t::const_iterator& iter) const;
    void	show_alphabets () const;
    void	show_transition () const;
    static string	getstr_rule_S (
			bool is_actually_nondeterministic_i,
			Transitions_t::const_iterator iter1_i,
			Transitions_t::const_iterator iter2_i,
			size_t max_state_size_i,
			size_t max_symbol_size_i,
			int    subno_i = -1
			);
    void	show_tape (const Tapes_t::const_iterator& iter) const;
    void	show1_situation (
			const Transitions_t::const_iterator& find_iter_i,
			bool init_configuration_i = false
			) const;
    void	show2_situation (
			const string& msg_id_info_i, 
			size_t        configuration_no_i
			) const;

    void	show_statistics (const string& msg_i = string()) const;
    void	show_resources_report (const string& msg_i = string()) const;

    void	show_input (
			const vector<vector<symbol_t> >& input_words_i, 
			const vector<long>&              init_pos_i,
			bool full_flag_i = true
			) const;			


  public :
    NondeterministicTuringMachine (
	size_t	number_of_tapes_i,
	// ---
	const vector<vector<string> >& descr_i, 
	// ---
	const vector<state_t>& initial_states_i, 
	const vector<state_t>& halting_states_i, 
	const vector<state_t>& internal_states_i,
	// ---
	const vector<symbol_t>& empty_symbols_alphabet_i, 
	const vector<symbol_t>& internal_alphabet_i,
	const vector<symbol_t>& input_alphabet_i,
	// ---
	const Transitions_t& transitions_i,
	// ---
	const string& msg_i = string ()
	);
    ~NondeterministicTuringMachine ();  
    bool	process_input (
			const vector<vector<symbol_t> >& input_words_i,
			const string& msg_i = string()
			);
    void	show_env () const;
    bool	get_check_results () const;

};

#endif	// _TURING_M_H
