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
class NormClass
{
 public static void argit1 (__arglist)
   {
   ArgIterator args = new ArgIterator( __arglist );
   Boolean b= args.Equals(new Object());
   }
}
public class Co6663Equals
{
 static String strName = "System.ArgIterator";
 static String strTest = "Co6663Equals";
 static String strPath = "";
 private Boolean runTest( Boolean verbose )
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   try
     {
     ++iCountTestcases;   
     if ( verbose ) Console.WriteLine( "checking if ArgIterator.Equals throws the NotSupportedException" );	
     try	{
     NormClass.argit1( __arglist(   ) );				
     }
     catch (NotSupportedException) {}
     catch (Exception ex) {
     ++iCountErrors;
     Console.WriteLine( "Err_008a,  Unexpected exception was thrown ex: " + ex.ToString() );
     }			
     }
   catch (Exception exc_runTest)
     {
     ++iCountErrors;			
     Console.Error.WriteLine (strName+" "+strTest+" "+strPath);
     Console.Error.WriteLine ("Unexpected Exception (runTest99): "+exc_runTest.ToString());
     }
   Console.WriteLine ();
   Console.WriteLine ("FINAL TEST RESULT:" + strName+" "+strTest+" "+strPath);
   Console.WriteLine ();
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false;	
   Co6663Equals oReplaceTest = new Co6663Equals();
   Console.WriteLine( "Starting "+ strName +" ..." );                                               	
   Console.WriteLine( "USAGE:  Co6663Equals.exe OR Co6663Equals.exe verbose" );
   Boolean verbose=false;
   try
     {			
     if ( args.Length > 0 ) {
     Console.WriteLine( "Verbose ON!" );
     verbose = true;
     }
     Console.WriteLine();                                                                        
     bResult = oReplaceTest.runTest(verbose);				
     }
   catch ( Exception exc_main ) {
   bResult = false;
   Console.Error.WriteLine (strName+" "+strTest+" "+strPath);
   Console.Error.WriteLine ("Unexpected Exception (Main99): "+exc_main.ToString());
   }
   if ( ! bResult )
     Console.Error.WriteLine(  "PATHTOSOURCE: (FAIL) " + strPath + strTest );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
}
