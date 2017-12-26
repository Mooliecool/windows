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
public class Co4223Equals
{
 static String strName = "Byte.Equals";
 static String strTest = "Co4223Equals";
 public Boolean runTest()
   {
   int iCountTestcases = 0;
   int iCountErrors = 0; 
   Console.Error.WriteLine( strName + ": " + strTest+ " runTest started..." );
   SByte[] SByteValues = 
   {
     (SByte)2, (SByte)(-2), (SByte)0, (SByte)(-0), unchecked((SByte)0xFF),
     SByte.MaxValue,
     SByte.MinValue
   };
   SByte[] SByteCheckValues = 
   {
     (SByte)2, (SByte)(-2), (SByte)0, (SByte)(-0), unchecked((SByte)0xFF),
     SByte.MaxValue,
     SByte.MinValue
   };
   SByte SByteObj = (SByte)0;
   SByte SByteCheckObj = (SByte)0;
   for ( int ii = 0; ii < SByteValues.Length; ++ii )
     {
     iCountTestcases++;
     SByteObj =  SByteValues[ii];
     SByteCheckObj = SByteCheckValues[ii] ;
     if ( SByteObj.Equals( SByteCheckObj ) != true )
       {
       iCountErrors++; print ("E_vfr4");
       }
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
   Co4223Equals oCbTest = new Co4223Equals();
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
