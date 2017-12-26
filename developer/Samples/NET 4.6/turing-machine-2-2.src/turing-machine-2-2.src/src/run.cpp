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
//  FILE     : run.cpp
//
//  DESCRIPTION :
//         Class Run (Implementation)
//
// ##############################################################




// ###############
#include "run.h"

#define	setw3	3
#define	setw4	4
#define	setw5	5
#define	setw7	7
#define	setw8	8
#define	setw9	9
#define	setw10	10
#define	setw12	12
#define	setw13	13
#define	setw14	14
#define	setw15	15
#define	setw23	23
#define	setw43	43

const string	space3 (3, ' ');
const string	space4 (4, ' ');
const string	space5 (5, ' ');
const string	space6 (6, ' ');
const string	space8 (8, ' ');
const string	space9 (9, ' ');

const string	prefixa ("%%");


enum MetafileFields
{
  // --- DON'T CHANGE ---
  METAFILE_FIELD__DESCR_FILE_NAME,
  METAFILE_FIELD__NUMBER_OF_TAPES,
  METAFILE_FIELD__STATES_FILE_NAME,
  METAFILE_FIELD__ALPHABET_FILE_NAME,
  METAFILE_FIELD__TRANSITIONS_FILE_NAME,
  METAFILE_FIELD__INPUT_WORDS_FILE1_NAME
};

enum StatesFileRows
{
  // --- DON'T CHANGE ---
  STATES_FILE_ROW__INITIAL_STATES,
  STATES_FILE_ROW__HALTING_STATES,
  STATES_FILE_ROW__INTERNAL_STATES,
  STATES_FILE__TOTAL_ROWS
};

enum AlphabetFileRows
{
  // --- DON'T CHANGE ---
  ALPHABET_FILE_ROW__EMPTY_SYMBOLS,
  ALPHABET_FILE_ROW__INTERNAL_SYMBOLS,
  ALPHABET_FILE_ROW__INPUT_SYMBOLS,
  ALPHABET_FILE__TOTAL_ROWS
};


// ---- Tapes nummeration : 0, 1, 2, ...
#define	TRANSITIONS_FILE__TOTAL_FIELDS(number_of_tapes)			(3*number_of_tapes + 2)
#define	TRANSITIONS_FILE__CUR_STATE_FIELD				0
#define	TRANSITIONS_FILE__NEXT_STATE_FILED(number_of_tapes)		(number_of_tapes + 1)
#define	TRANSITIONS_FILE__CUR_SYMBOL_FIELD(tape_no)			(tape_no + 1)
#define	TRANSITIONS_FILE__NEXT_SYMBOL_FIELD(number_of_tapes, tape_no)	(number_of_tapes + 2*tape_no + 2)
#define	TRANSITIONS_FILE__SHIFT_FIELD(number_of_tapes, tape_no)		(number_of_tapes + 2*tape_no + 3)

// =========
// =========
// Constructor-0
Aux::Aux ()
{
  init();
}

// =========
// Constructor-0
Aux::~Aux ()
{
}

// =========
void Aux::init ()
{
  assert (help_request_keys_.empty());
  help_request_keys_.push_back ("-h");
  help_request_keys_.push_back ("--help");
  help_request_keys_.push_back ("?");
}

// =========
// =========
string			Run::metafile_name_s;
vector<vector<string> >	Run::metafile_data_s;

Aux			aux;
const vector<string>	Run::help_request_keys_s (aux.help_request_keys_);


// =========
// =========
// Constructor-1
Run::Run (
		size_t			serial_number_i,
		size_t			number_of_tapes_i,
		const string&		descr_file_name_i,
		const string&		states_file_name_i,
		const string&		alphabet_file_name_i,
		const string&		transitions_file_name_i,
		const vector<string>&	input_words_files_names_i
		)
		:
		serial_number_ (serial_number_i)
{
ostringstream oss1, oss21, oss22;
  cout	<< endl;
  
  oss1	<< space8 
	<< prefixa
	<< string (setw3, '=') 
	<< " Nondeterministic Turing Machine# "
	<< serial_number_ 
	<< " "
	<< string (setw4, '='); 
  
  oss21	<< space8 
	<< prefixa
	<< string (setw3, '=') 
	<< " PreProcessing Metafile Data : "; 
  
  oss22	<< " " 
	<< string (setw3, '='); 

  cout << space8 << prefixa << string (setw43, '=') << endl;
  cout << oss1.str() << endl;
  cout << oss21.str() << "BEGIN" << oss22.str() << endl;
  cout << space8 << prefixa << string (setw43, '=') << endl;

  check_results_ = init_number_of_tapes (number_of_tapes_i);
  if (check_results_) check_results_ = init_descr (descr_file_name_i);
  if (check_results_) check_results_ = init_states (states_file_name_i);
  if (check_results_) check_results_ = init_alphabet (alphabet_file_name_i);
  if (check_results_) check_results_ = init_transitions (transitions_file_name_i);
  if (check_results_) check_results_ = create_transitions();
  if (check_results_) check_results_ = init_input_words (input_words_files_names_i);

  cout << space8 << prefixa << string (setw43, '=') << endl;
  cout << oss21.str() << " END " << oss22.str() << endl;
  cout << oss1.str() << endl;
  cout << space8 << prefixa << string (setw43, '=') << endl;

  if (check_results_)
  {
    check_results_ = invoke_machine ();
  }

  if (!check_results_)
  {
    FATAL_MSG	("Can't invoke this Nondeterministic Turing Machine");
  }
} 

// =========
// Destructor
Run::~Run ()
{
} 


// =========
bool Run::init_number_of_tapes (size_t number_of_tapes_i)
{
  number_of_tapes_ = number_of_tapes_i;
  if (number_of_tapes_ == 0)
  {
    FATAL_MSG	("Illegal number of tapes : "
		<< number_of_tapes_
		);
    return false;
  }
  return true;
}


// =========
bool Run::init_descr(const string& descr_file_name_i)
{
bool	ret_bool_value;

  descr_file_name_ = descr_file_name_i;
  ret_bool_value = read_descr_file ();

  if (!ret_bool_value)
  {
    cout << endl;
    show_descr_file_structure_S ();
    show_descr_file_sample_S ();
    cout << endl;
  }
  return ret_bool_value; 

}


// =========
bool Run::init_states(const string& states_file_name_i)
{
bool	ret_bool_value;

  states_file_name_ = states_file_name_i;
  ret_bool_value = read_states_file ();

  if (!ret_bool_value)
  {
    cout << endl;
    show_states_file_structure_S ();
    show_states_file_sample_S ();
    cout << endl;
  }
  return ret_bool_value; 

}


// =========
bool Run::init_alphabet(const string& alphabet_file_name_i)
{
bool	ret_bool_value;

  alphabet_file_name_ = alphabet_file_name_i;
  ret_bool_value = read_alphabet_file ();

  if (!ret_bool_value )
  {
    cout << endl;
    show_alphabet_file_structure_S ();
    show_alphabet_file_sample_S ();
    cout << endl;
  }

  return ret_bool_value; 

}

// =========
bool Run::init_transitions(const string& transitions_file_name_i)
{

bool	ret_bool_value;

  transitions_file_name_ = transitions_file_name_i;
  ret_bool_value = read_transitions_file ();

  if (!ret_bool_value)
  {
    cout << endl;
    show_transitions_file_structure ();
    show_transitions_file_sample_S ();
    cout << endl;
  }

  return ret_bool_value; 

}

