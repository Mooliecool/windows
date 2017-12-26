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
public class Co1592Clone
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Array.Clone()";
 public static readonly String s_strTFName        = "Co1592Clone";
 public static readonly String s_strTFAbbrev      = "Co1592";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine( s_strTFPath +" "+ s_strTFName +" ,for "+ s_strClassMethod +"  ,Source ver "+ s_strDtTmVer );
   String strLoc="Loc_000oo";
   String strBaseLoc;
   StringBuilder sblMsg = new StringBuilder( 99 );
   int inCountErrors = 0;
   int inErrorBits = 0;
   int inCountTestcases = 0;
   int[] in4Arr1Orig = null;
   int[] in4Arr1Newer = null;
   Object[] objArr1Orig = null;
   Object[] objArr1Newer = null;
   String[] strArr1Orig = null;
   String[] strArr1Newer = null;
   try
     {
     LABEL_860_GENERAL:
     do
       {
       strLoc="Loc_110dt";
       in4Arr1Orig = new int[3];
       for ( int ia = 0 ;ia < in4Arr1Orig.Length ;ia++ )
	 {
	 in4Arr1Orig[ia] = ia + 100;  
	 }
       in4Arr1Newer = (int[])in4Arr1Orig.Clone();
       ++inCountTestcases;
       if ( in4Arr1Orig == in4Arr1Newer )
	 {
	 ++inCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_408rh!" );
	 }
       ++inCountTestcases;
       for ( int ia = 0 ;ia < in4Arr1Orig.Length;ia++ )
	 {
	 if ( in4Arr1Orig[ia] != in4Arr1Newer[ia] )
	   {
	   ++inCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_855bx!  ia=="+ ia +" ,in4Arr1Newer[ia]=="+ in4Arr1Newer[ia] );
	   }
	 if ( (inErrorBits & 0x1) != 0 )
	   {
	   break;
	   }
	 else
	   {
	   inErrorBits |= 0x1;
	   }
	 }
       strLoc="Loc_113ek";
       in4Arr1Orig = new int[0];
       for ( int ia = 0 ;ia < in4Arr1Orig.Length;ia++ )
	 {
	 in4Arr1Orig[ia] = ia;
	 }
       in4Arr1Newer = (int[])in4Arr1Orig.Clone();
       ++inCountTestcases;
       if ( in4Arr1Orig == in4Arr1Newer )
	 {
	 ++inCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_265wv!" );
	 }
       ++inCountTestcases;
       for ( int ia = 0 ;ia < in4Arr1Newer.Length; ia++ )  
	 {
	 if ( in4Arr1Orig[ia] != in4Arr1Newer[ia] )
	   {
	   ++inCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_773bf!  ia=="+ ia +" ,in4Arr1Newer[ia]=="+ in4Arr1Newer[ia] );
	   }
	 if ( (inErrorBits & 0x8) != 0 )
	   {
	   break;
	   }
	 else
	   {
	   inErrorBits |= 0x8;
	   }
	 }
       strLoc="Loc_120yt";
       objArr1Orig = new Object[3];
       for ( int ia = 0 ;ia < objArr1Orig.Length;ia++ )
	 {
	 objArr1Orig[ia] = ia;
	 }
       objArr1Newer = (Object[])objArr1Orig.Clone();
       ++inCountTestcases;
       if ( objArr1Orig == objArr1Newer )
	 {
	 ++inCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_816os!" );
	 }
       ++inCountTestcases;
       for ( int ia = 0 ;ia < objArr1Orig.Length;ia++ )
	 {
	 if ( objArr1Orig[ia] != objArr1Newer[ia] )
	   {
	   ++inCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_004mr!  ia=="+ ia +" ,objArr1Newer[ia]=="+ objArr1Newer[ia] );
	   }
	 if ( (inErrorBits & 0x2) != 0 )
	   {
	   break;
	   }
	 else
	   {
	   inErrorBits |= 0x2;
	   }
	 }
       strLoc="Loc_142gr";
       strArr1Orig = new String[3];
       for ( int ia = 0 ;ia < strArr1Orig.Length; ia++ )
	 {
	 strArr1Orig[ia] = ia.ToString();
	 }
       strArr1Newer = (String[])strArr1Orig.Clone();
       ++inCountTestcases;
       if ( strArr1Orig == strArr1Newer )
	 {
	 ++inCountErrors;
	 Console.WriteLine( s_strTFAbbrev +"Err_167qi!" );
	 }
       ++inCountTestcases;
       for ( int ia = 0 ;ia < strArr1Orig.Length; ia++ )
	 {
	 if ( strArr1Orig[ia].Equals( strArr1Newer[ia] ) != true )
	   {
	   ++inCountErrors;
	   Console.WriteLine( s_strTFAbbrev +"Err_456kz!  ia=="+ ia +" ,strArr1Newer[ia]=="+ strArr1Newer[ia] );
	   }
	 if ( (inErrorBits & 0x4) != 0 )
	   {
	   break;
	   }
	 else
	   {
	   inErrorBits |= 0x4;
	   }
	 }
       } while ( false );
     }
   catch( Exception exc_general )
     {
     ++inCountErrors;
     Console.WriteLine( s_strTFAbbrev +"Error Err_8888yyy!  strLoc=="+ strLoc +" ,exc_general=="+ exc_general );
     }
   if ( inCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   "+ s_strTFPath +" "+ s_strTFName +"  ,inCountTestcases=="+ inCountTestcases );
     return true;
     }
   else
     {
     Console.Error.WriteLine( "FAiL!   "+ s_strTFPath +" "+ s_strTFName +"  ,inCountErrors=="+ inCountErrors +" ,BugNums?: "+ s_strActiveBugNums );
     return false;
     }
   } 
 public static void Main( String[] args )
   {
   bool bResult = false; 
   StringBuilder sblMsg = new StringBuilder( 99 );
   Co1592Clone cbA = new Co1592Clone();
   try
     {
     bResult = cbA.runTest();
     }
   catch ( Exception exc_main )
     {
     bResult = false;
     Console.WriteLine( s_strTFAbbrev +"FAiL!  Error Err_9999zzz!  Uncaught Exception caught in main(), exc_main=="+ exc_main );
     }
   if ( ! bResult )
     {
     Console.WriteLine( s_strTFName +s_strTFPath );
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev );  
     Console.Error.WriteLine( " " );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11; 
   }
} 
