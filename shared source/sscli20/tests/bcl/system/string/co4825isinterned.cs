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
public class Co4825IsInterned
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "String.IsIntern";
 public static String s_strTFName        = "Co4825IsInterned";
 public static String s_strTFAbbrev      = "Co4825";
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
     StringBuilder sb1;
     strLoc = "Loc_299nu";
     try {
     String.IsInterned (null);
     }
     catch (ArgumentNullException ) {
     }
     strLoc = "Loc_230ema.2";
     iCountTestcases++;
     if(String.IsInterned (new StringBuilder (String.Empty).ToString ()) == null)	
       {
       iCountErrors++;
       printerr( "Error_230a.2! String.Empty should be already interned!");
       }
     strLoc = "Loc_231loa";
     sb1 = new StringBuilder().Append("a").Append("b").Append("c").Append("d").Append("e").Append("f").Append("g").Append("h").Append("i").Append("j").Append("k").Append("l").Append("m").Append("n").Append("o").Append("p").Append("q").Append("r").Append("s").Append("t").Append("u").Append("v").Append("w").Append("x").Append("y").Append("z").Append("aa").Append("bb").Append("cc").Append("dd").Append("ee").Append("ff").Append("gg").Append("hh").Append("ii").Append("jj").Append("kk").Append("ll").Append("mm").Append("nn").Append("oo").Append("pp").Append("qq").Append("rr").Append("ss").Append("tt").Append("uu").Append("vv").Append("ww").Append("xx").Append("yy").Append("zz");
     str1 = sb1.ToString();
     strLoc = "Loc_231loa.2";
     iCountTestcases++;
     if(String.IsInterned (sb1.ToString()) != null)
       {
       iCountErrors++;
       printerr( "Error_231a.2! References of a built string can't be already interned!");
       }
     IntlStrings intl = new IntlStrings();
     sb1 = new StringBuilder().Append(intl.GetString(4, true, true)).Append(intl.GetString(8, false, true));
     str1 = sb1.ToString();
     if(String.IsInterned (sb1.ToString()) != null)
       {
       iCountErrors++;
       printerr( "Error_234949 References of a built string can't be already interned!");
       }
     strLoc = "Loc_232syma";
     sb1 = new StringBuilder().Append("abc").Append("`").Append("-").Append("=").Append("[").Append("]").Append(";").Append("'").Append(",").Append(".").Append("/").Append("~").Append("!").Append("@").Append("#").Append("$").Append("%").Append("^").Append("&").Append("*").Append("(").Append(")").Append("_").Append("+").Append("{").Append("}").Append("|").Append(":").Append("<").Append(">").Append("?");
     str1 = sb1.ToString();
     str2 = "abc`-=[];',./~!@#$%^&*()_+{}|:<>?";
     strLoc = "Loc_232syma.2";
     iCountTestcases++;
     if(String.IsInterned (sb1.ToString()) == null)
       {
       iCountErrors++;
       printerr( "Error_231yma.2! even though built this string should 've been interned at JIT time!");
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
   Co4825IsInterned cbA = new Co4825IsInterned();
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