// =========
bool Run::init_input_words (const vector<string>& input_words_files_names_i)
{

  for (size_t i = 0; i < input_words_files_names_i.size(); i++)
  {
    input_words_files_names_.push_back (input_words_files_names_i[i]);
  }

  assert (!input_words_files_names_.empty());
  for (size_t i = 0; i < input_words_files_names_.size(); i++)
  {
    if (!read_input_words_file (input_words_files_names_[i]))
    {
      cout << endl;
      show_input_words_file_structure_S ();
      show_input_words_file_sample_S ();
      cout << endl;
      return false;
    }
  }

  show_input_words_files_content ();

  return true; 
}


// =========
bool Run::create_transitions()
{
state_t	cur_state;
vector<symbol_t>	cur_symbols;

state_t	next_state;
vector<SymbolAndShift> next_symbols_and_shifts;

typedef Transitions_t::value_type value_type;
pair<Transitions_t::iterator, bool> the_pair;

Transitions_t the_transitions;

  transitions_.clear();

  assert (!transitions_file_data_.empty());

  for (size_t i = 0; i < transitions_file_data_.size(); i++)
  {
    assert (transitions_file_data_[i].size() == TRANSITIONS_FILE__TOTAL_FIELDS(number_of_tapes_));

    cur_symbols.clear();
    next_symbols_and_shifts.clear();

    cur_state = transitions_file_data_[i][TRANSITIONS_FILE__CUR_STATE_FIELD];
    next_state = transitions_file_data_[i][TRANSITIONS_FILE__NEXT_STATE_FILED(number_of_tapes_)];

    for (size_t j = 0; j < number_of_tapes_; j++)
    {
      cur_symbols.push_back (
		transitions_file_data_[i][TRANSITIONS_FILE__CUR_SYMBOL_FIELD(j)]); 

      next_symbols_and_shifts.push_back (
		SymbolAndShift (
			transitions_file_data_[i][TRANSITIONS_FILE__NEXT_SYMBOL_FIELD(number_of_tapes_, j)], 
			transitions_file_data_[i][TRANSITIONS_FILE__SHIFT_FIELD(number_of_tapes_, j)]
			)
			);
    }

    const CurSituation cur_situation (cur_state, cur_symbols);
    if (the_transitions.count(cur_situation) == 0)
    {
      the_transitions[cur_situation] = vector<NextSituation> ();
    }
    assert (the_transitions.count(cur_situation) == 1);
    the_transitions[cur_situation].push_back (NextSituation (next_state, next_symbols_and_shifts));
  }

Transitions_t::const_iterator pos;

  for (pos = the_transitions.begin(); pos != the_transitions.end(); pos++)
  {
    the_pair = transitions_.insert (value_type (pos->first, pos->second));
    assert (the_pair.second);
  }
  return true;
}


// =========
bool Run::invoke_machine ()
{
ostringstream oss1, oss21, oss22;
  cout	<< endl;
  
  oss1	<< space8 
	<< prefixa
	<< string (setw3, '=') 
	<< " Nondeterministic Turing Machine# "
	<< serial_number_ 
	<< " "
	<< string (setw4, '='); 
  
  oss21	<< space8 
	<< prefixa
	<< string (setw7, '=') 
	<< " Machine Definition : "; 
  
  oss22	<< " " 
	<< string (setw8, '='); 

  cout << endl;
  cout << space8 << prefixa << string (setw43, '=') << endl;
  cout << oss1.str() << endl;  
  cout << oss21.str() << "BEGIN" << oss22.str() << endl;
  cout << space8 << prefixa << string (setw43, '=') << endl;

NondeterministicTuringMachine turing_machine (
	number_of_tapes_,
	// ---------------
	descr_file_data_,
	// ---------------
	states_file_data_[STATES_FILE_ROW__INITIAL_STATES],	// initial states
	states_file_data_[STATES_FILE_ROW__HALTING_STATES],	// halting states
	states_file_data_[STATES_FILE_ROW__INTERNAL_STATES],	// internal states
	// ---------------
	alphabet_file_data_[ALPHABET_FILE_ROW__EMPTY_SYMBOLS],	// empty symbols
	alphabet_file_data_[ALPHABET_FILE_ROW__INTERNAL_SYMBOLS],	// internal symbols
	alphabet_file_data_[ALPHABET_FILE_ROW__INPUT_SYMBOLS],	// input symbols
	// ---------------
	transitions_
	);


  if (!turing_machine.get_check_results())
  {
    FATAL_MSG	("Invalid Nondeterministic Turing Machine definition");
  }

  cout << space8 << prefixa << string (setw43, '=') << endl;
  cout << oss21.str() << " END " << oss22.str() << endl;
  cout << oss1.str() << endl;  
  cout << space8 << prefixa << string (setw43, '=') << endl;

  if (!turing_machine.get_check_results())	return false;

  assert (turing_machine.get_check_results());

  // ------------------------
  for (size_t i = 0; i < input_words_files_data_.size(); i++)
  {
    ostringstream oss31, oss32, oss41, oss42;
    oss31	<< space8 
		<< prefixa
		<< string (setw9, '=') 
		<< " Processing Input Words ";

    oss41	<< space8 
		<< prefixa
		<< string (setw13, '=') 
		<< " Set# "
		<< (i + 1)
		<< " : "; 
  
    oss32	<< " " 
    		<< string (setw9, '='); 

    oss42	<< " " 
		<< string (setw14, '='); 

    cout << endl;
    cout << endl;
    cout << endl;
    cout << space8 << prefixa << string (setw43, '=') << endl;
    cout << oss1.str() << endl;  
    cout << oss31.str() << oss32.str() << endl;
    cout << oss41.str() << "BEGIN" << oss42.str() << endl;
    cout << space8 << prefixa << string (setw43, '=') << endl;

    turing_machine.process_input (input_words_files_data_[i]);

    cout << endl;
    cout << space8 << prefixa << string (setw43, '=') << endl;
    cout << oss41.str() << " END " << oss42.str() << endl;
    cout << oss31.str() << oss32.str() << endl;
    cout << oss1.str() << endl;  
    cout << space8 << prefixa << string (setw43, '=') << endl;

  }
  return true;
} // invoke_machine


// =========
bool Run::read_file_S (const string& filename_i, vector<vector<string> >& data_o)
{
string file_line;

  assert (!filename_i.empty());

  ifstream fin (filename_i.c_str());
  if (!fin)
  {
    DIAGNOSTIC_MSG	("Unable to open file <"
			<< filename_i
			<< 
			">"
			);
    return false;
  }


  cout << endl;
  cout << "\t" << string (setw23, '-') << endl;
  cout << "\t--- File " << filename_i << endl;
  cout << "\t" << string (setw23, '-') << endl;

  data_o.clear();
  for (int i = 0; getline (fin, file_line); i++)
  {
    data_o.push_back(vector<string>());
    istringstream isstr (file_line.c_str ());

    copy(
         istream_iterator<string>(isstr),
         istream_iterator<string> (),
         back_inserter (data_o[i])
        );

    cout << "\t  Row#" << setw(3) << (i + 1) << " ---> ";
    copy (
          data_o[i].begin (),
          data_o[i].end (),
          ostream_iterator<string> (cout, " ")
         );
    cout << endl;


  } // while
  cout << "\t" << string (setw23, '-') << endl;
  cout << endl;

  return true;

} // read_file_S


