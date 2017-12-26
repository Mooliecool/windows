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
public class Co7008Log_Double
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Math.Log(Double,Double)"; 
 public static String s_strTFName        = "Co7008Log_Double.cs";
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
     Double da, dbase, dret;			
     strLoc = "Loc_198yg";
     da = 0;
     dbase = 10;
     dret = Math.Log(da,dbase);
     iCountTestcases++;
     if (dret != Double.NegativeInfinity)
       {
       iCountErrors++;
       printerr("Error_100ab Expected==Double.NegativeInfinity, value==" + dret.ToString());
       }
     strLoc = "Loc_298yg";
     da = -1;
     dbase = 10;
     dret = Math.Log(da,dbase);
     iCountTestcases++;
     if (Double.IsNaN(dret) == false)
       {
       iCountErrors++;
       printerr("Error_200ab Expected==NaN, value==" + dret.ToString());
       }
     strLoc = "Loc_398yg";
     da = 1;
     dbase = 10;
     dret = Math.Log(da,dbase);
     iCountTestcases++;
     if (dret != 0)
       {
       iCountErrors++;
       printerr("Error_300cd Expected==0, value==" + dret.ToString());
       }
     strLoc = "Loc_698yg";
     da = 12.2;
     dbase = -1;
     dret = Math.Log(da,dbase);
     iCountTestcases++;
     if (Double.IsNaN(dret) == false)
       {
       iCountErrors++;
       printerr("Error_600ab Expected==NaN, value==" + dret.ToString());
       }
     strLoc = "Loc_798yg";
     da = Double.PositiveInfinity;
     dbase = 10;
     dret = Math.Log(da,dbase);
     iCountTestcases++;
     if (dret != Double.PositiveInfinity)
       {
       iCountErrors++;
       printerr("Error_700ab Expected==PositiveInfinity, value==" + dret.ToString());
       }				
     strLoc = "Loc_898yg";
     da = Double.NegativeInfinity;
     dbase = 10;
     dret = Math.Log(da,dbase);
     iCountTestcases++;
     if (Double.IsNaN(dret) == false)
       {
       iCountErrors++;
       printerr("Error_800ab Expected==NaN, value==" + dret.ToString());
       }				
     strLoc = "Loc_998aa";
     da = 16;
     dbase = 4;
     dret = Math.Log(da,dbase);
     iCountTestcases++;
     if (dret != 2)
       {
       iCountErrors++;
       printerr("Error_900aa Expected==2, value==" + dret.ToString());
       }	
     strLoc = "Loc_998bb";
     da = 4;
     dbase = 4;
     dret = Math.Log(da,dbase);
     iCountTestcases++;
     if (dret != 1)
       {
       iCountErrors++;
       printerr("Error_900bb Expected==1, value==" + dret.ToString());
       }	
     strLoc = "Loc_998cc";
     da = 1;
     dbase = 4;
     dret = Math.Log(da,dbase);
     iCountTestcases++;
     if (dret != 0)
       {
       iCountErrors++;
       printerr("Error_900cc Expected==0, value==" + dret.ToString());
       }	
     strLoc = "Loc_998dd";
     da = 0.25;
     dbase = 4;
     dret = Math.Log(da,dbase);
     iCountTestcases++;
     if (dret != -1)
       {
       iCountErrors++;
       printerr("Error_900dd Expected==-1, value==" + dret.ToString());
       }	
     strLoc = "Loc_098yy";
     da = Double.NaN;
     dbase = 10;
     dret = Math.Log(da,dbase);
     iCountTestcases++;
     if (Double.IsNaN(dret) == false)
       {
       iCountErrors++;
       printerr("Error_100zz Expected==NaN, value==" + dret.ToString());
       }
     strLoc = "Loc_198yy";
     da = 10;
     dbase = Double.NaN;
     dret = Math.Log(da,dbase);
     iCountTestcases++;
     if (Double.IsNaN(dret) == false)
       {
       iCountErrors++;
       printerr("Error_100zz Expected==NaN, value==" + dret.ToString());
       }
     strLoc = "Loc_598yy";
     da = Double.MaxValue;
     dbase = 10;
     dret = Math.Log(da,dbase);
     iCountTestcases++;
     if ((Math.Abs (dret - 308.254715559916)) > 0.000000000001)
       {
       iCountErrors++;
       printerr("Error_500zz Expected==308.254715559916, value==" + dret.ToString());
       }
     strLoc = "Loc_698yy";
     da = Double.MinValue;
     dbase = 10;
     dret = Math.Log(da,dbase);
     iCountTestcases++;
     if (Double.IsNaN(dret) == false)
       {
       iCountErrors++;
       printerr("Error_600zz Expected==NaN, value==" + dret.ToString());
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
     Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
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
   Co7008Log_Double cbA = new Co7008Log_Double();
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
