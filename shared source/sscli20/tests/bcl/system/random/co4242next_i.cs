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
public class Co4242Next_i
{
 internal static String strName = "Random.Co4242Next_i";
 internal static String strTest = "Co4242Next_i";
 public virtual bool runTest()
   {
   int iCountTestcases = 0;
   int iCountErrors = 0; 
   Console.Error.WriteLine( strName + ": " + strTest+ " runTest started..." );
   Random rdm = null;
   String strLoc = null;
   int retVal;
   iCountTestcases++;
   rdm = null;
   try {
   rdm.Next(10);
   }
   catch (NullReferenceException ) {}
   rdm = new Random();
   Random random = new Random();
   for(int i=0;i<20;i++)
     {
     try 
       {
       iCountTestcases++; strLoc = "L_gkjhe";
       retVal = rdm.Next(random.Next(-1000,0));
       iCountErrors++;
       }
     catch (ArgumentOutOfRangeException) 
       { 
       }
     catch (Exception e)
       {
       print (strLoc); printexc (e);
       iCountErrors++;
       }
     }
   try 
     {
     iCountTestcases++; strLoc = "L_gkjhe";
     retVal = rdm.Next(Int32.MinValue);
     iCountErrors++;
     }
   catch (ArgumentOutOfRangeException) 
     { 
     }
   catch (Exception e)
     {
     print (strLoc); printexc (e);
     iCountErrors++;
     }
   rdm = new Random();
   for (int i = 0; i < 100; i++) {
   iCountTestcases++;
   retVal = rdm.Next(150);
   if ( retVal > 150) {
   iCountErrors++;
   print ("E_3287");
   }
   }
   retVal = rdm.Next(Int32.MaxValue);
   if(retVal<0)
     {
     iCountErrors++;
     Console.WriteLine("Err_348tsfg! Unexpected value returned");
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
   output.Append(" (" + strTest + ")");
   Console.Error.WriteLine(output.ToString());
   }
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co4242Next_i oCbTest = new Co4242Next_i();
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
