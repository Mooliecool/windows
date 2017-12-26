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
using System.Collections;
using System.Globalization;
using System.Text;
using System.Threading;
public class Co7022ToType_Type
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "DBNull.ToType(Type)"; 
 public static String s_strTFName        = "Co7022ToType_Type.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = "";
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   try
     {
     String strRet;
     Int32 i32Ret;
     Type typParam = null;
     strLoc = "Loc_398vy";
     typParam = Type.GetType("System.String");
     iCountTestcases++;
     strRet = (String) ((IConvertible)DBNull.Value).ToType(typParam,null);
     if (strRet != String.Empty)
       {
       ++iCountErrors;	
       printerr( "Error_211aa! Expected==String.Empty Value==" + strRet);
       }
     strLoc = "Loc_400vy";
     typParam = Type.GetType("System.Int32");
     iCountTestcases++;
     try
       {
       i32Ret = (Int32) ((IConvertible)DBNull.Value).ToType(typParam,null);
       ++iCountErrors;	
       printerr( "Error_100aa! Expected InvalidCastException not thrown");
       }
     catch (InvalidCastException icex)
       {
       printinfo( "Info_100cc! Caught InvalidCastException, icex=="+icex.Message);
       }
     catch (Exception exc_general)
       {
       ++iCountErrors;	
       Console.WriteLine (s_strTFAbbrev + " : Error Err_100bb!  Incorrect Exception thrown strLoc=="+ strLoc +", exc_general=="+exc_general.ToString());
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
   Console.WriteLine ("INFO: ("+ s_strTFAbbrev + ") "+ info);
   }
 public static void Main(String[] args)
   {
   bool bResult = false;
   Co7022ToType_Type cbA = new Co7022ToType_Type();
   try {
   bResult = cbA.runTest();
   } catch (Exception exc_main){
   bResult = false;
   Console.WriteLine(s_strTFAbbrev + " : FAiL! Error Err_9999zzz! Uncaught Exception in main(), exc_main=="+exc_main.ToString());
   }
   if (!bResult)
     {
     Console.WriteLine ("Path: "+s_strTFPath+"\\"+s_strTFName);
     Console.WriteLine( " " );
     Console.WriteLine( "FAiL!  "+ s_strTFAbbrev);
     Console.WriteLine( " " );
     }
   if (bResult) Environment.ExitCode = 0; else Environment.ExitCode = 1;
   }
}
