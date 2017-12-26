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
public class Co5155Insert_is
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "String.Insert(int, String)";
 public static readonly String s_strTFName        = "Co5155Insert_is.";
 public static readonly String s_strTFAbbrev      = "Co5155";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String str1;
   String strInsert;
   int iStartIndex;
   try {
   int[] iArrInvalidValues = new Int32[]{ -1, -2, -100, -1000, -10000, -100000, -1000000, -10000000, -100000000, -1000000000, Int32.MinValue};
   int[] iArrLargeValues = new Int32[]{ Int32.MaxValue, Int32.MaxValue-1, Int32.MaxValue/2 , Int32.MaxValue/10 , Int32.MaxValue/100 };
   int[] iArrValidValues = new Int32[]{ 10000, 100000 , Int32.MaxValue/200 , Int32.MaxValue/1000 };
   iCountTestcases++;
   String strNewString = "This is a tesing string" ;
   String strTemp = new String( 'a', 10000);
   for(int iLoop = 0 ; iLoop < iArrInvalidValues.Length ; iLoop++ ){
   try
     {
     strNewString = strNewString.Insert(iArrInvalidValues[iLoop], strTemp);
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
     strNewString = strNewString.Insert(iArrLargeValues[iLoop], strTemp);
     iCountErrors++;                        
     Console.Error.WriteLine( "Error_1111!! Expected exception not occured");
     } catch ( ArgumentOutOfRangeException ){
     } catch ( Exception ex )
       {
       Console.Error.WriteLine( "Error_3333!!! Unexpected exception " + ex.ToString() );
       iCountErrors++ ;
       }
   }
   iCountTestcases++;
   strTemp =  new String( 'a' , Int32.MaxValue/200 );
   strNewString = strTemp ;
   for(int iLoop = 0 ; iLoop < iArrValidValues.Length ; iLoop++ ){
   try
     {                            
     strNewString = strNewString.Insert(iArrValidValues[iLoop], strTemp);
     if ( strNewString.Length != (Int32.MaxValue/200 ) * ( 2+iLoop) ){
     iCountErrors++;
     Console.Error.WriteLine( "Error_6666!!!! Incorrect string length.... Expected...{0},  Actual...{1}", (Int32.MaxValue/200 ) * ( 1+iLoop), strNewString.Length );
     }
     } catch ( Exception ex ){
     Console.Error.WriteLine( "Error_7777!!! Unexpected exception " + ex.ToString() );
     iCountErrors++ ;
     }
   }
   strLoc = "Loc_38947";
   str1 = "\t\t";
   strInsert = "  ";
   iStartIndex = -1;
   iCountTestcases++;
   try {
   str1.Insert(iStartIndex, strInsert);
   iCountErrors++;
   Console.WriteLine( s_strTFAbbrev+ "Err_4982u");
   } catch (ArgumentException ) {}
   catch ( Exception exc) {
   iCountErrors++;
   Console.WriteLine( s_strTFAbbrev+ "Err_9083u! , exc=="+exc);
   }
   strLoc = "Loc_129ew";
   str1 = "\t\t";
   strInsert = "  ";
   iStartIndex = 3;
   iCountTestcases++;
   try {
   str1.Insert(iStartIndex, strInsert);
   iCountErrors++;
   Console.WriteLine( s_strTFAbbrev+ "Err_983uq");
   } catch ( ArgumentException) {}
   catch ( Exception exc) {
   iCountErrors++;
   Console.WriteLine( s_strTFAbbrev+ "Err_5388g! , exc=="+exc);
   }
   strLoc = "Loc_538wi";
   str1 = "\t\t";
   strInsert = null;
   iStartIndex = 0;
   iCountTestcases++;
   try {
   str1.Insert(iStartIndex, strInsert);
   iCountErrors++;
   Console.WriteLine( s_strTFAbbrev+ "Err_128qu");
   } catch ( ArgumentException) {}
   catch ( Exception exc) {
   iCountErrors++;
   Console.WriteLine( s_strTFAbbrev+ "Err_3948u! , exc=="+exc);
   }
   strLoc = "Loc_3894u";
   str1 = "";
   strInsert = "This is a string";
   iStartIndex = 0;
   iCountTestcases++;
   if (!str1.Insert(iStartIndex, strInsert).Equals(strInsert))
     {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_438du");
     }
   IntlStrings intl = new IntlStrings();
   str1 = intl.GetString(19, true, true);
   strInsert = intl.GetString(12, false, true);
   iStartIndex = 0;
   iCountTestcases++;
   if (!str1.Insert(iStartIndex, strInsert).Equals(String.Concat(strInsert, str1)))
     {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_299138du");
     }
   strLoc = "Loc_3948r";
   str1 = "This is a string";
   strInsert = "\t\t\t\t";
   iStartIndex = 0;
   iCountTestcases++;
   if(!str1.Insert(iStartIndex, strInsert).Equals("\t\t\t\tThis is a string"))
     {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_8904e");
     }
   strLoc = "Loc_3498j";
   str1 = "This is a \n\n";
   strInsert = "string";
   iStartIndex = str1.ToCharArray().Length;
   iCountTestcases++;
   if(!str1.Insert(iStartIndex, strInsert).Equals(str1+strInsert))
     {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_1289m");
     }
   strLoc = "Loc_3487y";
   str1 = "\t is a string";
   strInsert = "This\t";
   iStartIndex = 1;
   iCountTestcases++;
   if(!str1.Insert(iStartIndex, strInsert).Equals("\tThis\t is a string"))
     {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_3739c");
     }
   strLoc = "Loc_238us";
   str1 = "This is a string";
   strInsert = "!@#$%^&*()";
   iStartIndex = str1.ToCharArray().Length-1;
   iCountTestcases++;
   if(!str1.Insert(iStartIndex, strInsert).Equals("This is a strin!@#$%^&*()g"))
     {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_34fdy");
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
   Co5155Insert_is cbA = new Co5155Insert_is();
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
