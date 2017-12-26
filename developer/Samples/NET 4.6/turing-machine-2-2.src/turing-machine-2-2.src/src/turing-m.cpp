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
//  FILE     : turing-m.cpp
//
//  DESCRIPTION :
//         Class NondeterministicTuringMachine (Implementation)
//
// ##############################################################


// ###############
#include "turing-m.h"


#define	SETW_RULE_NUMBER	4
#define	UNBELIEVABLE_NUMBER_OF_TAPES	100


// =========
size_t	NondeterministicTuringMachine::counter_s (0);
size_t	MachineStatus::next_id_to_be_processed_s (0);


// =========
// =========
// Constructor-0
MachineStatus::MachineStatus ()	: the_configuration_no_(0), the_transition_subno_ (0)
{
}
 

// =========
// Destructor
MachineStatus::~MachineStatus ()
{
} 
		      

// =========
// =========
// Constructor-0
NondeterministicTuringMachine::NondeterministicTuringMachine (
	size_t	number_of_tapes_i,
	// ---
	const vector<vector<string> >& descr_i, 
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
	const string& msg_i
	)
	:
	is_actually_nondeterministic_ (false),
	descr_ (descr_i),
	initial_states_ (initial_states_i),
	halting_states_ (halting_states_i),
	internal_states_ (internal_states_i),
	transitions_ (transitions_i),
	check_results_ (true),
	cur_input_set_no_ (0),
	transition_statistics_ (transitions_i.size()),
	max_state_size_ (0),
	max_symbol_size_ (0),
	serial_no_ (++counter_s),
	input_size_ (0),
	output_size_ (0),
	space_used_ (0),
	time_used_ (0)

{

  IF_NOT_EMPTY (msg_i, 5, '=');

typedef Tapes_t::value_type value_type;
pair<Tapes_t::iterator, bool> tapes_pair;

  if (number_of_tapes_i == 0)
  {
    check_results_ = false;
    FATAL_MSG	("Illegal number of tapes : "
		<< number_of_tapes_i
		);
    return;
  }

  if (number_of_tapes_i > UNBELIEVABLE_NUMBER_OF_TAPES)
  {
    WARNING_MSG	("Unbelievable number of tapes : "
		<< number_of_tapes_i
		);
  }

  // -----------------------------------
  for (size_t i = 0; i < number_of_tapes_i; i++)
  {
    tapes_pair = init_tapes_.insert (value_type(i, Tape (empty_symbols_alphabet_i, internal_alphabet_i, input_alphabet_i)));
    if (!tapes_pair.second)
    {
      check_results_ = false;
      FATAL_MSG	("The same current position occurs more than once in transition rules");
      break;
    }
    assert (tapes_pair.second);

    if (!tapes_pair.first->second.check_results_)
    {
      check_results_ = false;
      FATAL_MSG	("Tape#" << i << " has problems");
      break;
    }
  }

  // ------------------------------------
  set_max_state_size();
  set_max_symbol_size();


  // ----------------------
  if (check_results_)	check_results_ = check_states ();
  if (check_results_)	check_results_ = check_alphabets ();
  if (check_results_)	check_results_ = check_transition ();
  detect_if_is_actually_nondterministic ();

  // ---------------
  show_env();

} 

// =========
// Destructor
NondeterministicTuringMachine::~NondeterministicTuringMachine ()
{
} 

// =========
void NondeterministicTuringMachine::set_max_state_size()
{
vector<state_t>	tmp_all_states = get_all_states();
  assert (max_state_size_ == 0);
  for (size_t i = 0; i < tmp_all_states.size(); i++)
  {
    max_state_size_ = MAX_VALUE (max_state_size_, tmp_all_states[i].size()); 
  }

}

// =========
void NondeterministicTuringMachine::set_max_symbol_size()
{
Tapes_t::const_iterator pos_iter;
  assert (max_symbol_size_ == 0);
  for (pos_iter = init_tapes_.begin(); pos_iter != init_tapes_.end(); pos_iter++)
  {
    max_symbol_size_ = MAX_VALUE (max_symbol_size_, pos_iter->second.max_symbol_size_);
  }
}

// =========
vector<state_t> NondeterministicTuringMachine::get_all_states () const
{
vector<state_t>	ret_vector;

  copy (initial_states_.begin(), initial_states_.end(), back_inserter(ret_vector));
  copy (halting_states_.begin(), halting_states_.end(), back_inserter(ret_vector));
  copy (internal_states_.begin(), internal_states_.end(), back_inserter(ret_vector));

  return ret_vector;

}


// =========
void NondeterministicTuringMachine::set_input (
		const vector<vector<symbol_t> >& input_words_i, 
		const vector<long>&              init_pos_i
		)
{
  assert (!cur_status_.empty());
  get_cur_state() = initial_states_.front();

Tapes_t::iterator pos_iter;
  for (pos_iter = get_cur_tapes().begin(); pos_iter != get_cur_tapes().end(); pos_iter++)
  {
    pos_iter->second.clear_it();
  }
  get_cur_tapes().clear();

  get_cur_tapes() = init_tapes_;


  for (size_t i = 0; i < get_cur_tapes().size(); i++) 
  {
    get_cur_tapes()[i].clear_it();
  }

  for (size_t i = 0; i < input_words_i.size(); i++) 
  {
    get_cur_tapes()[i].set_input(input_words_i[i], init_pos_i[i]);
  }

  // --------------------------
  assert (input_size_ == 0);
  for (size_t i = 0; i < input_words_i.size(); i++) 
  {
    if (
         (input_words_i[i].size() == 1) 
         && 
         (get_cur_tapes()[i].is_empty_symbol(input_words_i[i].back()))
       ) 
    {
     continue;
    }

    input_size_ += input_words_i[i].size();
  }

}



