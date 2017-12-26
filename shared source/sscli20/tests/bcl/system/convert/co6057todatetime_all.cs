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
using System.Globalization;
using System.Reflection;
using System.Collections;
interface IDescribeTestedMethods
{
 MemberInfo[] GetTestedMethods();
}
class Co6057ToDateTime_all : IDescribeTestedMethods
{
 public MemberInfo[] GetTestedMethods()
   {
   Type type = typeof(Convert);
   ArrayList list = new ArrayList();
   list.Add(type.GetMethod("ToDateTime", new Type[]{typeof(String)}));
   list.Add(type.GetMethod("ToDateTime", new Type[]{typeof(Object)}));
   list.Add(type.GetMethod("ToDateTime", new Type[]{typeof(String), typeof(IFormatProvider)}));
   list.Add(type.GetMethod("ToDateTime", new Type[]{typeof(Object), typeof(IFormatProvider)}));
   list.Add(type.GetMethod("ToDateTime", new Type[]{typeof(DateTime)}));
   list.Add(type.GetMethod("ToDateTime", new Type[]{typeof(Boolean)}));
   list.Add(type.GetMethod("ToDateTime", new Type[]{typeof(Char)}));
   list.Add(type.GetMethod("ToDateTime", new Type[]{typeof(Int16)}));
   list.Add(type.GetMethod("ToDateTime", new Type[]{typeof(Int32)}));
   list.Add(type.GetMethod("ToDateTime", new Type[]{typeof(Int64)}));
   list.Add(type.GetMethod("ToDateTime", new Type[]{typeof(UInt16)}));
   list.Add(type.GetMethod("ToDateTime", new Type[]{typeof(UInt32)}));
   list.Add(type.GetMethod("ToDateTime", new Type[]{typeof(UInt64)}));
   list.Add(type.GetMethod("ToDateTime", new Type[]{typeof(Single)}));
   list.Add(type.GetMethod("ToDateTime", new Type[]{typeof(Double)}));
   list.Add(type.GetMethod("ToDateTime", new Type[]{typeof(Decimal)}));
   MethodInfo[] methods = new MethodInfo[list.Count];
   list.CopyTo(methods, 0);
   return methods;
   }
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Convert.ToDateTime( all )";
 public static readonly String s_strTFName        = "Co6057ToDateTime_all";
 public static readonly String s_strTFAbbrev      = "Co6057";
 public static readonly String s_strTFPath        = "";
 public bool verbose = false;
 public void printoutCoveredMethods()
   {
   Console.Error.WriteLine( "" );
   MemberInfo[] mmm  = GetTestedMethods();
   Console.Error.WriteLine( "Method_Count== ("+ mmm.Length +"==confirm) !!" );
   Console.Error.WriteLine( "" );
   foreach(MemberInfo mm in mmm)
     Console.WriteLine(mm);
   Console.Error.WriteLine( "" );
   }
 public virtual Boolean runTest() {
 Console.Error.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source ver "+ s_strDtTmVer );
 String strLoc="Loc_000oo";
 int iCountTestcases = 0;
 int iCountErrors = 0;
 printoutCoveredMethods();
 if ( verbose ) Console.WriteLine( "Testing Method: String" );
 try {
 String expected = null;
 String[]   testValues = {
   new DateTime(1999,12,31,23,59,59).ToString(),
   new DateTime(100,1,1).ToLongDateString(),
   new DateTime(1492,2,29).ToLongDateString(),
   new DateTime(1,1,1).ToLongDateString(),
 };
 DateTime[]   expectedValues = {
   new DateTime( 1999,12,31, 23,59,59 ),
   new DateTime( 100,1,1, 0,0,0 ),
   new DateTime( 1492, 2, 29, 0,0,0 ),
   new DateTime( 1,1,1, 0,0,0 ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 DateTime result = Convert.ToDateTime( testValues[i] );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 Console.WriteLine("{0} - {1} - {2}", result.Ticks, expectedValues[i].Ticks, testValues[i]);
 iCountErrors++;
 strLoc = "Err_9456tdsfg," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_3947sg," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 expected = "System.ArgumentNullException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 DateTime result = Convert.ToDateTime( null );
 if(result!=new DateTime(0)){
 iCountErrors++;
 strLoc = "Err_EXstriArA2";
 Console.WriteLine( strLoc + " Exception not Thrown! "  + result);
 }
 } catch( ArgumentNullException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXstriArB2";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXstriArC2";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 expected = "System.FormatException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 DateTime result = Convert.ToDateTime( "null" );
 iCountErrors++;
 strLoc = "Err_EXstriArA9";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( FormatException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXstriArB1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXstriArC1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_948dy! Uncaught Exception in DateTime Convert.ToDateTime( String )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 if ( verbose ) Console.WriteLine( "Testing Method: String" );
 try {
 String expected = null;
 String[]   testValues = {
   "12/31/1999 11:59:59 PM",
   "0100/01/01 12:00:00 AM",
   "1492/02/29 12:00:00 AM",
   "0001/01/01 12:00:00 AM",
 };
 DateTime[]   expectedValues = {
   new DateTime( 1999,12,31, 23,59,59 ),
   new DateTime( 100,1,1, 0,0,0 ),
   new DateTime( 1492, 2, 29, 0,0,0 ),
   new DateTime( 1,1,1, 0,0,0 ),
 };
 for( int i = 0; i < testValues.Length; i++ ) {
 iCountTestcases++;
 if ( verbose ) Console.Write( "Testing " + testValues[i] + " ==> " );
 try {
 DateTime result = Convert.ToDateTime( testValues[i], new DateTimeFormatInfo() );
 if ( verbose ) Console.WriteLine( "'" + expectedValues[i] + "' == '" + result + "'" );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_04375tsf," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_347tgfdsf," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 expected = "System.ArgumentNullException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 DateTime result = Convert.ToDateTime( null , new DateTimeFormatInfo());
 if(result!=new DateTime(0)){
 iCountErrors++;
 strLoc = "Err_EXstriArA2";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 }
 } catch( ArgumentNullException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXstriArB2";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXstriArC2";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 expected = "System.FormatException";
 try {
 iCountTestcases++;
 if( verbose ) Console.WriteLine( " Exception Testing: " + expected );
 DateTime result = Convert.ToDateTime( "null" , new DateTimeFormatInfo());
 iCountErrors++;
 strLoc = "Err_EXstriArA9";
 Console.WriteLine( strLoc + " Exception not Thrown!" );
 } catch( FormatException e ) {
 if ( !e.GetType().FullName.Equals( expected ) ) {
 iCountErrors++;
 strLoc = "Err_EXstriArB1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_EXstriArC1";
 Console.WriteLine( strLoc + " Wrong Exception Thrown " + e.GetType().FullName );
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_948dy! Uncaught Exception in DateTime Convert.ToDateTime( String )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 strLoc = "Err_dew452,";
 try
   {
   iCountTestcases++;
   DateTime bTest = Convert.ToDateTime (new Object());	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 strLoc = "Err_dew452,";
 try
   {
   iCountTestcases++;
   DateTime bTest = Convert.ToDateTime (new Object(), new DateTimeFormatInfo());	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 try {
 DateTime[]   expectedValues = {
   new DateTime( 1999,12,31, 23,59,59 ),
   new DateTime( 100,1,1, 0,0,0 ),
   new DateTime( 1492, 2, 29, 0,0,0 ),
   new DateTime( 1,1,1, 0,0,0 ),
 };
 for( int i = 0; i < expectedValues.Length; i++ ) {
 iCountTestcases++;
 try {
 DateTime result = Convert.ToDateTime( expectedValues[i] );
 if ( !result.Equals( expectedValues[i] ) ) {
 iCountErrors++;
 strLoc = "Err_385rsfg," + i;
 Console.Error.WriteLine( strLoc + " Expected = '" + expectedValues[i] +
			  "' ... Received = '" +  result + "'." );
 }
 } catch( Exception e ) {
 iCountErrors++;
 strLoc = "Err_12075tsg," + i;
 Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
 }
 }
 } catch( Exception e ) {
 iCountErrors++;
 Console.WriteLine( "Error_948dy! Uncaught Exception in DateTime Convert.ToDateTime( DateTiem )");
 Console.WriteLine( "Exception->" + e.GetType().FullName );
 }
 strLoc = "Err_8sg,";
 try
   {
   iCountTestcases++;
   DateTime bTest = Convert.ToDateTime (false);	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 strLoc = "Err_8sg,";
 try
   {
   iCountTestcases++;
   DateTime bTest = Convert.ToDateTime ('a');	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 strLoc = "Err_8sg,";
 try
   {
   iCountTestcases++;
   DateTime bTest = Convert.ToDateTime ((short)5);	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 strLoc = "Err_8sg,";
 try
   {
   iCountTestcases++;
   DateTime bTest = Convert.ToDateTime (6);	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 strLoc = "Err_8sg,";
 try
   {
   iCountTestcases++;
   DateTime bTest = Convert.ToDateTime ((long)5);	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 strLoc = "Err_8sg,";
 try
   {
   iCountTestcases++;
   DateTime bTest = Convert.ToDateTime ((ushort)5);	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 strLoc = "Err_8sg,";
 try
   {
   iCountTestcases++;
   DateTime bTest = Convert.ToDateTime ((uint)5);	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 strLoc = "Err_8sg,";
 try
   {
   iCountTestcases++;
   DateTime bTest = Convert.ToDateTime ((ulong)5);	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 strLoc = "Err_8sg,";
 try
   {
   iCountTestcases++;
   DateTime bTest = Convert.ToDateTime (1.0f);	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 strLoc = "Err_8sg,";
 try
   {
   iCountTestcases++;
   DateTime bTest = Convert.ToDateTime (1.1);	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 strLoc = "Err_8sg,";
 try
   {
   iCountTestcases++;
   DateTime bTest = Convert.ToDateTime (1.0m);	
   Console.Error.WriteLine ( strLoc + " No Exception Thrown");
   }
 catch (InvalidCastException)
   {}
 catch (Exception e)
   {
   Console.WriteLine (e.ToString());
   Console.Error.WriteLine( strLoc + " Exception Thrown: " + e.GetType().FullName );
   }
 Console.Error.Write( s_strTFName );
 Console.Error.Write( ": " );
 if ( iCountErrors == 0 )
   {
   Console.Error.WriteLine( " iCountTestcases==" + iCountTestcases + " paSs" );
   return true;
   }
 else
   {
   Console.Error.WriteLine( s_strTFPath + s_strTFName + ".cs" );
   Console.Error.WriteLine( " iCountTestcases==" + iCountTestcases );
   Console.Error.WriteLine( "FAiL" );
   Console.Error.WriteLine( " iCountErrors==" + iCountErrors );
   return false;
   }
 }
 public static void Main( String[] args )
   {
   bool bResult = false; 
   Co6057ToDateTime_all cbX = new Co6057ToDateTime_all();
   try { if ( args[0].Equals( "-v" ) ) cbX.verbose = true; } catch( Exception) {}
   try
     {
     bResult = cbX.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.WriteLine( "FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main(), exc_main=="+ exc_main );
     }
   if ( ! bResult )
     {
     Console.WriteLine( s_strTFPath + s_strTFName );
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "Try 'cb6054ToByte_all.exe -v' to see tests..." );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.Error.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1; 
   }
}
