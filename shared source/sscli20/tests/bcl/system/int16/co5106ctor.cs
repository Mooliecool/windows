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
public class Co5106ctor
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Integer2.CompareTo()";
 public static readonly String s_strTFName        = "Co5106ctor.cs";
 public static readonly String s_strTFAbbrev      = "Co5106";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc;
   Int16 in2a = (Int16)0;
   Int16 in2b = (Int16)0;
   short sh2a = (short)0;
   short sh2b = (short)0;
   short [] shArr = {Int16.MinValue,
		     (short)(-10000),
		     (short)(-0),
		     (short)0,
		     (short)1000,
		     (short)10000,
		     Int16.MaxValue
   };
   try {
   LABEL_860_GENERAL:
   do
     {
     strLoc = "Loc_111ue";
     in2a = new Int16();
     iCountTestcases++;
     if(in2a != 0)
       {
       iCountTestcases++;
       Console.WriteLine( s_strTFAbbrev+ "Err_947au  in2a=="+in2a);
       }
     strBaseLoc = "Loc_747se_";
     for(int ii = 0; ii < shArr.Length ; ii++)
       {
       strLoc = strBaseLoc + ii.ToString();
       iCountTestcases++;
       if( ((Int16)shArr[ii]) != shArr[ii])
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_483ay");
	 }
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
   Co5106ctor cbA = new Co5106ctor();
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
