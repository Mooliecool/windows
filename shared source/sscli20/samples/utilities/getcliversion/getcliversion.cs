//------------------------------------------------------------------------------
// <copyright file="getcliversion.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

using System;
using DevService.Util;

public class GetCliVersion
{
	static public int Main( string[] clargs )
	{
		PEReader pereadr = null;
		int
			 aa
			,iReturnCode = 0;  // 8=Fine ,128>=Problem
		bool
			 bGetVersion = false
			,bOpenPereadr = false
			,bConfirmSet = false;  // Safety versus speed.
		string
			 sRequestedNewVersion = null
			,sGetNewVersion = null
			,sGottenVersion = null
			,sCliBinaryFileName = "UserError No such filename";

		// Edit the command line args.

		if ( clargs.Length < 1 )
		{
			iReturnCode += 128 + 256;
      Console.WriteLine();
			Console.WriteLine( "UserError_gar802:  Insufficient number of arguments." );
      Console.WriteLine( "File name is required.");
			GetCliVersion.PrintHelpUsage();
			goto LABEL_yog552_RETURN_LABEL;
		}
		else if ( clargs.Length > 5 )
		{
			iReturnCode += 128 + 256;
			Console.WriteLine( "UserError_gar713:  Max number of command line args is 3, yet 4 or more were passed in." );
			GetCliVersion.PrintHelpUsage();
			goto LABEL_yog552_RETURN_LABEL;
		}


		for ( aa=0 ;aa<clargs.Length ;aa++ )
		{
			if ( clargs[aa].ToLower().Substring( 0 ,1 ) != "/" )
			{
				if ( aa == clargs.Length - 1 )
				{
					sCliBinaryFileName = clargs[aa];
					continue;
				}
				else
				{
					Console.WriteLine( "UserError_gar833:  Param value unmatchable to a param /name.  Name of cil binary file must come last, if given." );
					GetCliVersion.PrintHelpUsage();
					goto LABEL_yog552_RETURN_LABEL;
				}
			}

			if ( clargs[aa].ToLower() == "/?" || clargs[aa].ToLower() == "/help" )
			{
				Console.WriteLine( "Here is the documentation you requested..." );
				GetCliVersion.PrintHelpUsage();
				goto LABEL_yog552_RETURN_LABEL;
			}
			else if ( clargs[aa].ToLower() == "/getver" )
			{
				bGetVersion = true;
			}
			else if ( clargs[aa].ToLower() == "/setver" )
			{
				++aa;
				if ( aa >= clargs.Length )
				{
					Console.WriteLine( "UserError_gar782:  /setver requires subsequent value param." );
					GetCliVersion.PrintHelpUsage();
					goto LABEL_yog552_RETURN_LABEL;
				}
				sRequestedNewVersion = clargs[aa];
			}
			else if ( clargs[aa].ToLower() == "/confirmset" )
			{
				++aa;
				if ( aa >= clargs.Length )
				{
					Console.WriteLine( "UserError_gar783:  /confirmset requires subsequent value param, true or false." );
					GetCliVersion.PrintHelpUsage();
					goto LABEL_yog552_RETURN_LABEL;
				}

				if ( clargs[aa].ToLower() == "true" )
				{
					bConfirmSet = true;
				}
				else if ( clargs[aa].ToLower() == "false" )
				{
					bConfirmSet = false;
				}
				else
				{
					Console.WriteLine( "UserError_gar784:  Value following /confirmset unrecognized.  Give true or false." );
					GetCliVersion.PrintHelpUsage();
					goto LABEL_yog552_RETURN_LABEL;
				}
			}
			else
			{
				iReturnCode += 128 + 256;
				Console.WriteLine( "UserError_gar624:  Unrecognized command line arg name at {0}: {1}." ,aa.ToString() ,clargs[aa] );
				GetCliVersion.PrintHelpUsage();
				goto LABEL_yog552_RETURN_LABEL;
			}
		}

		// Edit the command line results status from the loop.
		if ( bGetVersion )
		{
			if ( sRequestedNewVersion != null )
			{
				iReturnCode += 128 + 256;
				Console.WriteLine( "UserError_gab501:  Invalid command line arg combination." );
				goto LABEL_yog552_RETURN_LABEL;
			}
		}
		else  // No explicit GET.
		{
			if ( sRequestedNewVersion == null )
			{
				bGetVersion = true;  // Applying default when zero command line args.
			}
		}

		if ( sRequestedNewVersion == null )
		{
			if ( bConfirmSet )
			{
				iReturnCode += 128 + 256;
				Console.WriteLine( "UserError_gab478:  Invalid command line arg combination." );
				goto LABEL_yog552_RETURN_LABEL;
			}
		}
		else
		{
			sRequestedNewVersion = sRequestedNewVersion.Replace( "\0" ,"" ).Trim();
		}

		// Instantiate a PEReader object.

		try
		{
			pereadr = new PEReader( sCliBinaryFileName, !bGetVersion );
			bOpenPereadr = true;
		}
		catch ( Exception exc883 )
		{
			iReturnCode += 128 + 512;
			Console.WriteLine( "Error_vep535:  Unable to open PEReader, possible incorrect filename?  exc=..." );
			Console.WriteLine( exc883.ToString() );
			goto LABEL_yog552_RETURN_LABEL;
		}

		// Take the action.

		if ( bGetVersion )
		{
			sGottenVersion = pereadr.GetSignatureVersion().Replace( "\0" ,"" ).Trim();
			Console.WriteLine( "For {0}, existing CLI version found to be = {1}" ,sCliBinaryFileName ,sGottenVersion );
		}
		else  // sRequestedNewVersion cannot possibly be null in here.
		{
			pereadr.SetSignatureVersion( sRequestedNewVersion );

			if ( bConfirmSet )
			{
				// Close, reopen.
				pereadr.Close();  // Persisting change is needed before Get can confirmset the Set.
				bOpenPereadr = false;
				// Reopen.
				pereadr = new PEReader( sCliBinaryFileName , false );
				bOpenPereadr = true;

				sGetNewVersion = pereadr.GetSignatureVersion().Replace( "\0" ,"" ).Trim();
				if ( sRequestedNewVersion != sGetNewVersion )
				{
					iReturnCode += 128 + 1024;
					Console.WriteLine( "Error_vep433:  Attempt to confirmset that new Set matches the requested version found a discrepancy, possible program error.  NewVer = {0}." ,sGetNewVersion );
					goto LABEL_yog552_RETURN_LABEL;
				}
			}
			Console.WriteLine( "For {0}, new CLI version set to = {1}" ,sCliBinaryFileName ,sRequestedNewVersion );
		}


	LABEL_yog552_RETURN_LABEL:;

		if ( bOpenPereadr )
		{
			pereadr.Close();
			bOpenPereadr = false;
		}

		if ( iReturnCode < 128 )
			iReturnCode += 8;
		return iReturnCode;
	}


