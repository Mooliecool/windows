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
using System.Globalization;
using System.Threading;
using System;
public class Co5020Parse_int
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Single.Parse(String s, int style)";
 public static readonly String s_strTFName        = "Co5020Parse_int.cs";
 public static readonly String s_strTFAbbrev      = "Co5020";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   Single singa = (Single)0;
   Single singb = (Single)0;
   String strOut = null;
   String str2 = null;
   Single [] singTestValues = {Single.MinValue,
			       (Single)(-1000.54),
			       (Single)(-99.339),
			       (Single)(-5.45),
			       (Single)(-0.0),
			       (Single)0.0,
			       (Single)5.621,
			       (Single)13.4223,
			       (Single)101.81,
			       (Single)1000.999,
			       Single.MaxValue
   };
   Single [] resultValues1 = {Single.MinValue,
			      (Single)(-1000.54),
			      (Single)(-99.34),
			      (Single)(-5.45),
			      (Single)0,
			      (Single)0,
			      (Single)5.62,
			      (Single)13.42,
			      (Single)101.81,
			      (Single)1001.00,
			      Single.MaxValue
   };
   Single [] resultValues2 = {Single.MinValue,
			      (Single)(-1000.5),
			      (Single)(-99.339),
			      (Single)(-5.45),
			      (Single)0,
			      (Single)0,
			      (Single)5.621,
			      (Single)13.422,
			      (Single)101.81,
			      (Single)1001,
			      Single.MaxValue
   };
   try {
   LABEL_860_GENERAL:
   do
     {
     strBaseLoc = "Loc_1100ds_";
     iCountTestcases = 15;
     if(Thread.CurrentThread.CurrentCulture.LCID==0x0409){
     for (int i=1; i < singTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc+ i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString( "");
       singa = Single.Parse(strOut, NumberStyles.Any);
       if(singa != singTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i=="+i+" singa=="+singa+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1200er_";
     for (int i=1; i < singTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString( "G");
       singa = Single.Parse(strOut, NumberStyles.Any);
       if(singa != singTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347ew! , i=="+i+" singa=="+singa+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1300we_";
     for (int i=1; i < singTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString( "G8");
       singa = Single.Parse(strOut, NumberStyles.Any);
       if(singa != singTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_349ex! , i=="+i+" singa=="+singa+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1400fs_";
     for (int i=1; i < singTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString( "C");
       singa = Single.Parse(strOut, NumberStyles.Any);
       if(singa != resultValues1[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_832ee! , i=="+i+" singa=="+singa+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1500ez_";
     for (int i=1; i < singTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString( "C4");
       singa = Single.Parse(strOut, NumberStyles.Any);
       if(singa != singTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_273oi! , i=="+i+" singa=="+singa+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1800ns_";
     for (int i=1; i < singTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString( "E");
       singa = Single.Parse(strOut, NumberStyles.Any);
       if(singa != singTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_347sq! , i=="+i+" singa=="+singa+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_1900wq_";
     for (int i=1; i < singTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString( "e4");
       singa = Single.Parse(strOut, NumberStyles.Any);
       if(singa != resultValues2[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_873op! , i=="+i+" singa=="+singa+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2000ne_";
     for (int i=1; i < singTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString( "N");
       singa = Single.Parse(strOut, NumberStyles.Any);
       if(singa != resultValues1[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_129we! , i=="+i+" singa=="+singa+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2100qu_";
     for (int i=1; i < singTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString( "N4");
       singa = Single.Parse(strOut, NumberStyles.Any);
       if(singa != singTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_321sj! , i=="+i+" singa=="+singa+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2500qi_";
     for (int i=1; i < singTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString( "F");
       singa = Single.Parse(strOut, NumberStyles.Any);
       if(singa != resultValues1[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_815jd! , i=="+i+" singa=="+singa+" strOut=="+strOut);
	 }
       }
     strBaseLoc = "Loc_2600qi_";
     for (int i=1; i < singTestValues.Length-1;i++)
       {
       strLoc = strBaseLoc + i.ToString();
       iCountTestcases++;
       strOut = singTestValues[i].ToString( "F4");
       singa = Single.Parse(strOut, NumberStyles.Any);
       if(singa != singTestValues[i])
	 {
	 iCountErrors++;
	 Console.WriteLine(s_strTFAbbrev+ "Err_193jd! , i=="+i+" singa=="+singa+" strOut=="+strOut);
	 }
       }
     strOut = null;
     iCountTestcases++;
     try {
     singa = Single.Parse(strOut, NumberStyles.Any);
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_273qp! , singa=="+singa);
     } catch (ArgumentException aExc) {}
     catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine(s_strTFAbbrev+ "Err_982qo! ,exc=="+exc);
     }
     strOut = "1E39";
     iCountTestcases++;
     try{
     singa = Single.Parse(strOut, NumberStyles.Any);
     }catch(OverflowException){
     }catch(Exception ex){
     ++iCountErrors;
     Console.Error.WriteLine(  "Err_3wrg! Wrong exception thrown, " + ex.GetType().Name  );
     }
     strOut ="-1E39";
     iCountTestcases++;
     try{
     singa = Single.Parse(strOut, NumberStyles.Any);
     }catch(OverflowException){
     }catch(Exception ex){
     ++iCountErrors;
     Console.Error.WriteLine(  "Err_3wrg! Wrong exception thrown, " + ex.GetType().Name  );
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
   Co5020Parse_int cbA = new Co5020Parse_int();
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
