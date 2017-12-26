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
public class Co1252Append_string
{
 public static String s_strActiveBugNums = "";
 public static String s_strTFName        = "Co1252Append_string.csc";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);    	
 public virtual bool runTest()
   {
   int iCountErrors = 0; 
   int iCountTestcases = 0;
   System.String strResult = "StringBuilder.Append: Co1252Append_string iCountErrors==";
   System.String strCheckpoint = "CHECKPOINT";
   System.Console.Out.WriteLine( "StringBuilder.Append: Co1252Append_string runTest running..." );
   StringBuilder sbwrMyBuffer = null;
   StringBuilder sbwrMyResult = null;
   StringBuilder sbwrAlpha = null;
   System.String strAlpha = null;
   System.String strWork = null;
   StringBuilder sbwrStrBuff2 = null;
   StringBuilder sbwrStrBuff3 = null;
   sbwrMyResult = new StringBuilder( "My bufferabcdefghijklmnopqrstuvwxyz" );
   iCountTestcases++;
   if ( sbwrMyResult == null )
     {
     iCountErrors++;
     return false;
     }
   iCountTestcases++;
   sbwrMyBuffer = new StringBuilder( "My buffer" );
   strAlpha = "abcdefghijklmnopqrstuvwxyz";
   if ( sbwrMyBuffer == null  || strAlpha == null )
     {
     iCountErrors++;
     return false;
     }
   System.Console.Out.WriteLine( sbwrMyBuffer.ToString() );
   sbwrMyBuffer.Append( strAlpha );
   System.Console.Out.WriteLine( sbwrMyBuffer.ToString() );
   iCountTestcases++;
   if ( sbwrMyResult.ToString().Equals( sbwrMyBuffer.ToString() ) != true )
     {
     iCountErrors++;
     }
   iCountTestcases++;
   sbwrMyBuffer = new StringBuilder( "My buffer" );
   sbwrAlpha = new StringBuilder( strAlpha );
   if ( sbwrMyBuffer == null  || strAlpha == null )
     {
     iCountErrors++;
     return false;
     }
   System.Console.Out.WriteLine( sbwrMyBuffer.ToString() );
   sbwrMyBuffer.Append( sbwrAlpha );
   System.Console.Out.WriteLine( sbwrMyBuffer.ToString() );
   iCountTestcases++;
   if ( sbwrMyResult.ToString().Equals( sbwrMyBuffer.ToString() ) != true )
     {
     iCountErrors++;
     }
   sbwrMyBuffer = new StringBuilder( "My buffer" );
   sbwrStrBuff2 = new StringBuilder( "My buffer" );
   iCountTestcases++;
   if ( sbwrMyBuffer == null || sbwrStrBuff2 == null )
     {
     iCountErrors++;
     return false;
     }
   System.Console.Out.WriteLine( sbwrMyBuffer.ToString() );
   strWork = sbwrMyBuffer.ToString();
   sbwrMyBuffer.Append( "" );
   iCountTestcases++;
   System.Console.Out.WriteLine( sbwrMyBuffer.ToString() );
   if ( sbwrMyBuffer.ToString().Equals( strWork ) != true )
     {
     iCountErrors++;
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
 public static void Main( String [] args )
   {
   bool bResult = false;	
   System.String strTest = "Co1252Append_string";
   Co1252Append_string oCbTest = new Co1252Append_string();
   try
     {
     bResult = oCbTest.runTest();
     }
   catch( System.Exception ex )
     {
     System.Console.Error.Write( strTest );
     System.Console.Error.WriteLine(  " uncaught exception" );
     }
   if ( bResult == true )
     {
     System.Console.Error.Write( strTest );
     System.Console.Error.WriteLine( " paSs" );
     Environment.ExitCode = 0;
     }
   else
     {
     System.Console.Error.Write( strTest );
     System.Console.Error.WriteLine( " FAiL" );
     Environment.ExitCode = 1;
     }
   }
} 