	static internal void PrintHelpUsage()
	{
		Console.WriteLine();
		Console.WriteLine( "Gets or sets the CLI runtime version (not Assembly version)");
    Console.WriteLine( "which the CLI will extract from the PE header of the CIL assembly." );
    Console.WriteLine();
		Console.WriteLine( "Name of CIL binary file to process must come last, if given." );
    Console.WriteLine( "This program cannot assess itself, instead copy it to junk.exe and" );
    Console.WriteLine( "use this sample on junk.exe." );
    Console.WriteLine();
		Console.WriteLine( "Command line argument usage:" );
		Console.WriteLine( "----------------------------" );
		Console.WriteLine( "/? or /help       Print out this usage help to the console." );
		Console.WriteLine();
		Console.WriteLine( "/getver           Print the CLI version of the binary CLI assembly." );
    Console.WriteLine( "                  Defaults to /getver when only name of CIL binary is given." );
		Console.WriteLine();
		Console.WriteLine( "/setver           Change the version inside the CIL assembly.  " );
    Console.WriteLine( "                  Version format uses only the first 3 of the 4 build ");
    Console.WriteLine( "                  version number nodes shown in example below.");
		Console.WriteLine();
		Console.WriteLine( "/confirmset       Values are TRUE and FALSE (case ignored), default is FALSE." );
		Console.WriteLine( "                  Controls whether the file is closed, reopened and the");
    Console.WriteLine( "                  version setting confirmed.");
    Console.WriteLine( " ----------------------------" );
    Console.WriteLine();
		Console.WriteLine( "EXAMPLE 1:   GetCliVersion.exe  My.exe" );
		Console.WriteLine( "EXAMPLE 2:   GetCliVersion.exe  /getver  My.exe" );
		Console.WriteLine( "EXAMPLE 3:   GetCliVersion.exe  /confirmset true  /setver v1.0.2804  Your.dll" );
		Console.WriteLine();
	}

	public static string GetCliVerFromPEHeader( string sCliBinaryFileName )
	{
		PEReader pereadr = null;
		string sGottenVersion = null;
		try
		{
			pereadr = new PEReader( sCliBinaryFileName , false );
		}
		catch ( Exception exc883 )
		{
			Console.WriteLine( "Error_vep535:  Unable to open PEReader, possible incorrect filename?  fileName=" + sCliBinaryFileName );
			Console.WriteLine( exc883.ToString() );
			goto LABEL_gcv552_RETURN_LABEL;
		}

		// Take cli version from PE header.

		sGottenVersion = pereadr.GetSignatureVersion().Replace( "\0" ,"" ).Trim();
		Console.WriteLine( "For {0}, existing CLI version found to be = {1}" ,sCliBinaryFileName ,sGottenVersion );
		pereadr.Close(); 
	LABEL_gcv552_RETURN_LABEL:
		return sGottenVersion;
	}

	public static Boolean SetCliVerInPEHeader( string sCliBinaryFileName, string sRequestedNewVersion )
	{
		PEReader pereadr = null;

		try
		{
			pereadr = new PEReader( sCliBinaryFileName , true );
		}
		catch ( Exception exc883 )
		{
			Console.WriteLine( "Error_vep535:  Unable to open PEReader, possible incorrect filename?  fileName=" + sCliBinaryFileName );
			Console.WriteLine( exc883.ToString() );
			return false;
		}

		pereadr.SetSignatureVersion( sRequestedNewVersion );
		pereadr.Close(); 

		Console.WriteLine( "For {0}, new CLI version set to = {1}" ,sCliBinaryFileName ,sRequestedNewVersion );
		return true;		
	}//SetCliVerInPEHeader()
} //GetCliVersion
