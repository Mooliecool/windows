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
//  FILE     : tape.cpp
//
//  DESCRIPTION :
//         Class Tape (Implementation)
//
// ##############################################################




// ###############
#include "tape.h"


// =========
#define	LEFT_USUAL_DELIM	" "
#define	RIGHT_USUAL_DELIM	" "
#define	LEFT_SCAN_DELIM		"["
#define	RIGHT_SCAN_DELIM	"]"

// #define	SEMI_TAPES_DELIM	"--- "
#define	SEMI_TAPES_DELIM	""


// =========
// Constructor-0
Tape::Tape (
	const vector<symbol_t>& empty_symbols_alphabet_i, 
	const vector<symbol_t>& internal_alphabet_i,
	const vector<symbol_t>& input_alphabet_i,
	const string&           msg_i
	) 
	:
	empty_symbols_alphabet_ (empty_symbols_alphabet_i),
	internal_alphabet_ (internal_alphabet_i),
	input_alphabet_ (input_alphabet_i),
	logical_position_ (HEAD_START_POSITION_DEFAULT),
	max_symbol_size_ (0)
{
  IF_NOT_EMPTY (msg_i, 3, '=');

  check_results_ = check_alphabet ();
  set_max_symbol_size_ ();
  //show_alphabet ("Tape constructed");

} 

// =========
// Destructor
Tape::~Tape ()
{
}
 
// =========
void Tape::clear_it ()
{
  logical_position_ = HEAD_START_POSITION_DEFAULT;

  right_semi_tape_.clear();
  right_semi_tape_.resize ((logical_position_ >= 0) ? logical_position_ : 0);

  left_semi_tape_.clear();
  left_semi_tape_.resize ((logical_position_ < 0) ? labs(logical_position_) : 0);

  cell_visits_.clear();

} 



// =========
void Tape::set_input (const vector<symbol_t>& input_words_i, long init_pos_i)
{
  assert (init_pos_i >= 0);

  logical_position_ = init_pos_i;

  assert (logical_position_ >= 0); // actual_semi_tape is right
  assert ((ulong(logical_position_) + input_words_i.size()) < LONG_MAX);

  assert (ulong(logical_position_) < input_words_i.size());

  copy (input_words_i.begin(), input_words_i.end(), back_inserter(right_semi_tape_));

  assert (get_actual_position () < right_semi_tape_.size());

  /*
  ################## old ##################
  if (actual_semi_tape_is_right ())
  {
    right_semi_tape_.resize(get_actual_position ());

    for (ulong k = 0; k < right_semi_tape_.size(); k++)
    {
      right_semi_tape_[k] = empty_symbols_alphabet_.front();
    }
    copy (input_words_i.begin(), input_words_i.end(), back_inserter(right_semi_tape_));

    assert (get_actual_position () < right_semi_tape_.size());

  }
  else
  {
    assert (actual_semi_tape_is_left ());
    assert (logical_position_ < 0);

    for (ulong k = 0; k < left_semi_tape_.size(); k++)
    {
      left_semi_tape_[k] = empty_symbols_alphabet_.front();
    }

    const size_t split_size = MIN_VALUE (left_semi_tape_.size(), input_words_i.size());
    vector<symbol_t> word_head;
    vector<symbol_t> word_tail;
    for (size_t k = 0; k < input_words_i.size(); k++)
    {
      if (k < split_size) word_head.push_back(input_words_i[k]);
      else                word_tail.push_back(input_words_i[k]);
    }
    assert ((word_head.size() + word_tail.size()) == input_words_i.size());
    // -------------------------------
    for (size_t k = 0; k < word_head.size(); k++)
    {
      left_semi_tape_[left_semi_tape_.size() - 1 - k] = word_head[k];
    }

    copy (word_tail.begin(), word_tail.end(), back_inserter(right_semi_tape_));

    assert (get_actual_position () < left_semi_tape_.size());

  }

  #########################################
  */


  // -------------------------

  assert (cell_visits_.empty());
  for (size_t k = 0; k < input_words_i.size(); k++)
  {
    assert (cell_visits_.count (logical_position_ + k) == 0);
    cell_visits_[logical_position_ + k]	= 0;
  }
  assert (cell_visits_.count (logical_position_) == 1);
  assert (cell_visits_[logical_position_] == 0);
  cell_visits_[logical_position_]++;

}