// =========
bool Run::read_metafile_S (const string& filename_i)
{
// ------------------------------------------
// Metafile structure :
// Field#1 : <description file name>
// Field#2 : <number of tapes>
// Field#3 : <states file name>
// Field#4 : <alphabet file name>
// Field#5 : <transitions file name>
// Field#6 : <input words file#1 name>
// Field#7 : <input words file#2 name> [optionaly]
// .........
// Field#n : <input words file#(n-4) name> [optionaly]
// ------------------------------------------
#define MIN_NUMBER_OF_FIELDS_IN_META_FILE_ROW	5

  metafile_name_s = filename_i;
  if (!read_file_S (metafile_name_s , metafile_data_s))
  {
    FATAL_MSG	("Unable to read metafile <"
		<< metafile_name_s
		<< ">"
		);
    assert (metafile_data_s.empty());
    return false;
  }

  if (metafile_data_s.empty())
  {
    FATAL_MSG	("Metafile <"
		<< metafile_name_s
		<< "> is empty"
		);
    metafile_data_s.clear();
    return false;
  }

  for (size_t i = 0; i< metafile_data_s.size(); i++)
  {
    if (metafile_data_s[i].empty())
    {
      FATAL_MSG	("Metafile <"
		<< metafile_name_s
		<< ">, Row#"
		<< (i + 1)
		<< " is empty"
		);
      metafile_data_s.clear();
      return false;
    }

    if (metafile_data_s[i].size() < MIN_NUMBER_OF_FIELDS_IN_META_FILE_ROW)
    {
      FATAL_MSG	("Metafile <"
		<< metafile_name_s
		<< ">, Row#"
		<< (i + 1)
		<< " ---> illegal number of fields : "
		<< metafile_data_s[i].size()
		<< "; MIN ALLOWED = "
		<< MIN_NUMBER_OF_FIELDS_IN_META_FILE_ROW
		);
      metafile_data_s.clear();
      return false;
    }

    // --- Checking ---
    for (size_t j = 0; j < metafile_data_s[i].size(); j++)
    {

      assert (!metafile_data_s[i][j].empty());

      switch (j)
      {
        case METAFILE_FIELD__NUMBER_OF_TAPES :
          if (!(is_udec (const_cast<char*>(metafile_data_s[i][j].c_str()))))
          {
            FATAL_MSG	("Metafile <"
			<< metafile_name_s
			<< ">, Row#"
			<< (j + 1)
			<< " ---> non-numeric Number-Of-Tapes: <"
			<< metafile_data_s[i][j]
			<< ">"
			);
            metafile_data_s.clear();
            return false;
          }
          break;

        case METAFILE_FIELD__DESCR_FILE_NAME :
        case METAFILE_FIELD__STATES_FILE_NAME :
        case METAFILE_FIELD__ALPHABET_FILE_NAME :
        case METAFILE_FIELD__TRANSITIONS_FILE_NAME :
          // Do nothing
          break;

        default :
          // Do nothing
          break;

       } // switch (j)

    } // for (size j = 0; ...

  } // for (size_t i = 0; ...
  // ----------------
  show_metafile_content_S ();
  return true;

} // read_metafile_S

// =========
bool Run::read_alphabet_file ()
{
  assert (!alphabet_file_name_.empty());
  if (!read_file_S (alphabet_file_name_, alphabet_file_data_))
  {
    FATAL_MSG	("Unable to read alphabet file <"
		<< alphabet_file_name_
		<< ">"
		);
    assert (alphabet_file_data_.empty());
    return false;
  }

  if (alphabet_file_data_.empty())
  {
    FATAL_MSG	("Alphabet file <"
		<< alphabet_file_name_
		<< "> is empty"
		);
    alphabet_file_data_.clear();
    return false;
  }

  if (alphabet_file_data_.size() != ALPHABET_FILE__TOTAL_ROWS)
  {
    FATAL_MSG	("Illegal number of rows in Alphabet file <"
		<< alphabet_file_name_
		<< "> : must be "
		<< ALPHABET_FILE__TOTAL_ROWS
		<< ", actulally - "
		<< alphabet_file_data_.size()
		);
    alphabet_file_data_.clear();
    return false;
  }

  assert (alphabet_file_data_.size() == ALPHABET_FILE__TOTAL_ROWS);

  for (size_t i = 0; i < alphabet_file_data_.size(); i++)
  {
    if (i == ALPHABET_FILE_ROW__INTERNAL_SYMBOLS) continue;

    if (alphabet_file_data_[i].empty())
    {
      FATAL_MSG	("Alphabet file <"
		<< alphabet_file_name_
		<< ">, Row#"
		<< (i + 1)
		<< " is empty"
		);
      alphabet_file_data_.clear();
      return false;
    }
  } // for (size_t i = 0; ...


  // ----------------
  show_alphabet_file_content ();

  return true;

} // read_alphabet_file



// =========
bool Run::read_states_file ()
{
  assert (!states_file_name_.empty());
  if (!read_file_S (states_file_name_, states_file_data_))
  {
    FATAL_MSG	("Unable to read states file <"
		<< states_file_name_
		<< ">"
		);
    assert (states_file_data_.empty());
    return false;
  }

  if (states_file_data_.empty())
  {
    FATAL_MSG	("States file <"
		<< states_file_name_
		<< "> is empty"
		);
    states_file_data_.clear();
    return false;
  }

  if (states_file_data_.size() != STATES_FILE__TOTAL_ROWS)
  {
    FATAL_MSG	("Illegal number of rows in States file <"
		<< states_file_name_
		<< "> : must be "
		<< STATES_FILE__TOTAL_ROWS
		<< ", actulally - "
		<< states_file_data_.size()
		);
    states_file_data_.clear();
    return false;
  }

  assert (states_file_data_.size() == STATES_FILE__TOTAL_ROWS);

  for (size_t i = 0; i < states_file_data_.size(); i++)
  {
    if (states_file_data_[i].empty())
    {
      FATAL_MSG	("States file <"
		<< states_file_name_
		<< ">, Row#"
		<< (i + 1)
		<< " is empty"
		);
      states_file_data_.clear();
      return false;
    }
  } // for (size_t i = 0; ...


  // ----------------
  show_states_file_content ();

  return true;

} // read_states_file



// =========
bool Run::read_descr_file ()
{
  assert (!descr_file_name_.empty());
  if (!read_file_S (descr_file_name_, descr_file_data_))
  {
    WARNING_MSG	("Unable to read descr file <"
		<< descr_file_name_
		<< ">"
		);
    assert (descr_file_data_.empty());
    return true;
  }

  if (descr_file_data_.empty())
  {
    WARNING_MSG	("Description file <"
		<< descr_file_name_
		<< "> is empty"
		);
    descr_file_data_.clear();
    return true;
  }

  // ----------------
  show_descr_file_content ();

  return true;

} // read_descr_file



