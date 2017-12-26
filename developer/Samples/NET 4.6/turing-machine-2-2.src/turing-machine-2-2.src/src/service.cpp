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
//  FILE     : service.cpp
//
//  DESCRIPTION :
//         Auxilary classes/functions (Implementation)
//
// ##############################################################




// ###############
#include "service.h"

// =============================
// =============================
// Constructor-0
Logo::Logo ()
{
	show_logo ("START");
}

// =============================
// Destructor
Logo::~Logo ()
{
	cout << endl;
	cout << endl;
	cout << endl;
	show_logo ("FINISH");
}

// =============================
void	Logo::show_logo (const string& msg_i) const
{
time_t timer;
	timer	= time(NULL);

const string pref1 ("\t#");
const string pref2 (pref1 + " ");

vector<string>	product_info;
  product_info.push_back (product_name);
  product_info.push_back (string (2, ' ') + product_url);
  product_info.push_back (string (2, ' ') + string (sw_version));
  product_info.push_back (string (2, ' ') + string (string (sw_version).size(), '-'));
  product_info.push_back (string (2, ' ') + author_name);
  product_info.push_back (string (2, ' ') + author_url);
  product_info.push_back (string (2, ' ') + author_email);

vector<vector<string> >	system_info;

#if (__DJGPP__)
  system_info.push_back(vector<string>());
  system_info.rbegin()->push_back (pref2);
  system_info.rbegin()->push_back ("DJGPP ");
  system_info.rbegin()->push_back (to_string(__DJGPP__));
#ifdef __DJGPP_MINOR__ 
  system_info.rbegin()->push_back (".");
  system_info.rbegin()->push_back (to_string(__DJGPP_MINOR__));
#endif
#endif  

#if (__MINGW32__ || __MINGW__ )
  system_info.push_back(vector<string>());
  system_info.rbegin()->push_back (pref2);
  system_info.rbegin()->push_back ("MINGW");
#endif  

#if (__CYGWIN32__ || __CYGWIN__)
  system_info.push_back(vector<string>());
  system_info.rbegin()->push_back (pref2);
  system_info.rbegin()->push_back ("CYGWIN");
#endif  

// ------ GNU gcc ------
#ifdef __GNUC__ 
  system_info.push_back(vector<string>());
  system_info.rbegin()->push_back (pref2);
  system_info.rbegin()->push_back ("GNU gcc version ");
  system_info.rbegin()->push_back (to_string(__GNUC__));
#ifdef __GNUC_MINOR__  
  system_info.rbegin()->push_back (".");
  system_info.rbegin()->push_back (to_string(__GNUC_MINOR__));
#ifdef __GNUC_PATCHLEVEL__   
#if (__GNUC_PATCHLEVEL__)
  system_info.rbegin()->push_back (".");
  system_info.rbegin()->push_back (to_string(__GNUC_PATCHLEVEL__));
#endif
#endif
#endif
#endif  

// ------ Visual C++ ------
#ifdef _MSC_VER 
  system_info.push_back(vector<string>());
  system_info.rbegin()->push_back (pref2);
  system_info.rbegin()->push_back ("Visual C++");
  switch (_MSC_VER)
  {
    case 1200 :
      system_info.rbegin()->push_back (to_string (" (VC6)"));
      break;

    case 1300 :
      system_info.rbegin()->push_back (to_string (" (VC7)"));
      break;

    case 1310 :
      system_info.rbegin()->push_back (to_string (" (VC7.1)"));
      break;

    default :
      break;
  }
#endif  


// ---------------------

size_t the_width = 0;
  for (size_t i = 0; i < product_info.size(); i++)
  {
    the_width = MAX_VALUE (the_width, product_info[i].size());
  }
  
  cout << endl;
  cout << pref1 << string (the_width + 1, '=') << endl;
  for (size_t i = 0; i < product_info.size(); i++)
  {
    cout << pref2 << product_info[i] << endl;
  }

  if (!system_info.empty())
  {
    cout << pref1 << string (the_width + 1, '-') << endl;
  }
  for (size_t i = 0; i < system_info.size(); i++)
  {
    copy (system_info[i].begin(), system_info[i].end(), ostream_iterator<string> (cout, ""));
    cout << endl;
  }
  cout << pref1 << string (the_width + 1, '-') << endl;

  cout << pref2 << msg_i << endl;
  cout << pref2 << asctime (localtime(&timer));
  cout << pref1 << string (the_width + 1, '=') << endl;
  cout << endl;

}

//===========			   
Logo	logo;
//===========			   
