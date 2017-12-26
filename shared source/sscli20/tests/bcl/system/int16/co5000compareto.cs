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
public class Co5000CompareTo
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Integer2.CompareTo()";
 public static String s_strTFName        = "Co5000CompareTo.cs";
 public static String s_strTFAbbrev      = "Co5000";
 public static String s_strTFPath        = "";
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   Int16 in2a = (Int16)0;
   Int16 in2b = (Int16)0;
   short sh2a = (short)0;
   short sh2b = (short)0;
   try {
   do
     {
     strLoc = "Loc_122fj";
     sh2a = (short)5;
     sh2b = (short)5;
     in2a = (Int16)(sh2a);
     in2b = (Int16)(sh2b);
     iCountTestcases++;
     if(in2a.CompareTo(in2b) != 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev +"Err_873ye!");
       }
     strLoc = "Loc_312je";
     in2a = Int16.MinValue;
     in2b = Int16.MinValue;
     iCountTestcases++;
     if(in2a.CompareTo(in2b) != 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev +"Err_399ad!");
       }
     strLoc = "Loc_348je";
     sh2a = (short)0;
     sh2b = (short)0;
     in2a = (Int16)sh2a;
     in2b = (Int16)sh2b;
     iCountTestcases++;
     if(in2a.CompareTo(in2b) != 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_838su! , in2a=="+in2a);
       }
     strLoc = "Loc_938jd";
     in2a = Int16.MaxValue;
     in2b = Int16.MinValue;
     iCountTestcases++;
     if(in2a.CompareTo(in2b) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_387aj!");
       }
     strLoc = "Loc_578sj";
     sh2a = (short)11;
     sh2b = (short)10;
     in2a = (Int16)sh2a;
     in2b = (Int16)sh2b;
     iCountTestcases++;
     if(in2a.CompareTo(in2b) <=0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_428ju!");
       }
     strLoc = "Loc_743je";
     sh2a = (short)10;
     sh2b = (short)11;
     in2a = (Int16)sh2a;
     in2b = (Int16)sh2b;
     iCountTestcases++;
     if(in2a.CompareTo(in2b) >= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_218sk!");
       }
     strLoc = "Loc_939eu";
     in2a = Int16.MinValue;
     in2b = Int16.MaxValue;
     iCountTestcases++;
     if(in2a.CompareTo(in2b) >= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_377as!");
       }
     strLoc = "Loc_377su";
     iCountTestcases++;
     try {
     in2a.CompareTo(null);
     } catch (ArgumentException ){
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_382hf! , return=="+in2a.CompareTo(null));
     }
     strLoc = "Loc_838dh";
     in2a = (Int16)5;
     iCountTestcases++;
     if(in2a.CompareTo(null) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_237fe! ,result=="+in2a.CompareTo(null));
       }
     strLoc = "Loc_878ju";
     in2a = (Int16)0;
     iCountTestcases++;
     if(in2a.CompareTo(null) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_737sa! ,result=="+in2a.CompareTo(null));
       }
     strLoc = "Loc_573kd";
     sh2a = (short)-2;
     in2a = (Int16) (sh2a);
     iCountTestcases++;
     if(in2a.CompareTo(null) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_727aj! ,result=="+in2a.CompareTo(null));
       }
     strLoc = "Loc_109cu";
     in2a = (Int16)5;
     iCountTestcases++;
     try {
     in2a.CompareTo((Int64)5);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_209cu! , result=="+in2a.CompareTo((Int64)5));
     } catch (ArgumentException ) {
     } catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_109ux! Unexpected Exception == "+exc.ToString());
     }
     in2a = -12;
     ++iCountTestcases;
     if ( 	in2a.CompareTo( (Int16)66) >=  0)
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_337uf!"  );
       }
     strLoc = "Loc_938cd";
     in2a = (Int16)(-12);
     in2a = (Int16)in2a.CompareTo( (Int16)(-77) );
     ++iCountTestcases;
     if ( !( in2a > (Int16)0 ) )
       {
       ++iCountErrors;
       Console.Error.WriteLine(  "POINTTOBREAK:  Error Err_411wq!  in2a=="+ in2a  );
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
   Boolean bResult = false;
   Co5000CompareTo cbA = new Co5000CompareTo();
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