// =========
bool Run::read_transitions_file ()
{
  assert (!transitions_file_name_.empty());
  if (!read_file_S (transitions_file_name_, transitions_file_data_))
  {
    FATAL_MSG	("Unable to read transitions file <"
		<< transitions_file_name_ 
		<< ">"
		);
    assert (transitions_file_data_.empty());
    return false;
  }


  if (transitions_file_data_.empty())
  {
    FATAL_MSG	("Transitions file <"
		<< transitions_file_name_
		<< "> is empty"
		);
    transitions_file_data_.clear();
    return false;
  }

  for (size_t i = 0; i < transitions_file_data_.size(); i++)
  {
    if (transitions_file_data_[i].empty())
    {
      FATAL_MSG	("Transitions file <"
		<< transitions_file_name_
		<< ">, Row#"
		<< (i + 1)
		<< " is empty"
		);
      transitions_file_data_.clear();
      return false;
    }
  } // for (size_t i = 0; ...


  for (size_t i = 0; i < transitions_file_data_.size(); i++)
  {
    if (transitions_file_data_[i].size() != TRANSITIONS_FILE__TOTAL_FIELDS(number_of_tapes_))
    {
      FATAL_MSG	("Transitions file <"
		<< transitions_file_name_
		<< ">"
		<< endl
		<< string (22, ' ')
		<< "Row#"
		<< (i + 1)
		<< " ---> Invalid number of fields"
		<< endl
		<< string (22, ' ')
		<< "Must be (for Nondeterministic Turing Machine with "
		<< number_of_tapes_
		<< " tapes) "
		<< TRANSITIONS_FILE__TOTAL_FIELDS(number_of_tapes_)
		<< ", actually - "
		<< transitions_file_data_[i].size()
		);
      transitions_file_data_.clear();
      return false;
    }
  } // for (size_t i = 0; ...


  // ----------------
  show_transitions_file_content ();


  return true;
} // read_transitions_file

// =========
bool Run::read_input_words_file (const string& filename_i)
{
  input_words_files_data_.push_back (vector<vector<string> >());
  assert (!filename_i.empty());
  if (!read_file_S (filename_i, *input_words_files_data_.rbegin()))
  {
    FATAL_MSG	("Unable to read input words file <"
		<< filename_i
		<< ">"
		);
    assert (input_words_files_data_.rbegin()->empty());
    return false;
  }

  // ----------------
  // show_input_words_files_content ();

  return true;
} // read_input_words_file


// =========
void Run::show_metafile_structure_S ()
{
// ------------------------------------------
// Metafile structure :
// Field#1 : <descr file name>
// Field#2 : <number of tapes>
// Field#3 : <states file name>
// Field#4 : <alphabet file name>
// Field#5 : <transitions file name>
// Field#6 : <input words file#1 name>
// Field#7 : <input words file#2 name> [optionaly]
// .........
// Field#n : <input words file#(n-4) name> [optionaly]
// ------------------------------------------
const string text1 ("Metafile structure");

  cout	<< endl;
  cout	<< space3
	<< string (setw10, '#')
	<< " "
	<< text1
	<< " "
	<< string (setw10, '#')
	<< endl;

  cout	<< space4 << "Each row contains data related to some Nondeterministic Turing Machine" << endl;
  cout	<< space3 << string (setw10 + 1 + text1.size() + 1 + setw10, '-') << endl;

  cout	<< space4 << "Field#1 : " << "description file name" << endl;
  cout	<< space4 << "Field#2 : " << "number of tapes" << endl;
  cout	<< space4 << "Field#3 : " << "states file name" << endl;
  cout	<< space4 << "Field#4 : " << "alphabet file name" << endl;
  cout	<< space4 << "Field#4 : " << "transitions file name" << endl;
  cout	<< space4 << "Field#6 : " << "input words file#1 name" << endl;
  cout	<< space4 << "Field#7 : " << "input words file#2 name [optional]" << endl;
  cout	<< space4 << "....... : " << "........." << endl;

  cout	<< space4 << "Field#n : " << "input words file#(n-4) name [optional]" << endl;

  cout	<< space3 << string (setw10 + 1 + text1.size() + 1 + setw10, '#') << endl;
} // show_metafile_structure_S


// =========
void Run::show_alphabet_file_structure_S  ()
{
// ------------------------------------------
// States file structure :
// Row#1 : list of empty symbols (empty symbols alphabet)
// Row#2 : list of internal symbols
// Row#3 : list of input symbols
// ------------------------------------------
const string text1 ("Alphabet file structure");

  cout	<< endl;
  cout	<< space3
	<< string (setw10, '=')
	<< " "
	<< text1
	<< " "
	<< string (setw10, '=')
	<< endl;

  cout	<< space4 << "Row#1 : " << "list of empty symbols (empty symbols alphabet)" << endl;
  cout	<< space4 << "Row#2 : " << "list of internal symbols" << endl;
  cout	<< space4 << "Row#3 : " << "list of input symbols" << endl;


  cout	<< space3 << string (setw10 + 1 + text1.size() + 1 + setw10, '=') << endl;

} // show_alphabet_file_structure_S



// =========
void Run::show_descr_file_structure_S  ()
{
const string text1 ("Description file structure");

  cout	<< endl;
  cout	<< space3
	<< string (setw10, '=')
	<< " "
	<< text1
	<< " "
	<< string (setw10, '=')
	<< endl;

  cout	<< space4 << "--- Your text :" << endl;
  cout	<< space4 << "--- Description of your Nondeterministic Turing Machine" << endl;

  cout	<< space3 << string (setw10 + 1 + text1.size() + 1 + setw10, '=') << endl;

} // show_descr_file_structure_S



// =========
void Run::show_states_file_structure_S  ()
{
// ------------------------------------------
// States file structure :
// Row#1 : list of initial states
// Row#2 : list of halting states
// Row#3 : list of internal states
// ------------------------------------------
const string text1 ("States file structure");

  cout	<< endl;
  cout	<< space3
	<< string (setw10, '=')
	<< " "
	<< text1
	<< " "
	<< string (setw10, '=')
	<< endl;

  cout	<< space4 << "Row#1 : " << "list of initial states" << endl;
  cout	<< space4 << "Row#2 : " << "list of halting states" << endl;
  cout	<< space4 << "Row#3 : " << "list of internal states" << endl;


  cout	<< space3 << string (setw10 + 1 + text1.size() + 1 + setw10, '=') << endl;

} // show_states_file_structure_S


// =========
void Run::show_transitions_file_structure () const
{
  show_transitions_file_structure_S();

const string text1 ("Transition file structure");

  cout	<< endl;
  cout	<< space3
	<< string (setw10, '=')
	<< " "
	<< text1
	<< " "
	<< string (setw10, '=')
	<< endl;

size_t	field_no = 0;
const string text_field ("Field#");

const string text2 ("current symbol");
const string text3 ("next symbol");
const string text4 ("shift");
size_t the_setw = 0;

  the_setw = MAX_VALUE (the_setw, text2.size());
  the_setw = MAX_VALUE (the_setw, text3.size());
  the_setw = MAX_VALUE (the_setw, text4.size());
  assert (the_setw != 0);

  cout	<< space4 << "Each row contains some transition rule" << endl;
  cout	<< space4 << "Number of tapes = " << number_of_tapes_ << endl;

  cout	<< space3 << string (setw10 + 1 + text1.size() + 1 + setw10, '-') << endl;

  cout << space4 << text_field << setw(4) << right << field_no++ << " : " << "current state" << endl;
  for (size_t i = 0; i < number_of_tapes_; i++)
  {
    cout << space4 << text_field << setw(4) << right << field_no++ << " : " << setw (the_setw) << left << text2.c_str() << " on tape#" << i << endl;
  }

  cout	<< space4 << text_field << setw(4) << right << field_no++ << " : " << "next state" << endl;
  for (size_t i = 0; i < number_of_tapes_; i++)
  {
    cout << space4 << text_field << setw(4) << right << field_no++ << " : " << setw (the_setw) << left << text3.c_str() << " on tape#" << i << endl;
    cout << space4 << text_field << setw(4) << right << field_no++ << " : " << setw (the_setw) << left << text4.c_str() << " on tape#" << i << endl;
  }

  cout	<< space3 << string (setw10 + 1 + text1.size() + 1 + setw10, '=') << endl;

} // show_transitions_file_structure



