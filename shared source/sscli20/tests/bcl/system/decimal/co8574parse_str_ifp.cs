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
using System.Globalization;
using System.Threading;
public class Co8574Parse_str_ifp
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Decimal.Parse(String format, IFP nfi)";
 public static String s_strTFName        = "Co8574Parse_str_ifp.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0, 6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public Boolean runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "Loc_0000oo_";
   Decimal dcml1;
   String strOut = null;
   Decimal[] dcmExpValues = {Decimal.MinValue, 
			     Decimal.MinusOne,
			     0,
			     Decimal.One,
			     Decimal.Parse("1.234567890123456789012345678"),
			     Decimal.MaxValue
   };
   String[] strGFormat1Values = {"^79228162514264337593543950335", 
				 "^1",
				 "0",
				 "1",
				 "1.234567890123456789012345678",
				 "79228162514264337593543950335"
   };
   NumberFormatInfo nfi1 = new NumberFormatInfo();
   nfi1.NegativeSign = "^";  
   nfi1.PositiveSign = "~"; 
   try {
   if(Thread.CurrentThread.CurrentCulture.LCID==0x0409){
   strBaseLoc = "Loc_1100ds_";
   for (int i=0; i < dcmExpValues.Length;i++)
     {
     strLoc = strBaseLoc + i;
     dcml1 = Decimal.Parse(strGFormat1Values[i], nfi1);
     iCountTestcases++;
     if(dcml1 != dcmExpValues[i])
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_293qu! , i==" + i + " dcml1==" + dcml1);
       }
     }
   strBaseLoc = "Loc_1200er_";
   for (int i=0; i < dcmExpValues.Length;i++)
     {
     strLoc = strBaseLoc + i.ToString();
     strOut = dcmExpValues[i].ToString("G29", nfi1);
     dcml1 = Decimal.Parse(strOut, nfi1);
     iCountTestcases++;
     if(dcml1 != dcmExpValues[i])
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_347ew! , i==" + i + " dcml1==" + dcml1);
       }
     }
   strOut = null;
   iCountTestcases++;
   try {
   dcml1 = Decimal.Parse(strOut, nfi1);
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_273qp! , dcml1=="+dcml1);
   } catch (ArgumentException) {}
   catch (Exception exc) {
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_982qo! ,exc=="+exc);
   }
   strOut = "79228162514264337593543950336";
   iCountTestcases++;
   try {
   dcml1 = Decimal.Parse(strOut, nfi1);
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_481sm! , dcml1=="+dcml1);
   } catch (OverflowException) {}
   catch (Exception exc) {
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_523eu! ,exc=="+exc);
   }
   strOut ="^79228162514264337593543950336";
   iCountTestcases++;
   try {
   dcml1 = Decimal.Parse(strOut, nfi1);
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_382er! ,dcml1=="+dcml1);
   } catch (OverflowException) {}
   catch (Exception exc) {
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_84rwesdg! ,exc=="+exc);
   }
   strOut ="-2147483649";
   iCountTestcases++;
   strOut = dcmExpValues[0].ToString( "C", nfi1);
   try {
   dcml1 = Decimal.Parse(strOut, nfi1);
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_382er! ,dcml1=="+dcml1);
   } catch (FormatException) {}
   catch (Exception exc) {
   iCountErrors++;
   Console.WriteLine(s_strTFAbbrev+ "Err_8756tedfsg! ,exc=="+exc);
   }
   dcml1 = Decimal.Parse("~123", nfi1);
   iCountTestcases++;
   if(dcml1 != 123)
     {
     iCountErrors++;
     Console.WriteLine("Err_93475sdg! dcml1=="+dcml1);
     }
   dcml1 = Decimal.Parse("123 ^", nfi1);
   iCountTestcases++;
   if(dcml1 != -123)
     {
     iCountErrors++;
     Console.WriteLine("Err_8345sdgf! dcml1=="+dcml1);
     }
   dcml1 = Decimal.Parse("123 ~", nfi1);
   iCountTestcases++;
   if(dcml1 != 123)
     {
     iCountErrors++;
     Console.WriteLine("Err_23475sd! dcml1=="+dcml1);
     }
   }else{
   Console.WriteLine("WARNING!!! this test cases wasnt run in this locale!!!!");
   iCountTestcases = 50;
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
   Boolean bResult = false;
   Co8574Parse_str_ifp cbA = new Co8574Parse_str_ifp();
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
