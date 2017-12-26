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
public class Co9101DoubleToInt64Bits_dbl
{
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "BitConverter.DoubleToInt64Bits";
 public static String s_strTFName        = "Co9101DoubleToInt64Bits_dbl.cs";
 public static String s_strTFAbbrev      = "Co9101";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_0000";
   try
     {
     Random rand = new Random((int)DateTime.Now.Ticks);
     Double dbl ;
     Int64 iResult ;
     strLoc = "Loc_0001";
     for(int iLoop = 0 ; iLoop < 10 ; iLoop++ ){
     iCountTestcases++;
     dbl = rand.NextDouble();                                
     iResult = BitConverter.DoubleToInt64Bits( dbl ); 
     if( dbl != BitConverter.ToDouble( BitConverter.GetBytes(iResult) , 0 )){
     iCountErrors++;
     printerr( "Error_0002! DoubleToInt64Bits method is not working ::" + iResult);
     }
     }  
     strLoc = "Loc_0003";
     iCountTestcases++;  
     dbl = Double.MaxValue + Double.MaxValue;
     iResult = BitConverter.DoubleToInt64Bits( dbl ); 
     if( dbl != BitConverter.ToDouble( BitConverter.GetBytes(iResult) , 0 )){
     iCountErrors++;
     printerr( "Error_0004! DoubleToInt64Bits method is not working ::" + iResult);
     }
     strLoc = "Loc_0005";
     iCountTestcases++;
     dbl = Double.MinValue + Double.MinValue  ;
     iResult = BitConverter.DoubleToInt64Bits( dbl ); 
     if( dbl != BitConverter.ToDouble( BitConverter.GetBytes(iResult) , 0 )){
     iCountErrors++;
     printerr( "Error_0006! DoubleToInt64Bits method is not working ::" + iResult);
     }
     strLoc = "Loc_0007";
     iCountTestcases++;
     dbl = Double.Epsilon ;
     iResult = BitConverter.DoubleToInt64Bits( dbl ); 
     if( dbl != BitConverter.ToDouble( BitConverter.GetBytes(iResult) , 0 )){
     iCountErrors++;
     printerr( "Error_0008! DoubleToInt64Bits method is not working ::" + iResult);
     }
     strLoc = "Loc_0009";
     iCountTestcases++;
     dbl = Double.NaN ;
     iResult = BitConverter.DoubleToInt64Bits( dbl ); 
     if(! Double.IsNaN( BitConverter.ToDouble( BitConverter.GetBytes(iResult) , 0 ) )){
     iCountErrors++;
     printerr( "Error_0010! DoubleToInt64Bits method is not working ::" + iResult);
     }
     strLoc = "Loc_0011";
     iCountTestcases++;
     dbl = Double.NegativeInfinity ;
     iResult = BitConverter.DoubleToInt64Bits( dbl ); 
     if( dbl != BitConverter.ToDouble( BitConverter.GetBytes(iResult) , 0 )){
     iCountErrors++;
     printerr( "Error_0012! DoubleToInt64Bits method is not working ::" + iResult);
     }
     strLoc = "Loc_0013";
     iCountTestcases++;
     dbl = Double.PositiveInfinity ;
     iResult = BitConverter.DoubleToInt64Bits( dbl ); 
     if( dbl != BitConverter.ToDouble( BitConverter.GetBytes(iResult) , 0 )){
     iCountErrors++;
     printerr( "Error_0014! DoubleToInt64Bits method is not working ::" + iResult);
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
     Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString());
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
   Co9101DoubleToInt64Bits_dbl dtoi = new Co9101DoubleToInt64Bits_dbl();
   try {
   bResult = dtoi.runTest();
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
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
