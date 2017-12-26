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
public class Co8576ToString_str
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Guid.ToString(String format)";
 public static String s_strTFName        = "Co8576ToString_str.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   Guid gd1;
   String retValue;
   String[] formats = {"d", "D", "n", "N", "p", "P", "b", "B"};
   String[] exceptionFormats = {"g", "G", "c", "C", "e", "E", "x", "X", "#", "0", "####.####", "000.000", "%###", "##E+0", "{{"};
   try
     {
     strLoc = "Loc_93475sdg";
     iCountTestcases++;
     gd1 = new Guid("a4434021-80db-4b65-aadd-f138e53e0e07");
     foreach(String format in formats){
     retValue = gd1.ToString(format);
     switch(Char.ToUpper(format[0])){
     case 'B':
       if(!retValue.Equals("{a4434021-80db-4b65-aadd-f138e53e0e07}")){
       iCountErrors++;
       Console.WriteLine("Err_734wdg! wrong result returned, " + retValue);
       }
       break;
     case 'D':
       if(!retValue.Equals("a4434021-80db-4b65-aadd-f138e53e0e07")){
       iCountErrors++;
       Console.WriteLine("Err_4937wsdg! wrong result returned, " + retValue);
       }
       break;
     case 'N':
       if(!retValue.Equals("a443402180db4b65aaddf138e53e0e07")){
       iCountErrors++;
       Console.WriteLine("Err_29475tsdg! wrong result returned, " + retValue);
       }
       break;
     case 'P':
       if(!retValue.Equals("(a4434021-80db-4b65-aadd-f138e53e0e07)")){
       iCountErrors++;
       Console.WriteLine("Err_4893sgfdg! wrong result returned, " + retValue);
       }
       break;
     }
     }
     strLoc = "Loc_93475sdg";
     iCountTestcases++;
     gd1 = Guid.Empty;
     foreach(String format in formats){
     retValue = gd1.ToString(format);
     switch(Char.ToUpper(format[0])){
     case 'B':
       if(!retValue.Equals("{00000000-0000-0000-0000-000000000000}")){
       iCountErrors++;
       Console.WriteLine("Err_734wdg! wrong result returned, " + retValue);
       }
       break;
     case 'D':
       if(!retValue.Equals("00000000-0000-0000-0000-000000000000")){
       iCountErrors++;
       Console.WriteLine("Err_4937wsdg! wrong result returned, " + retValue);
       }
       break;
     case 'N':
       if(!retValue.Equals("00000000000000000000000000000000")){
       iCountErrors++;
       Console.WriteLine("Err_29475tsdg! wrong result returned, " + retValue);
       }
       break;
     case 'P':
       if(!retValue.Equals("(00000000-0000-0000-0000-000000000000)")){
       iCountErrors++;
       Console.WriteLine("Err_4893sgfdg! wrong result returned, " + retValue);
       }
       break;
     }
     }
     strLoc = "Loc_7349gsd";
     iCountTestcases++;
     gd1 = Guid.NewGuid();
     foreach(String exp in exceptionFormats){
     try{
     retValue = gd1.ToString(exp);
     iCountErrors++;
     Console.WriteLine("Err_4754ewg! No exception thrown, " + retValue);
     }catch(FormatException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine("Err_4754ewg! Wrong exception thrown, " + ex.GetType().Name);
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
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co8576ToString_str cbA = new Co8576ToString_str();
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
