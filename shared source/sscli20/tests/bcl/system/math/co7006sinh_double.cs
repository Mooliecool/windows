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
public class Co7006Sinh_Double
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Math.Sinh(Double)"; 
 public static String s_strTFName        = "Co7006Sinh_Double.cs";
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
     Double da, dret, dactual;			
     strLoc = "Loc_498yg";
     for (da = -10; da <= 10; da = da + 0.01)
       {
       iCountTestcases++;
       dactual = (Math.Exp (da) - Math.Exp (-1 * da))/2;
       dret = Math.Sinh (da);
       if (Math.Abs (dactual - dret) > 0.00000000001)
	 {
	 iCountErrors++;
	 printerr("Error_100ab_" + da.ToString() + " Expected==" + dactual.ToString() + ", value==" + dret.ToString());
	 }
       }
     strLoc = "Loc_398vy";
     da = Double.MaxValue;
     iCountTestcases++;
     dactual = (Math.Exp (da) - Math.Exp (-1 * da))/2;
     dret = Math.Sinh (da);
     Console.WriteLine ("value returned for Max value is " + dret.ToString());
     if (Math.Abs (dactual - dret) > 0.00000000001)
       {
       iCountErrors++;
       printerr("Error_200ab Expected==" + dactual.ToString() + ", value==" + dret.ToString());
       }
     strLoc = "Loc_698vy";
     da = Double.MinValue;
     iCountTestcases++;
     dactual = (Math.Exp (da) - Math.Exp (-1 * da))/2;
     dret = Math.Sinh (da);
     Console.WriteLine ("value returned for Min value is " + dret.ToString());
     if (Math.Abs (dactual - dret) > 0.00000000001)
       {
       iCountErrors++;
       printerr("Error_300ab Expected==" + dactual.ToString() + ", value==" + dret.ToString());
       }
     strLoc = "Loc_998vy";
     da = Double.Epsilon;
     iCountTestcases++;
     dactual = (Math.Exp (da) - Math.Exp (-1 * da))/2;
     dret = Math.Sinh (da);
     Console.WriteLine ("value returned for Epsilon value is " + dret.ToString());
     if (Math.Abs (dactual - dret) > 0.00000000001)
       {
       iCountErrors++;
       printerr("Error_400ab Expected==" + dactual.ToString() + ", value==" + dret.ToString());
       }
     strLoc = "Loc_198vy";
     da = Double.Epsilon * -1;
     iCountTestcases++;
     dactual = (Math.Exp (da) - Math.Exp (-1 * da))/2;
     dret = Math.Sinh (da);
     Console.WriteLine ("value returned for negative Epsilon value is " + dret.ToString());
     if (Math.Abs (dactual - dret) > 0.00000000001)
       {
       iCountErrors++;
       printerr("Error_500ab Expected==" + dactual.ToString() + ", value==" + dret.ToString());
       }
     strLoc = "Loc_198pp";
     da = Double.PositiveInfinity;
     iCountTestcases++;
     dactual = (Math.Exp (da) - Math.Exp (-1 * da))/2;
     dret = Math.Sinh (da);
     Console.WriteLine ("value returned for Positive Infinity value is " + dret.ToString());
     if (Math.Abs (dactual - dret) > 0.00000000001)
       {
       iCountErrors++;
       printerr("Error_600ab Expected==" + dactual.ToString() + ", value==" + dret.ToString());
       }
     strLoc = "Loc_298pp";
     da = Double.NegativeInfinity;
     iCountTestcases++;
     dactual = (Math.Exp (da) - Math.Exp (-1 * da))/2;
     dret = Math.Sinh (da);
     Console.WriteLine ("value returned for Negative Infinity value is " + dret.ToString());
     if (Math.Abs (dactual - dret) > 0.00000000001)
       {
       iCountErrors++;
       printerr("Error_700ab Expected==" + dactual.ToString() + ", value==" + dret.ToString());
       }
     strLoc = "Loc_398pp";
     da = Double.NaN;
     iCountTestcases++;
     dret = Math.Sinh(da);
     Console.WriteLine ("value returned for NaN value is " + dret.ToString());
     if (Double.IsNaN (dret) == false)
       {
       iCountErrors++;
       printerr("Error_900ab Expected==NaN, value==" + dret.ToString());
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
   Co7006Sinh_Double cbA = new Co7006Sinh_Double();
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
