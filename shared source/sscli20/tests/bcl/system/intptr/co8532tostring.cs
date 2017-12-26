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
public class Co8532ToString
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "IntPtr.ToString()";
 public static readonly String s_strTFName        = "Co8532ToString.cs";
 public static readonly String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
 public static readonly String s_strTFPath        = Environment.CurrentDirectory;
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   Int32 iValue;
   IntPtr ip1;
   try {
   strLoc = "Loc_743wg";
   iValue = 16;
   ip1 = new IntPtr(iValue);
   iCountTestcases++;
   if(ip1.ToString() != iValue.ToString()){
   iCountErrors++;
   Console.WriteLine("Err_2975sf! Wrong value returned");
   }
   strLoc = "Loc_0084wf";
   iValue = 0;
   ip1 = new IntPtr(iValue);
   iCountTestcases++;
   if(ip1.ToString() != iValue.ToString()){
   iCountErrors++;
   Console.WriteLine("Err_974325sdg! Wrong value returned");
   }
   strLoc = "Loc_00s42f";
   iValue = -15;
   ip1 = new IntPtr(iValue);
   iCountTestcases++;
   if(ip1.ToString() != iValue.ToString()){
   iCountErrors++;
   Console.WriteLine("Err_9374fzdg! Wrong value returned, " + ip1.ToString());
   }
   strLoc = "Loc_93476sdg";
   iValue = Int32.MaxValue;
   ip1 = new IntPtr(iValue);
   iCountTestcases++;
   if(ip1.ToString() != iValue.ToString()){
   iCountErrors++;
   Console.WriteLine("Err_07536tsg! Wrong value returned");
   }
   iValue = Int32.MinValue;
   ip1 = new IntPtr(iValue);
   if(ip1.ToString() != iValue.ToString()){
   iCountErrors++;
   Console.WriteLine("Err_9875wrsg! Wrong value returned");
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
   Co8532ToString cbA = new Co8532ToString();
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
