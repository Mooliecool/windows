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
public class Co9102Int64BitsToDouble_i64   
{
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "BitConverter.Int64BitsToDouble";
 public static String s_strTFName        = "Co9102Int64BitsToDouble_i64.cs";
 public static String s_strTFAbbrev      = "Co9102";
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_0000";
   try
     {
     Double dblResult ;
     Int64 intVal ;                        
     Random rand = new Random((int)DateTime.Now.Ticks);
     strLoc = "Loc_0001";
     for(int iLoop = 0 ; iLoop < 10 ; iLoop++ ){
     iCountTestcases++;
     intVal = rand.Next(0, Int32.MaxValue) * rand.Next(0,Int32.MaxValue);
     dblResult = BitConverter.Int64BitsToDouble( intVal ); 
     if( intVal != BitConverter.ToInt64( BitConverter.GetBytes(dblResult) , 0 )){
     iCountErrors++;
     printerr( "Error_0002! Int64BitsToDouble method is not working ::" + dblResult);
     }
     }  
     strLoc = "Loc_0003";
     iCountTestcases++;  
     intVal = unchecked(Int64.MaxValue + 1);
     dblResult = BitConverter.Int64BitsToDouble( intVal );  
     if( intVal != BitConverter.ToInt64( BitConverter.GetBytes(dblResult) , 0 )){
     iCountErrors++;
     printerr( "Error_0004! Int64BitsToDouble method is not working ::" + dblResult);
     }
     strLoc = "Loc_0003";
     iCountTestcases++;  
     intVal = Int64.MaxValue;
     dblResult = BitConverter.Int64BitsToDouble( intVal );  
     if( intVal != BitConverter.ToInt64( BitConverter.GetBytes(dblResult) , 0 ))
       {
       iCountErrors++;
       printerr( "Error_0004! Int64BitsToDouble method is not working ::" + dblResult);
       }
     strLoc = "Loc_0005";
     iCountTestcases++;
     intVal = unchecked(Int64.MinValue - 1)  ;
     dblResult = BitConverter.Int64BitsToDouble( intVal );  
     if( intVal != BitConverter.ToInt64( BitConverter.GetBytes(dblResult) , 0 )){
     iCountErrors++;
     printerr( "Error_0006! Int64BitsToDouble method is not working ::" + dblResult);
     }
     strLoc = "Loc_0005";
     iCountTestcases++;
     intVal = Int64.MinValue;
     dblResult = BitConverter.Int64BitsToDouble( intVal );  
     if( intVal != BitConverter.ToInt64( BitConverter.GetBytes(dblResult) , 0 ))
       {
       iCountErrors++;
       printerr( "Error_0006! Int64BitsToDouble method is not working ::" + dblResult);
       }
     } 
   catch (Exception exc_general ) 
     {
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
   Co9102Int64BitsToDouble_i64 dtoi = new Co9102Int64BitsToDouble_i64();
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