// =========
ulong NondeterministicTuringMachine::get_output_size ()
{
ulong ret_value = 0;

  for (size_t i = 0; i < get_cur_tapes().size(); i++) 
  {
    ret_value += get_cur_tapes()[i].get_output_word ().size();
  }

  return ret_value;
}



// =========
ulong NondeterministicTuringMachine::get_space_used ()
{
ulong ret_value = 0;

  for (size_t i = 0; i < get_cur_tapes().size(); i++) 
  {
    ret_value += get_cur_tapes()[i].cell_visits_.size();
  }

  return ret_value;
}


// =========
void NondeterministicTuringMachine::clear_it ()
{
  cur_status_.clear();
  MachineStatus::next_id_to_be_processed_s = 0;
  cur_status_.push_back (MachineStatus());
  assert (!cur_status_.empty());

  cur_input_set_no_++;

  input_size_   = 0;
  output_size_  = 0;
  space_used_   = 0;
  time_used_    = 0;

  for (size_t i = 0; i < transition_statistics_.size(); i++)
  {
    transition_statistics_[i] = 0;
  } 

}


// =========
bool	NondeterministicTuringMachine::check_input_words (
		vector<vector<symbol_t> >& input_words_io,
		vector<long>& init_pos_o
		) const
{
Tapes_t::const_iterator	pos_iter;

  if (input_words_io.empty())
  {
    FATAL_MSG	("No input words");
    return false;
  }

  init_pos_o.clear();
  init_pos_o.resize(input_words_io.size(), HEAD_START_POSITION_DEFAULT);
  assert (init_pos_o.size() == input_words_io.size());

  if (input_words_io.size() != init_tapes_.size())
  {
    FATAL_MSG	("Number of input words ("
		<< input_words_io.size()
		<< ") is NOT equal number of tapes ("
		<< init_tapes_.size()
		<< ")"
		);
    return false;  
  }

size_t i;
  for (i = 0, pos_iter = init_tapes_.begin(); i < input_words_io.size(); i++, pos_iter++)
  {
    assert (pos_iter->first == i);
    if (input_words_io[i].empty())
    {
      FATAL_MSG	("No input word for tape#"
		<< i
		);
      return false;
    }

    vector<symbol_t>::iterator find_iter;
    // ---------------------------------
    const vector<symbol_t> original_input_word (input_words_io[i]);
    find_iter = find (input_words_io[i].begin(), input_words_io[i].end(), HEAD_START_POSITION_POINTER_REDERVED_SYMBOL); 
    if (find_iter != input_words_io[i].end())
    {
      init_pos_o [i] = long (distance (input_words_io[i].begin(), find_iter));
      input_words_io[i].erase(find_iter);
    }

    find_iter = find (input_words_io[i].begin(), input_words_io[i].end(), HEAD_START_POSITION_POINTER_REDERVED_SYMBOL); 
    if (find_iter != input_words_io[i].end())
    {
      ostringstream oss;
      copy (original_input_word.begin(), original_input_word.end(), ostream_iterator<symbol_t> (oss, " "));
      FATAL_MSG	("Tape#"
		<< i
		<< ", input word <"
		<< oss.str()
		<< ">" 
		<< " contains more than one reserved symbol "
		<< "\""
		<< HEAD_START_POSITION_POINTER_REDERVED_SYMBOL
		<< "\""
		<< "; Note! That symbol is used to point at initial position of the machine head"
		);

      return false;
    }


    // ---------------------------------

    for (size_t j = 0; j < input_words_io[i].size(); j++)
    {
      assert (!(input_words_io[i][j] == HEAD_START_POSITION_POINTER_REDERVED_SYMBOL));
      const bool is_input_symbol = pos_iter->second.is_input_symbol (input_words_io[i][j]);
      const bool is_empty_symbol = pos_iter->second.is_empty_symbol (input_words_io[i][j]);
      // old --- if (!((j == 0) && (input_words_io[i].size() == 1) && (pos_iter->second.is_empty_symbol (input_words_io[i][j]))))

      if (!(is_input_symbol || is_empty_symbol))
      {
        FATAL_MSG	("Tape#"
			<< i
			<< ", symbol#"
			<< j
			<< " : "
			<< "\""
			<< input_words_io[i][j]
			<< "\""
			<< " is NOT an input symbol"
			);
        return false;
      }
    } // for (size_t j = 0; ...
  } // for (size_t i = 0; ...

  return true;
}


