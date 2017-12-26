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
public class Co8595GetHashCode
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer       = "";
 public static String s_strClassMethod   = "Version.GetHashCode()"; 
 public static String s_strTFName        = "Co8595GetHashCode.cs";
 public static String s_strTFAbbrev      = s_strTFName.Substring(0,6);
 public static String s_strTFPath        = Environment.CurrentDirectory;
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   try
     {
     Version vTest1 = new Version (5,5);
     Version vTest2 = new Version (5,4);
     strLoc = "Loc_100vy";
     iCountTestcases++;
     if (vTest1.GetHashCode() == vTest2.GetHashCode() )
       {
       ++iCountErrors;	
       printerr( "Error_100aa! Expected==false");
       }
     strLoc = "Loc_300vy";
     vTest2 = new Version (5,5);
     iCountTestcases++;
     if (vTest1.GetHashCode() != vTest2.GetHashCode() )
       {
       ++iCountErrors;	
       printerr( "Error_300aa! Expected==true");
       }
     vTest1 = new Version (10,10,10);
     vTest2 = new Version (10,10,2);
     strLoc = "Loc_400vy";
     iCountTestcases++;
     if (vTest1.GetHashCode() == vTest2.GetHashCode() )
       {
       ++iCountErrors;	
       printerr( "Error_400aa! Expected==false");
       }
     strLoc = "Loc_600vy";
     vTest2 = new Version (10,10,10);
     iCountTestcases++;
     if (vTest1.GetHashCode() != vTest2.GetHashCode() )
       {
       ++iCountErrors;	
       printerr( "Error_600aa! Expected==true");
       }
     vTest1 = new Version (550,500,2,7);
     vTest2 = new Version (550,500,2,5);
     strLoc = "Loc_700vy";
     iCountTestcases++;
     if (vTest1.GetHashCode() == vTest2.GetHashCode() )
       {
       ++iCountErrors;	
       printerr( "Error_700aa! Expected==false");
       }
     strLoc = "Loc_600vy";
     vTest2 = new Version (550,500,2,7);
     iCountTestcases++;
     if (vTest1.GetHashCode() != vTest2.GetHashCode() )
       {
       ++iCountErrors;	
       printerr( "Error_900aa! Expected==true");
       }
     strLoc = "Loc_100bb";
     vTest1 = new Version (550,500);
     vTest2 = new Version (550,500,2,7);
     iCountTestcases++;
     if (vTest1.GetHashCode() == vTest2.GetHashCode() )
       {
       ++iCountErrors;	
       printerr( "Error_100xx! Expected==false");
       }
     strLoc = "Loc_110zz";
     vTest1 = new Version (Int32.MaxValue,500);
     vTest2 = new Version (550,500,2,0);
     iCountTestcases++;
     if (vTest1.GetHashCode() == vTest2.GetHashCode() )
       {
       ++iCountErrors;	
       printerr( "Error_100xx! Expected==false");
       }
     strLoc = "Loc_120zz";
     vTest1 = new Version (5,5);
     vTest2 = new Version (5,5,0,0);
     iCountTestcases++;
     if (vTest1.GetHashCode() == vTest2.GetHashCode() )
       {
       ++iCountErrors;	
       printerr( "Error_120xx! Expected==false value==" + vTest1.Equals(vTest2));
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
   Co8595GetHashCode cbA = new Co8595GetHashCode();
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
