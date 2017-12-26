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
public class Co4214GetHashCode : Object
{
 internal static String strName = "Boolean.GetHashCode";
 internal static String strTest = "";
 public virtual bool runTest()
   {
   int iCountTestcases = 0;
   int iCountErrors = 0; 
   Console.Error.WriteLine( strName + ": " + strTest+ " runTest started..." );
   Boolean boolX = false;
   Boolean boolY = false;
   do
     {
     iCountTestcases++;
     boolX = true ;
     boolY = boolX;
     if ( boolX.Equals( boolY ) != true )
       {
       iCountErrors++; print ("E_bdu1");
       }
     if ( boolX.GetHashCode() != boolY.GetHashCode() )
       {
       iCountErrors++; print ("E_ik4");
       }
     iCountTestcases++;
     boolX =  true ;
     boolY =  false ;
     if ( boolX.Equals( boolY ) != false )
       {
       iCountErrors++; print ("E_j58");
       }
     if ( boolX.GetHashCode() == boolY.GetHashCode() )
       {
       iCountErrors++; print ("E_he8");
       }
     iCountTestcases++;
     if ( (boolX.GetHashCode() != 1) || (boolY.GetHashCode() != 0) )
       {
       iCountErrors++; print ("E_dh2");
       }
     }
   while (false);
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs. " + strTest + "   iCountTestCases == " + iCountTestcases);
     return true;
     }
   else
     {
     Console.Error.WriteLine( " FAiL : " + strTest + "    iCountErrors==" + iCountErrors);
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
   bool bResult = false;	
   Co4214GetHashCode oCbTest = new Co4214GetHashCode();
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
