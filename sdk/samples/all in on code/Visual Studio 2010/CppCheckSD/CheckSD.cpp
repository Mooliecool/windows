/****************************** Module Header ******************************\
Module Name:  CheckSD.cpp
Project:      CppCheckSD
Copyright (c) Microsoft Corporation

This module calls a variety of different functions in the other modules to 
obtain security descriptor information associated with the object name passed 
to this module.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "CheckSD.h"
#include <stdio.h>
#include <strsafe.h>

//
//   FUNCTION: DisplayUsage(void)
//
//   PURPOSE: Displays how to run the application
//   CppCheckSD
//
//   PARAMETERS:
//   * none
//
//   RETURN VALUE: none
//

void DisplayUsage(void)
{
     wprintf(L"\nUsage: CppCheckSD [object] [name] </v> </a>\n");
	 wprintf(L" /v : VERBOSE\n");
	 wprintf(L" /a : Show Audit Aces\n\n");
	 wprintf(L" -a : mailslot, use \\\\[server]\\mailslot\\[mailslotname]\n");
	 wprintf(L" -c : service control manager\n");
     wprintf(L" -d : directory or driver letter, use \\\\.\\[driveletter]\n");
     wprintf(L" -e : event\n");
     wprintf(L" -f : file\n");
	 wprintf(L" -h : thread, use tid instead of name\n");
     wprintf(L" -i : memory mapped file\n");
	 wprintf(L" -j : job object\n");
     wprintf(L" -k : desktop, use [window station\\desktop]\n");
     wprintf(L" -l : printer, use \\\\[server]\\[printername]\n");
     wprintf(L" -m : mutex\n");
     wprintf(L" -n : named pipe, use \\\\[server or .]\\pipe\\[pipename]\n");
     wprintf(L" -o : process access token, use pid instead of name\n");
     wprintf(L" -p : process, use pid instead of name\n");
     wprintf(L" -r : registry key, use CLASSES_ROOT, CURRENT_USER, MACHINE, or USERS suchas MACHINE\\Software\n");
     wprintf(L" -s : sempahore\n");
     wprintf(L" -t : network share, use [\\\\server\\sharename]\n");
     wprintf(L" -v : service, use [\\\\server\\service]\n");
     wprintf(L" -w : window station\n");

	 if (Is64())
		 wprintf(L" -x : 32 bit registry key\n");

	 wprintf(L" -z : waitable timer\n");
     return;
}

void wmain(int argc, WCHAR *argv[])
{
     int  u        = 0;
	 BOOL bVerbose = FALSE;
	 BOOL bAudit   = FALSE;

     // Display usage
     if (argc < 3 || argc > 5)
	 {
          DisplayUsage();
          return;
     }


	 // Do validation on the user input to help avoid buffer overruns.   
	 _try
	 {
		// argv[1] should be a null terminated string 
	    // of exactly two characters in length (not including the NULL). 
		if (('-' != argv[1][0]) || ('\0' == argv[1][1]) || ('\0' != argv[1][2]))
		{
			 DisplayUsage();
			 return;
		}

		// argv[2] should have at least one non-NULL character before a NULL character.
		if ('\0' == argv[2][0])
		{
	         DisplayUsage();
			 return;	 
	    }

		// argv[2] should not be a string of more than MAX_PATH characters.
		u = 0;
		while ('\0' != argv[2][u])
		{
			 if (MAX_PATH == u)
			 {
			      DisplayUsage();
			      return;	 
			 }
			 u++;
		}

		if (argc == 4)
		{
			// argv[3] should be a null terminated string 
			// of exactly two characters in length (not including the NULL). 
			if (('/' != argv[3][0]) || ('v' != argv[3][1]) || ('\0' != argv[3][2]))
			{
				DisplayUsage();
				return;
			}
			else
				bVerbose = TRUE;
		}

		if (argc == 5)
		{
			// argv[3] should be a null terminated string 
			// of exactly two characters in length (not including the NULL). 
			if (('/' != argv[4][0]) || ('a' != argv[4][1]) || ('\0' != argv[4][2]))
			{
				DisplayUsage();
				return;
			}
			else
				bAudit = TRUE;
		}

	 } 
	 _except(EXCEPTION_EXECUTE_HANDLER)
	 {
          DisplayUsage();
          return;	 
	 }

     wprintf(L">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
     wprintf(L">>                 SECURITY INFORMATION                >>\n");
     wprintf(L">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n");
     wprintf(L"object name ........ %s\n", argv[2]);
     wprintf(L"object type ........ ");

	switch (argv[1][1])
	{
		case 'a':
			wprintf(L"mailslot\n");
			DumpObjectWithHandle(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 'c':
			wprintf(L"service control manager\n");
			DumpObjectWithHandle(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 'd':
			wprintf(L"directory\n");
			DumpObjectWithName(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 'e':
			wprintf(L"event\n");
			DumpObjectWithName(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 'f':
			wprintf(L"file\n");
			DumpObjectWithName(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 'h':
			wprintf(L"thread\n");
			DumpObjectWithHandle(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 'i':
			wprintf(L"memory mapped file\n");
			DumpObjectWithName(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 'j':
			wprintf(L"job object\n");
			DumpObjectWithHandle(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 'k':
			wprintf(L"desktop\n");
			DumpObjectWithHandle(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 'l':
			 wprintf(L"printer\n");
			DumpObjectWithName(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 'm':
			wprintf(L"mutex\n");
			DumpObjectWithName(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 'n':
			wprintf(L"named pipe\n");
			DumpObjectWithHandle(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 'o':
			wprintf(L"process access token\n");
			DumpObjectWithHandle(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 'p':
			wprintf(L"process\n");
			DumpObjectWithHandle(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 'r':
			wprintf(L"registry\n");
			DumpObjectWithName(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 's':
			wprintf(L"semaphore\n");
			DumpObjectWithName(argv[2], argv[1][1], bVerbose, bAudit);
			break;   
		case 't':
			wprintf(L"network share\n");
			DumpObjectWithName(argv[2], argv[1][1], bVerbose, bAudit);
			break; 
		case 'v':
			wprintf(L"service\n");
			DumpObjectWithName(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 'w':
			wprintf(L"window station\n");
			DumpObjectWithHandle(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		case 'x':
			if (Is64())
			{
				wprintf(L"32 bit registry key\n");
				DumpObjectWithName(argv[2], argv[1][1], bVerbose, bAudit);
			}
			else
				wprintf(L"This is a 32 bit Application!\n");
			break;
		case 'z':
			wprintf(L" -z : waitable timer\n");
			DumpObjectWithName(argv[2], argv[1][1], bVerbose, bAudit);
			break;
		default:
			DisplayUsage();
     }
}