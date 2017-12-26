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
using System.Text;
using System.Globalization;
using GenStrings;
public class Co2346Replace_ss
{
 static String strName = "String.Replace(String)";
 static String strTest = "Co2346Replace_ss";
 static String strPath = "";
 static int iCountErrors = 0;
 static int iCountTestcases = 0;
 const int MAXRANDOMSTRING = 90;
 const int MAXUNICODE = 65535; 
 const int MINUNICODE = 0;
 const int CHARINSTRPERCENTAGE = 100;
 public Boolean runTest(double timeInterval, int repetitions)
   {
   try
     {
     try {										
     String trialStr = "asdf";
     trialStr=trialStr.Replace (null, null);
     iCountErrors++;
     Console.WriteLine( "Error_num701:  Should have thrown." );
     }
     catch (System.ArgumentNullException) {
     }
     try {				
     IntlStrings intl = new IntlStrings();						
     String trialStr = intl.GetString(18, false, true);
     trialStr=trialStr.Replace (null ,"s");
     iCountErrors++;
     Console.WriteLine( "Error_nuz801:  Should have thrown." );
     }
     catch (System.ArgumentNullException) {
     }
     try {										
     String trialStr = "asdf";
     trialStr=trialStr.Replace (null ,"s");
     iCountErrors++;
     Console.WriteLine( "Error_nuz801:  Should have thrown." );
     }
     catch (System.ArgumentNullException) {
     }
     testString ("asdf", "adf", "s", null, "Rep_ss200");
     testString ("Yehia", "Ye7ia", "h", "7", "Rep_ss00");		
     testString ("Redmond", "Redmond", "x", "d", "Rep_ss01");          
     testString ("Redmond", "Redmond", "d", "d", "Rep02");      	
     testString ("Redmond", "redmond", "R", "r", "Rep_ss03");           
     testString ("Redmond", "Redmond", "r", "c", "Rep_ss04");           
     iCountTestcases++;
     testString ("Redmond", "Re mon ", "d", " ", "Rep_ss05");          
     iCountTestcases++;
     testString ("Reddod", "Rexxox", "d", "x", "Rep_ss06");     	
     iCountTestcases++;
     testString ("Redmo", "Rexmo", "d", "x", "Rep_ss07");     	
     iCountTestcases++;
     testString ("Null", "N"+'\u0000'+"ll", "u", "\u0000", "Rep_ss08");     
     iCountTestcases++;
     testString ("In", "I"+'\uFFFF', "n", "\uFFFF", "Rep_ss09");	
     iCountTestcases++;
     testString (" ", " ", " ", " ", "Rep_ss10");				
     iCountTestcases++;
     testString ("  ", "aa", " ", "a", "Rep_ss11");	
     testString ("Seattle", "Sea\\n\\nle", "t", "\\n", "Rep_ss12");         
     iCountTestcases++;
     testString ("Seattle", "Sea\\r\\rle", "t", "\\r", "Rep_ss13");			
     testString ("Seattle", "Sea\\t\\tle", "t", "\\t", "Rep_ss14");
     testString ("Seattle", "S\\attl\\", "e", "\\", "Rep_ss15");
     testString ("Seattle", "Sea\\a\\ale", "t", "\\a", "Rep_ss16");
     testString ("Seattle", "Sea\\0\\0le", "t", "\\0", "Rep_ss17");
     testString ("", "", "f", "s", "Rep_ss18");
     testString ("asdf", "asdss", "f", "ss", "Rep_ss19");
     testString ("asdf", "asdf", "ss", "f", "Rep_ss20");
     testString (" ", "asdf", " ", "asdf", "Rep_ss21");
     testString ("as ", "ax ", "s", "x", "Rep_ss22");
     testString ("  ", "  ", "x", "a", "Rep_ss23");
     testString ("ss", "ssss", "s", "ss", "Rep_ss24");
     testString ("sdsd", "", "sd", "", "Rep_ss25");
     testString ("   ", "a ", "  ", "a", "Rep_ss26");
     testString ("sf", "sc", "f", "c", "Rep_ss25");
     testString ("sfff", "scc", "fff", "cc", "Rep_ss25");
     String point="  ";
     String other=point;
     testString (point, other, " ", " ", "Rep_ss27");
     testString ("asdf", "asdf", "dfgg", "vb", "Rep_ss28");
     for (int index=0; index<repetitions; index++){
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
 private void testString (String originalStr, String expectedStr, String removeStr, String addStr, String pmGuid)
   {
   String replaceSSOutcome = "No such string value";
   try
     {
     ++iCountTestcases;
     replaceSSOutcome = originalStr.Replace (removeStr, addStr);
     }
   catch ( Exception exc )
     {
     Console.WriteLine( "Error_vub811:  Unexpected exc ("+ pmGuid +"): exc="+ exc.ToString() );
     }
   if ( String.Equals(replaceSSOutcome, expectedStr) != true ) {
   ++iCountErrors;
   Console.WriteLine ("Original String "+originalStr+"Remove String"+removeStr+" with"+addStr);
   Console.WriteLine("Err_"+pmGuid+ ": " + "New string is " + replaceSSOutcome + " expected " + expectedStr);
   }
   }
 private void randomTest (double numSeconds)
   {
   int seedCount=0;
   int[] seeds=new int[3];
   DateTime seedCore=new DateTime();
   seedCore=DateTime.Now;
   seeds[0]=seedCore.Millisecond;
   Random randStr = new Random (seeds[0]);
   seedCount++;
   seeds[1]=seedCore.Millisecond+randStr.Next();
   Random randChar = new Random (seeds[1]);
   seedCount++;
   seeds[2]=seedCore.Millisecond-randStr.Next();
   Random randIndex = new Random (seeds[2]);
   seedCount++;
   DateTime dtCurrent = new DateTime ();
   DateTime dtStop = new DateTime ();
   dtStop=DateTime.Now;
   dtStop=dtStop.AddSeconds (numSeconds);
   for (;;)
     {			
     dtCurrent=DateTime.Now;		
     if (dtCurrent>dtStop)
       break;
     String originalString = getRandomString(randStr, randChar);
     String oldValue = ""; 
     for (int strInStr=0; ; strInStr++)
       {
       dtCurrent=DateTime.Now;		
       if (dtCurrent>dtStop)
	 break;
       String newValue = getRandomString(randStr, randChar);
       if (strInStr<CHARINSTRPERCENTAGE)
	 oldValue = getSubString (originalString, randIndex);
       else
	 oldValue = getRandomString(randStr, randChar);				
       String generatedReplace="No such string one";
       String generatedMyReplace="No such string two"; 
       try{
       generatedReplace= originalString.Replace (oldValue, newValue);
       generatedMyReplace = MyReplace (originalString, oldValue, newValue);
       }
       catch (Exception ex){
       Console.WriteLine( "Error_vdb434:  Unexpected ex : ex="+ ex.ToString() );
       }
       ++iCountTestcases;				
       if ( String.Equals(generatedReplace, generatedMyReplace) != true ) {
       ++iCountErrors;    				
       Console.WriteLine("------Original String: ");
       for (int index=0; index<originalString.Length; index++)
	 Console.Write (Convert.ToInt16(originalString[index])+"-");
       Console.WriteLine();
       Console.WriteLine("------Old value: ");
       for (int index=0; index<oldValue.Length; index++)
	 Console.Write(Convert.ToInt16(oldValue[index])+"-");
       Console.WriteLine();
       Console.WriteLine("------New value: ");
       for (int index=0; index<newValue.Length; index++)
	 Console.Write(Convert.ToInt16(newValue[index])+"-");
       Console.WriteLine();
       Console.WriteLine("------Seeds "+seeds[0]+" "+seeds[1]+" "+seeds[2]);
       }
       if (strInStr==100)
	 strInStr=0;					
       }				
     }
   }
 private String MyReplace (String originalStr, String oldValue, String newValue)
   {
   StringBuilder newStr=new StringBuilder();
   if (oldValue=="") 
     if (originalStr=="") {
     newStr=newStr.Append(newValue);
     return newStr.ToString();
     }
   Boolean inspecting=false;
   int index=0, count=0, other=0;
   for (index=0; index<originalStr.Length; index++) {
   if (originalStr[index]==oldValue[0]) {
   for (count=index, other=0; (other<oldValue.Length)&&(count<originalStr.Length); count++, other++) 
     {
     if (originalStr[count]==oldValue[other]){
     inspecting=true;
     if (other==oldValue.Length-1){
     newStr.Append(newValue);
     index=index+oldValue.Length-1;
     inspecting=false;
     break;
     }							
     }
     else {
     newStr.Append (originalStr[index]);
     inspecting=false;
     break;
     }						
     }	
   if ((count==originalStr.Length)&&(inspecting==true))
     newStr.Append (originalStr[index]);										
   }				
   else {
   newStr.Append (originalStr[index]);										
   inspecting=false;
   }
   }
   return newStr.ToString();
   }
 private String getSubString (String originalString, Random randIndex)
   {		
   int startIndex=0;
   int endIndex=0;
   int temp=0;
   startIndex=randIndex.Next (0, originalString.Length-1);
   endIndex=randIndex.Next (0, originalString.Length-1);
   if (endIndex<startIndex){
   temp=endIndex;
   endIndex=startIndex;
   startIndex=temp;
   }
   StringBuilder sbStr = new StringBuilder ();	
   for (int index=startIndex; index<=endIndex; index++)
     sbStr=sbStr.Append (originalString[index]);
   return sbStr.ToString ();
   }
 private String getRandomString(Random randStr, Random randChar)
   {
   int maxString=1;
   for ( int ii=0; ii<10; ii++ ){			
   maxString = randStr.Next(0, MAXRANDOMSTRING);
   if (maxString>10)
     break;
   }
   StringBuilder sbStr = new StringBuilder (maxString);	
   for (int index=0; index<maxString; index++)
     {
     char temp=' ';
     temp=(char)randChar.Next(MINUNICODE, MAXUNICODE);
     sbStr=sbStr.Append (temp);
     }
   return 	sbStr.ToString();
   }
 public static void Main( String[] args ) 
   {
   Console.WriteLine( " Starting "+ strName +" ..." );
   Console.WriteLine( "USAGE:  Co2346Replace_ss.exe  Time_in_Seconds_per_rep  Number_of_repetitions" );
   Console.WriteLine( "USAGE:  Defaults args are:   2  2   (must give both args if give any)" );
   Console.WriteLine();
   Boolean bResult = false;	
   Co2346Replace_ss oReplaceTest = new Co2346Replace_ss();
   double timeInSecs=2.0;
   int numOfReps=2;
   try {
   if ( args.Length >= 1 )
     timeInSecs=Convert.ToInt64(args[0]);
   if ( args.Length >= 2 )
     numOfReps = Convert.ToInt32(args[1]);			
   }
   catch (System.FormatException) {
   Console.WriteLine ("INVALID ARGUMENTS");			
   Console.WriteLine ("USING DEFAULT: Co2346Replace_ss.exe  2  2 ");
   timeInSecs=2.0;
   numOfReps=2;
   }
   catch ( Exception exc )
     {
     Console.WriteLine( "Error_abc637:  Conversion error about input args?" );
     Console.WriteLine( exc.ToString() );
     goto LABEL_882_RETURN_LABEL;
     }
   try {
   bResult = oReplaceTest.runTest(timeInSecs ,numOfReps);
   }
   catch ( Exception exc_main ) {
   bResult = false;
   String strInfo =  "Unexpected Exception: (runTest)" ;
   strInfo = strInfo + exc_main.ToString() ;
   Console.Error.WriteLine (strName+" "+strTest+" "+strPath);
   Console.Error.WriteLine (strInfo);
   }
   LABEL_882_RETURN_LABEL:;
   if ( ! bResult )
     Console.Error.WriteLine(  "PATHTOSOURCE:  " + strPath + strTest + "   FAIL"  );
   if ( bResult == true ) Environment.ExitCode = 0; else Environment.ExitCode = 11;
   }
}