// =========
vector<symbol_t> Tape::get_output_word () const
{
vector<symbol_t> ret_vect;
  copy (left_semi_tape_.rbegin(), left_semi_tape_.rend(), back_inserter(ret_vect));
  copy (right_semi_tape_.begin(), right_semi_tape_.end(), back_inserter(ret_vect));

  assert (!ret_vect.empty());

  while (!ret_vect.empty() && is_empty_symbol(ret_vect.front()))
  {
    ret_vect.erase (ret_vect.begin());  
  }

  while (!ret_vect.empty() && is_empty_symbol(ret_vect.back()))
  {
    ret_vect.erase (ret_vect.end() - 1);  
  }

  return ret_vect;
}


// =========
void Tape::right_shift ()
{
 assert (get_actual_position () < LONG_MAX);

  logical_position_++;
  if (actual_semi_tape_is_left ()) 
  {
    visual_cleaning ();
    return;
  }

  // ---------
  assert (actual_semi_tape_is_right ());
  assert (get_actual_position () <= right_semi_tape_.size());

  if (get_actual_position () == right_semi_tape_.size()) right_semi_tape_.push_back (empty_symbols_alphabet_.front());
  assert (get_actual_position () < right_semi_tape_.size());

}


// =========
void Tape::left_shift ()
{
  assert (get_actual_position () < LONG_MAX);

  logical_position_--;
  if (actual_semi_tape_is_right ()) 
  {
    visual_cleaning ();
    return;
  }

  // ---------
  assert (actual_semi_tape_is_left ());
  assert (get_actual_position () <= left_semi_tape_.size());

  if (get_actual_position () == left_semi_tape_.size()) left_semi_tape_.push_back (empty_symbols_alphabet_.front());
  assert (get_actual_position () < left_semi_tape_.size());

}


// =========
void Tape::shift_position (shift_t shift_i)
{
  assert (is_valid_shift (shift_i));

  if (shift_i == LEFT_SHIFT)  left_shift();
  if (shift_i == RIGHT_SHIFT) right_shift();

  // if (shift_i == NO_SHIFT) : Do nothing

  // --------------------------------
  if (cell_visits_.count (logical_position_) == 0) cell_visits_[logical_position_] = 0;
  cell_visits_[logical_position_]++;

}


// =========
symbol_t Tape::get_scanned_symbol () const
{

symbol_t ret_symbol;
  if (actual_semi_tape_is_right ())
  {
    assert (get_actual_position () < right_semi_tape_.size());
    ret_symbol = right_semi_tape_[get_actual_position ()];

    assert (is_valid_symbol(ret_symbol));
  }
  else
  {
    assert (actual_semi_tape_is_left ());
    assert (get_actual_position () < left_semi_tape_.size());

    ret_symbol = left_semi_tape_[get_actual_position ()];
    assert (is_valid_symbol(ret_symbol));
  }

  return ret_symbol;

}


// =========
ActualLocation Tape::get_actual_location () const
{
ActualLocation	ret_actual_location;
  if (logical_position_ >= 0)
  {
    ret_actual_location.actual_semi_tape_type_ = RIGHT_SEMI_TAPE;
    ret_actual_location.actual_position_       = logical_position_;

    assert (ret_actual_location.actual_position_ <= right_semi_tape_.size());
  }
  else
  {
    ret_actual_location.actual_semi_tape_type_ = LEFT_SEMI_TAPE;
    ret_actual_location.actual_position_       = labs (logical_position_) - 1;

    assert (ret_actual_location.actual_position_ <= left_semi_tape_.size());
  }
  return ret_actual_location;
}