// =========
void NondeterministicTuringMachine::show_input (
	const vector<vector<symbol_t> >& input_words_i,
	const vector<long>&              init_pos_i,
	bool fool_flag_i
	) const
{
  cout << endl;
  cout << "\t"
       << string (5, '#')
       << " ";
  if (fool_flag_i)
  {
    cout << ""
         << getstr_id_info ();
  }

  cout << "Input word(s) & head start position(s) on tape(s)"
       << " " 
       << string (5, '#')
       << endl;

  assert (input_words_i.size() == init_pos_i.size());

const size_t number_of_tapes = input_words_i.size();
  for (size_t i = 0; i < number_of_tapes; i++)
  {
    cout << "Tape#" << i << " : Word = ";
    for (size_t j = 0; j < input_words_i[i].size(); j++)
    {
      cout << setw (max_symbol_size_) << to_string (input_words_i[i][j]).c_str() << " ";
    }
    cout << ";  Head pos = " << init_pos_i [i];
    //copy (input_words_i[i].begin(), input_words_i[i].end(), ostream_iterator<symbol_t> (cout, " "));
    cout << endl;


  }

  cout << endl;

}




// =========
size_t& NondeterministicTuringMachine::get_cur_configuration()
{
  assert (!cur_status_.empty());
  return cur_status_.back().the_configuration_no_;
}



// =========
size_t NondeterministicTuringMachine::get_processed_configuration() const
{
  assert (!cur_status_.empty());

size_t id;
const bool rc = get_next_id_to_be_processed (id);
  assert (rc);
  assert (id < cur_status_.size());

  return cur_status_[id].the_configuration_no_;
}



// =========
Tapes_t& NondeterministicTuringMachine::get_cur_tapes()
{
  assert (!cur_status_.empty());
  return cur_status_.back().the_tapes_;
}


// =========
Tapes_t NondeterministicTuringMachine::get_processed_tapes() const
{
  assert (!cur_status_.empty());

size_t id;
const bool rc = get_next_id_to_be_processed (id);
  assert (rc);
  assert (id < cur_status_.size());

  return cur_status_[id].the_tapes_;
}


// =========
state_t& NondeterministicTuringMachine::get_cur_state()
{
  assert (!cur_status_.empty());
  return cur_status_.back().the_state_;
}



// =========
state_t NondeterministicTuringMachine::get_processed_state() const
{
  assert (!cur_status_.empty());

size_t id;
const bool rc = get_next_id_to_be_processed (id);
  assert (rc);
  assert (id < cur_status_.size());

  return cur_status_[id].the_state_;
}



// =========
Transitions_t::const_iterator& NondeterministicTuringMachine::get_cur_transition()
{
  assert (!cur_status_.empty());
  return cur_status_.back().the_transition_;
}



// =========
Transitions_t::const_iterator NondeterministicTuringMachine::get_processed_transition() const
{
  assert (!cur_status_.empty());

size_t id;
const bool rc = get_next_id_to_be_processed (id);
  assert (rc);
  assert (id < cur_status_.size());

  return cur_status_[id].the_transition_;
}



// =========
int& NondeterministicTuringMachine::get_cur_transition_subno()
{
  assert (!cur_status_.empty());
  return cur_status_.back().the_transition_subno_;
}



// =========
int NondeterministicTuringMachine::get_processed_transition_subno() const
{
  assert (!cur_status_.empty());

size_t id;
const bool rc = get_next_id_to_be_processed (id);
  assert (rc);
  assert (id < cur_status_.size());

  return cur_status_[id].the_transition_subno_;
}



// =========
vector<int> NondeterministicTuringMachine::get_cur_branch() const
{
  assert (!cur_status_.empty());
  return cur_status_.back().the_branch_;
}



// =========
vector<int> NondeterministicTuringMachine::get_processed_branch() const
{
  assert (!cur_status_.empty());

size_t id;
const bool rc = get_next_id_to_be_processed (id);
  assert (rc);
  assert (id < cur_status_.size());

  return cur_status_[id].the_branch_;
}



// =========
string NondeterministicTuringMachine::getstr_cur_branch() const
{
  assert (!cur_status_.empty());
vector<int> branch (get_cur_branch());
ostringstream oss;
  copy (branch.begin(), branch.end(), ostream_iterator<int> (oss, "-"));

const string str (oss.str());
  return str.substr (0, str.size() - 1); 
}



// =========
string NondeterministicTuringMachine::getstr_processed_branch() const
{
  assert (!cur_status_.empty());
vector<int> branch (get_processed_branch());
ostringstream oss;
  copy (branch.begin(), branch.end(), ostream_iterator<int> (oss, "-"));

const string str (oss.str());
  return str.substr (0, str.size() - 1); 
}




// =========
bool NondeterministicTuringMachine::get_next_id_to_be_processed (size_t& id_o) const
{
  assert (!cur_status_.empty());
  id_o = MachineStatus::next_id_to_be_processed_s;
  return (MachineStatus::next_id_to_be_processed_s < cur_status_.size());
}