// =========
void Run::show_transitions_file_structure_S()
{
const string text1 ("Transition file structure");

  cout	<< endl;
  cout	<< space3
	<< string (setw10, '=')
	<< " "
	<< text1
	<< " "
	<< string (setw10, '=')
	<< endl;

size_t	field_no = 0;
const string field_plus_no ("n+");

const string text_field ("Field#");

const string text2 ("current symbol");
const string text3 ("next symbol");
const string text4 ("shift");
size_t the_setw = 0;

  the_setw = MAX_VALUE (the_setw, text2.size());
  the_setw = MAX_VALUE (the_setw, text3.size());
  the_setw = MAX_VALUE (the_setw, text4.size());


size_t	tmp_no = 3;
const size_t	next_setw = field_plus_no.size() + 2;
const size_t	cur_setw = next_setw + 1;


  cout	<< space4 << "Each row contains some transition rule" << endl;
  cout	<< space4 << "Let n be number of tapes" << endl;

  cout	<< space3 << string (setw10 + 1 + text1.size() + 1 + setw10, '-') << endl;

  cout << space4 << text_field << setw(cur_setw) << right << field_no++ << " : " << "current state" << endl;
  for (size_t i = 0; i < tmp_no; i++)
  {
    cout << space4 << text_field << setw(cur_setw) << right << field_no++ << " : " << setw (the_setw) << left << text2.c_str() << " on tape#" << i << endl;
  }
  cout << space4 << string (20, '.') << endl;

  field_no = 1;
  cout	<< space4 << text_field << setw (next_setw) << right << setw (next_setw) << right << field_plus_no.c_str() << field_no++ << " : " << "next state" << endl;
  for (size_t i = 0; i < tmp_no; i++)
  {
    cout << space4 << text_field << setw (next_setw) << right << field_plus_no.c_str() << field_no++ << " : " << setw (the_setw) << left << text3.c_str() << " on tape#" << i << endl;
    cout << space4 << text_field << setw (next_setw) << right << field_plus_no.c_str() << field_no++ << " : " << setw (the_setw) << left << text4.c_str() << " on tape#" << i << endl;
  }
  cout << space4 << string (20, '.') << endl;

  cout	<< space3 << string (setw10 + 1 + text1.size() + 1 + setw10, '=') << endl;

} // show_transitions_file_structure_S


			     
// =========
void Run::show_input_words_file_structure_S ()
{
const string text1 ("Input words file structure");

  cout	<< endl;
  cout	<< space3
	<< string (setw10, '=')
	<< " "
	<< text1
	<< " "
	<< string (setw10, '=')
	<< endl;

size_t	row_no = 0;
const string last_row_no ("n-1");

const string text_row ("Row#");

size_t the_setw = 0;

const string text2 ("input word");
  the_setw = MAX_VALUE (the_setw, text2.size());
  assert (the_setw != 0);

size_t	tmp_no = 3;
const size_t	the2_setw = last_row_no.size() + 2;


  cout	<< space4 << "Each row contains input word for some tape" << endl;
  cout	<< space4 << "Let n be number of tapes" << endl;
  cout	<< space3 << string (setw10 + 1 + text1.size() + 1 + setw10, '-') << endl;

  for (size_t i = 0; i < tmp_no; i++)
  {
    cout << space4 << text_row << setw(the2_setw) << right << row_no++ << " : " << setw (the_setw) << left << text2.c_str() << " on tape#" << i << endl;
  }
  cout << space4 << string (20, '.') << endl;

  cout << space4 << text_row << setw(the2_setw) << right << last_row_no.c_str() << " : " << setw (the_setw) << left << text2.c_str() << " on tape#" << "(n-1)" << endl;

  cout	<< space3 << string (setw10 + 1 + text1.size() + 1 + setw10, '=') << endl;

  cout	<< space3
        << "Note! Symbol " 
        << "\"" 
        << HEAD_START_POSITION_POINTER_REDERVED_SYMBOL 
        << "\"" 
        << " may be used as (optional) pointer"
        << endl

        << space3
        << "      to start position of machine head on tape(s)."
        << endl

        << space3
        << "      In this case that symbol should be inserted"
        << endl

        << space3
        << "      before a pointed position"
        << endl;


} // show_input_words_file_structure_S


// =========
void Run::show_metafile_sample_S ()
{
const string text1("Metafile sample");
vector<vector<string> >	sample;

  sample.push_back(vector<string>());
  sample.rbegin()->push_back("sample1.dsc");
  sample.rbegin()->push_back("2");
  sample.rbegin()->push_back("sample1.sta");
  sample.rbegin()->push_back("sample1.abt");
  sample.rbegin()->push_back("sample1.rul");
  sample.rbegin()->push_back("sample11.in");
  sample.rbegin()->push_back("sample12.in");
  sample.rbegin()->push_back("sample13.in");

  sample.push_back(vector<string>());
  sample.rbegin()->push_back("sample2.dsc");
  sample.rbegin()->push_back("5");
  sample.rbegin()->push_back("sample2.sta");
  sample.rbegin()->push_back("sample2.abt");
  sample.rbegin()->push_back("sample2.rul");
  sample.rbegin()->push_back("sample21.in");


  sample.push_back(vector<string>());
  sample.rbegin()->push_back("descr.txt");
  sample.rbegin()->push_back("3");
  sample.rbegin()->push_back("states.txt");
  sample.rbegin()->push_back("alphabet.txt");
  sample.rbegin()->push_back("rules.txt");
  sample.rbegin()->push_back("input1.txt");
  sample.rbegin()->push_back("input2.txt");


  sample.push_back(vector<string>());
  sample.rbegin()->push_back("sample3.dsc");
  sample.rbegin()->push_back("1");
  sample.rbegin()->push_back("sample3.sta");
  sample.rbegin()->push_back("sample3.abt");
  sample.rbegin()->push_back("sample3.rul");
  sample.rbegin()->push_back("sample31.in");
  sample.rbegin()->push_back("sample32.in");



  cout	<< endl;
  cout	<< space3
	<< string (setw10, '#')
	<< " "
	<< text1
	<< " "
	<< string (setw10, '#')
	<< endl;

  for (size_t i = 0; i < sample.size(); i++)
  {
    copy (sample[i].begin(), sample[i].end(), ostream_iterator<string> (cout, " "));
    cout << endl;
  }

  cout	<< space3 << string (setw10 + 1 + text1.size() + 1 + setw10, '#') << endl;

}


// =========
void Run::show_descr_file_sample_S  ()
{
const string text1("Description file sample");
vector<vector<string> >	sample;


  sample.push_back(vector<string>());
  sample.rbegin()->push_back("A Noneterministic Turing Machine example : Partition Problem");

  sample.push_back(vector<string>());
  sample.rbegin()->push_back("* from \"The Design and Analysis of Computer Algorithms [1976]\"");

  sample.push_back(vector<string>());
  sample.rbegin()->push_back("* by A.V.Aho, J.E.Hopcroft, J.D.Ullman");

  sample.push_back(vector<string>());
  sample.rbegin()->push_back("* --> See examples 10.1");

  cout	<< endl;
  cout	<< space3
	<< string (setw10, '=')
	<< " "
	<< text1
	<< " "
	<< string (setw10, '=')
	<< endl;

  for (size_t i = 0; i < sample.size(); i++)
  {
    copy (sample[i].begin(), sample[i].end(), ostream_iterator<string> (cout, " "));
    cout << endl;
  }

  cout	<< space3 << string (setw10 + 1 + text1.size() + 1 + setw10, '=') << endl;

}

