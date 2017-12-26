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
public class Co4385GetUpperBound
{
 public virtual bool runTest()
   {
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int inCountErrors = 0;
   int inCountTestcases = 0;
   Array genarr2 = null;
   int[] in4aArr = null;
   int in4b = -2;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       in4aArr = new int[9];
       genarr2 = (Array)in4aArr;
       in4b = genarr2.GetUpperBound( 0 );  
       ++inCountTestcases;
       if ( in4b != 8 )
	 {
	 ++inCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_143jw!  in4b=="+ in4b  );
	 }
       in4aArr = new int[9];
       genarr2 = (Array)in4aArr;
       try
	 {
	 ++inCountTestcases;
	 in4b = genarr2.GetUpperBound( -2 );
	 ++inCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_262sx!  in4b=="+ in4b  );
	 }
       catch ( IndexOutOfRangeException iorexc ) {}
       catch ( Exception excep )
	 {
	 ++inCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_262dc!  excep=="+ excep  );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++inCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co4385GetUpperBound) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     }
   if ( inCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Co4385GetUpperBound   inCountTestcases==" + inCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Co4385GetUpperBound   inCountErrors==" + inCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co4385GetUpperBound cbA = new Co4385GetUpperBound();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co4385GetUpperBound) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "PATHTOSOURCE:  Co4385GetUpperBound   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
}
