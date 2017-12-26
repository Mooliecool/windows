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
public class Co1504get_DayOfYear
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1504get_DayOfYear  runTest() started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   DateTime dt2;
   int inDOY2 = -2;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="100bb";
       dt2 = new DateTime
	 (
	  1999  
	  ,2  
	  ,1  
	  ,23  
	  ,58  
	  ,59  
	  );
       inDOY2 = dt2.DayOfYear;
       ++iCountTestcases;
       if ( inDOY2 != 32 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_162hc!  inDOY2=="+ inDOY2  );
	 }
       strLoc="110cc";
       dt2 = new DateTime
	 (
	  1999  
	  ,1  
	  ,1  
	  ,00  
	  ,00  
	  ,00  
	  );
       inDOY2 = dt2.DayOfYear;
       ++iCountTestcases;
       if ( inDOY2 != 1 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_163wc!  inDOY2=="+ inDOY2  );
	 }
       strLoc="120dd";
       dt2 = new DateTime
	 (
	  2000  
	  ,12  
	  ,31  
	  ,23  
	  ,59  
	  ,59  
	  );
       inDOY2 = dt2.DayOfYear;
       ++iCountTestcases;
       if ( inDOY2 != 366 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_164rc!  inDOY2=="+ inDOY2  );
	 }
       strLoc="130ee";
       dt2 = new DateTime
	 (
	  2000  
	  ,2  
	  ,29  
	  ,11  
	  ,11  
	  ,11  
	  );
       inDOY2 = dt2.DayOfYear;
       ++iCountTestcases;
       if ( inDOY2 != 60 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_165tc!  inDOY2=="+ inDOY2  );
	 }
       strLoc="140ff";
       dt2 = new DateTime
	 (
	  2000  
	  ,2  
	  ,28  
	  ,11  
	  ,11  
	  ,11  
	  );
       inDOY2 = dt2.DayOfYear;
       ++iCountTestcases;
       if ( inDOY2 != 59 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_166yc!  inDOY2=="+ inDOY2  );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1504get_DayOfYear) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Co1504get_DayOfYear.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Co1504get_DayOfYear.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1504get_DayOfYear cbA = new Co1504get_DayOfYear();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1504get_DayOfYear) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "Co1504get_DayOfYear.cs   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
