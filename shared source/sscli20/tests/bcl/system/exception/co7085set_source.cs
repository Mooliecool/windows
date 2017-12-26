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
using System;
using System.IO;
using System.Collections;
using System.Globalization;
using System.Text;
using System.Threading;
public class Co7085set_Source
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Exception.Source"; 
 public static String s_strTFName        = "Co7085set_Source.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = "";
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   try
     {
     String strSource;
     Exception e = new Exception();
     strLoc = "Loc_498yg";
     iCountTestcases += 1;
     strSource = null;
     try
       {                               
       e.Source = strSource;
       throw e;
       }
     catch (Exception exc)
       {
       if (e.Source.ToLower() != "co7085set_source")
	 {
	 iCountErrors += 1;
	 printerr ( "Error_484ee! Send in==" + "co7085set_source" + "\" Expected==" + "co7085set_source" + "\" Received==\"" + exc.Source + "\"" );
	 }				
       }
     strLoc = "Loc_945ek";
     iCountTestcases += 1;
     strSource = "";
     try
       {
       e.Source = strSource;
       throw e;
       }
     catch (Exception exc)
       {
       if (!e.Source.Equals(strSource))
	 {
	 iCountErrors += 1;
	 printerr ( "Error_889wc! Send in==\"" + e.Source + "\" Expected==\"" + strSource + "\" Received==\"" + exc.Source + "\"" );
	 }				
       }
     strLoc = "Loc_893ko";
     iCountTestcases += 1;
     strSource = "Co7085set_Source.cs";
     try
       {
       e.Source = strSource;
       throw e;
       }
     catch (Exception exc)
       {
       if (!e.Source.Equals("Co7085set_Source.cs")) 
	 {
	 iCountErrors += 1;
	 printerr ( "Error_141ps! Send in=" + "Co7085set_Source.cs" + "\" Expected=" + "Co7085set_Source.cs" + "\" Received==\"" + exc.Source + "\"" );
	 }				
       }
     } catch (Exception exc_general ) {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
     }
   if ( iCountErrors == 0 )
     {
     Console.WriteLine( "paSs. "+s_strTFName+" ,iCountTestcases=="+iCountTestcases.ToString());
     return true;
     }
   else
     {
     Console.WriteLine("FAiL! "+s_strTFName+" ,inCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public void printerr ( String err )
   {
   Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
   }
 public void printinfo ( String info )
   {
   Console.WriteLine ("INFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co7085set_Source cbA = new Co7085set_Source();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFPath+"\\"+s_strTFName);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
