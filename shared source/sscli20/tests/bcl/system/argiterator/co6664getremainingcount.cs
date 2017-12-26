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
using System.Reflection;
using System.Collections;
class NormClass
{
 public static void argit1( __arglist )
   {
   ArgIterator args = new ArgIterator( __arglist );
   int iCount = args.GetRemainingCount();
   for ( int i = 0; i < iCount + 15; i++ ){
   try	{
   try {
   TypedReference trTypRef =  args.GetNextArg();
   }
   catch (InvalidOperationException){}
   if (args.GetRemainingCount()!=0)
     if ( args.GetRemainingCount() != (iCount - i - 1) ){
     throw new Exception( "ExcErr5  ,Should have had remaining count " + (iCount - i - 1) + " but had remaining count " + args.GetRemainingCount() );
     }
   }
   catch(Exception ex){			
   if ( i < iCount )
     Console.WriteLine( "Err_argit4_00: Loop has not gone through only " + i + " arguments" );
   int iRemCount = args.GetRemainingCount();
   if ( iRemCount != 0 ){
   Console.WriteLine( "Err_argit4_01: Should have had remaining count 0 but had remaining count " + iRemCount );
   }
   throw ex;
   }
   }
   if ( args.GetRemainingCount() != 0 ){
   throw new Exception( "ExcErr4  ,Should have had remaining count 0");
   }
   }
}
public class Co6664GetRemainingCount
{
 static String strName = "System.ArgIterator";
 static String strTest = "Co6664GetRemainingCount";
 static String strPath = "";
 private Boolean runTest( Boolean verbose )
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   try
     {
     ++iCountTestcases;   		
     if ( verbose ) Console.WriteLine( "Make sure GetRemainingCount is correctly incremented decremented for different arglists" );	
     try {
     NormClass.argit1( __arglist( ) );			
     NormClass.argit1( __arglist( "a", "b", "c", "a", "b", "c", "a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c","a", "b", "c"  ) );			
     NormClass.argit1( __arglist( ) );
     NormClass.argit1( __arglist(typeof(System.String), null, (int) 1975, (long) 6, (float) 4.3, BindingFlags.NonPublic, new Object(), new Hashtable(), (char) 'a', (byte) 0x80, "Some String", Guid.NewGuid(), Int16.MinValue, DateTime.Now));
     NormClass.argit1( __arglist( ) );
     }
     catch (Exception ex) {	
     ++iCountErrors;
     Console.WriteLine( "Err_001a,  Unexpected exception was thrown ex: " + ex.ToString() );	
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
   Co6664GetRemainingCount oReplaceTest = new Co6664GetRemainingCount();
   Console.WriteLine( "Starting "+ strName +" ..." );                                               	
   Console.WriteLine( "USAGE:  Co6664GetRemainingCount.exe OR Co6664GetRemainingCount.exe verbose" );
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
