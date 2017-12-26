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
using GenStrings;
using System;
public class Co5152ToCharArray
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "String.ToCharArray()";
 public static readonly String s_strTFName        = "Co5152ToCharArray.";
 public static readonly String s_strTFAbbrev      = "Cb5150";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "";
   IntlStrings intl = new IntlStrings();
   String tempString = intl.GetString(4, true, true);
   String str1;
   Char[] chArr;
   Char[] chResultArr1 = {};
   Char[] chResultArr2 = {tempString[0], tempString[1], tempString[2], tempString[3]};
   Char[] chResultArr3 = {'a','k','k','a', ' ', 'b','a','k','k','a'};
   Char[] chResultArr4 = {'\\', '\\'};
   Char[] chResultArr5 = {'M', 'I', 'C', 'R', 'O', 'S', 'O', 'F', 'T'};
   Char[] chResultArr6 = {'C', 'O', 'M', '+'};
   Char[] chResultArr7 = {'1','2','3','4','5'};
   Char[] chResultArr8 = {']','['};
   String[] strTestArray = {"",
			    tempString,
			    "akka bakka",
			    "\\\\",
			    "MICROSOFT",
			    "COM+",
			    "12345",
			    "]["
   };
   try {
   LABEL_860_GENERAL:
   do
     {
     strBaseLoc = "Loc_1000uq_";
     chArr = strTestArray[0].ToCharArray();
     iCountTestcases++;
     if(chArr.Length != chResultArr1.Length)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_983cd");
       }
     iCountTestcases++;
     for (int ii = 0 ; ii < chResultArr1.Length ; ii++)
       {
       strLoc = strBaseLoc + ii.ToString();
       if(chArr[ii] != chResultArr1[ii])
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_598uq");
	 }
       }
     strBaseLoc = "Loc_1100fd_";
     chArr = strTestArray[1].ToCharArray();
     iCountTestcases++;
     if(chArr.Length != chResultArr2.Length)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_930oo");
       }
     iCountTestcases++;
     for( int ii = 0  ; ii < chResultArr2.Length ; ii++)
       {
       strLoc = strBaseLoc + ii.ToString();
       if(chArr[ii] != chResultArr2[ii])
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_803ii");
	 }
       }
     strBaseLoc = "Loc_1200ds_";
     chArr = strTestArray[2].ToCharArray();
     iCountTestcases++;
     if(chArr.Length != chResultArr3.Length)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_932oo");
       }
     iCountTestcases++;
     for( int ii = 0 ; ii < chResultArr3.Length ; ii++)
       {
       strLoc = strBaseLoc + ii.ToString();
       if(chArr[ii] != chResultArr3[ii])
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_348uw");
	 }
       }
     strBaseLoc = "Loc_1300re_";
     chArr = strTestArray[3].ToCharArray();
     iCountTestcases++;
     if(chArr.Length != chResultArr4.Length)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_934oo");
       }
     iCountTestcases++;
     for ( int ii = 0 ; ii < chResultArr4.Length ; ii++)
       {
       strLoc = strBaseLoc + ii.ToString();
       if(chArr[ii] != chResultArr4[ii])
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_903ww! ii=="+ii+", chArr[ii]=="+chArr[ii]+", chResultArr[ii]="+chResultArr4[ii]);
	 }
       }
     strBaseLoc = "Loc_1400er_";
     chArr = strTestArray[4].ToCharArray();
     iCountTestcases++;
     if(chArr.Length != chResultArr5.Length)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_936oo");
       }
     iCountTestcases++;
     for ( int ii = 0;ii<chResultArr5.Length;ii++)
       {
       strLoc = strBaseLoc + ii.ToString();
       if(chArr[ii] != chResultArr5[ii])
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_349cs");
	 }
       }
     strBaseLoc = "Loc_1500ei_";
     chArr = strTestArray[5].ToCharArray();
     iCountTestcases++;
     if(chArr.Length != chResultArr6.Length)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_940oo");
       }
     iCountTestcases++;
     for ( int ii = 0 ; ii < chResultArr6.Length ; ii++)
       {
       strLoc = strBaseLoc + ii.ToString();
       if(chArr[ii] != chResultArr6[ii])
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_490op");
	 }
       }
     strBaseLoc = "Loc_1600du_";
     chArr = strTestArray[6].ToCharArray();
     iCountTestcases++;
     if(chArr.Length != chResultArr7.Length)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_945oo");
       }
     iCountTestcases++;
     for(int ii = 0;ii < chResultArr7.Length ; ii++)
       {
       strLoc = strBaseLoc + ii.ToString();
       if(chArr[ii] != chResultArr7[ii])
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_893ao");
	 }
       }
     strBaseLoc = "Loc_1700uf_";
     chArr = strTestArray[7].ToCharArray();
     iCountTestcases++;
     if(chArr.Length != chResultArr8.Length)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_950oo");
       }
     iCountTestcases++;
     for ( int ii = 0 ; ii < chResultArr8.Length ; ii++)
       {
       strLoc = strBaseLoc + ii.ToString();
       if(chArr[ii] != chResultArr8[ii])
	 {
	 iCountErrors++;
	 Console.WriteLine( s_strTFAbbrev+ "Err_4398u");
	 }
       }
     } while (false);
   } catch (Exception exc_general ) {
   ++iCountErrors;
   Console.WriteLine(s_strTFAbbrev +" Error Err_8888yyy!  strLoc=="+ strLoc +", exc_general=="+exc_general);
   }
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co5152ToCharArray cbA = new Co5152ToCharArray();
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
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
}
