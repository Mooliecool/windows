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
 public void argit1( Object[] objExpectedArr, __arglist )
   {
   ArgIterator args = new ArgIterator( __arglist );
   int hashcode = args.GetHashCode();
   int iCount = args.GetRemainingCount();
   Object[] objArr = new Object[iCount];
   for ( int i = 0; i < iCount; i++ ){
   objArr[i] = TypedReference.ToObject(args.GetNextArg());
   if ( objExpectedArr[i] == null ){
   if ( objArr[i] != null ){
   throw new Exception( "argit1 - 1, __arglist[i] was null but it did not equal to objExpectedArr[i]" );
   }
   }
   else if ( ! objArr[i].Equals( objExpectedArr[i] ) ) {
   throw new Exception( "argit1 - 2, __arglist[i] was null but it did not equal to objExpectedArr[i]" );
   }
   }		
   ArgIterator args2 = new ArgIterator( __arglist );
   int iCount2 = args2.GetRemainingCount();
   Object[] objArr2 = new Object[iCount];
   for ( int i = 0; i < iCount2; i++ ){
   objArr2[i] = TypedReference.ToObject(args2.GetNextArg());
   if ( objExpectedArr[i] == null ){
   if ( objArr2[i] != null ){
   throw new Exception( "argit1 - 3, __arglist[i] was null but it did not equal to objExpectedArr[i]" );
   }
   }
   else if ( ! objArr2[i].Equals( objExpectedArr[i] ) ) {
   throw new Exception( "argit1 - 4, __arglist[i] was null but it did not equal to objExpectedArr[i]" );
   }
   }				
   int hashcodecompare= args.GetHashCode();
   if (! hashcode.Equals(hashcodecompare))
     {
     throw new Exception( "argit1 - 5, hashcode changed" );
     }
   }
 public static void argit2 (__arglist)
   {
   ArgIterator args = new ArgIterator( __arglist );	
   int hashcode = args.GetHashCode();
   try{
   NormClass.argit2b( __arglist(   ) );
   }
   catch (InvalidOperationException) {}
   int hashcodecompare= args.GetHashCode();
   if (! hashcode.Equals(hashcodecompare))
     {
     throw new Exception( "argit2 - 1, hashcode changed" );
     }
   }
 public static void argit2b (__arglist)
   {
   ArgIterator args = new ArgIterator( __arglist );
   int hashcode = args.GetHashCode();
   TypedReference trTypRef = args.GetNextArg();
   int hashcodecompare= args.GetHashCode();
   if (! hashcode.Equals(hashcodecompare))
     {
     throw new Exception( "argit2b - 1, hashcode changed" );
     }
   }
}
public class Co6665GetHashCode
{
 static String strName = "System.ArgIterator";
 static String strTest = "Co6665GetHashCode";
 static String strPath = "";
 private Boolean runTest( Boolean verbose )
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   try
     {
     ++iCountTestcases;   
     if ( verbose ) {
     Console.WriteLine( "checking GetHashCode returns the same value ");
     Console.WriteLine( "checking if we can use ArgIterators on any class" );	
     }
     try	{
     NormClass st = new NormClass();
     Object[] o     = new Object[] { typeof(System.String), null, (int) 1975, (long) 6, (float) 4.3, BindingFlags.NonPublic, new Object(), new Hashtable(), (char) 'a', (byte) 0x80, "Some String", Guid.NewGuid(), Int16.MinValue, DateTime.Now };
     st.argit1( o, __arglist( (Type) o[0], o[1], (int) o[2], (long) o[3], (float) o[4], (BindingFlags) o[5], o[6], (Hashtable) o[7], (char) o[8],  (byte) o[9], (String) o[10], (Guid) o[11], (short) o[12], (DateTime) o[13] ) );
     }
     catch (Exception ex) {
     ++iCountErrors;
     Console.WriteLine( "Err_001a,  Unexpected exception was thrown ex: " + ex.ToString() );
     }
     ++iCountTestcases;   
     if ( verbose ){
     Console.WriteLine( "checking GetHashCode returns the same value ");
     Console.WriteLine( "checking ArgIterators with Arrays" );	
     }
     try	{
     NormClass st = new NormClass();
     Object[,] compObj = new Object[2,2];
     Object[] o = new Object[] {  new DBNull[]{}, new Object[]{}, compObj };
     st.argit1( o, __arglist(    (DBNull[]) o[0], 
				 (Object[]) o[1], 
				 (Object[,]) o[2] ) );
     }
     catch (Exception ex) {
     ++iCountErrors;
     Console.WriteLine( "Err_001b,  Unexpected exception was thrown ex: " + ex.ToString() );
     }
     ++iCountTestcases;   
     if ( verbose ){
     Console.WriteLine( "checking GetHashCode returns the same value ");
     Console.WriteLine( "check nesting of ArgIterators " );	
     }
     try	{
     NormClass.argit2( __arglist( "a", "b", "c" ) );				
     }
     catch (Exception ex) {
     ++iCountErrors;
     Console.WriteLine( "Err_002a,  Unexpected exception was thrown ex: " + ex.ToString() );
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
   Co6665GetHashCode oReplaceTest = new Co6665GetHashCode();
   Console.WriteLine( "Starting "+ strName +" ..." );                                               	
   Console.WriteLine( "USAGE:  Co6665GetHashCode.exe OR Co6665GetHashCode.exe verbose" );
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
