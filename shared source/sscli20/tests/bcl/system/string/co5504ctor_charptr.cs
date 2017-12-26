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
using System.Globalization; 
using GenStrings;
using System;
using System.IO;
using System.Collections;
public class Co5504Ctor_charptr
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "String(char*)";
 public static String s_strTFName        = "Co5504Ctor_charptr.";
 public static String s_strTFAbbrev      = "Co5504";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public unsafe bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   try
     {
     char* chptr = null;
     String str2 = null;
     strLoc = "Loc_498hv";
     iCountTestcases++;
     str2 = new String(chptr);
     if(!str2.Equals(String.Empty))
       {
       iCountErrors++;
       printerr( "Error_498ch! incorrect string returned for null argument=="+str2);
       } 
     strLoc = "Loc_982hc";
     iCountTestcases++;
     char c = 'k';
     char *chptr1 = stackalloc char[2]; //to make it's ok on mac machine
     chptr1[0] = c;
     chptr1[1] = '\0';
     str2 = new String(chptr1);
     if(!str2.Equals("k"))
       {
       iCountErrors++;
       printerr( "Error_49hcx! incorrect string constructed=="+str2);
       }
     strLoc = "Loc_9fhc1";
     *chptr1 = '\t';
     str2 = new String(chptr1);
     iCountTestcases++;
     if(!str2.Equals("\t"))
       {
       iCountErrors++;
       printerr( "Error_89hfd! incorrect string constructed=="+str2);
       }
     for(int i = 30 ; i < 150 ; i++)
       {
       c = (char) i;
       chptr1[0] = c;
       str2 = new String(chptr1);
       iCountTestcases++;
       if(!str2.Equals(c.ToString()))
	 { 
	 iCountErrors++;
	 printerr( "Error_209xu! Expected=="+c+" , Incorrect string=="+str2);
	 }
       }
     iCountTestcases++;
     IntlStrings intl = new IntlStrings();
     String testString = intl.GetString(1, true, true);
     c = testString[0];
     chptr1[0] = c;
     str2 = new String(chptr1);
     if(!str2.Equals(testString))
       {
       iCountErrors++;
       printerr( "Error_49hcx! incorrect string constructed=="+str2 + "should be==" + testString);
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
   Console.WriteLine ("EXTENDEDINFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co5504Ctor_charptr cbA = new Co5504Ctor_charptr();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
}
