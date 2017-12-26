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
public class Co1613ToString_int_static_dupl2
{
 public virtual bool runTest()
   {
   Console.Error.WriteLine( "Co1613ToString_int_static_dupl2 runTest started." );
   Console.Out.WriteLine ("");
   Console.Out.WriteLine ("Method_Covered: Integer4.ToString()");
   Console.Out.WriteLine ("Method_Covered: Integer4.ToString(int)");
   Console.Out.WriteLine ("Method_Count==2 (Co1613ToString_int_static_dupl2)");
   Console.Out.WriteLine ("");
   String strLoc="Loc_000oo";
   StringBuilder sblMsg = new StringBuilder( 99 );
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String str0Expected = null;
   String str2 = null;
   String str3 = null;
   int in4z = -2;
   Int32 int4a = -2;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="Loc_100ms";
       in4z = 3;  
       str0Expected = "3";
       int4a = (Int32)in4z;  
       str2 = int4a.ToString();
       str3 = in4z.ToString();
       ++iCountTestcases;
       if ( str0Expected.Equals( str2 ) == false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_101wo!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( str0Expected.Equals( str3 ) == false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_102he!  str3==" + str3  );
	 }
       strLoc="Loc_110ms";
       in4z = -3;  
       str0Expected = "-3";
       int4a = (Int32)in4z;  
       str2 = int4a.ToString();
       str3 = in4z.ToString();
       ++iCountTestcases;
       if ( str0Expected.Equals( str2 ) == false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_111wo!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( str0Expected.Equals( str3 ) == false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_112he!  str3==" + str3  );
	 }
       strLoc="Loc_120ms";
       in4z = -0;  
       str0Expected = "0";
       int4a = (Int32)in4z;  
       str2 = int4a.ToString();
       str3 = in4z.ToString();
       ++iCountTestcases;
       if ( str0Expected.Equals( str2 ) == false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_121wo!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( str0Expected.Equals( str3 ) == false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_122he!  str3==" + str3  );
	 }
       strLoc="Loc_130ms";
       in4z = Int32.MinValue;  
       str0Expected = "-2147483648";
       int4a = (Int32)in4z;  
       str2 = int4a.ToString();
       str3 = in4z.ToString();
       ++iCountTestcases;
       if ( str0Expected.Equals( str2 ) == false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_131wo!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( str0Expected.Equals( str3 ) == false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_132he!  str3==" + str3  );
	 }
       strLoc="Loc_140ms";
       in4z = Int32.MaxValue;  
       str0Expected = "2147483647";
       int4a =  (Int32)in4z;  
       str2 = int4a.ToString();
       str3 = in4z.ToString();
       ++iCountTestcases;
       if ( str0Expected.Equals( str2 ) == false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_141wo!  str2==" + str2  );
	 }
       ++iCountTestcases;
       if ( str0Expected.Equals( str3 ) == false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error Err_142he!  str3==" + str3  );
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++iCountErrors;
     Console.Error.WriteLine(  "POINTTOBREAK: Error Err_343un! (Co1613ToString_int_static_dupl2) exc_general==" + exc_general  );
     Console.Error.WriteLine(  "EXTENDEDINFO: (Err_343un) strLoc==" + strLoc  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   Integer4\\Co1613ToString_int_static_dupl2.cs   iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   Integer4\\Co1613ToString_int_static_dupl2.cs   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1613ToString_int_static_dupl2 cbA = new Co1613ToString_int_static_dupl2();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (Co1613ToString_int_static_dupl2) Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     {
     Console.Error.WriteLine(  "Co1613ToString_int_static_dupl2.cs   FAiL!"  );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