// =========
void Run::show_states_file_sample_S  ()
{
const string text1("States file sample");
vector<vector<string> >	sample;

  sample.push_back(vector<string>());
  sample.rbegin()->push_back("q0");

  sample.push_back(vector<string>());
  sample.rbegin()->push_back("q5");

  sample.push_back(vector<string>());
  sample.rbegin()->push_back("q1");
  sample.rbegin()->push_back("q2");
  sample.rbegin()->push_back("q3");
  sample.rbegin()->push_back("q4");

  cout	<< endl;
  cout	<< space3
	<< string (setw10, '=')
	<< " "
	<< text1
	<< " "
	<< string (setw10, '=')
	<< endl;

  for (size_t i = 0; i < sample.size(); i++)
  {
    copy (sample[i].begin(), sample[i].end(), ostream_iterator<string> (cout, " "));
    cout << endl;
  }

  cout	<< space3 << string (setw10 + 1 + text1.size() + 1 + setw10, '=') << endl;

}


// =========
void Run::show_alphabet_file_sample_S  ()
{
const string text1("Alphabet file sample");
vector<vector<string> >	sample;

  sample.push_back(vector<string>());
  sample.rbegin()->push_back("b");

  sample.push_back(vector<string>());
  sample.rbegin()->push_back("x");

  sample.push_back(vector<string>());
  sample.rbegin()->push_back("0");
  sample.rbegin()->push_back("1");

  cout	<< endl;
  cout	<< space3
	<< string (setw10, '=')
	<< " "
	<< text1
	<< " "
	<< string (setw10, '=')
	<< endl;

  for (size_t i = 0; i < sample.size(); i++)
  {
    copy (sample[i].begin(), sample[i].end(), ostream_iterator<string> (cout, " "));
    cout << endl;
  }

  cout	<< space3 << string (setw10 + 1 + text1.size() + 1 + setw10, '=') << endl;
}

// =========
void Run::show_transitions_file_sample_S  ()
{
const string text1("Transitions file sample");
const string text2("Number of tapes = 3");
vector<vector<string> >	sample;

typedef char trow_t [10];

const trow_t row01[] = {"q0", " 1", "b", "b", "    q1", " 1", "N", " x", "R", " x", "R"}; 
const trow_t row02[] = {"q1", " 1", "b", "b", "    q2", " 1", "R", " b", "N", " b", "N"}; 
const trow_t row03[] = {"q1", " 1", "b", "b", "    q3", " 1", "R", " b", "N", " b", "N"}; 
const trow_t row04[] = {"q2", " 0", "b", "b", "    q2", " 0", "R", " 0", "R", " b", "N"}; 
const trow_t row05[] = {"q2", " 1", "b", "b", "    q1", " 1", "N", " b", "N", " b", "N"}; 
const trow_t row06[] = {"q2", " b", "b", "b", "    q4", " b", "N", " b", "L", " b", "L"}; 
const trow_t row07[] = {"q3", " 0", "b", "b", "    q3", " 0", "R", " b", "N", " 0", "R"}; 
const trow_t row08[] = {"q3", " 1", "b", "b", "    q1", " 1", "N", " b", "N", " b", "N"}; 
const trow_t row09[] = {"q3", " b", "b", "b", "    q4", " b", "N", " b", "L", " b", "L"}; 
const trow_t row10[] = {"q4", " b", "0", "0", "    q4", " b", "N", " 0", "L", " 0", "L"}; 
const trow_t row11[] = {"q4", " b", "x", "x", "    q5", " b", "N", " x", "N", " x", "N"}; 

  sample.push_back(vector<string> (row01, row01 + sizeof (row01)/sizeof(*row01)));
  sample.push_back(vector<string> (row02, row02 + sizeof (row02)/sizeof(*row02)));
  sample.push_back(vector<string> (row03, row03 + sizeof (row03)/sizeof(*row03)));
  sample.push_back(vector<string> (row04, row04 + sizeof (row04)/sizeof(*row04)));
  sample.push_back(vector<string> (row05, row05 + sizeof (row05)/sizeof(*row05)));
  sample.push_back(vector<string> (row06, row06 + sizeof (row06)/sizeof(*row06)));
  sample.push_back(vector<string> (row07, row07 + sizeof (row07)/sizeof(*row07)));
  sample.push_back(vector<string> (row08, row08 + sizeof (row08)/sizeof(*row08)));
  sample.push_back(vector<string> (row09, row09 + sizeof (row09)/sizeof(*row09)));
  sample.push_back(vector<string> (row10, row10 + sizeof (row10)/sizeof(*row10)));
  sample.push_back(vector<string> (row11, row11 + sizeof (row11)/sizeof(*row11)));


  cout	<< endl;
  cout	<< space3
	<< string (setw10, '=')
	<< " "
	<< text1
	<< " "
	<< string (setw10, '=')
	<< endl;

  cout	<< space3
	<< string (setw12, '-')
	<< " "
	<< text2
	<< " "
	<< string (setw12, '-')
	<< endl;

  for (size_t i = 0; i < sample.size(); i++)
  {
    copy (sample[i].begin(), sample[i].end(), ostream_iterator<string> (cout, " "));
    cout << endl;
  }

  cout	<< space3 << string (setw10 + 1 + text1.size() + 1 + setw10, '=') << endl;

}

// =========
void Run::show_input_words_file_sample_S  ()
{
// -------------------
const string text11("Input words file sample-1");
const string text12("Number of tapes = 3");
vector<vector<string> >	sample1;

typedef char irow_t [3];

const irow_t row101[] = {"1", "0", "1", "0", "0", "1", "0"}; 
const irow_t row102[] = {"b"}; 
const irow_t row103[] = {"b"}; 

  sample1.push_back(vector<string> (row101, row101 + sizeof (row101)/sizeof(*row101)));
  sample1.push_back(vector<string> (row102, row102 + sizeof (row102)/sizeof(*row102)));
  sample1.push_back(vector<string> (row103, row103 + sizeof (row103)/sizeof(*row103)));

  cout	<< endl;
  cout	<< space3
	<< string (setw10, '=')
	<< " "
	<< text11
	<< " "
	<< string (setw10, '=')
	<< endl;
  cout	<< space3
	<< string (setw12, '-')
	<< " "
	<< text12
	<< " "
	<< string (setw12, '-')
	<< endl;

  for (size_t i = 0; i < sample1.size(); i++)
  {
    copy (sample1[i].begin(), sample1[i].end(), ostream_iterator<string> (cout, " "));
    cout << endl;
  }

  cout	<< space3 << string (setw10 + 1 + text11.size() + 1 + setw10, '=') << endl;


// -------------------
const string text21("Input words file sample-2");
const string text22("Number of tapes = 3");
vector<vector<string> >	sample2;

typedef char irow_t [3];

const irow_t row201[] = {"1", "0", HEAD_START_POSITION_POINTER_REDERVED_SYMBOL, "1", "0", "0", "1", "0"}; 
const irow_t row202[] = {"b"}; 
const irow_t row203[] = {HEAD_START_POSITION_POINTER_REDERVED_SYMBOL, "b"}; 

  sample2.push_back(vector<string> (row201, row201 + sizeof (row201)/sizeof(*row201)));
  sample2.push_back(vector<string> (row202, row202 + sizeof (row202)/sizeof(*row202)));
  sample2.push_back(vector<string> (row203, row203 + sizeof (row203)/sizeof(*row203)));

  cout	<< endl;
  cout	<< space3
	<< string (setw10, '=')
	<< " "
	<< text21
	<< " "
	<< string (setw10, '=')
	<< endl;
  cout	<< space3
	<< string (setw12, '-')
	<< " "
	<< text22
	<< " "
	<< string (setw12, '-')
	<< endl;

  for (size_t i = 0; i < sample2.size(); i++)
  {
    copy (sample2[i].begin(), sample2[i].end(), ostream_iterator<string> (cout, " "));
    cout << endl;
  }

  cout	<< space3 << string (setw10 + 1 + text21.size() + 1 + setw10, '=') << endl;

  cout	<< space3
        << "Note! Symbol " 
        << "\"" 
        << HEAD_START_POSITION_POINTER_REDERVED_SYMBOL 
        << "\"" 
        << " is (optional) pointer to start position of machine head"
        << endl;

}


