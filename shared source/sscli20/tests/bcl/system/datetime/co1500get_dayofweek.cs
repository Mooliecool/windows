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
public class Co1500get_DayOfWeek
{
 public const int Sunday = 0;
 public const int Monday = 1;
 public const int Tuesday = 2;
 public const int Wednesday = 3;
 public const int Thursday = 4;
 public const int Friday = 5;
 public const int Saturday = 6;
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1500get_DayOfWeek  runTest() started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   DateTime dt2;
   int inDOW3 = -2;
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
	  ,7  
	  ,23  
	  ,58  
	  ,59  
	  );
       inDOW3 = (int)dt2.DayOfWeek;
       ++iCountTestcases;
       if ( inDOW3 != Sunday )  
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_172hc!  inDOW3=="+ inDOW3  );
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
       inDOW3 = (int) dt2.DayOfWeek;
       ++iCountTestcases;
       if ( inDOW3 != Friday )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_173wc!  inDOW3=="+ inDOW3  );
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
       inDOW3 = (int) dt2.DayOfWeek;
       ++iCountTestcases;
       if ( inDOW3 != Sunday )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_174rc!  inDOW3=="+ inDOW3  );
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
       inDOW3 = (int) dt2.DayOfWeek;
       ++iCountTestcases;
       if ( inDOW3 != Tuesday )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_175tc!  inDOW3=="+ inDOW3  );
	 }
       strLoc="200xx";
       dt2 = new DateTime
	 (
	  2000  
	  ,1  
	  ,1  
	  ,11  
	  ,11  
	  ,11  
	  );
       strLoc="200su";
       dt2 = dt2.AddDays( (double)1 );  
       inDOW3 = (int) dt2.DayOfWeek;
       ++iCountTestcases;
       if ( inDOW3 != Sunday )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_300su!  inDOW3=="+ inDOW3  );
	 }
       strLoc="201mo";
       dt2 = dt2.AddDays( (double)1 );  
       inDOW3 = (int) dt2.DayOfWeek;
       ++iCountTestcases;
       if ( inDOW3 != Monday )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_301mo!  inDOW3=="+ inDOW3  );
	 }
       strLoc="202tu";
       dt2 = dt2.AddDays( (double)1 );  
       inDOW3 = (int) dt2.DayOfWeek;
       ++iCountTestcases;
       if ( inDOW3 != Tuesday )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_302tu!  inDOW3=="+ inDOW3  );
	 }
       strLoc="203we";
       dt2 = dt2.AddDays( (double)1 );  
       inDOW3 = (int) dt2.DayOfWeek;
       ++iCountTestcases;
       if ( inDOW3 != Wednesday )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_303we!  inDOW3=="+ inDOW3  );
	 }
       strLoc="204th";
       dt2 = dt2.AddDays( (double)1 );  
       inDOW3 = (int) dt2.DayOfWeek;
       ++iCountTestcases;
       if ( inDOW3 != Thursday )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_304th!  inDOW3=="+ inDOW3  );
	 }
       strLoc="205fr";
       dt2 = dt2.AddDays( (double)1 );  
       inDOW3 = (int) dt2.DayOfWeek;
       ++iCountTestcases;
       if ( inDOW3 != Friday )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_305fr!  inDOW3=="+ inDOW3  );
	 }
       strLoc="206sa";
       dt2 = dt2.AddDays( (double)1 );  
       inDOW3 = (int) dt2.DayOfWeek;
       ++iCountTestcases;
       if ( inDOW3 != Saturday )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_306sa!  inDOW3=="+ inDOW3  );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1500get_DayOfWeek) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Co1500get_DayOfWeek.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Co1500get_DayOfWeek.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1500get_DayOfWeek cbA = new Co1500get_DayOfWeek();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1500get_DayOfWeek) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "Co1500get_DayOfWeek.cs   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
