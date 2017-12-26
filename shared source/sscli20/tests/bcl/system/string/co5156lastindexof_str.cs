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
using GenStrings;
using System.Globalization;
using System;
public class Co5156LastIndexOf_str
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "String.LastIndexOf(String)";
 public static readonly String s_strTFName        = "Co5156LastIndexOf_str.";
 public static readonly String s_strTFAbbrev      = "Co5156";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "";
   String strToSearch;
   String strValue;
   try {
   LABEL_860_GENERAL:
   do
     {
     strLoc = "Loc_30894";
     strToSearch = " ";
     strValue = null;
     iCountTestcases++;
     try {
     strToSearch.LastIndexOf(strValue);
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_234us");
     } catch (ArgumentException aExc) {}
     catch ( Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_417as");
     }
     strLoc = "Loc_3804u";
     strToSearch = "This is a \tstring";
     strValue = "  ";
     iCountTestcases++;
     if(strToSearch.LastIndexOf(strValue) != -1)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_0952u! , return=="+strToSearch.LastIndexOf(strValue));
       }
     strLoc = "Loc_842ey";
     strToSearch = "This is a \n\n\n small \t\t\t string \r\r\r";
     strValue = "\n\n";
     iCountTestcases++;
     if(strToSearch.LastIndexOf(strValue) != 11)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_348yu! , index=="+strToSearch.LastIndexOf(strValue));
       }
     strToSearch = "This is a \t\t\n small \t\t\t string \r\r\r";
     strValue = "\t";
     iCountTestcases++;
     if(strToSearch.LastIndexOf(strValue) != 22)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_473yf! , index=="+strToSearch.LastIndexOf(strValue));
       }
     strToSearch = "This is a \n\n\n \r\r\r\r small \t\t\t string \r\r\r\r";
     strValue = "\r\r\r";
     iCountTestcases++;
     if(strToSearch.LastIndexOf(strValue) != 37)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_9837u! , index=="+strToSearch.LastIndexOf(strValue));
       }
     IntlStrings intl = new IntlStrings();
     strToSearch = intl.GetString(5, true, true);
     strValue = intl.GetString(18, true, true);
     strToSearch = String.Concat(strToSearch, strValue);
     if(strToSearch.LastIndexOf(strValue) < 5) {
     ++iCountErrors;
     }
     strValue = " ";
     strToSearch = "This is \t a String";
     iCountTestcases++;
     if(strToSearch.LastIndexOf(strValue) != 11)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_3489d! , index=="+strToSearch.LastIndexOf(strValue));
       }
     strToSearch = "this is a string";
     iCountTestcases++;
     if(strToSearch.LastIndexOf(String.Empty) != strToSearch.Length-1)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_3247s! , index=="+strToSearch.LastIndexOf(String.Empty));
       }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine(s_strTFAbbrev +" Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
   }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co5156LastIndexOf_str cbA = new Co5156LastIndexOf_str();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev+ "FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main);
   }
   if (!bResult)
     {
     Console.WriteLine(s_strTFName+ s_strTFPath);
     Console.Error.WriteLine( " " );
     Console.Error.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.Error.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
}
