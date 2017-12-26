//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//-----------------------------------------------------------------------

/*=====================================================================
  File:      ArgParse.cs

  Summary:   Reusable class for parsing command-line arguments.

=====================================================================*/

// Add the classes in the following namespaces to our namespace
using System;
using System.Globalization;


///////////////////////////////////////////////////////////////////////////////
namespace Microsoft.Samples
{
	public abstract class ArgumentParser
	{
		private String[] switchChars;             // For example: "/", "-"

		private String[] switchSymbols;           // Switch character(s)

		private Boolean caseSensitiveSwitches;   // Are switches case-sensitive?

		// Domain of results when processing a command-line argument switch
		protected enum SwitchStatus { NoError, Error, ShowUsage };

		// Constructor that defaults to case-insensitive switches and 
		// defaults to "/" and "-" as the only valid switch characters
		protected ArgumentParser(String[] switchSymbols) 
        : this(switchSymbols, false, new string[] { "/", "-" })
		{
		}

		// Constructor that defaults to "/" and "-" as the only valid switch characters
		protected ArgumentParser(String[] switchSymbols, Boolean caseSensitiveSwitches) 
        : this(switchSymbols, caseSensitiveSwitches, new string[] { "/", "-" })
		{
		}

		// Constructor with no defaults
		protected ArgumentParser(String[] switchSymbols, Boolean caseSensitiveSwitches, String[] switchChars)
		{
			this.switchSymbols = switchSymbols;
			this.caseSensitiveSwitches = caseSensitiveSwitches;
			this.switchChars = switchChars;
		}

		// Every derived class must implement an OnUsage method
		public abstract void OnUsage(String errorInfo);

		// Every derived class must implement an OnSwitch method or a switch is considerred an error
		protected virtual SwitchStatus OnSwitch(String switchSymbol, String switchValue)
		{
			return (SwitchStatus.Error);
		}

		// Every derived class must implement an OnNonSwitch method or a non-switch is considerred an error
		protected virtual SwitchStatus OnNonSwitch(String value)
		{
			return (SwitchStatus.Error);
		}

		// The derived class is notified after all command-line switches have been parsed.
		// The derived class can perform any sanity checking required at this time.
		protected virtual SwitchStatus OnDoneParse()
		{
			// By default, we'll assume that all parsing was successful
			return (SwitchStatus.Error);
		}

		// This Parse method always parses the application's command-line arguments
		public Boolean Parse()
		{
			// Visual Basic will use this method since its entry-point function 
			// doesn't get the command-line arguments passed to it.
			return (Parse(Environment.GetCommandLineArgs()));
		}

		// This Parse method parses an arbitrary set of arguments
		public Boolean Parse(String[] args)
		{
			SwitchStatus ss = SwitchStatus.NoError;	    // Assume parsing is sucessful.
			int argNum;

			for (argNum = 0; (ss == SwitchStatus.NoError) && (argNum < args.Length); argNum++)
			{
				// Determine if this argument starts with a valid switch character
				Boolean fIsSwitch = false;

				for (int n = 0; !fIsSwitch && (n < switchChars.Length); n++)
				{
					fIsSwitch = (0 == String.CompareOrdinal(args[argNum], 0, switchChars[n], 0, 1));
				}

				if (fIsSwitch)
				{
					// Does the switch begin with a legal switch symbol?
					Boolean fLegalSwitchSymbol = false;
					int n;

					for (n = 0; !fLegalSwitchSymbol && (n < switchSymbols.Length); n++)
					{
						if (caseSensitiveSwitches)
						{
							fLegalSwitchSymbol = (0 == String.CompareOrdinal(args[argNum], 1, switchSymbols[n], 0, switchSymbols[n].Length));
						}
						else
						{
							fLegalSwitchSymbol = (0 == String.CompareOrdinal(args[argNum].ToUpper(CultureInfo.InvariantCulture), 1, switchSymbols[n].ToUpper(CultureInfo.InvariantCulture), 0, switchSymbols[n].Length));
						}

						if (fLegalSwitchSymbol) break;
					}

					if (!fLegalSwitchSymbol)
					{
						// User specified an unrecognized switch, exit
						ss = SwitchStatus.Error;
						break;
					}
					else
					{
						// This is a legal switch, notified the derived class of this switch and its value
						ss = OnSwitch(caseSensitiveSwitches ? switchSymbols[n] : switchSymbols[n].ToLower(CultureInfo.InvariantCulture), args[argNum].Substring(1 + switchSymbols[n].Length));
					}
				}
				else
				{
					// This is not a switch, notified the derived class of this "non-switch value"
					ss = OnNonSwitch(args[argNum]);
				}
			}

			// Finished parsing arguments
			if (ss == SwitchStatus.NoError)
			{
				// No error occurred while parsing, let derived class perform a 
				// sanity check and return an appropraite status
				ss = OnDoneParse();
			}

			if (ss == SwitchStatus.ShowUsage)
			{
				// Status indicates that usage should be shown, show it
				OnUsage(null);
			}

			if (ss == SwitchStatus.Error)
			{
				// Status indicates that an error occurred, show it and the proper usage
				OnUsage((argNum == args.Length) ? null : args[argNum]);
			}

			// Return whether all parsing was sucessful.
			return (ss == SwitchStatus.NoError);
		}
	}
}

///////////////////////////////// End of File /////////////////////////////////