// =========
ulong Tape::get_actual_position () const
{
  return get_actual_location().actual_position_;
}

// =========
bool Tape::actual_semi_tape_is_right () const
{
  return (get_actual_location().actual_semi_tape_type_ == RIGHT_SEMI_TAPE);
}


// =========
bool Tape::actual_semi_tape_is_left () const
{
  return (get_actual_location().actual_semi_tape_type_ == LEFT_SEMI_TAPE);
}



// =========
vector<symbol_t> Tape::get_full_alphabet () const
{
vector<symbol_t>	ret_vector;

  copy (empty_symbols_alphabet_.begin(), empty_symbols_alphabet_.end(), back_inserter(ret_vector));
  copy (input_alphabet_.begin(), input_alphabet_.end(), back_inserter(ret_vector));
  copy (internal_alphabet_.begin(), internal_alphabet_.end(), back_inserter(ret_vector));

  return ret_vector;
}



// =========
void Tape::set_symbol (const symbol_t& symbol_i)
{
  assert (is_valid_symbol(symbol_i));

  if (actual_semi_tape_is_right ())
  {
    assert (get_actual_position () < right_semi_tape_.size());
    right_semi_tape_[get_actual_position ()] = symbol_i;
  }
  else
  {
    assert (actual_semi_tape_is_left ());
    assert (get_actual_position () < left_semi_tape_.size());
    left_semi_tape_[get_actual_position ()] = symbol_i;
  }
}


// =========
void Tape::set_max_symbol_size_ ()
{
vector<symbol_t>	tmp_full_alphabet = get_full_alphabet();
  assert (max_symbol_size_ == 0);
  for (size_t i = 0; i < tmp_full_alphabet.size(); i++)
  {
    max_symbol_size_ = MAX_VALUE (max_symbol_size_, tmp_full_alphabet[i].size()); 
  }
}


// =========
bool Tape::is_input_symbol (const symbol_t& symbol_i) const
{
  return (find (input_alphabet_.begin(), input_alphabet_.end(), symbol_i) != input_alphabet_.end());
}

// =========
bool Tape::is_internal_symbol (const symbol_t& symbol_i) const
{
  return (find (internal_alphabet_.begin(), internal_alphabet_.end(), symbol_i) != internal_alphabet_.end());
}

// =========
bool Tape::is_empty_symbol (const symbol_t& symbol_i) const
{
  return (find (empty_symbols_alphabet_.begin(), empty_symbols_alphabet_.end(), symbol_i) != empty_symbols_alphabet_.end());
}

// =========
bool Tape::is_valid_symbol (const symbol_t& symbol_i) const
{
  return (is_input_symbol (symbol_i) || is_internal_symbol (symbol_i) || is_empty_symbol (symbol_i));
}

// =========
bool Tape::is_valid_shift (shift_t shift_i) const
{
  return ((shift_i == LEFT_SHIFT) || (shift_i == NO_SHIFT) || (shift_i == RIGHT_SHIFT)); 
}