// =========
// =========
bool NondeterministicTuringMachine::perform_step (
		const state_t& state_i, 
		const vector<symbol_t>& scanned_symbols_i, 
		Transitions_t::const_iterator& find_iter_o,
		const string& msg_i
		)
{
  IF_NOT_EMPTY (msg_i, 5, '=');

  assert (state_i != symbol_t());

find_iter_o = transitions_.find (CurSituation(state_i, scanned_symbols_i));

  if (find_iter_o == transitions_.end())	
  {
    assert (state_i != symbol_t());
    return false;
  }

size_t dist = distance (transitions_.begin(), find_iter_o);
  assert (dist < transitions_.size());
  transition_statistics_[dist]++;

  // -----------------------
  // -----------------------
const Tapes_t last_tapes (get_processed_tapes());
const vector<int> last_branch (get_processed_branch());

const size_t next_configuration_no = get_cur_configuration () + 1;

  for (size_t id = 0; id < find_iter_o->second.size(); id++)
  {
    const int subno = id + 1;
    cur_status_.push_back (MachineStatus());

    cur_status_.back().the_branch_ = last_branch;
    cur_status_.back().the_branch_.push_back(subno);
    time_used_ = MAX_VALUE (time_used_, cur_status_.back().the_branch_.size());

    get_cur_configuration() = next_configuration_no;

    Tapes_t::iterator pos_iter;
    for (pos_iter = get_cur_tapes().begin(); pos_iter != get_cur_tapes().end(); pos_iter++)
    {
      pos_iter->second.clear_it();
    }
    get_cur_tapes().clear();

    get_cur_tapes() = last_tapes;
    // -----------------------

    get_cur_transition() = find_iter_o;
    get_cur_transition_subno() = subno;
    get_cur_state() = find_iter_o->second[id].get_state ();
    for (size_t i = 0; i < get_cur_tapes().size(); i++)
    {
      get_cur_tapes()[i].set_symbol (find_iter_o->second[id].get_symbol (i));
      get_cur_tapes()[i].shift_position (find_iter_o->second[id].get_shift(i));
    }
 
    assert (get_cur_state() != symbol_t());
  }

  MachineStatus::next_id_to_be_processed_s++;

  return true;

}

// =========
bool NondeterministicTuringMachine::process_input (
		const vector<vector<symbol_t> >& input_words_i,
		const string& msg_i
		)
{
  IF_NOT_EMPTY (msg_i, 5, '=');

  if (!check_results_)
  {
    FATAL_MSG	("Invalid Nondeterministic Turing Machine definition");
    return false;
  }

vector<vector<symbol_t> > input_words (input_words_i);
vector<long> init_pos;

  cout << endl;
  if (!check_input_words(input_words, init_pos))
  {
    show_input (input_words, init_pos, false);
    FATAL_MSG	("Invalid input words");
    cur_input_set_no_++;
    return false;
  }
  clear_it ();
  assert (input_words.size() == init_pos.size());
  show_input (input_words, init_pos);
  set_input (input_words, init_pos);


  // ------------------
state_t	 state;
vector<symbol_t>	scanned_symbols;

bool	ret_bool_value;

  cout << endl;
  cout << endl;
  cout << "\t"
       << string (5, '#') 
       << " "
       << getstr_id_info ()
       << "Processing"
       << " "
       << string (5, '#') 
       << endl;

Transitions_t::const_iterator find_iter = transitions_.end();

size_t next_id;

  for (int step_no = 0; ; step_no++)
  {

    const int actual_step_no = step_no;
    if (!get_next_id_to_be_processed (next_id)) break;
    assert (next_id < cur_status_.size());

    scanned_symbols.clear();
    for (size_t i = 0; i < cur_status_[next_id].the_tapes_.size(); i++)
    {
      scanned_symbols.push_back(cur_status_[next_id].the_tapes_[i].get_scanned_symbol ());
    }

    // -------------------------------------
    show2_situation (getstr_id_info (), actual_step_no);
    ret_bool_value = perform_step (cur_status_[next_id].the_state_, scanned_symbols, find_iter);  

    state = get_processed_state();

    // -------------------
    show1_situation (find_iter);
    assert (((find_iter != transitions_.end()) && (ret_bool_value)) || ((find_iter == transitions_.end()) && (!ret_bool_value)));
    if (!ret_bool_value)
    { 
      cout << "\t" << getstr_id_info () << "Failure" << (is_actually_nondeterministic_ ? " of the Branch" : "") << endl;
      cout << "\t" << "  1) There exists no appropriate rule for" << endl;
      cout << "\t" << "     " << state << " [ ";
      copy (scanned_symbols.begin(), scanned_symbols.end(), ostream_iterator<string> (cout, " "));
      cout << "]" << endl;
      cout << "\t" << "  2) Current state (" << state << ") is not halting one" << endl;

      cout << endl;
      cout << endl;

      MachineStatus::next_id_to_be_processed_s++;
      continue;
    }

    assert (ret_bool_value);
    // -------------------
    // show2_situation (getstr_id_info (), "Configuration#" + to_string (actual_step_no));

    // -------------------
    if (is_halting_state (state))
    {
      show2_situation (getstr_id_info (), actual_step_no + 1);

      cout << "\t" << getstr_id_info () << "Success : Current state (" << state << ") is halting one" << endl;
      cout << endl;
      cout << endl;

      MachineStatus::next_id_to_be_processed_s++;
      break;
    }

  } // for (step_no = 

const size_t the2_setw = 3;

ostringstream oss;
  oss << "" 
       << string (the2_setw, '-')
       << " "
       << getstr_id_info (true)
       << " Result : Input word(s) " 
       << (ret_bool_value ? "ACCEPTED" : "REJECTED") 
       << " " 
       << string (the2_setw, '-'); 
  
const size_t the1_setw = oss.str().size();

const string prefix (3, ' ');

  cout << endl;
  cout << prefix << string (the1_setw, '-') << endl;
  cout << prefix 
       << oss.str()
       << endl;
  cout << prefix << string (the1_setw, '-') << endl;
  cout <<  endl;

  // -----------------------------
  assert (output_size_ == 0);
  output_size_ = get_output_size();

  assert (space_used_ == 0);
  space_used_ = get_space_used();

  show_resources_report ();

  // -----------------------------
  show_statistics ();

  return ret_bool_value;
}


