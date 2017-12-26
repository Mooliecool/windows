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
public class Co8525Equals_obj
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "IntPtr.Equals(Object)";
 public static readonly String s_strTFName        = "Co8525Equals_obj.cs";
 public static readonly String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
 public static readonly String s_strTFPath        = Environment.CurrentDirectory;
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   IntPtr ip1;
   IntPtr ip2;
   Int32 iValue;
   try {
   strLoc = "Loc_743wg";
   iValue = 16;
   ip1 = new IntPtr(iValue);
   ip2 = new IntPtr(iValue);
   iCountTestcases++;
   if(!ip1.Equals(ip2)){
   iCountErrors++;
   Console.WriteLine("Err_865sg! Wrong value returned");
   }             
   strLoc = "Loc_9047tdsg";
   iValue = 16;
   ip1 = new IntPtr(iValue);
   ip2 = new IntPtr(iValue*2);
   iCountTestcases++;
   if(ip1.Equals(ip2)){
   iCountErrors++;
   Console.WriteLine("Err_9765sgf! Wrong value returned");
   }             
   strLoc = "Loc_98736zdg";
   iValue = 16;
   ip1 = new IntPtr(iValue);
   iCountTestcases++;
   if(ip1.Equals(iValue)){
   iCountErrors++;
   Console.WriteLine("Err_9756gf! Wrong value returned");
   }             
   strLoc = "Loc_98736zdg";
   iValue = 16;
   ip1 = new IntPtr(iValue);
   iCountTestcases++;
   if(ip1.Equals(null)){
   iCountErrors++;
   Console.WriteLine("Err_973sdg! Wrong value returned");
   }             
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine(s_strTFAbbrev +" Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
   }
   if ( iCountErrors == 0 )
     {
     Console.Error.WriteLine( "paSs.   "+s_strTFPath +" "+s_strTFName+" ,iCountTestcases=="+iCountTestcases);
     return true;
     }
   else
     {
     Console.Error.WriteLine("FAiL!   "+s_strTFPath+" "+s_strTFName+" ,iCountErrors=="+iCountErrors+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co8525Equals_obj cbA = new Co8525Equals_obj();
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
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