// =========
void Run::show_metafile_content_S ()
{
size_t	text_setw = 0;
const string text1 ("Number of tapes");
const string text2 ("States file name");
const string text3 ("Alphabet file name");
const string text4 ("Transitions file name");
const string text5 ("Input words files names");
const string text6 ("Description file name");

  text_setw = MAX_VALUE (text_setw, text1.size());
  text_setw = MAX_VALUE (text_setw, text2.size());
  text_setw = MAX_VALUE (text_setw, text3.size());
  text_setw = MAX_VALUE (text_setw, text4.size());
  text_setw = MAX_VALUE (text_setw, text5.size());
  text_setw = MAX_VALUE (text_setw, text6.size());


  cout << space3 << string (setw10, '=') << " Data selected from Metafile " << metafile_name_s << " " << string (setw10, '=') << endl;

  for (size_t i = 0; i < metafile_data_s.size(); i++)
  {
    cout << space5 << string (setw10, '-') << " Nondeterministic Turing Machine# " << (i + 1) << " " << string (setw10, '-') << endl;
    cout << space6 << setw(text_setw) << left << text6.c_str() << " : " << metafile_data_s[i][METAFILE_FIELD__DESCR_FILE_NAME] << endl;
    cout << space6 << setw(text_setw) << left << text1.c_str() << " : " << metafile_data_s[i][METAFILE_FIELD__NUMBER_OF_TAPES] << endl;
    cout << space6 << setw(text_setw) << left << text2.c_str() << " : " << metafile_data_s[i][METAFILE_FIELD__STATES_FILE_NAME] << endl;
    cout << space6 << setw(text_setw) << left << text3.c_str() << " : " << metafile_data_s[i][METAFILE_FIELD__ALPHABET_FILE_NAME] << endl;
    cout << space6 << setw(text_setw) << left << text4.c_str() << " : " << metafile_data_s[i][METAFILE_FIELD__TRANSITIONS_FILE_NAME] << endl;
    cout << space6 << setw(text_setw) << left << text5.c_str() << " : ";
    copy (metafile_data_s[i].begin() + METAFILE_FIELD__INPUT_WORDS_FILE1_NAME, metafile_data_s[i].end(), ostream_iterator<string> (cout, " "));
    cout << endl;
    cout << endl;
  }


} // show_metafile_content_S

// =========
void Run::show_alphabet_file_content  () const
{
size_t	text_setw = 0;
const string text1 ("Empty state");
const string text2 ("Internal symbols");
const string text3 ("Input symbols");

  text_setw = MAX_VALUE (text_setw, text1.size());
  text_setw = MAX_VALUE (text_setw, text2.size());
  text_setw = MAX_VALUE (text_setw, text3.size());


  //cout << space3 << string (setw10, '=') << " INFO selected from Alphabet file " << alphabet_file_name_ << " " << string (setw10, '=') << endl;

  cout << space6 << setw(text_setw) << left << text1.c_str() << " : "; 
  copy (alphabet_file_data_[ALPHABET_FILE_ROW__EMPTY_SYMBOLS].begin(), alphabet_file_data_[ALPHABET_FILE_ROW__EMPTY_SYMBOLS].end(), ostream_iterator<string> (cout, " "));
  cout << endl;

  cout << space6 << setw(text_setw) << left << text2.c_str() << " : "; 
  copy (alphabet_file_data_[ALPHABET_FILE_ROW__INTERNAL_SYMBOLS].begin(), alphabet_file_data_[ALPHABET_FILE_ROW__INTERNAL_SYMBOLS].end(), ostream_iterator<string> (cout, " "));
  cout << endl;

  cout << space6 << setw(text_setw) << left << text3.c_str() << " : "; 
  copy (alphabet_file_data_[ALPHABET_FILE_ROW__INPUT_SYMBOLS].begin(), alphabet_file_data_[ALPHABET_FILE_ROW__INPUT_SYMBOLS].end(), ostream_iterator<string> (cout, " "));
  cout << endl;

  cout << endl;


} // show_alphabet_file_content


// =========
void Run::show_descr_file_content  () const
{

  for (size_t i = 0; i < descr_file_data_.size(); i++)
  {
    cout << space6;
    copy (descr_file_data_[i].begin(), descr_file_data_[i].end(), ostream_iterator<string> (cout, " "));
    cout << endl;
  }

  cout << endl;
				 
} // show_descr_file_content



// =========
void Run::show_states_file_content  () const
{
size_t	text_setw = 0;
const string text1 ("Initial state");
const string text2 ("Halting state");
const string text3 ("Internal states");

  text_setw = MAX_VALUE (text_setw, text1.size());
  text_setw = MAX_VALUE (text_setw, text2.size());
  text_setw = MAX_VALUE (text_setw, text3.size());


  //cout << space3 << string (setw10, '=') << " INFO selected from States file " << states_file_name_ << " " << string (setw10, '=') << endl;

  cout << space6 << setw(text_setw) << left << text1.c_str() << " : "; 
  copy (states_file_data_[STATES_FILE_ROW__INITIAL_STATES].begin(), states_file_data_[STATES_FILE_ROW__INITIAL_STATES].end(), ostream_iterator<string> (cout, " "));
  cout << endl;

  cout << space6 << setw(text_setw) << left << text2.c_str() << " : "; 
  copy (states_file_data_[STATES_FILE_ROW__HALTING_STATES].begin(), states_file_data_[STATES_FILE_ROW__HALTING_STATES].end(), ostream_iterator<string> (cout, " "));
  cout << endl;

  cout << space6 << setw(text_setw) << left << text3.c_str() << " : "; 
  copy (states_file_data_[STATES_FILE_ROW__INTERNAL_STATES].begin(), states_file_data_[STATES_FILE_ROW__INTERNAL_STATES].end(), ostream_iterator<string> (cout, " "));
  cout << endl;

  cout << endl;
				 
} // show_states_file_content