// =========
void NondeterministicTuringMachine::show_env () const
{
  cout << endl;
  cout << "\t###### Nondeterministic Turing Machine Definition ######" << endl;
  if (is_actually_nondeterministic_)
  {
    cout << "\t###### This Machine is actually Nondeterministic  ######" << endl;
  }
  else
  {
    cout << "\t###### This Machine is actually Deterministic!!!  ######" << endl;
  }

  cout << endl;
  show_descr ();

  cout << endl;
  cout << endl;
  show_states ();

  cout << endl;
  cout << endl;
  show_alphabets();

  cout << endl;
  cout << endl;
  show_transition ();

}


// =========
void NondeterministicTuringMachine::show_descr (const string& msg_i) const
{
  cout << "\t   ====== Description ======" << endl;

  IF_NOT_EMPTY (msg_i, 5, '=');

  if (descr_.empty())
  {
    cout << "No description" << endl;
  }

  for (size_t i = 0; i < descr_.size(); i++)
  {
    copy (descr_[i].begin(), descr_[i].end(), ostream_iterator<string> (cout, " "));
    cout << endl;
  }

}


// =========
void NondeterministicTuringMachine::show_states (const string& msg_i) const
{
  cout << "\t   ====== States Definition ======" << endl;

  IF_NOT_EMPTY (msg_i, 5, '=');


string text_initial_states ("Initial states");
string text_halting_states ("Halting states");
string text_internal_states ("Internal states");
size_t text_max_size = 0;

  text_max_size	= MAX_VALUE(text_max_size, text_initial_states.size());
  text_max_size	= MAX_VALUE(text_max_size, text_halting_states.size());
  text_max_size	= MAX_VALUE(text_max_size, text_internal_states.size());


  cout << setw(text_max_size) << left << text_initial_states.c_str() << " : ";
  for (size_t i = 0; i < initial_states_.size(); i++)
  {
    cout << setw (max_state_size_) << initial_states_[i].c_str() << " ";
  }
  cout << endl;

  cout << setw(text_max_size) << left << text_halting_states.c_str() << " : ";
  for (size_t i = 0; i < halting_states_.size(); i++)
  {
    cout << setw (max_state_size_) << halting_states_[i].c_str() << " ";
  }
  cout << endl;

  cout << setw(text_max_size) << left << text_internal_states.c_str() << " : ";
  for (size_t i = 0; i < internal_states_.size(); i++)
  {
    cout << setw (max_state_size_) << internal_states_[i].c_str() << " ";
  }
  cout << endl;


}

// =========
void NondeterministicTuringMachine::show_alphabet (const Tapes_t::const_iterator& iter_i) const
{
  iter_i->second.show_alphabet();
}

// =========
void NondeterministicTuringMachine::show_alphabets () const
{
Tapes_t::const_iterator pos_iter;
  cout << "\t   ====== Alphabet Definition ======" << endl;
  for (pos_iter = init_tapes_.begin(); pos_iter != init_tapes_.end(); pos_iter++)
  {
    cout << "\t      ------ Tape# " << distance (init_tapes_.begin(), pos_iter) << " ------" << endl;
    show_alphabet (pos_iter);
    cout << endl;
  }
}

// =========
void NondeterministicTuringMachine::show_transition () const
{
Transitions_t::const_iterator pos_iter;
  cout << "\t   ====== Transition Rules Definition ======" << endl;
  for (pos_iter = transitions_.begin(); pos_iter != transitions_.end(); pos_iter++)
  {
    cout << getstr_rule_S (
		is_actually_nondeterministic_, 
		transitions_.begin(), 
		pos_iter, 
		max_state_size_, 
		max_symbol_size_
		);
  }
  cout << endl;
}