// =========
bool Tape::check_alphabet () const
{
bool	ret_bool_value = true;

symbol_t phisical_empty_symbol = string();
vector<symbol_t>::const_iterator iter;

  assert (!empty_symbols_alphabet_.empty());
  assert (!input_alphabet_.empty());
  // assert (!internal_alphabet_.empty()); ---> internal_alphabet_ may be empty

  // ---------
  // ---------
  iter = find (empty_symbols_alphabet_.begin(), empty_symbols_alphabet_.end(), HEAD_START_POSITION_POINTER_REDERVED_SYMBOL); 
  if (iter != empty_symbols_alphabet_.end())
  {
    ret_bool_value = false;
    FATAL_MSG	("Sorry. Empty symbols alphabet -> symbol#"
		<< distance (empty_symbols_alphabet_.begin(), iter)
		<< " is reserved symbol (for machine head) : <"
		<< *iter
		<< ">"
		);
  }


  iter = find (input_alphabet_.begin(), input_alphabet_.end(), HEAD_START_POSITION_POINTER_REDERVED_SYMBOL); 
  if (iter != input_alphabet_.end())
  {
    ret_bool_value = false;
    FATAL_MSG	("Sorry. Input alphabet -> symbol#"
		<< distance (input_alphabet_.begin(), iter)
		<< " is reserved symbol (for machine head) : <"
		<< *iter
		<< ">"
		);
  }


  iter = find (internal_alphabet_.begin(), internal_alphabet_.end(), HEAD_START_POSITION_POINTER_REDERVED_SYMBOL); 
  if (iter != internal_alphabet_.end())
  {
    ret_bool_value = false;
    FATAL_MSG	("Sorry. Internal alphabet -> symbol#"
		<< distance (internal_alphabet_.begin(), iter)
		<< " is reserved symbol (for machine head) : <"
		<< *iter
		<< ">"
		);
  }


  // ---------
  // ---------
  // ---------
  iter = find (empty_symbols_alphabet_.begin(), empty_symbols_alphabet_.end(), phisical_empty_symbol); 
  if (iter != empty_symbols_alphabet_.end())
  {
    ret_bool_value = false;
    FATAL_MSG	("Empty symbols alphabet -> symbol#"
		<< distance (empty_symbols_alphabet_.begin(), iter)
		<< " is phisically empty : <"
		<< *iter
		<< ">"
		);
  }

  iter = find (input_alphabet_.begin(), input_alphabet_.end(), phisical_empty_symbol); 
  if (iter != input_alphabet_.end())
  {
    ret_bool_value = false;
    FATAL_MSG	("Input alphabet -> symbol#"
		<< distance (input_alphabet_.begin(), iter)
		<< " is phisically empty : <"
		<< *iter
		<< ">"
		);
  }


  iter = find (internal_alphabet_.begin(), internal_alphabet_.end(), phisical_empty_symbol); 
  if (iter != internal_alphabet_.end())
  {
    ret_bool_value = false;
    FATAL_MSG	("Internal alphabet -> symbol#"
		<< distance (internal_alphabet_.begin(), iter)
		<< " is phisically empty : <"
		<< *iter
		<< ">"
		);
  }

  // ------
vector<symbol_t> tmp_full_alphabet = get_full_alphabet();
vector<symbol_t>::iterator	iter2;
  for (iter2 = tmp_full_alphabet.begin(); iter2 != tmp_full_alphabet.end(); iter2++)
  {
    assert (count (iter2, tmp_full_alphabet.end(), *iter2));
    if (count (iter2, tmp_full_alphabet.end(), *iter2) > 1)
    {
      ret_bool_value = false;
      FATAL_MSG	("Alphabets -> symbol "
		<< "<"
		<< (*iter2)
	 	<< ">"
		<< " occurs more than once"
		);
    }
  }
  
  return ret_bool_value;

}

// =========
void Tape::show_alphabet (const string& msg_i) const
{
  IF_NOT_EMPTY (msg_i, 3, '=');

string text_empty_symbols_alphabet ("Empty symbols alphabet");
string text_input_alphabet ("Input alphabet");
string text_internal_alphabet ("Internal alphabet");
size_t text_max_size = 0;

  text_max_size	= MAX_VALUE(text_max_size, text_empty_symbols_alphabet.size());
  text_max_size	= MAX_VALUE(text_max_size, text_input_alphabet.size());
  text_max_size	= MAX_VALUE(text_max_size, text_internal_alphabet.size());


  cout << setw(text_max_size) << left << text_empty_symbols_alphabet.c_str() << " : ";
  for (size_t i = 0; i < empty_symbols_alphabet_.size(); i++)
  {
    cout << setw (max_symbol_size_) << empty_symbols_alphabet_[i].c_str() << " ";
  }
  cout << endl;

  cout << setw(text_max_size) << left << text_input_alphabet.c_str() << " : ";
  for (size_t i = 0; i < input_alphabet_.size(); i++)
  {
    cout << setw (max_symbol_size_) << input_alphabet_[i].c_str() << " ";
  }
  cout << endl;

  cout << setw(text_max_size) << left << text_internal_alphabet.c_str() << " : ";
  for (size_t i = 0; i < internal_alphabet_.size(); i++)
  {
    cout << setw (max_symbol_size_) << internal_alphabet_[i].c_str() << " ";
  }
  cout << endl;


}


