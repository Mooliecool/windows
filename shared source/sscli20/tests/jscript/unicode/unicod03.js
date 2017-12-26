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


var iTestID = 77541;

/* -------------------------------------------------------------------------
  Test: 	UNICOD03
   
  
 
  Component:	JScript
 
  Major Area:	Unicode compliance
 
  Test Area:	Unicode digits used in math expressions
 
  Keywords:	unicode literal escape eval digit math expressions
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:
		1,  Arabic digits (\u0030 - \u0039)

		2,  Arabic-Indic digits (\u0660 - \u0669)

		3,  Devanagari digits (\u0966 - \u096F)

		4,  Bengali digits (\u09E6 - \u09EF)

		5,  Gurmukhi digits (\u0A66 - \u0A6F)

		6,  Gujarati digits (\u0AE6 - \u0AEF)

		7,  Oriya digits (\u0B66 - \u0B6F)

		8,  Tamil digits (\u0BE7 - \u0BEF)

		9,  Telugu digits (\u0C66 - \u0C6F)

		10, Kannada digits (\u0CE6 - \u0CEF)  

		11, Malayalam digits (\u0D66 - \u0D6F)

		12, Thai digits (\u0E50 - \u0E59)

		13, Lao digits (\u0ED0 - \u0ED9)

		14, Tibetan digits (\u0F20 - \u0F29)

		15, Myanmar digits (\u1040 - \u1049)

		16, Ethiopic digits (\u1369 - \u1371)

		17, Khmer digits (\u17E0 - \u17E9)

		18, Mongolian digits (\u1810 - \u1819)

		19, Circled digits (\u2460 - \u2469)

		20, Parenthesized digits (\u2474 - \u247C)

		21, Fullwidth digits (\uFF10 - \uFF19)


  Abstract:	 Unicode digits are illegal in math expressions, with the exception
			 of Arabic digits (\u0030 - \u0039)
 ---------------------------------------------------------------------------
  Category:			Functionality
 
  Product:			JScript
 
  Related Files: 
 
  Notes:
 ---------------------------------------------------------------------------
  
 

 -------------------------------------------------------------------------*/



//////////
//
//  Helper functions


function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes+" ", sExp+" ", sAct, "");
}


//////////
//
// Global variables




function unicod03() {

    apInitTest("unicod02: Unicode compliance -- Unicode digits used in math expressions."); 

	var iResult;
	var bFailed;

	apInitScenario("1,  Arabic digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u0031 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "number", "1.1 Wrong data type");
		verify (iResult, 4, "1.2 Wrong value returned");
	}


	apInitScenario("2,  Arabic-Indic digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u0661 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "2.1 Wrong data type");
		verify (iResult, 3, "2.2 Wrong value returned");
	}


	apInitScenario("3,  Devanagari digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u0968 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "3.1 Wrong data type");
		verify (iResult, 3, "3.2 Wrong value returned");
	}


	apInitScenario("4,  Bengali digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u09E8 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "4.1 Wrong data type");
		verify (iResult, 3, "4.2 Wrong value returned");
	}


	apInitScenario("5,  Gurmukhi digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u0A68 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "5.1 Wrong data type");
		verify (iResult, 3, "5.2 Wrong value returned");
	}


	apInitScenario("6,  Gujarati digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u0AE8 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "6.1 Wrong data type");
		verify (iResult, 3, "6.2 Wrong value returned");
	}


	apInitScenario("7,  Oriya digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u0B68 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "7.1 Wrong data type");
		verify (iResult, 3, "7.2 Wrong value returned");
	}


	apInitScenario("8,  Tamil digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u0BE8 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "8.1 Wrong data type");
		verify (iResult, 3, "8.2 Wrong value returned");
	}


	apInitScenario("9,  Telugu digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u0C68 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "9.1 Wrong data type");
		verify (iResult, 3, "9.2 Wrong value returned");
	}


	apInitScenario("10, Kannada digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u0CE8 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "10.1 Wrong data type");
		verify (iResult, 3, "10.2 Wrong value returned");
	}


	apInitScenario("11, Malayalam digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u0D68 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "11.1 Wrong data type");
		verify (iResult, 3, "11.2 Wrong value returned");
	}


	apInitScenario("12, Thai digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u0E53 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "12.1 Wrong data type");
		verify (iResult, 3, "12.2 Wrong value returned");
	}


	apInitScenario("13, Lao digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u0ED3 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "13.1 Wrong data type");
		verify (iResult, 3, "13.2 Wrong value returned");
	}


	apInitScenario("14, Tibetan digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u0F23 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "14.1 Wrong data type");
		verify (iResult, 3, "14.2 Wrong value returned");
	}


	apInitScenario("15, Myanmar digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u1043 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "15.1 Wrong data type");
		verify (iResult, 3, "15.2 Wrong value returned");
	}


	apInitScenario("16, Ethiopic digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u136B + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "16.1 Wrong data type");
		verify (iResult, 3, "16.2 Wrong value returned");
	}


	apInitScenario("17, Khmer digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u17E3 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "17.1 Wrong data type");
		verify (iResult, 3, "17.2 Wrong value returned");
	}


	apInitScenario("18, Mongolian digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u1813 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "18.1 Wrong data type");
		verify (iResult, 3, "18.2 Wrong value returned");
	}


	apInitScenario("19, Circled digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u2463 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "19.1 Wrong data type");
		verify (iResult, 3, "19.2 Wrong value returned");
	}


	apInitScenario("20, Parenthesized digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\u2476 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "20.1 Wrong data type");
		verify (iResult, 3, "20.2 Wrong value returned");
	}


	apInitScenario("21, Fullwidth digits");
	iResult = undefined;
	bFailed = false;
	try {
		iResult = eval("\uFF13 + 3");
	}
	catch (e) {
		bFailed = true;
	}
	if (!bFailed) {
		verify (typeof(iResult), "Number", "21.1 Wrong data type");
		verify (iResult, 3, "21.2 Wrong value returned");
	}


	apEndTest();
}

unicod03();


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