// =========
string NondeterministicTuringMachine::getstr_rule_S (
		bool is_actually_nondeterministic_i,
		Transitions_t::const_iterator iter1_i,
		Transitions_t::const_iterator iter2_i,
		size_t	max_state_size_i,
		size_t	max_symbol_size_i,
		int	subno_i
		)
{
  assert (subno_i != 0);
  assert ((subno_i > 0) || (subno_i == -1));

ostringstream oss;
  for (size_t k = 0; k < iter2_i->second.size(); k++)
  {
    if ((subno_i == -1)	|| (size_t(subno_i) == (k + 1)))
    {
      oss << ""
          << "Rule#"
          << setw (SETW_RULE_NUMBER)
          << right
          << distance (iter1_i, iter2_i);
      if (is_actually_nondeterministic_i)
      {
        oss << "-"
            << (k + 1);
      }
      oss << " :   "
          << setw (max_state_size_i)
          << iter2_i->first.get_state().c_str()
          << " [ " 
          << iter2_i->first.getstr_symbols(max_symbol_size_i)
          << "] ---> "
          << setw (max_state_size_i)
          << iter2_i->second[k].get_state().c_str()
          << " [ " 
          << iter2_i->second[k].getstr_symbols_and_shifts(max_symbol_size_i)
          << "]"
          << endl;
    }
  }

  return oss.str();
}
   
// =========
void NondeterministicTuringMachine::show_tape (const Tapes_t::const_iterator& iter_i) const
{
  iter_i->second.show_tape();
}


// =========
void NondeterministicTuringMachine::show1_situation (
	const Transitions_t::const_iterator& find_iter_i,
	bool init_configuration_i
	) const
{
  assert (!cur_status_.empty());
  assert (!init_configuration_i);

  if (find_iter_i != transitions_.end())
  {
    for (size_t k = 0; k < find_iter_i->second.size(); k++)
    {
      cout << "\t" 
           << getstr_id_info ()
           << "Applied " 
           << getstr_rule_S (
		is_actually_nondeterministic_, 
		transitions_.begin(), 
		find_iter_i, 
		max_state_size_, 
		max_symbol_size_, 
		k + 1
		); 
    }
  }
  else
  {
      cout << "\t" 
           << getstr_id_info ()
           << "No Applied Rule"
           << endl;
  }
  cout << endl;


}



// =========
void NondeterministicTuringMachine::show2_situation (
	const string& msg_id_info_i, 
	size_t        configuration_no_i
	) const
{
Tapes_t::const_iterator pos_iter;
const size_t size1 = 5;

  assert (!cur_status_.empty());

  if (configuration_no_i)
  {
    cout << endl;
    if (is_actually_nondeterministic_)
    {
      cout << "\t" << msg_id_info_i << "Branch : " << getstr_processed_branch() << endl;
    }
  }
const string configuration_info (configuration_no_i ? ("Configuration#" + to_string(configuration_no_i)) : "Initial Configuration");
  IF_NOT_EMPTY ((msg_id_info_i + configuration_info), size1, '-');


  cout << "\tState  : " << get_processed_state() << endl;
  //cout << "\t" << string (size2, '-') << endl;

const Tapes_t the_tapes (get_processed_tapes());
  for (pos_iter = the_tapes.begin(); 
       pos_iter != the_tapes.end(); 
       pos_iter++
      )
  {
    cout << "Tape#" << distance (the_tapes.begin(), pos_iter) << " : ";
    show_tape (pos_iter);
  }
  //cout << endl;

}


// =========
void NondeterministicTuringMachine::show_resources_report (const string& msg_i) const
{
const size_t size1 = 5;

  IF_NOT_EMPTY (msg_i, size1, '-');

ostringstream oss, oss2;
const string prefix (3, ' ');
const string li_sign (" * ");
const size_t info_setw = 10;

  oss2 << getstr_id_info (true) << " Resource Report";

  oss << "" 
      << prefix
      << oss2.str()
      << endl

      << prefix
      << string (oss2.str().size(), '-')
      << endl

      << prefix
      << getstr_id_info (true)
      << li_sign
      << "Input size    : "
      << setw (info_setw)
      << std::right
      << input_size_
      << " symbols"
      << endl

      << prefix
      << getstr_id_info (true)
      << li_sign
      << "Output size   : "
      << setw (info_setw)
      << std::right
      << output_size_
      << " symbols"
      << endl

      << prefix
      << getstr_id_info (true)
      << li_sign
      << "TM-Space used : "
      << setw (info_setw)
      << std::right
      << space_used_
      << " cells"
      << endl

      << prefix
      << getstr_id_info (true)
      << li_sign
      << "TM-Time used  : "
      << setw (info_setw)
      << std::right
      << time_used_;
  if (is_actually_nondeterministic_)
  {
    oss << " (smart)";
  }
  oss << " transitions"
      << endl;


  cout << oss.str() << endl << endl << endl;

}


