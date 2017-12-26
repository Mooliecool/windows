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
public class Co1256Append_Float
{
 public static String s_strActiveBugNums = "";
 public static String s_strTFName        = "Co1256Append_Float.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 internal static String strName = "StringBuilder.Append_Float";
 internal static String strTest = "Co1256Append_Float";
 internal static String strPath = "";
 public virtual bool runTest()
   {
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Console.Out.Write( strName );
   Console.Out.Write( ": " );
   Console.Out.Write( strTest );
   Console.Out.WriteLine( " runTest started..." );
   StringBuilder sbwrMyBuffer = null;
   StringBuilder sbwrMyResult = null;
   String strResult = null;
   String strFltResult = null;
   StringBuilder sbwrAlpha = null;
   String strAlpha = null;
   String strWork = null;
   sbwrMyBuffer = new StringBuilder( "My buffer" );
   strResult =  "My buffer1" ;
   strFltResult =  "My buffer1" ;
   iCountTestcases++;
   Console.Out.WriteLine( "Append float value" );
   sbwrMyBuffer = new StringBuilder( "My buffer" );
   sbwrMyBuffer.Append( (float) 1.0f );  
   if ( sbwrMyBuffer.ToString().Equals( strFltResult ) != true )
     {
     String strInfo =  "Append error: " ;
     strInfo += "Expected Result = <" + strFltResult + "> ";
     strInfo += "Returned Result = <" + sbwrMyBuffer.ToString() + "> ";
     Console.WriteLine( strTest+ "E_202" );
     Console.WriteLine( strTest+ strInfo );
     ++iCountErrors;
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public static void Main( String[] args )
   {
   bool bResult = false;	
   Co1256Append_Float oCbTest = new Co1256Append_Float();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch( Exception ex )
     {
     bResult = false;
     Console.WriteLine( strTest+ strPath );
     Console.WriteLine( strTest+ "E_1000000" );
     Console.WriteLine( strTest+ "FAiL: Uncaught exception detected in Main()" );
     Console.WriteLine( strTest+ ex.ToString() );
     }
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
} 