// =========
void Run::show_transitions_file_content  () const
{
  for (size_t i = 0; i < transitions_file_data_.size(); i++)
  {
    //cout << space6 << "Row#" << setw(4) << right << i << " : ";
    cout << space6 << "Rule" << " :   ";
    copy (transitions_file_data_[i].begin(), transitions_file_data_[i].end(), ostream_iterator<string> (cout, " "));
    cout << endl;
  }

  cout << endl;

} // show_transitions_file_content

// =========
void Run::show_input_words_files_content  () const
{
  for (size_t i = 0; i < input_words_files_data_.size(); i++)
  {
    cout << space6 << "Input words --> Set#" << setw(3) << (i + 1) << endl;
    for (size_t j = 0; j < input_words_files_data_[i].size(); j++)
    {
      cout << space9 << "Tape#" << setw(3) << j << " : ";
      copy (input_words_files_data_[i][j].begin(), input_words_files_data_[i][j].end(), ostream_iterator<symbol_t> (cout, " "));
      cout << endl;
    }
    cout << endl;
  }


} // show_input_words_files_content


// =========
void Run::invoke_turing_machine_S (
		size_t		serial_number_i,
		size_t		number_of_tapes_i,
		const string&	descr_file_name_i,
		const string&	states_file_name_i,
		const string&	alphabet_file_name_i,
		const string&	transitions_file_name_i,
		const vector<string>&	input_words_files_names_i
		)
{
ostringstream oss1, oss2;

  cout	<< endl;
  cout	<< endl;
  cout	<< endl;
  cout	<< endl;
  
  oss1	<< space5 
	<< string (setw10, '#') 
	<< " Nondeterministic Turing Machine# " 
	<< serial_number_i 
	<< " : "; 
  
  oss2	<< " " 
	<< string (setw10, '#'); 

  cout << oss1.str() << "BEGIN" << oss2.str() << endl;


Run run_inst(
	serial_number_i,
	number_of_tapes_i,
	descr_file_name_i,
	states_file_name_i,
	alphabet_file_name_i,
	transitions_file_name_i,
	input_words_files_names_i
	);

  if (!run_inst.check_results_)
  {
    cout	<< space5 
    		<< "---> " 
       		<< "Invalid Data !!!" 
      		<< endl; 
  }
  cout	<< endl;
  cout	<< endl;

  cout << oss1.str() << " END " << oss2.str() << endl;

} // invoke_turing_machine_S


// =========
void Run::show_usage_S (const string& exe_name_i)
{
  cout << "\tUSAGE    : " 
       << exe_name_i 
       << " [ metafile-name : Default = " 
       << DEFAULT_METAFILE 
       << " ]"
       << endl;
 
  cout << "\t         : " 
       << exe_name_i 
       << " [ ";
  for (size_t i = 0; i < help_request_keys_s.size(); i++)
  {
    if (i) cout << " | ";
    cout << help_request_keys_s[i];
  }
  cout << " ]" << endl;

  cout << endl;
  cout << "\tEXAMPLES : " << exe_name_i << endl;
  cout << "\t         : " << exe_name_i << " metafile1" << endl;
  for (size_t i = 0; i < help_request_keys_s.size(); i++)
  {
    cout << "\t         : " << exe_name_i << " " << help_request_keys_s[i] << endl;
  }

}

// =========
void Run::show_help_S (const string& exe_name_i)
{
  show_usage_S(exe_name_i);

  cout << endl;
  show_metafile_structure_S ();
  cout << endl;
  show_descr_file_structure_S ();
  cout << endl;
  show_states_file_structure_S ();
  cout << endl;
  show_alphabet_file_structure_S ();
  cout << endl;
  show_transitions_file_structure_S ();
  cout << endl;
  show_input_words_file_structure_S ();
  cout << endl;

  show_sample_S();
}


// =========
void Run::show_sample_S ()
{
  cout << endl << endl;
  show_metafile_sample_S ();
  cout << endl;		    

  cout << endl;		    

const string prefix1 (9, ' ');	   
const string prefix2 (5, ' ');	   
const string l_border ("/// ---");	   
const string r_border ("--- \\\\\\");	   

const string text1 ("Sample of Nondeterministic Turing Machine Definition");
  cout << endl;
  cout << endl;
  cout << ""
       << prefix2 << l_border << " " << text1 << " : " << "BEGIN" << " " << r_border 
       << endl;

  cout << "" 
       << prefix1 << "A Nondeterministic Turing Machine example : Partition Problem"
       << endl
       << prefix1 << "* from \"The Design and Analysis of Computer Algorithms [1976]\""
       << endl
       << prefix1 << "* by A.V.Aho, J.E.Hopcroft, J.D.Ullman"
       << endl
       << prefix1 << "* --> See examples 10.1"
       << endl
       << prefix1 << "is used in samples below" 
       << endl;
  cout << endl;

  show_descr_file_sample_S ();
  cout << endl;
  show_states_file_sample_S ();
  cout << endl;
  show_alphabet_file_sample_S ();
  cout << endl;
  show_transitions_file_sample_S ();
  cout << endl;
  show_input_words_file_sample_S ();
  cout << endl;

  cout << endl;
  cout << ""
       << prefix2 << l_border << " " << text1 << " : " << "END" << " " << r_border 
       << endl;
  cout << endl;

}


// =========
void Run::run (uint argc, char** argv)
{
  cout << endl;
  cout << endl;
  cout << "\t---> YOUR COMMAND LINE : ";
  for (uint i = 0; i < argc; i++)
  {
    cout << argv[i] << " ";
  }
  cout << endl;
  cout << endl;

  assert (!help_request_keys_s.empty());

  if (argc >= 2)
  {
    if (find (help_request_keys_s.begin(), help_request_keys_s.end(), string(argv[1])) != help_request_keys_s.end())
    {
      show_help_S(argv[0]);
      return;
    }
  }

  if (argc == 1)
  {
    cout << endl;
    show_usage_S (argv[0]);
    cout << endl;
    cout << endl;
  }
  
  // ---------------------------
  if (!((argc < 2) ? read_metafile_S() : read_metafile_S (argv[1])))
  {
    cout << endl;
    FATAL_MSG	("Unable to select data from metafile");
    cout << endl;
    show_metafile_structure_S ();
    show_metafile_sample_S ();
    cout << endl;
    return ;
  }

  cout << endl;

size_t	number_of_tapes;

  for (size_t i = 0; i < metafile_data_s.size(); i++)
  {
     vector<string>	input_words_files_names;

     number_of_tapes = atoi (metafile_data_s[i][METAFILE_FIELD__NUMBER_OF_TAPES].c_str());

     for (size_t j = METAFILE_FIELD__INPUT_WORDS_FILE1_NAME; 
		 j < metafile_data_s[i].size(); 
		 j++
		 )
     {
       input_words_files_names.push_back (metafile_data_s[i][j]);
     }

     //copy (metafile_data_s[i].begin() + METAFILE_FIELD__INPUT_WORDS_FILE1_NAME, metafile_data_s[i].end(), input_words_files_names.begin());

     //assert (number_of_tapes != 0);
     invoke_turing_machine_S (
	(i + 1),
	number_of_tapes,
	metafile_data_s[i][METAFILE_FIELD__DESCR_FILE_NAME], 
	metafile_data_s[i][METAFILE_FIELD__STATES_FILE_NAME], 
	metafile_data_s[i][METAFILE_FIELD__ALPHABET_FILE_NAME], 
	metafile_data_s[i][METAFILE_FIELD__TRANSITIONS_FILE_NAME], 
	input_words_files_names
	);

  }

} // run