// =========
void NondeterministicTuringMachine::show_statistics (const string& msg_i) const
{
const size_t size1 = 5;

  IF_NOT_EMPTY (msg_i, size1, '-');

uint total = accumulate (transition_statistics_.begin(), transition_statistics_.end(), 0);

const string text_statistics ("Statistics");
const string text_transition ("Transition");
const string text_rules ("Rules");
const string text_times ("Times");
const string text_total ("Total");
const string delim (":");

const size_t max_size1 = MAX_VALUE (text_statistics.size(), text_transition.size());
const size_t the_size2 = 3;
const size_t the_size1 = the_size2 + 1 + max_size1 + 1 + the_size2;
const size_t the_size3 = (the_size1 - 1)/2;
const size_t the_size4 = (the_size1 - 1) - the_size3;
  assert (text_rules.size() < (the_size3 - 1));
  assert (text_times.size() < (the_size4 - 1));

const string l_border ("\t      |");		
const string r_border ("|");		

  cout << l_border << string (the_size1, '=') << r_border << endl;

  cout << l_border 
       << string (the_size2, '-') 
       << " "
       << setw (max_size1)
       << left
       << text_statistics.c_str()
       << " "
       << string (the_size2, '-') 
       << r_border 
       << endl;

  cout << l_border 
       << string (the_size2, '.') 
       << " "
       << setw (max_size1)
       << left
       << text_transition.c_str()
       << " "
       << string (the_size2, '.') 
       << r_border 
       << endl;

  cout << l_border << string (the_size1, '-') << r_border << endl;

  cout << l_border 
       << setw (the_size3 - 1)
       << right
       << text_rules.c_str()
       << " "
       << delim
       << setw (the_size4 - 1)
       << right
       << text_times.c_str()
       << " "
       << r_border 
       << endl;

  cout << l_border << string (the_size1, '-') << r_border << endl;

  for (size_t i = 0; i < transition_statistics_.size(); i++)
  {
    cout << l_border 
         << setw (the_size3 - 1)
         << right
         << i
         << " "
         << delim
         << setw (the_size4 - 1)
         << right
         << transition_statistics_[i]
         << " "
         << r_border 
         << endl;
  }
  cout << l_border << string (the_size1, '-') << r_border << endl;

  cout << l_border 
       << setw (the_size3 - 1)
       << right
       << text_total.c_str()
       << " "
       << delim
       << setw (the_size4 - 1)
       << right
       << total
       << " "
       << r_border 
       << endl;

  cout << l_border << string (the_size1, '=') << r_border << endl;

}


// =========
bool NondeterministicTuringMachine::is_initial_state (const state_t& state_i) const
{
  return (find (initial_states_.begin(), initial_states_.end(), state_i) != initial_states_.end());
}

// =========
bool NondeterministicTuringMachine::is_halting_state (const state_t& state_i) const
{
  return (find (halting_states_.begin(), halting_states_.end(), state_i) != halting_states_.end());
}


// =========
bool NondeterministicTuringMachine::is_internal_state (const state_t& state_i) const
{
  return (find (internal_states_.begin(), internal_states_.end(), state_i) != internal_states_.end());
}

// =========
bool NondeterministicTuringMachine::is_valid_state (const state_t& state_i) const
{
  return (is_initial_state(state_i) || is_halting_state (state_i) || is_internal_state (state_i));
}



// =========
bool NondeterministicTuringMachine::check_states () const
{
bool	ret_bool_value = true;

state_t	phisical_empty_state = string();
vector<state_t>::const_iterator	find_iter;

  assert (!initial_states_.empty());
  assert (!halting_states_.empty());
  assert (!internal_states_.empty());

  // ---------
  find_iter = find (initial_states_.begin(), initial_states_.end(), phisical_empty_state); 
  if (find_iter != initial_states_.end())
  {
    ret_bool_value = false;
    FATAL_MSG	("Initial state#"
		<< distance (initial_states_.begin(), find_iter)
		<< " is empty : <"
		<< *find_iter
		<< ">"
		);
  }

  find_iter = find (halting_states_.begin(), halting_states_.end(), phisical_empty_state); 
  if (find_iter != halting_states_.end())
  {
    ret_bool_value = false;
    FATAL_MSG	("Halting state#"
		<< distance (halting_states_.begin(), find_iter)
		<< " is empty : <"
		<< *find_iter
		<< ">"
		);
  }


  find_iter = find (internal_states_.begin(), internal_states_.end(), phisical_empty_state); 
  if (find_iter != internal_states_.end())
  {
    ret_bool_value = false;
    FATAL_MSG	("Internal state#"
		<< distance (internal_states_.begin(), find_iter)
		<< " is empty : <"
		<< *find_iter
		<< ">"
		);
  }

  // ------
vector<state_t> tmp_all_states = get_all_states();
vector<state_t>::iterator	pos_iter;
  for (pos_iter = tmp_all_states.begin(); pos_iter != tmp_all_states.end(); pos_iter++)
  {
    assert (count (pos_iter, tmp_all_states.end(), *pos_iter));
    if (count (pos_iter, tmp_all_states.end(), *pos_iter) > 1)
    {
      ret_bool_value = false;
      FATAL_MSG	("State "
		<< "<"
		<< (*pos_iter)
	 	<< ">"
		<< " occurs more than once"
		);
    }
  }

  // -----------
  return ret_bool_value;
} // check_states


// =========
bool NondeterministicTuringMachine::check_alphabets () const
{
bool	ret_bool_value = true;

Tapes_t::const_iterator	iter;
  for (iter = init_tapes_.begin(); iter != init_tapes_.end(); iter++)
  {
    assert (iter->first == static_cast<size_t> (distance (init_tapes_.begin(), iter)));
    if (!(iter->second.check_alphabet()))	ret_bool_value = false;	
  }

  return ret_bool_value;
}


