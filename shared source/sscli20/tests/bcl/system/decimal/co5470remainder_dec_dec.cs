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
public class Co5470Remainder_dec_dec
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Decimal.Remainder(Decimal, Decimal)";
 public static String s_strTFName        = "Co5470Remainder_dec_dec.cs";
 public static String s_strTFAbbrev      = "Co5470";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   Decimal[] arrD1 = {Decimal.MinValue, (Decimal)Int64.MinValue, (Decimal)Int32.MinValue, (Decimal)Int16.MinValue, -128m, -1m, 0, 1, 
		      127, 255, (Decimal)Int16.MaxValue, (Decimal)Int32.MaxValue, (Decimal)Int64.MaxValue, Decimal.MaxValue,
   };
   Decimal[] arrD2 = {Decimal.MinValue, (Decimal)Int64.MinValue, (Decimal)Int32.MinValue, (Decimal)Int16.MinValue, -128m, -1m, 0, 1, 
		      127, 255, (Decimal)Int16.MaxValue, (Decimal)Int32.MaxValue, (Decimal)Int64.MaxValue, Decimal.MaxValue,
   };
   Object[] arrD3 = {(byte)1, (byte)255, 
		     (short)1, (short)-32768, (short)32767,
		     (int)1, (int)-2147483648, (int)2147483647,
		     (long)1, (long)-9223372036854775808, (long)9223372036854775807,
   };
   Decimal[] arrD4 = {Decimal.MinValue, Decimal.MaxValue};
   Decimal[] results = {1m, 1m, 255m, 255m, 
			1m, 1m, -32768m, -32768m, 32767m, 32767m, 
			1m, 1m, -2147483648m, -2147483648m, 2147483647m, 2147483647m,
			1m, 1m, -9223372036854775808m, -9223372036854775808, 9223372036854775807m, 9223372036854775807m, 
   };
   try
     {
     Decimal dec1, dec2;
     strLoc = "Loc_2098u";
     dec1 = new Decimal(50);
     dec2 = new Decimal();
     iCountTestcases++;
     try {
     Decimal.Remainder(dec1, dec2);
     iCountErrors++;
     printerr( "Error_908hc! DivideByZeroException expected");
     } catch (DivideByZeroException) {
     } catch (Exception exc) {
     iCountErrors++;
     printerr( "Error_09c98! DivideByZeroException expected, got exc=="+exc.ToString());
     }
     iCountTestcases++;
     try {
     Console.WriteLine(dec1%dec2);
     iCountErrors++;
     printerr( "Error_2908x! DivideByZeroException expected");
     } catch (DivideByZeroException) {
     } catch (Exception exc) {
     iCountErrors++;
     printerr( "Error_109ux! DivideByZeroException expected, got exc=="+exc.ToString());
     }
     strLoc = "Loc_0ucx8";
     dec1 = 5;
     dec2 = 3;
     iCountTestcases++;
     if(Decimal.Remainder(dec1, dec2) != 2)
       {
       iCountErrors++;
       printerr( "Error_209cx! Expected==2 , got value=="+Decimal.Remainder(5, 3).ToString());
       }
     iCountTestcases++;
     if(dec1%dec2 != 2)
       {
       iCountErrors++;
       printerr( "Error_289cy! Expected==2, got value=="+(dec1%dec2).ToString());
       }
     strLoc = "Loc_109xu";
     dec1 = Decimal.MaxValue;
     dec2 = Decimal.MaxValue;
     iCountTestcases++;
     if(Decimal.Remainder(dec1, dec2) != 0)
       {
       iCountErrors++;
       printerr( "Error_198cy! Expected==0, got value=="+Decimal.Remainder(dec1, dec2).ToString());
       }
     iCountTestcases++;
     if(dec1%dec2 != 0)
       {
       iCountErrors++;
       printerr( "Error_298xy! Expected==0, got value=="+(dec1%dec2).ToString());
       }
     strLoc = "Loc_39ccs";
     dec1 = 10;
     dec2 = -3;
     iCountTestcases++;
     if(Decimal.Remainder(dec1, dec2) != 1)
       {
       iCountErrors++;
       printerr( "Error_298cx! Expected==1, got value=="+Decimal.Remainder(dec1, dec2).ToString());
       }
     dec1 = -10;
     dec2 = 3;
     iCountTestcases++;
     if(dec1%dec2 != -1)
       {
       iCountErrors++;
       printerr( "Erro_28hxc! Expected==-1, got value=="+(dec1%dec2).ToString());
       }
     strLoc = "Loc_129dc";
     dec1 = Decimal.MaxValue;
     dec2 = Decimal.MinValue;
     iCountTestcases++;
     if(Decimal.Remainder(dec1, dec2) != 0)
       {
       iCountErrors++;
       printerr( "Error_198yc! Expected==0, got value=="+Decimal.Remainder(dec1, dec2).ToString());
       }
     strLoc = "Loc_189hc";
     dec1 = new Decimal(17.3);
     dec2 = 3;
     iCountTestcases++;
     if(dec1%dec2 != new Decimal(2.3))
       {
       iCountErrors++;
       printerr( "Error_28fy3! Expected==2.3, got value=="+(dec1%dec2).ToString());
       }
     strLoc = "Loc_98ycy";
     dec1 = new Decimal(8.55);
     dec2 = new Decimal(2.25);
     iCountTestcases++;
     if(Decimal.Remainder(dec1,dec2) != new Decimal(1.8))
       {
       iCountErrors++;
       printerr( "Error_298chx! Expected==0.8, got value=="+Decimal.Remainder(dec1,dec2).ToString());
       }
     strLoc = "Loc_98ycy";
     iCountTestcases++;
     try{
     for(int i=0; i<arrD1.Length; i++){
     for(int j=0; j<arrD2.Length; j++){
     dec1 = Decimal.Remainder(arrD1[i], arrD2[j]);
     }
     }
     }catch (DivideByZeroException){
     }catch(Exception ex){
     iCountErrors++;
     Console.WriteLine( "Err_857wG! Exception thrown, ", ex);
     }
     strLoc = "Loc_98ycy";
     iCountTestcases++;
     for(int i=0, k=0; i<arrD3.Length; i++){
     for(int j=0; j<arrD4.Length; j++){					
     if(results[k]!=Decimal.Remainder(Convert.ToDecimal(arrD3[i]), arrD4[j])){
     iCountErrors++;
     Console.WriteLine("Err_8745t_{0}_{1} Wrong result returned, Expected: {2} Returned: {3}", i, j, results[k], Decimal.Remainder(Convert.ToDecimal(arrD3[i]), arrD4[j]));
     }
     k++;
     }
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
     Console.WriteLine("FAiL! "+s_strTFName+" ,iCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
     return false;
     }
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
   Co5470Remainder_dec_dec cbA = new Co5470Remainder_dec_dec();
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
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
