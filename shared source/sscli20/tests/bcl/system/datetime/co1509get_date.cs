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
public class Co1509get_Date
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1509get_Date  runTest() started." );
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   DateTime dt2;
   DateTime dt3;
   bool bo4 = false;
   int in4a = -2;
   int in4SetYear = 1958;
   int in4SetMonth = 11;
   int in4SetDayOfMonth = 27;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="100bb";
       in4SetYear = 1958;
       in4SetMonth = 11;
       in4SetDayOfMonth = 27;
       dt2 = new DateTime
	 (
	  in4SetYear
	  ,in4SetMonth
	  ,in4SetDayOfMonth
	  ,00  
	  ,00  
	  ,00  
	  );
       dt3 = dt2.Date;
       bo4 = DateTime.Equals( dt2 ,dt3 );
       ++iCountTestcases;
       if ( bo4 != true )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_410yk!  dt3.ToString()=="+ dt3.ToString()  );
	 }
       in4a = dt3.Year;
       ++iCountTestcases;
       if ( in4a != in4SetYear )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_521sp!  in4a=="+ in4a  );
	 }
       in4a = dt3.Month;
       ++iCountTestcases;
       if ( in4a != in4SetMonth )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_522id!  in4a=="+ in4a  );
	 }
       in4a = dt3.Day;
       ++iCountTestcases;
       if ( in4a != in4SetDayOfMonth )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_523rw!  in4a=="+ in4a  );
	 }
       in4a = dt3.Hour;
       ++iCountTestcases;
       if ( in4a != 0 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_531mn!  in4a=="+ in4a  );
	 }
       in4a = dt3.Minute;
       ++iCountTestcases;
       if ( in4a != 0 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_532za!  in4a=="+ in4a  );
	 }
       in4a = dt3. Second;
       ++iCountTestcases;
       if ( in4a != 0 )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_533vq!  in4a=="+ in4a  );
	 }
       strLoc="101bc";
       dt2 = dt2.AddSeconds( (double)1 );  
       dt3 = dt2.Date;
       bo4 = DateTime.Equals( dt2 ,dt3 );
       ++iCountTestcases;
       if ( bo4 != false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Err_410yk!  dt3.ToString()=="+ dt3.ToString()  );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1509get_Date) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Co1509get_Date.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Co1509get_Date.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1509get_Date cbA = new Co1509get_Date();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1509get_Date) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "Co1509get_Date.cs   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