// =========
bool NondeterministicTuringMachine::check_transition () const
{
bool	ret_bool_value = true;

Transitions_t::const_iterator	pos1_iter;
Tapes_t::const_iterator		pos2_iter;
size_t		i;
state_t		cur_state;
size_t		cur_total_symbols;
symbol_t	cur_symbol;

state_t		next_state;
size_t		next_total_symbols;
symbol_t	next_symbol;
shift_t	shift;

  if (transitions_.empty())
  {
    ret_bool_value = false;
    FATAL_MSG	("No transition function");
  }

  for (pos1_iter = transitions_.begin(); pos1_iter != transitions_.end(); pos1_iter++)
  {
    const string transitions_line_info ("Transition Line#" + to_string (distance (transitions_.begin(), pos1_iter)));
    const string transitions_line_and_tape_no_info (transitions_line_info + ", tape#");

    // --- first ---
    cur_state = pos1_iter->first.get_state();
    cur_total_symbols = pos1_iter->first.get_total_symbols();

    if (!((is_initial_state (cur_state)) || (is_internal_state (cur_state))))
    {
      ret_bool_value = false;
      FATAL_MSG	(transitions_line_info
		<< " : illegal cur-state = <"
		<< cur_state
		<< ">"
		);
    }

    if (cur_total_symbols != init_tapes_.size())
    {
      ret_bool_value = false;
      FATAL_MSG	(transitions_line_info
		<< " : number-of-cur-symbols = "
		<< cur_total_symbols
		<< " is not equal number-of-tapes = "
		<< init_tapes_.size()
		);
    }


    for (i = 0, pos2_iter = init_tapes_.begin(); i < cur_total_symbols; i++, pos2_iter++)
    {
      assert (pos2_iter->first == i);
      cur_symbol = pos1_iter->first.get_symbol(i);

      if (!(pos2_iter->second.is_valid_symbol(cur_symbol)))
      {
        ret_bool_value = false;
        FATAL_MSG	(transitions_line_and_tape_no_info
			<< i
			<< " : illegal cur-symbol = <"
			<< cur_symbol
			<< ">"
			);
      }
    }

    
    // --- second ---
    for (size_t k = 0; k < pos1_iter->second.size(); k++)
    {

      const string transitions_line_second_info (transitions_line_info + "-" + to_string (k + 1));
      const string transitions_line_and_tape_no_second_info (transitions_line_second_info + ", tape#");

      next_state = pos1_iter->second[k].get_state();
      next_total_symbols = pos1_iter->second[k].get_total_symbols();
  
      //if (!((is_halting_state (next_state)) || (is_internal_state (next_state))))
      if (!is_valid_state(next_state))
      {
        ret_bool_value = false;
        FATAL_MSG	(transitions_line_second_info
  		<< " : illegal next-state = <"
  		<< next_state
  		<< ">"
  		);
      }
  
      if (next_total_symbols != init_tapes_.size())
      {
        ret_bool_value = false;
        FATAL_MSG	(transitions_line_second_info
  		<< " : number-of-next-symbols = "
  		<< next_total_symbols
  		<< " is not equal number-of-tapes = "
  		<< init_tapes_.size()
  		);
      }
  
      for (i = 0, pos2_iter = init_tapes_.begin(); i < next_total_symbols; i++, pos2_iter++)
      {
        assert (pos2_iter->first == i);
        next_symbol = pos1_iter->second[k].get_symbol(i);
        shift = pos1_iter->second[k].get_shift(i);
  
        if (!(pos2_iter->second.is_valid_symbol(next_symbol)))
        {
          ret_bool_value = false;
          FATAL_MSG	(transitions_line_and_tape_no_second_info
  			<< i
  			<< " : illegal next-symbol = <"
  			<< next_symbol
  			<< ">"
  			);
        }
  
        if (!(pos2_iter->second.is_valid_shift(shift)))
        {
          ret_bool_value = false;
          FATAL_MSG	(transitions_line_and_tape_no_second_info
  			<< i
  			<< " : illegal shift = <"
  			<< shift
  			<< ">"
  			);
        }
  
      }
  
    } //for (size_t k = 0
 
  } // for (iter = transition.begin(); ...
  return ret_bool_value;
}

	
// =========
void NondeterministicTuringMachine::detect_if_is_actually_nondterministic ()
{
Transitions_t::const_iterator	pos_iter;
  for (pos_iter = transitions_.begin(); pos_iter != transitions_.end(); pos_iter++)
  {
    assert (pos_iter->second.size() > 0);
    if (pos_iter->second.size() > 1)
    {
      is_actually_nondeterministic_ = true;
      break;
    }
  }

}


// =========
bool NondeterministicTuringMachine::get_check_results () const
{
  return check_results_;
}

// =========
string NondeterministicTuringMachine::getstr_id_info (bool detail_flag_i) const
{
ostringstream oss;
  oss << "< ";
  if (detail_flag_i)
  {
    oss << (is_actually_nondeterministic_ ? "N" : "D")
        << "TM #"
        << serial_no_ 
        << ", "
        << "Input #"
        << cur_input_set_no_;
  }
  else
  {
    oss << "Run "
        << serial_no_ 
        << "."
        << cur_input_set_no_;
  }
  oss << " > ";

  return oss.str();
}


