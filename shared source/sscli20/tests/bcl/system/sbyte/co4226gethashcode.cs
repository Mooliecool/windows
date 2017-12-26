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
using System.Text;
public class Co4226GetHashCode
{
 static String strName = "SignedByte.GetHashCode";
 static String strTest = "Co4226GetHashCode";
 public Boolean runTest()
   {
   int iCountTestcases = 0;
   int iCountErrors = 0; 
   Console.Error.WriteLine( strName + ": " + strTest+ " runTest started..." );
   for ( SByte i = -10; i < 10; ++i )
     {
     iCountTestcases++;
     if ( i.GetHashCode() < 0 )
       {
       iCountErrors++; print ("E_vfr4");
       Console.Error.WriteLine (" SByte.GetHashCode() == " + i.GetHashCode()
				+ " which is less then zero");
       }
     for (SByte j = -10; j < i; ++j)
       {
       if (i.GetHashCode() == j.GetHashCode())
	 {
	 iCountErrors++; print ("E_vff4");
	 Console.Error.WriteLine (i + ".GetHashCode() == " + i.GetHashCode()
				  + " which is the same as " + j + ".GetHashCode() == " + j.GetHashCode());					
	 }
       }
     }
   SByte sTest = SByte.MinValue;
   if ( sTest.GetHashCode() < 0 )
     {
     iCountErrors++; print ("E_vvr4");
     Console.Error.WriteLine (" SByte.GetHashCode() == " + sTest.GetHashCode()
			      + " which is less then zero");
     }
   sTest = SByte.MaxValue;
   if ( sTest.GetHashCode() < 0 )
     {
     iCountErrors++; print ("E_vvr4");
     Console.Error.WriteLine (" SByte.GetHashCode() == " + sTest.GetHashCode()
			      + " which is less then zero");
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
   Boolean bResult = false;	
   Co4226GetHashCode oCbTest = new Co4226GetHashCode();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch( Exception ex )
     {
     bResult = false;
     Console.Error.Write( strTest );
     Console.Error.WriteLine(  ": Main() Uncaught exception" );
     Console.Error.WriteLine( ex.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
} 
