//# ==++== 
//# 
//#   
//#    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//#   
//#    The use and distribution terms for this software are contained in the file
//#    named license.txt, which can be found in the root of this distribution.
//#    By using this software in any fashion, you are agreeing to be bound by the
//#    terms of this license.
//#   
//#    You must not remove this notice, or any other, from this software.
//#   
//# 
//# ==--== 
//####################################################################################
@cc_on


import System;

var NULL_DISPATCH = null;
var apGlobalObj;
var apPlatform;
var lFailCount;


var iTestID = 52872;

var g_OnDBCS = false;
var g_lang;

function GetLang()
{
	g_lang = apGlobalObj.apGetLangExt(apGlobalObj.LangHost());
	if(g_lang == "JP" || g_lang == "KO" || g_lang == "CHS" || g_lang == "CHT")
		g_OnDBCS = true;
}

function lstnd02() {
  @if(@_fast)
    var stBug, expected;
  @end

    apInitTest("lstnd02 ");
    GetLang();

    var VariableString;
    var FixedString;
    var Temp;
    var NullString;

    stBug = "";

    apInitScenario("Scenario 1 - Omit START, verify that 1 is assumed");

    VariableString = "ABCDEFG-ABCDEFG-ABCDEFG";

    Temp = "@ABCDEFG".lastIndexOf("A")
    if (Temp != 1) {
	 apLogFailInfo("(lastIndexOf() without START - constant string)" , 1, Temp,stBug);
    }
	
    if(g_OnDBCS)
    {
	    Temp = "@‚ ‚¦‚¢‚¨‚¤".lastIndexOf("‚ ")
	    if (Temp != 1) {
		 apLogFailInfo("(lastIndexOf() without START - constant DBCS string)" , 1, Temp,stBug);
	    }
    }

    Temp = VariableString.lastIndexOf("A")
    if (Temp !=16) {
	 apLogFailInfo("(lastIndexOf() without START - variable string)" , 1, Temp, stBug);
    }
	
    if(g_OnDBCS)
    {
	    VariableString = "‚ ‚¦‚¢‚¨‚¤‚ ‚¦‚¢‚¨‚¤-‚ ‚¦‚¢‚¨‚¤‚ ‚¦‚¢‚¨‚¤"
	    Temp = VariableString.lastIndexOf("‚¨")
	    if (Temp !=19) {
		 apLogFailInfo("(lastIndexOf() without START - variable DBCS string)" , 1, Temp, stBug);
	    }
    }

    apInitScenario("Scenario 3 - Verify that the return value is correct when the string");

    VariableString = "ABCDEFG-ABCDEFG-ABCDEFG";

    Temp = "ABCDEFG-ABCDEFG".lastIndexOf("BC",13)
    if (Temp != 9) {
	 apLogFailInfo("(START != 1 - constant string)" , 9, Temp,stBug);
    }
    
    if(g_OnDBCS)
    {
	    "‚ ‚¦‚¢‚¨‚¤-‚ ‚¦‚¢‚¨‚¤-‚ ‚¦‚¢‚¨‚¤";

	    Temp = "‚ ‚¦‚¢‚¨‚¤-‚ ‚¦‚¢‚¨‚¤-‚ ‚¦‚¢‚¨‚¤".lastIndexOf("‚¨‚¤",9)
	    if (Temp != 9) {
		 apLogFailInfo("(START != 1 - constant DBCS string)" , 9, Temp,stBug);
	    }
    }

    Temp = VariableString.lastIndexOf( "CDE", 14)
    if (Temp != 10) {
	 apLogFailInfo("(START != 1 - variable string)" ,10,Temp,stBug);
    }
    
    if(g_OnDBCS)
    {
	    VariableString = "‚ ‚¦‚¢‚¨‚¤‚ ‚¦‚¢‚¨‚¤";
	    Temp = VariableString.lastIndexOf( "‚¦‚¢‚¨", 9)
	    if (Temp != 6) {
		 apLogFailInfo("(START != 1 - variable DBCS string)" ,6,Temp,stBug);
	    }
    }

    apInitScenario("Scenario 4 - Verify that when START is greater than the length");

    VariableString = "ABCDEFG-ABCDEFG-ABCDEFG"

    Temp = "@ABCDEFG-ABCDEFG".lastIndexOf( "BC",1)
    if (Temp != -1) {
	 apLogFailInfo("(START > Length - constant string)" ,-1,Temp,stBug);
    }

    Temp = VariableString.lastIndexOf("CDE",1)
    if (Temp != -1) {
	 apLogFailInfo("(START > Length - variable string)" ,-1,Temp,stBug);
    }

    apInitScenario("Scenario 5 - Verify that a null string expression 1 returns zero");

    VariableString = "";

    Temp = "".lastIndexOf(" ")
    if (Temp != -1) {
	 apLogFailInfo("(Null string 1 - constant string)" ,-1,Temp,"VBE 13579");
    }

    Temp = VariableString.lastIndexOf( " ")
    if (Temp != -1) {
	 apLogFailInfo("(Null string 1 - variable string)" ,-1,Temp,"VBE 13579");
    }

    apInitScenario("Scenario 6 - Verify that when string expression 2 cannot be found,          ");

    VariableString = "ABCDEFG-ABCDEFG-ABCDEFG"

    Temp = "ABCDEFG-ABCDEFG".lastIndexOf("*BC")
    if (Temp != -1) {
	 apLogFailInfo("(String 2 not found - constant string)" ,"","","VBE 13579");
    }

    Temp = VariableString.lastIndexOf( "CDE*")
    if (Temp != -1) {
	 apLogFailInfo("(String 2 not found  - variable string)" ,"","",stBug);
    }

    apInitScenario("Scenario 7 - Verify that when string expression 2 is null, the start       ");

    VariableString = "ABCDEFG-ABCDEFG-ABCDEFG"
    NullString = ""

    Temp = "ABCDEFG-ABCDEFG".lastIndexOf(NullString)

    @if (@_jscript_version < 5.5)
        expected = 14;
    @else
        expected = 15;
    @end

    if (Temp != expected) {
	 apLogFailInfo("(String 2 is null  - constant string)" ,expected,Temp,stBug);
    }

    Temp = VariableString.lastIndexOf(NullString,2)
    if (Temp != 2) {
	 apLogFailInfo("(String 2 is null  - variable string)" ,2,Temp,"VBE 13693");
    }

    apEndTest();
}


