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
public class Co5153Split_chArr_int
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "String.ToCharArray()";
 public static readonly String s_strTFName        = "Co5153Split_chArr_int.";
 public static readonly String s_strTFAbbrev      = "Co5153";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "";
   String str1;
   String strResult;
   String strTestString;
   String strSeperator;
   String [] strSplit;
   String [] result1 = {"ab","ba","cd","dc","ef","fe"};
   String [] strExpected = new String[10];
   bool passed;
   int count;
   try {
   Char[] chrArr ;
   String[] strArr ;
   int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
   int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };
   int[] iArrValidValues = new Int32[]{ 10000, 100000 , 50000 , 1000 , 10, 1, 0 };
   iCountTestcases++;
   String strNewString = "This is a tesing string" ;
   chrArr = strNewString.ToCharArray();
   for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
   try
     {
     strArr = strNewString.Split(chrArr , iArrInvalidValues[iLoop]);
     iCountErrors++;                        
     Console.Error.WriteLine( "Error_0000!! Expected exception not occured");
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_2222!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   for(int iLoop = 0 ; iLoop < iArrLargeValues.Length ; iLoop++ ){
   try
     {
     strArr = strNewString.Split(chrArr , iArrLargeValues[iLoop]);                        
     if( strArr.Length != 24 ){
     iCountErrors++;                        
     Console.Error.WriteLine( "Error_1111!! Expected exception not occured");
     }        
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_3333!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   strNewString =  new String( 'a' , 100000 );
   chrArr = new Char[]{'a'};
   for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
   try
     {          
     strArr = strNewString.Split(chrArr , iArrValidValues[iLoop]);
     if ( strArr.Length !=  iArrValidValues[iLoop]){
     iCountErrors++;
     Console.Error.WriteLine( "Error_4444!!!! Incorrect string array length.... Expected...{0},  Actual...{1}", iArrValidValues[iLoop], strArr.Length );
     }
     } catch ( Exception ex ){
     Console.Error.WriteLine( "Error_5555!!! Unexpected exception " + ex.ToString() );
     iCountErrors++ ;
     }
   }
   strLoc = "Loc_984uw";
   strTestString = "";
   strSeperator = "";
   count = -1;
   iCountTestcases++;
   try {
   strTestString.Split(strSeperator.ToCharArray(), count);
   iCountErrors++;
   Console.WriteLine( s_strTFAbbrev+ "Err_328qu");
   } catch (ArgumentException aExc) {}
   catch (Exception exc) {
   iCountErrors++;
   Console.WriteLine( s_strTFAbbrev+ "Err_583ue");
   }
   strLoc = "Loc_9435u";
   strTestString = "";
   strSeperator = "";
   count = 1;
   iCountTestcases++;
   strSplit = strTestString.Split(strSeperator.ToCharArray(), count);
   if(!strSplit[0].Equals(String.Empty))
     {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_9384u! , strSplit.length=="+strSplit[0]);
     }
   iCountTestcases++;
   if(strSplit.Length != 1)
     {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_34sdy");
     }
   strLoc = "Loc_43uww";
   str1 = "ab\t\tbc\n\n   \rcd";
   strExpected[0] = "ab";
   strExpected[1] = String.Empty;
   strExpected[2] = "bc";
   strExpected[3] = "\n   \rcd";
   count = 4;
   strSplit = str1.Split(String.Empty.ToCharArray(), count);
   for(int i=0;i<strSplit.Length;i++)	{
   iCountTestcases++;
   if(!(strSplit[i].Equals(strExpected[i])))
     {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_489uq");
     Console.Error.WriteLine ("Expected: Expected["+i+"] ==" + strExpected[i]);
     Console.Error.WriteLine ("Returned: strSplit["+i+"] ==" + strSplit[i]);
     }
   }
   IntlStrings intl = new IntlStrings(631571587507955825);
   String tempString1 = intl.GetString(1, true, true, true).Replace("1","3").Replace("2", "3");
   String tempString2 = intl.GetString(1, false, true, true).Replace("1","3").Replace("2", "3");
   String tempString3 = intl.GetString(1, true, true, true).Replace("1","3").Replace("2", "3");
   str1 = String.Concat(tempString1, "1", tempString2);
   str1 = String.Concat(str1, "2", tempString3);  
   strExpected[0] = tempString1;
   strExpected[1] = tempString2;
   strExpected[2] = tempString3;
   count = 3;
   String strSp = "12";
   strSplit = str1.Split(strSp.ToCharArray(), count);
   for(int i=0;i<3;i++)	{
   iCountTestcases++;
   if(!(strSplit[i].Equals(strExpected[i])))
     {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_4339uq");
     Console.Error.WriteLine ("Expected: Expected["+i+"] ==" + strExpected[i]);
     Console.Error.WriteLine ("Returned: strSplit["+i+"] ==" + strSplit[i]);
     }
   }
   strLoc = "Loc_847ca";
   str1 = "ab!ba#cd@dc!ef@fe";
   strSeperator = "!@#";
   strExpected[0] = "ab";
   strExpected[1] = "ba";
   strExpected[2] = "cd@dc!ef@fe";
   strSplit= str1.Split(strSeperator.ToCharArray(), 3);
   passed = true;
   for (int i=0;i<strSplit.Length;i++) {
   iCountTestcases++;
   if(!(strSplit[i].Equals(strExpected[i])))
     {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_5982i");
     Console.Error.WriteLine ("Expected: strExpected["+i+"] == " + result1[i]);
     Console.Error.WriteLine ("Returned: strSplit["+i+"] == " + strSplit[i]);
     passed = false;
     }
   }
   strLoc = "Loc_958uw";
   str1 = "abEbaAcdkdcCefDfe";
   strSeperator = "ABCDEFk";
   strExpected[0] = "ab";
   strExpected[1] = "ba";
   strExpected[2] = "cd";
   strExpected[3] = "dcCefDfe";
   strSplit = str1.Split(strSeperator.ToCharArray(), 4);
   for(int i=0;i<strSplit.Length; i++) {
   iCountTestcases++;
   if(!(strSplit[i].Equals(strExpected[i])))
     {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_4398i");
     Console.Error.WriteLine ("Expected: strExpected["+i+"] == " + result1[i]);
     Console.Error.WriteLine ("Returned: strSplit["+i+"] == " + strSplit[i]);
     }
   }
   strLoc = "Loc_238gg";
   str1 = "ab.ba1cd7dc6ef9fe4";
   strSeperator = "145679.";
   strSplit = str1.Split(strSeperator.ToCharArray(), 2);
   strExpected[0] = "ab";
   strExpected[1] = "ba1cd7dc6ef9fe4";
   for(int i=0;i<strSplit.Length;i++) {
   iCountTestcases++;
   if(!(strSplit[i].Equals(strExpected[i])))
     {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_9348u");
     Console.Error.WriteLine ("Expected: strExpected["+i+"] == " + result1[i]);
     Console.Error.WriteLine ("Returned: strSplit["+i+"] == " + strSplit[i]);
     }
   }
   strLoc = "Loc_8732q";
   str1 = "abcdefghijlmnopqrs";
   strSeperator = "hi";
   strSplit = str1.Split(strSeperator.ToCharArray(), 1);
   iCountTestcases++;
   if(!(strSplit[0].Equals(str1)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_666m (Cb3043Split.)");
     }
   strLoc = "Loc_84sde";
   str1 = "abcdefghijkl";
   strSeperator = "DEF";
   strSplit = str1.Split(strSeperator.ToCharArray(), 7);
   iCountTestcases++;
   if(!(strSplit[0].Equals(str1)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_666n (Cb3043Split.)");
     }
   strLoc = "Loc_374qu";
   str1 = "abcdefghiDSFGJSD";
   strSplit = str1.Split(String.Empty.ToCharArray(), 4);
   iCountTestcases++;
   if(!(strSplit[0].Equals(str1)))
     {
     iCountErrors++;
     Console.Error.WriteLine("POINTTOBREAK: find E_666b (Cb3043Split.)");
     }
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
   Co5153Split_chArr_int cbA = new Co5153Split_chArr_int();
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
