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
public class Co5026CompareTo
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Boolean.CompareTo(Object obj)";
 public static readonly String s_strTFName        = "Co5026CompareTo.cs";
 public static readonly String s_strTFAbbrev      = "Co5026";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   Boolean bool2a = false;
   Boolean bool2b = true;
   try {
   LABEL_860_GENERAL:
   do
     {
     strLoc = "Loc_384eu";
     iCountTestcases++;
     if(bool2a.CompareTo(null) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_387wq! , return=="+bool2a.CompareTo(null));
       }
     iCountTestcases++;
     if(bool2b.CompareTo(null) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_382qu! ,return=="+bool2b.CompareTo(null));
       }
     strLoc = "Loc_819wy";
     bool2a = false;
     iCountTestcases++;
     if(bool2a.CompareTo(false) != 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_281wu! , return=="+bool2a.CompareTo(false));
       }
     strLoc = "Loc_982qy";
     bool2a = true;
     iCountTestcases++;
     if(bool2a.CompareTo(true) != 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_391hw! ,return=="+bool2a.CompareTo(true));
       }
     strLoc = "Loc_734ww";
     bool2a = true;
     iCountTestcases++;
     if(bool2a.CompareTo(false) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_832hq! ,return=="+bool2a.CompareTo(false));
       }
     strLoc = "Loc_195ch";
     bool2a = false;
     iCountTestcases++;
     if(bool2a.CompareTo(true) >= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_128fh!");
       }
     } while (false);
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
   Co5026CompareTo cbA = new Co5026CompareTo();
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
