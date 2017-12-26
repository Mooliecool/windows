// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==
using System.IO;
using System.Text;
using System;
public class Co4216Parse
{
 internal static String strName = "Boolean.Parse";
 internal static String strTest = "";
 public virtual bool runTest()
   {
   int iCountTestcases = 0;
   int iCountErrors = 0; 
   Console.Error.WriteLine( strName + ": " + strTest+ " runTest started..." );
   String[] strTrues =
   {
     "true",
     "TRUE",
     "True",
     "  TRue",
     "TRUe  ",
     "  tRue",
     "tRUe                       ",
     "                     tRUE  ",
   };
   String[] strFalses =
   {
     "false",
     "FALSE",
     "  False",
     "FalsE  ",
     "  FaLsE",
     "FAlse  ",
     "  fALSe                     ",
     "                     faLSE  ",
   };
   try {
   for ( int ii = 0; ii < strTrues.Length; ++ii )
     {
     iCountTestcases++;
     if ( true != Boolean.Parse( strTrues[ii] ) )
       {
       Console.Error.Write( "Parse error (13w): " );
       Console.Error.Write( "Case = <" );
       Console.Error.Write( ii );
       Console.Error.Write( "> " );
       Console.Error.Write( "Expected Result = <" );
       Console.Error.Write( true );
       Console.Error.Write( "> " );
       Console.Error.Write( "Returned Result = <" );
       Console.Error.Write( Boolean.Parse( strTrues[ii] ) );
       Console.Error.WriteLine( "> " );
       iCountErrors++;
       }
     }
   }
   catch (Exception exc) {
   print ("E_gfy46");
   printexc (exc);
   }
   try {
   for ( int ii = 0; ii < strFalses.Length; ++ii )
     {
     iCountTestcases++;
     if ( false != Boolean.Parse( strFalses[ii] ) )
       {
       Console.Error.Write( "Parse error (25f): " );
       Console.Error.Write( "Case = <" );
       Console.Error.Write( ii );
       Console.Error.Write( "> " );
       Console.Error.Write( "Expected Result = <" );
       Console.Error.Write( false );
       Console.Error.Write( "> " );
       Console.Error.Write( "Returned Result = <" );
       Console.Error.Write( Boolean.Parse( strFalses[ii] ) );
       Console.Error.WriteLine( "> " );
       iCountErrors++;
       }
     }
   }
   catch (Exception exc) {
   print ("E_3y8");
   printexc (exc);
   }
   iCountTestcases++;
   try {
   if (Boolean.Parse( "Silly Invalid String") ) {
   iCountErrors++;
   print ("E_f287");
   }
   }
   catch (Exception ) {
   }
   iCountTestcases++;
   try
     {
     Boolean.Parse( null );
     iCountErrors++;
     print( "E_65v)" );
     }
   catch( Exception ) 
     {
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs. " + strTest + "   iCountTestCases == " + iCountTestcases);
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL. " + strTest + "    iCountErrors==" + iCountErrors);
     return false;
     }
   }
 private void printexc(Exception exc)
   {
   String output = "EXTENDEDINFO: "+exc.ToString();
   Console.Error.WriteLine(output);
   }
 private void print(String error)
   {
   StringBuilder output = new StringBuilder("POINTTOBREAK: find ");
   output.Append(error);
   output.Append(" (Cb4201Put.cs)");
   Console.Error.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co4216Parse oCbTest = new Co4216Parse();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch( Exception ex )
     {
     bResult = false;
     Console.Error.Write( strTest );
     Console.Error.WriteLine(  ": Main() Uncaught exception, FAiL!" );
     Console.Error.WriteLine( ex.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
} 
