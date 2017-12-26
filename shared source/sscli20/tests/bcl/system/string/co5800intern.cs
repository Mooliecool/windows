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
using System.Reflection;
using System.Globalization;
using GenStrings;
using System;
using System.Text;
public class Co5800Intern
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "String.Intern";
 public static String s_strTFName        = "Co5800Intern";
 public static String s_strTFAbbrev      = "Co5800";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   try
     {
     String str1, str2;
     StringBuilder sb1, sb2;
     strLoc = "Loc_299nu";
     str1 = null;
     str2 = null;
     iCountTestcases++;
     if((object)str1 != (object)str2)
       {
       iCountErrors++;
       printerr( "Error_229a! References of two null strings are not equal");
       }
     strLoc = "Loc_299em";
     str1 = String.Empty;
     str2 = String.Empty;
     iCountTestcases++;
     if((object)str1 != (object)str2)
       {
       iCountErrors++;
       printerr( "Error_229b! References of two strings assigned with .Empty() directly are not equal");
       }
     strLoc = "Loc_230ema";
     sb2 = new StringBuilder().Append(String.Empty);
     str2 = sb2.ToString();
     iCountTestcases++;
     if((object)str1 == (object)str2)
       {
       iCountErrors++;
       printerr( "Error_230a! References of a built string with value String.Empty() and a string literal of .Empty() are equal");
       }
     IntlStrings intl = new IntlStrings();
     sb2 = new StringBuilder().Append(intl.GetString(18, false, true));
     str2 = sb2.ToString();
     iCountTestcases++;
     if((object)str1 == (object)str2)
       {
       iCountErrors++;
       printerr( "Error_23ekk2");
       }
     str1 = String.Empty;
     str2 = String.Empty;
     strLoc = "Loc_230emb";
     sb2 = new StringBuilder().Append(String.Empty);
     str2 = String.Intern(sb2.ToString());
     iCountTestcases++;
     if((object)str1 != (object)str2)
       {
       iCountErrors++;
       printerr( "Error_230b! References of an interned .Empty() string and string literal .Empty() are not equal");
       }
     strLoc = "Loc_231loa";
     sb1 = new StringBuilder().Append("a").Append("b").Append("c").Append("d").Append("e").Append("f").Append("g").Append("h").Append("i").Append("j").Append("k").Append("l").Append("m").Append("n").Append("o").Append("p").Append("q").Append("r").Append("s").Append("t").Append("u").Append("v").Append("w").Append("x").Append("y").Append("z").Append("aa").Append("bb").Append("cc").Append("dd").Append("ee").Append("ff").Append("gg").Append("hh").Append("ii").Append("jj").Append("kk").Append("ll").Append("mm").Append("nn").Append("oo").Append("pp").Append("qq").Append("rr").Append("ss").Append("tt").Append("uu").Append("vv").Append("ww").Append("xx").Append("yy").Append("zz");
     str1 = sb1.ToString();
     str2 = "abcdefghijklmnopqrstuvwxyzaabbccddeeffgghhiijjkkllmmnnooppqqrrssttuuvvwwxxyyzz";
     iCountTestcases++;
     if((object)str1 == (object)str2)
       {
       iCountErrors++;
       printerr( "Error_231a! References of a literal long string and a built long string are equal");
       }
     strLoc = "Loc_231lob";
     str1 = String.Intern(sb1.ToString());
     iCountTestcases++;
     if((object)str1 != (object)str2)
       {
       iCountErrors++;
       printerr( "Error_231b! References of a literal long string and an interned built long string are not equal");
       }
     strLoc = "Loc_232syma";
     sb1 = new StringBuilder().Append("abc").Append("`").Append("-").Append("=").Append("[").Append("]").Append(";").Append("'").Append(",").Append(".").Append("/").Append("~").Append("!").Append("@").Append("#").Append("$").Append("%").Append("^").Append("&").Append("*").Append("(").Append(")").Append("_").Append("+").Append("{").Append("}").Append("|").Append(":").Append("<").Append(">").Append("?");
     str1 = sb1.ToString();
     str2 = "abc`-=[];',./~!@#$%^&*()_+{}|:<>?";
     iCountTestcases++;
     if((object)str1 == (object)str2)
       {
       iCountErrors++;
       printerr( "Error_232a! References of a literal string having symbols and a built string having symbols are equal");
       }
     strLoc = "Loc_232symb";
     str1 = String.Intern(sb1.ToString());
     iCountTestcases++;
     if((object)str1 != (object)str2)
       {
       iCountErrors++;
       printerr( "Error_232b! References of a literal string having symbols and an interned built string having symbols are not equal");
       }
     strLoc = "Loc_233numa";
     sb1 = new StringBuilder().Append("xyz").Append("1").Append("2").Append("3").Append("4").Append("5").Append("6").Append("7").Append("8").Append("9").Append("0");
     str1 = sb1.ToString();
     str2 = "xyz1234567890";
     iCountTestcases++;
     if((object)str1 == (object)str2)
       {
       iCountErrors++;
       printerr( "Error_233a! References of a literal string having alphanumerics and a built string having alphanumerics are equal");
       }
     strLoc = "Loc_233numb";
     str1 = String.Intern(sb1.ToString());
     iCountTestcases++;
     if((object)str1 != (object)str2)
       {
       iCountErrors++;
       printerr( "Error_233b! References of a literal string having alphanumerics and an interned built string having alphanumerics are not equal");
       }
     } catch (Exception exc_general ) {
     ++iCountErrors;
     Console.WriteLine (s_strTFAbbrev + " : Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
     }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public void printerr ( String err )
   {
   Console.WriteLine ("POINTTOBREAK: ("+ s_strTFAbbrev + ") "+ err);
   }
 public void printinfo ( String info )
   {
   Console.WriteLine ("EXTENDEDINFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co5800Intern cbA = new Co5800Intern();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFName + s_strTFPath);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
}
