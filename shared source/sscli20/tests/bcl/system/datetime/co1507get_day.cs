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
public class Co1507get_Day
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1507get_Day  runTest() started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   DateTime dt2;
   int inDay4 = -2;
   int inDay5 = -3;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="100bb";
       inDay5 = 27;
       dt2 = new DateTime
	 (
	  1958  
	  ,11  
	  ,inDay5
	  ,11  
	  ,11  
	  ,11  
	  );
       inDay4 = dt2.Day;
       ++iCountTestcases;
       if ( inDay4 != inDay5 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_182hc!  inDay4=="+ inDay4  );
	 }
       strLoc="100bb";
       inDay5 = 29;
       dt2 = new DateTime
	 (
	  2000  
	  ,2  
	  ,inDay5
	  ,11  
	  ,11  
	  ,11  
	  );
       inDay4 = dt2.Day;
       ++iCountTestcases;
       if ( inDay4 != inDay5 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_183xc!  inDay4=="+ inDay4  );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1507get_Day) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Co1507get_Day.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Co1507get_Day.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1507get_Day cbA = new Co1507get_Day();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1507get_Day) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "Co1507get_Day.cs   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