lstnd02();


if(lFailCount >= 0) System.Environment.ExitCode = lFailCount;
else System.Environment.ExitCode = 1;

function apInitTest(stTestName) {
    lFailCount = 0;

    apGlobalObj = new Object();
    apGlobalObj.apGetPlatform = function Funca() { return "Rotor" }
    apGlobalObj.LangHost = function Funcb() { return 1033;}
    apGlobalObj.apGetLangExt = function Funcc(num) { return "EN"; }

    apPlatform = apGlobalObj.apGetPlatform();
    var sVer = "1.0";  //navigator.appVersion.toUpperCase().charAt(navigator.appVersion.toUpperCase().indexOf("MSIE")+5);
    apGlobalObj.apGetHost = function Funcp() { return "Rotor " + sVer; }
    print ("apInitTest: " + stTestName);
}

function apInitScenario(stScenarioName) {print( "\tapInitScenario: " + stScenarioName);}

function apLogFailInfo(stMessage, stExpected, stActual, stBugNum) {
    lFailCount = lFailCount + 1;
    print ("***** FAILED:");
    print ("\t\t" + stMessage);
    print ("\t\tExpected: " + stExpected);
    print ("\t\tActual: " + stActual);
}

function apGetLocale(){ return 1033; }
function apWriteDebug(s) { print("dbg ---> " + s) }
function apEndTest() {}
