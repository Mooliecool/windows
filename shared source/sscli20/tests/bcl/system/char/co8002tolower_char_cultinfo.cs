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
public class Co8002ToLower_Char_CultInfo
{
 public static String s_strActiveBugNums = "";
 public static String s_strDtTmVer   = "";
 public static String s_strClassMethod   = "Char.ToLower(char, CultInfo)";
 public static String s_strTFName= "Co8002ToLower_Char_CultInfo.cs";
 public static String s_strTFAbbrev  = s_strTFName.Substring(0,6);
 public static String s_strTFPath= "";
 public bool runTest()
   {
   Console.WriteLine(s_strTFPath + "\\" + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   String strLoc = "Loc_000oo";
   String strValue = String.Empty;
   int iCountErrors = 0;
   int iCountTestcases = 0;
   try
     {
     char c;
     int i;
     CultureInfo ci = CultureInfo.InvariantCulture;
     strLoc = "Loc_sb8002a";
     for(i = 0; i < 65; i++)
       {
       iCountTestcases++;
       c = (char)i;
       if(Char.ToLower(c, ci) != c)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_sb8002a! Character {0} (value {1}) failed", c, i);
	 }
       }
     strLoc = "Loc_sb8002b";
     for(; i < 91; i++)
       {
       iCountTestcases++;
       c = (char)i;
       if(Char.ToLower(c, ci) != (char)(i+32))
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_sb8002b! Character {0} (value {1}) failed", c, i);
	 }
       }
     strLoc = "Loc_sb8002c";
     for(; i < 192; i++)
       {
       iCountTestcases++;
       c = (char)i;
       if(Char.ToLower(c, ci) != c)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_sb8002c! Character \'{0}\' (value {1}) failed, returned \'{2}\'",
			   c, i, Char.ToLower(c, ci));
	 }
       }
     strLoc = "Loc_sb8002d";
     for(; i < 223; i++)
       {
       iCountTestcases++;
       c = (char)i;
       if( (Char.ToLower(c, ci) != (char)(i+32)) &&
	   (i != 215))
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_sb8002d1! Character \'{0}\' (value {1}) failed, returned \'{2}\'",
			   c, i, Char.ToLower(c, ci));
	 }
       }
     c = (char)215;
     if(Char.ToLower(c, ci) != c) 
       {
       iCountErrors++;
       Console.WriteLine("Err_sb8002d2! Character {0} (value 247) failed, returned \'{1}\'",
			 c, Char.ToLower(c, ci));
       }
     strLoc = "Loc_sb8002e";
     for(; i < 256; i++)
       {
       iCountTestcases++;
       c = (char)i;
       if(Char.ToLower(c, ci) != c)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_sb8002e! Character \'{0}\' (value {1}) failed, returned \'{2}\'",
			   c, i, Char.ToLower(c, ci));
	 }
       }
     strLoc = "Loc_sb8002f";
     c = (char)(0x6543);
     iCountTestcases++;
     if(Char.ToLower(c, ci) != c)
       {
       iCountErrors++;
       Console.WriteLine("Err_sb8002f! Character {0} (value {1}) failed", c, i);
       }
     strLoc = "Loc_sb8002g";
     foreach(CultureInfo cin in CultureInfo.GetCultures(CultureTypes.AllCultures))
       {
       iCountTestcases++;
       if(Char.ToLower('Z', cin) != 'z')
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_sb8002g! CultureInfo {0} failed", cin);
	 }
       }
     strLoc = "Loc_sb8002h";
     ci = null;
     c = 'a';
     iCountTestcases++;
     try
       {
       if(Char.ToLower(c, ci) == c)
	 {
	 iCountErrors++;
	 Console.WriteLine("Err_sb8002h1! No exception thrown for null CultureInfo (expected ArgumentException)");
	 }
       }
     catch (ArgumentException)
       {}
     catch (Exception e)
       {
       iCountErrors++;
       Console.WriteLine("Err_sb8002h2! Wrong exception type for null CultureInfo (expected ArgumentException, got "
			 + e.ToString() + ")");
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
     Console.WriteLine("FAiL! "+s_strTFName+" ,inCountErrors=="+iCountErrors.ToString()+" , BugNums?: "+s_strActiveBugNums );
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
   Co8002ToLower_Char_CultInfo cbA = new Co8002ToLower_Char_CultInfo();
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