// =========
void Tape::show_tape (const string& msg_i) const
{

  IF_NOT_EMPTY (msg_i, 3, '=');

  show_left_tape (msg_i);
  cout << SEMI_TAPES_DELIM;
  show_right_tape (msg_i);
  cout << endl;


}


// =========
void Tape::show_left_tape (const string& msg_i) const
{

  IF_NOT_EMPTY (msg_i, 3, '=');

string	left_delim;
string	right_delim;
  for (ulong k = left_semi_tape_.size(); k > 0; k--)
  {
    const ulong i = k - 1;
    if ((logical_position_ < 0) && (get_actual_position () == i))
    {
      left_delim  = LEFT_SCAN_DELIM;
      right_delim = RIGHT_SCAN_DELIM;
    }
    else
    {
      left_delim  = LEFT_USUAL_DELIM;
      right_delim = RIGHT_USUAL_DELIM;
    }

    cout << left_delim 
         << setw (max_symbol_size_) 
         << left_semi_tape_[i].c_str() 
         << right_delim 
         << " ";    
  }

}



// =========
void Tape::show_right_tape (const string& msg_i) const
{

  IF_NOT_EMPTY (msg_i, 3, '=');

string	left_delim;
string	right_delim;
  for (ulong i = 0; i < right_semi_tape_.size(); i++)
  {
    if ((logical_position_ >= 0) && (get_actual_position () == i))
    {
      left_delim  = LEFT_SCAN_DELIM;
      right_delim = RIGHT_SCAN_DELIM;
    }
    else
    {
      left_delim  = LEFT_USUAL_DELIM;
      right_delim = RIGHT_USUAL_DELIM;
    }

    cout << left_delim 
         << setw (max_symbol_size_) 
         << right_semi_tape_[i].c_str() 
         << right_delim 
         << " ";
  }

}



// =========
void Tape::visual_cleaning ()
{

  if (actual_semi_tape_is_right ())  visual_cleaning_right_semi_tape ();
  if (actual_semi_tape_is_left ())   visual_cleaning_left_semi_tape ();

}


// =========
void Tape::visual_cleaning_right_semi_tape ()
{

  assert (actual_semi_tape_is_right ());

  if (find (
		empty_symbols_alphabet_.begin(), 
		empty_symbols_alphabet_.end(), 
		right_semi_tape_.back()
		) 
		== empty_symbols_alphabet_.end())
  {
    return;
  }

  if (get_actual_position() == (right_semi_tape_.size() - 1)) return;

  assert (get_actual_position() < (right_semi_tape_.size() - 1));

  right_semi_tape_.erase (right_semi_tape_.end() - 1);

}



// =========
void Tape::visual_cleaning_left_semi_tape ()
{

  assert (actual_semi_tape_is_left ());

  if (find (
		empty_symbols_alphabet_.begin(), 
		empty_symbols_alphabet_.end(), 
		left_semi_tape_.back()
		) 
		== empty_symbols_alphabet_.end())
  {
    return;
  }

  if (get_actual_position() == (left_semi_tape_.size() - 1)) return;

  assert (get_actual_position() < (left_semi_tape_.size() - 1));

  left_semi_tape_.erase (left_semi_tape_.end() - 1);

}


