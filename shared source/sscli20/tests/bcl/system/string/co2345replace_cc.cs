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
using System;
using System.Globalization;
using GenStrings;
using System.Text;
public class Co2345Replace_cc
{
 static String strName = "String.Replace(char)";
 static String strTest = "Co2345Replace_cc";
 static String strPath = "";
 static int iCountErrors = 0;
 static int iCountTestcases = 0;
 const int MAXRANDOMSTRING = 100;
 const int MAXUNICODE = 65535; 
 const int MINUNICODE = 0;
 const int CHARINSTRPERCENTAGE = 80;
 private Boolean runTest(double timeInterval, int repetitions)
   {
   try
     {
     String emptyString="";
     emptyString=emptyString.Replace ('r', 't');
     IntlStrings intl = new IntlStrings();
     String testingString = intl.GetString(7, true, true);
     String copyTestString = testingString;
     Char charVar = testingString[4];
     testingString = testingString.Replace(testingString[4], 'h');
     testString(copyTestString, testingString, copyTestString[4], 'h', "Rep_cc-1");
     testString ("Yehia", "Ye7ia", 'h', '7', "Rep_cc00");	
     iCountTestcases++;
     testString ("Redmond", "Redmond", 'x', 'd', "Rep_cc01");      
     testString ("Redmond", "Redmond", 'd', 'd', "Rep_cc02");      
     iCountTestcases++;
     iCountTestcases++;
     testString ("Redmond", "redmond", 'R', 'r', "Rep_cc03");     
     testString ("Redmond", "Redmond", 'r', 'c', "Rep_cc04");     
     testString ("Redmond", "Re mon ", 'd', ' ', "Rep_cc05");     
     iCountTestcases++;
     testString ("Reddod", "Rexxox", 'd', 'x', "Rep_cc06");     
     testString ("Redmo", "Rexmo", 'd', 'x', "Rep_cc07");     
     iCountTestcases++;
     testString ("Null", "N"+'\u0000'+"ll", 'u', '\u0000', "Rep_cc08");	
     testString ("In", "I"+'\uFFFF', 'n', '\uFFFF', "Rep_cc09");		
     testString (" ", " ", ' ', ' ', "Rep_cc10");	
     testString ("  ", "aa", ' ', 'a', "Rep_cc11");	
     testString ("Seattle", "Sea"+'\n'+'\n'+"le", 't', '\n', "Rep_cc12");	
     testString ("Seattle", "Sea"+'\r'+'\r'+"le", 't', '\r', "Rep_cc13");			
     testString ("Seattle", "Sea"+'\t'+'\t'+"le", 't', '\t', "Rep_cc14");
     testString ("Seattle", "Sea"+'\\'+'\\'+"le", 't', '\\', "Rep_cc15");			
     testString ("Seattle", "Sea"+'\a'+'\a'+"le", 't', '\a', "Rep_cc16");
     testString ("Seattle", "Sea"+'\0'+'\0'+"le", 't', '\0', "Rep_cc17");
     for (int index=0; index<repetitions; index++) {
     Console.WriteLine ("Finished "+index+" repetition(s)");
     randomTest (timeInterval);	
     }
     }
   catch (Exception ex)
     {
     ++iCountErrors;
     String strInfo =  "Unexpected Exception: (runTest)" ;
     strInfo = strInfo + ex.ToString() ;
     Console.Error.WriteLine (strName+" "+strTest+" "+strPath);
     Console.Error.WriteLine (strInfo);
     }
   Console.WriteLine ("FINAL TEST RESULT:" + strName+" "+strTest+" "+strPath);
   if ( iCountErrors == 0 ) {   return true; }
   else {  return false;}
   }
 private void testString (String strOld, String strNew, char charOld, char charNew, String pmGuid)
   {
   String originalString=strOld;
   try {
   strOld=strOld.Replace (charOld, charNew);
   }
   catch (Exception ex){
   Console.WriteLine( "Error_vdb831:  Unexpected exc ("+ pmGuid +"): exc="+ ex.ToString() );
   }
   ++iCountTestcases;
   if ( String.Equals(strNew, strOld) != true )
     {
     ++iCountErrors;
     Console.WriteLine ("Original String "+originalString+"Replace old char"+charOld+" with"+charNew);
     Console.WriteLine("Err_"+pmGuid+ ": " + "New string is " + strNew + " should be" + strOld);
     }
   }
 static int seedCount=0;
 static int[] seeds=new int[5];
 private void getRandomGens (out Random [] randGenArr)
   {
   randGenArr=new Random[5];	
   DateTime seedCore=new DateTime();
   randGenArr[0] = new Random(seeds[0]=seedCore.Millisecond);
   randGenArr[1]= new Random(seeds[1]=seedCore.Millisecond);
   randGenArr[2] = new Random (seeds[2]=seedCore.Millisecond);
   randGenArr[3] = new Random(seeds[3]=seedCore.Millisecond);
   randGenArr[4] = new Random(seeds[4]=seedCore.Millisecond);
   ++seedCount;
   }
 private void randomTest (double numSeconds)
   {
   Random[] randGenArray;	
   getRandomGens (out randGenArray);
   Random randNumOld = randGenArray[0];
   Random randNumNew = randGenArray[1];
   Random randStr = randGenArray[2];
   Random randChar = randGenArray[3];
   Random randIndexGen = randGenArray[4];
   DateTime dtCurrent = new DateTime ();
   DateTime dtStop = new DateTime ();
   dtStop=DateTime.Now;
   dtStop=dtStop.AddSeconds (numSeconds);
   for (;;)
     {			
     dtCurrent=DateTime.Now;		
     if (dtCurrent>dtStop)
       break;
     String randStrOld= GetRandomString(randStr, randChar);
     char randCharOld='a';			
     for (int charInStr=0; ; charInStr++)
       {
       dtCurrent=DateTime.Now;		
       if (dtCurrent>dtStop)
	 break;
       String randStrNew= randStrOld;
       String originalString=randStrOld;
       int randIndex=randIndexGen.Next(0, randStrOld.Length-1);
       if (charInStr<CHARINSTRPERCENTAGE)
	 randCharOld = randStrOld[randIndex];
       else
	 randCharOld=(char)randNumOld.Next(MINUNICODE, MAXUNICODE);
       char randCharNew=(char)randNumNew.Next(MINUNICODE, MAXUNICODE);
       try {
       randStrOld = randStrOld.Replace (randCharOld, randCharNew);
       randStrNew = MyReplace (randStrNew, randCharOld, randCharNew);
       }
       catch (Exception ex) {
       Console.WriteLine( "Error_vdb234:  Unexpected ex : ex="+ ex.ToString() );			
       }
       ++iCountTestcases;				
       if ( String.Equals(randStrNew, randStrOld) != true )
	 {
	 ++iCountErrors;
	 for (int index=0; index<originalString.Length; index++)
	   Console.Write (Convert.ToInt32(originalString[index])+"-");
	 Console.WriteLine();
	 Console.WriteLine("Replace char"+randCharOld+" with"+randCharNew);
	 Console.WriteLine ("seeds: "+seeds[0]+" "+seeds[1]+" "+seeds[2]+" "+seeds[3]+" "+seeds[4]+" ");
	 }
       if (charInStr>100)
	 charInStr=0;					
       }				
     }
   }
 private String MyReplace (String randStrNew, Char randCharOld, Char randCharNew)
   {
   for (int index=0; index<randStrNew.Length; index++)
     {
     if (randCharOld==randStrNew[index]) 
       {
       if (index==0)
	 randStrNew=randCharNew+randStrNew.Substring(1, randStrNew.Length-1);								
       else if (index==(randStrNew.Length-1))
	 randStrNew=randStrNew.Substring(0, randStrNew.Length-1)+randCharNew;								
       else
	 randStrNew=randStrNew.Substring(0, index)+randCharNew+randStrNew.Substring(index+1, (randStrNew.Length-(index+1)));									
       }
     }	
   return randStrNew;
   }
 private String GetRandomString(Random randStr, Random randChar)
   {
   int maxString = randStr.Next(0, MAXRANDOMSTRING);
   StringBuilder sbStr = new StringBuilder (maxString);	
   for (int index=0; index<maxString; index++)
     sbStr=sbStr.Append ((char)randChar.Next(MINUNICODE, MAXUNICODE));
   return 	sbStr.ToString();
   }
 public static void Main( String[] args ) 
   {
   Boolean bResult = false;	
   Co2345Replace_cc oReplaceTest = new Co2345Replace_cc();
   Console.WriteLine( " Starting "+ strName +" ..." );                                               	
   Console.WriteLine( "USAGE:  Co2346Replace_ss.exe  Time_in_Seconds_per_rep  Number_of_repetitions" );
   Console.WriteLine( "USAGE:  Defaults args are:   2  2   (must give both args if give any)" );     	
   Console.WriteLine();                                                                        
   double timeInSecs=2.0;
   int numOfReps=2;
   try {			
   if (args.Length>=1) 
     timeInSecs=Convert.ToInt64(args[0]);
   if (args.Length>=2) 
     numOfReps = Convert.ToInt32(args[1]);			
   }
   catch (System.FormatException) {
   Console.WriteLine ("INVALID ARGUMENTS");			
   Console.WriteLine ("USING DEFAULT: Co2345Replace_cc.exe  2  2 ");
   timeInSecs=2.0;
   numOfReps=2;
   }
   catch ( Exception exc ) {                                                                                 
   Console.WriteLine( "Error_abc637:  Conversion error about input args?" );
   Console.WriteLine( exc.ToString() );
   goto LABEL_842_RETURN_LABEL;
   }
   try {			
   Console.WriteLine( "RUNNING: Co2345Replace_cc.exe  "+ timeInSecs+"  "+ numOfReps);
   bResult = oReplaceTest.runTest(timeInSecs, numOfReps);				
   }
   catch ( Exception exc_main ) {
   bResult = false;
   String strInfo =  "Unexpected Exception: (runTest)" ;
   strInfo = strInfo + exc_main.ToString() ;
   Console.Error.WriteLine (strName+" "+strTest+" "+strPath);
   Console.Error.WriteLine (strInfo);
   }
   LABEL_842_RETURN_LABEL:;
   if ( ! bResult )
     Console.Error.WriteLine(  "PATHTOSOURCE:  " + strPath + strTest + "   FAIL"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
}
