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
public class Co3452IsPositiveInfinity
{
 internal static String strName = "Double.IsPositiveInfinity";
 internal static String strTest = "Co3452IsPositiveInfinity.cs";
 internal static String strPath = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc="123_er";
   String strInfo = null;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strPath + strTest );
   Console.Out.WriteLine( " runTest started..." );
   Double dbl1;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       dbl1 = Double.PositiveInfinity;
       ++iCountTestcases;
       if ( Double.IsPositiveInfinity(dbl1) != true )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_972qr!  Double.IsPositiveInfinity(dbl1)==" + Double.IsPositiveInfinity(dbl1)  );
	 }
       dbl1 = (double)5.0/(double)0.0;
       ++iCountTestcases;
       if ( Double.IsPositiveInfinity(dbl1) != true )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_034jr!  Double.IsPositiveInfinity(dbl1)==" + Double.IsPositiveInfinity(dbl1)  );
	 }
       dbl1 = Double.NaN;
       ++iCountTestcases;
       if ( Double.IsPositiveInfinity(dbl1) != false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_023ml!  Double.IsPositiveInfinity(dbl1)==" + Double.IsPositiveInfinity(dbl1)  );
	 }
       dbl1 = Double.NegativeInfinity;
       ++iCountTestcases;
       if ( Double.IsPositiveInfinity(dbl1) != false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_925mi!  Double.IsPositiveInfinity(dbl1)==" + Double.IsPositiveInfinity(dbl1)  );
	 }
       dbl1 = (Single)0;
       ++iCountTestcases;
       if ( Double.IsPositiveInfinity(dbl1) != false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_076gd!  Double.IsPositiveInfinity(dbl1)==" + Double.IsPositiveInfinity(dbl1)  );
	 }
       dbl1 = Double.MaxValue;
       ++iCountTestcases;
       if ( Double.IsPositiveInfinity(dbl1) != false )
	 {
	 ++iCountErrors;
	 Console.Error.WriteLine(  "POINTTOBREAK: Error E_076gd!  Double.IsPositiveInfinity(dbl1)==" + Double.IsPositiveInfinity(dbl1)  );
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
   Co3452IsPositiveInfinity oCbTest = new Co3452IsPositiveInfinity();
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
