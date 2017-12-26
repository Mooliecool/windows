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
public class Co5003CompareTo
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Double.CompareTo()";
 public static readonly String s_strTFName        = "Co5003CompareTo.cs";
 public static readonly String s_strTFAbbrev      = "Co5003";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   Double do2a = (Double)0;
   Double do2b = (Double)0;
   Object o1;
   Double [] doArr = {-4.34E23
		      -10.1,
		      -7.7,
		      -4.54,
		      -2.26,
		      -3.32E-34,
		      -0.0,
		      0.0,
		      3.34E-23,
		      0.34,
		      3.342,
		      6.445,
		      1.23E39,
		      Double.NegativeInfinity,
		      Double.PositiveInfinity};
   try {
   do
     {
     strLoc = "Loc_103ue";
     iCountTestcases++;
     for (int i =0;i < doArr.Length; i++)
       {
       o1 = doArr[i];
       if(doArr[i].CompareTo(o1) != 0)
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_283sh! , doArr[i]=="+doArr[i]);
	 }
       }
     strLoc = "Loc_388iu";
     do2a = (Double)(0.0);
     do2b = (Double)(-0.0);
     o1 = do2b;
     iCountTestcases++;
     if(do2a.CompareTo(o1) != 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_807ue! ,do2a=="+do2a);
       }
     strLoc = "Loc_239ee";
     do2a = Double.PositiveInfinity;
     do2b = Double.PositiveInfinity;
     o1 = do2b;
     iCountTestcases++;
     if(do2a.CompareTo(o1) != 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_893js! ,do2a=="+do2a);
       }
     strLoc = "Loc_832as";
     do2a = Double.NaN;
     do2b = Double.NaN;
     o1 = do2b;
     if(do2a == do2b)
       {
       Console.Error.WriteLine("blah: These are not equal");
       iCountErrors++;
       }
     iCountTestcases++;
     if(do2a.CompareTo(o1) != 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_377iw! return=="+do2a.CompareTo(do2b));
       }
     strLoc = "Loc_938jd";
     do2a = Double.MaxValue;
     do2b = Double.MinValue;
     o1 = do2b;
     iCountTestcases++;
     if(do2a.CompareTo(o1) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_387aj!");
       }
     strLoc = "Loc_578sj";
     do2a = (Double)(5.44);
     do2b = (Double)(5.43);
     o1 = do2b;
     iCountTestcases++;
     if(do2a.CompareTo(o1) <=0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_428ju!");
       }
     strLoc = "Loc_743je";
     do2a = (Double)(-4.444);
     do2b = (Double)(-4.443);
     o1 = do2b;
     iCountTestcases++;
     if(do2a.CompareTo(o1) >= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_218sk!");
       }
     strLoc = "Loc_377su";
     iCountTestcases++;
     try {
     do2a.CompareTo(null);
     } catch (ArgumentException aExc){
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_382hf! , return=="+do2a.CompareTo(null));
     }
     strLoc = "Loc_838dh";
     do2a = (Double)5.35;
     iCountTestcases++;
     if(do2a.CompareTo(null) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_237fe! ,result=="+do2a.CompareTo(null));
       }
     strLoc = "Loc_878ju";
     do2a = (Double)0.0;
     iCountTestcases++;
     if(do2a.CompareTo(null) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_737sa! ,result=="+do2a.CompareTo(null));
       }
     strLoc = "Loc_573kd";
     do2a = (Double) (-2.238);
     iCountTestcases++;
     if(do2a.CompareTo(null) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_727aj! ,result=="+do2a.CompareTo(null));
       }
     strLoc = "Loc_121dt";
     do2a = Double.NaN;
     iCountTestcases++;
     if(do2a.CompareTo(null) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_747aj! ,result=="+do2a.CompareTo(null));
       }
     strLoc = "Loc_588ie";
     Single si1 = (Single)5.35;
     do2a = (Double)10.1;
     o1 = si1;
     iCountTestcases++;
     try {
       if(do2a.CompareTo(o1) <=0)
         {
           iCountErrors++;
           Console.WriteLine(s_strTFAbbrev+ "Err_890wq! ,result=="+do2a.CompareTo(si1));
         }
     } catch (ArgumentException aExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_872hh! ,exc=="+exc);
     }
     String str1 = "5.5";
     iCountTestcases++;
     try {
     do2a.CompareTo(str1);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_883hq! ,result=="+do2a.CompareTo(str1));
     } catch (ArgumentException aExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_378zj! ,exc=="+exc);
     }
     strLoc = "Loc_000dt";
     iCountTestcases++;
     try {
     do2a.CompareTo(new Object());
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_183dt! ,result=="+do2a.CompareTo(str1));
     } catch (ArgumentException aExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_379dt! ,exc=="+exc);
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
   Co5003CompareTo cbA = new Co5003CompareTo();
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
