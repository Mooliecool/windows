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
using System;
public class Co3450IsNegativeInfinity
{
 internal static String strName = "Single.IsNegativeInfinity";
 internal static String strTest = "Co3450IsNegativeInfinity.cs";
 internal static String strPath = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   String strInfo = null;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.WriteLine( strPath + strTest );
   Console.WriteLine( "Locale insensitive" );
   Console.Out.WriteLine( " runTest started..." );
   Single sgl1;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       sgl1 = Single.PositiveInfinity;
       ++iCountTestcases;
       if ( Single.IsNegativeInfinity(sgl1) != false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr!  Single.IsNegativeInfinity(sgl1)==" + Single.IsNegativeInfinity(sgl1)  );
	 }
       sgl1 = Single.NaN;
       ++iCountTestcases;
       if ( Single.IsNegativeInfinity(sgl1) != false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_023ml!  Single.IsNegativeInfinity(sgl1)==" + Single.IsNegativeInfinity(sgl1)  );
	 }
       sgl1 = Single.NegativeInfinity;
       ++iCountTestcases;
       if ( Single.IsNegativeInfinity(sgl1) != true )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_925mi!  Single.IsNegativeInfinity(sgl1)==" + Single.IsNegativeInfinity(sgl1)  );
	 }
       sgl1 = (float)-5.0/(float)0.0;
       ++iCountTestcases;
       if ( Single.IsNegativeInfinity(sgl1) != true )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_034jr!  Single.IsNegativeInfinity(sgl1)==" + Single.IsNegativeInfinity(sgl1)  );
	 }
       sgl1 = (Single)0;
       ++iCountTestcases;
       if ( Single.IsNegativeInfinity(sgl1) != false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_076gd!  Single.IsNegativeInfinity(sgl1)==" + Single.IsNegativeInfinity(sgl1)  );
	 }
       } while ( false );
     }
   catch (Exception exc_general)
     {
     ++iCountErrors;
     Console.Error.WriteLine ( "POINTTOBREAK: Error Err_103! strLoc=="+ strLoc + " ,exc_general=="+exc_general  );
     }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs. " + strPath + strTest + "  iCountTestcases==" + iCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL! " + strPath + strTest + "   iCountErrors==" + iCountErrors );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co3450IsNegativeInfinity oCbTest = new Co3450IsNegativeInfinity();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.Error.WriteLine(  "POINTTOBREAK:  FAiL!  Error Err_999zzz! (" + strTest + ") Uncaught Exception caught in main(), exc_main==" + exc_main  );
     }
   if ( ! bResult )
     Console.Error.WriteLine(  "PATHTOSOURCE:  " + strPath + strTest + "   FAiL!"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
