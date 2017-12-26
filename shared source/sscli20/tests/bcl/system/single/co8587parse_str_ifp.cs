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
using System;
public class Co8587Parse_str_ifp
{
 public static readonly String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "Single.Parse(String s, int style, NumberFormatInfo info)";
 public static readonly String s_strTFName        = "Co8587Parse_str_ifp.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   Single singa = (Single)0;
   String strOut = null;
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
   NumberFormatInfo nfi1 = new NumberFormatInfo();
   nfi1.NegativeSign = "^";  
   nfi1.PositiveSign = "~";  
   try {
   strBaseLoc = "Loc_1100ds_";
   for (int i=1; i < singTestValues.Length-1;i++)
     {
     strLoc = strBaseLoc+ i.ToString();
     iCountTestcases++;
     strOut = singTestValues[i].ToString( "", nfi1);
     singa = Single.Parse(strOut, nfi1);
     if(singa != singTestValues[i])
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i=="+i+" singa=="+singa+" strOut=="+strOut);
       }
     }
   strBaseLoc = "Loc_1300we_";
   for (int i=1; i < singTestValues.Length-1;i++)
     {
     strLoc = strBaseLoc + i.ToString();
     iCountTestcases++;
     strOut = singTestValues[i].ToString( "G8", nfi1);
     singa = Single.Parse(strOut, nfi1);
     if(singa != singTestValues[i])
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_349ex! , i=="+i+" singa=="+singa+" strOut=="+strOut);
       }
     }
   strOut = null;
   iCountTestcases++;
   try {
   singa = Single.Parse(strOut, nfi1);
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_273qp! , singa=="+singa);
   } catch (ArgumentException) {}
   catch (Exception exc) {
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_982qo! ,exc=="+exc);
   }
   strOut = "1E39";
   iCountTestcases++;
   try{
   singa = Single.Parse(strOut, nfi1);
   }catch(OverflowException){
   }catch(Exception ex){
   ++iCountErrors;
   Console.Error.WriteLine(  "Err_3wrg! Wrong exception thrown, " + ex.GetType().Name  );
   }
   strOut ="^1E39";
   iCountTestcases++;
   try{
   singa = Single.Parse(strOut, nfi1);
   }catch(OverflowException){
   }catch(Exception ex){
   ++iCountErrors;
   Console.Error.WriteLine(  "Err_3wrg! Wrong exception thrown, " + ex.GetType().Name  );
   }
   iCountTestcases++;
   strOut = singTestValues[0].ToString( "C", nfi1);
   try {
   singa = Single.Parse(strOut, nfi1);
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_382er! ,singa=="+singa);
   } catch (FormatException) {}
   catch (Exception exc) {
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_8756tedfsg! ,exc=="+exc);
   }
   singa = Single.Parse("~123", nfi1);
   iCountTestcases++;
   if(singa != 123)
     {
     iCountErrors++;
     Console.WriteLine("Err_93475sdg! singa=="+singa);
     }
   nfi1.NumberNegativePattern = 3;
   strOut ="123^";
   iCountTestcases++;
   try {
   singa = Single.Parse(strOut, nfi1);
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_382er! ,singa=="+singa);
   } catch (FormatException) {}
   catch (Exception exc) {
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_371jy! ,exc=="+exc);
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
   Co8587Parse_str_ifp cbA = new Co8587Parse_str_ifp();
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
