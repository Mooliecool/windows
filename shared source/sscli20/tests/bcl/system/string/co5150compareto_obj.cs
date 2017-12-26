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
public class Co5150CompareTo_obj
{
 public static readonly String s_strActiveBugNums = "";
 public static readonly String s_strDtTmVer       = "";
 public static readonly String s_strClassMethod   = "String.CompareTo(Object)";
 public static readonly String s_strTFName        = "Co5150CompareTo_obj.";
 public static readonly String s_strTFAbbrev      = "Co5150";
 public static readonly String s_strTFPath        = "";
 public virtual bool runTest()
   {
   Console.Error.WriteLine(s_strTFPath + " " + s_strTFName + " , for " + s_strClassMethod + " , Source ver " + s_strDtTmVer);
   int iCountErrors = 0;
   int iCountTestcases = 0;
   String strLoc = "Loc_000oo";
   String strBaseLoc = "";
   String str1, str2;
   Char ch;
   Byte byt1a;
   SByte sbyt1a;
   Int16 in2a;
   Int32 in4a;
   Int64 in8a;
   Single sgl2;
   Double dbl2;
   Decimal dec2;
   try {
   LABEL_860_GENERAL:
   do
     {
     try {
     str1 = "";
     ch = ' ';
     iCountTestcases++;
     if(str1.CompareTo(ch) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_394qi ,ch=["+ ch +"] ,str1=["+ str1 +"]" );
       }
     } catch (ArgumentException aexc) {}
     try {
     str1 = "";
     byt1a = 0;
     iCountTestcases++;
     if(str1.CompareTo(byt1a) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_219qi");
       }
     } catch (ArgumentException aexc) {}
     try {
     str1 = "";
     sbyt1a = 0;
     iCountTestcases++;
     if(str1.CompareTo(sbyt1a) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_3849a");
       }
     } catch (ArgumentException aexc) {}
     try {
     str1 = "";
     in2a = 0; 
     iCountTestcases++;
     if(str1.CompareTo(in2a) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_498uw");
       }
     } catch (ArgumentException aexc) {}
     IntlStrings intl = new IntlStrings();
     String intlString = intl.GetString(10, false, true);
     Object obj = (object)intlString;
     iCountTestcases++;
     if ( intlString.CompareTo(obj) != 0 )
       {
       ++iCountErrors;
       Console.WriteLine(intlString);
       } 
     try {
     str1 = "";
     in4a = 0; 
     iCountTestcases++;
     if(str1.CompareTo(in4a) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_948qj");
       }
     } catch (ArgumentException aexc) {}
     iCountTestcases++;
     try {
     str1 = "";
     in8a = 0; 
     iCountTestcases++;
     if(str1.CompareTo(in8a) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_983jq");
       }
     } catch (ArgumentException aexc) {}
     iCountTestcases++;
     try {
     str1 = "";
     sgl2 = 0; 
     iCountTestcases++;
     if(str1.CompareTo(sgl2) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_482ua");
       }
     } catch (ArgumentException aexc) {}
     try {
     str1 = "";
     dbl2 = 0; 
     iCountTestcases++;
     if(str1.CompareTo(dbl2) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_32580");
       }
     } catch (ArgumentException aexc) {}
     try {
     str1 = "";
     dec2 = 0; 
     iCountTestcases++;
     if(str1.CompareTo(dec2) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_480rr");
       }
     } catch (ArgumentException aexc) {}
     str1 = "";
     iCountTestcases++;
     try {
     if(str1.CompareTo(null) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev +  "  Err_493ca");  
       }
     } catch (Exception exc) {
     iCountErrors++;
     Console.WriteLine( s_strTFAbbrev+ "Err_583oi");
     }
     str1 = String.Empty;
     iCountTestcases++;
     if(str1.CompareTo("") != 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_938us");
       }
     str1 = "blaH";
     str2 = "blah";
     iCountTestcases++;
     if(str1.CompareTo(str2) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_3940i");
       }
     strLoc = "Loc_837hd";
     str1 = "0";
     str2 = "{";
     iCountTestcases++;
     if(str1.CompareTo(str2) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_371eh! , result=="+str1.CompareTo(str2));
       }
     strLoc = "Loc_192hw";
     str1 = "z";
     str2 = "{";
     iCountTestcases++;
     if(str1.CompareTo(str2) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_732qu! , result=="+str1.CompareTo(str2));
       }
     strLoc = "Loc_128yr";
     str1 = "A";
     str2 = "*";
     iCountTestcases++;
     if ( str1.CompareTo( str2 ) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_347hw! , result=="+str1.CompareTo( str2 ));
       }
     strLoc = "Loc_834yw";
     str1 = "0";
     str2 = "$";
     iCountTestcases++;
     if ( str1.CompareTo ( str2) <= 0)
       {
       iCountErrors++;
       Console.WriteLine( s_strTFAbbrev+ "Err_289nk! , result=="+str1.CompareTo ( str2 ));
       }
     strLoc = "Loc_372hu";
     str1 = "0";
     str2 = "5";
     iCountTestcases++;
     if ( str1.CompareTo ( str2 ) >= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_793rh! , result=="+str1.CompareTo ( str2 ));
       }
     strLoc = "Loc_373hu";
     str1 = "5";
     str2 = "0";
     iCountTestcases++;
     if ( str1.CompareTo ( str2 ) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_794rh! , result=="+str1.CompareTo ( str2 ));
       }
     strLoc = "Loc_373hu";
     str1 = "5";
     str2 = "0";
     iCountTestcases++;
     if ( str1.CompareTo ( str2 ) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_794rh! , result=="+str1.CompareTo ( str2 ));
       }
     strLoc = "Loc_373hu";
     str1 = "A";
     str2 = "a";
     iCountTestcases++;
     if ( str1.CompareTo ( str2 ) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_894rh! , result=="+str1.CompareTo ( str2 ));
       }
     strLoc = "Loc_481hu";
     str1 = "A";
     str2 = "B";
     iCountTestcases++;
     if ( str1.CompareTo ( str2 ) >= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_933jw! , result=="+str1.CompareTo ( str2 ));
       }
     strLoc = "Loc_473hu";
     str1 = "a";
     str2 = "A";
     iCountTestcases++;
     if ( str1.CompareTo ( str2 ) >= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_812ks! , result=="+str1.CompareTo ( str2 ));
       }
     strLoc = "Loc_751hu";
     str1 = "B";
     str2 = "A";
     iCountTestcases++;
     if ( str1.CompareTo ( str2 ) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_941jd! , result=="+str1.CompareTo ( str2 ));
       }
     strLoc = "Loc_573hu";
     str1 = "}";
     str2 = "<";
     iCountTestcases++;
     if ( str1.CompareTo ( str2 ) >= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_094rh! , result=="+str1.CompareTo ( str2 ));
       }
     strLoc = "Loc_673hu";
     str1 = "<";
     str2 = "{";
     iCountTestcases++;
     if ( str1.CompareTo ( str2 ) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_099uh! , result=="+str1.CompareTo ( str2 ));
       }
     strLoc = "Loc_683hu";
     str1 = "A";
     str2 = "0";
     iCountTestcases++;
     if ( str1.CompareTo ( str2 ) <= 0)
       {
       iCountErrors++;
       Console.WriteLine(s_strTFAbbrev+ "Err_134eh! , result=="+str1.CompareTo ( str2 ));
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
   Co5150CompareTo_obj cbA = new Co5150CompareTo_obj();
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
