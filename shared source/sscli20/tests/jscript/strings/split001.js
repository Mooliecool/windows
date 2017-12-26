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


var iTestID = 53673;

//two scenarios
//1) string.split() no param
//2) string.split('') emptry string param

var m_scen ='';
var sCat = '';

function VerifySplit(ArrayExp, StringToSplit, StringSeperator)
{var ArrayAct;
	if ((typeof StringSeperator) == "string")
		ArrayAct = StringToSplit.split(StringSeperator);
	else
		ArrayAct = StringToSplit.split();
	if (!ArrayEqual(ArrayExp, ArrayAct))
		apLogFailInfo((sCat.length?"--"+sCat:"")+" failed", ArrayExp,ArrayAct, "");

}

function ArrayEqual(Array1, Array2)
{var dIndex;
	var Btemp = true;

	if (Array1.length != Array2.length)
		return false;
	else
	{
		for (dIndex in Array1)
			if (Array1[dIndex] != Array2[dIndex])
				Btemp = false;
		return Btemp;
	}
}

function split001() {

	apInitTest("split001 ");

	apInitScenario("1. String Split with empty parameter");
	m_scen = "String Split with empty parameter";

	sCat = 'hello';
	VerifySplit(new Array('hello'), 'hello', "VS7 #95869");

	sCat = 'he\0lo';
	VerifySplit(new Array('he\0lo'), 'he\0lo', false);

	sCat = 'he\0\0o';
	VerifySplit(new Array('he\0\0o'), 'he\0\0o', false);

	sCat = 'h\0\0\0o';
	VerifySplit(new Array('h\0\0\0o'), 'h\0\0\0o', false);

	sCat = 'h\t\0\0o';
	VerifySplit(new Array('h\t\0\0o'), 'h\t\0\0o', false);
	
	sCat = 'h\t\0\0\\';
	VerifySplit(new Array('h\t\0\0\\'), 'h\t\0\0\\', false);
	
	sCat = '\"\t\0\0\\';
	VerifySplit(new Array('\"\t\0\0\\'), '\"\t\0\0\\', false);


	apInitScenario("2. String Split with empty String parameter");
	m_scen = "String Split with empty String parameter";

	sCat = 'hello';
	VerifySplit(new Array('h','e','l','l','o'), 'hello', '');

	sCat = 'he\0lo';
	VerifySplit(new Array('h','e','\0','l','o'), 'he\0lo', '');

	sCat = 'he\0\0o';
	VerifySplit(new Array('h','e','\0','\0','o'), 'he\0\0o', '');

	sCat = 'h\0\0\0o';
	VerifySplit(new Array('h','\0','\0','\0','o'), 'h\0\0\0o', '');

	sCat = 'h\t\0\0o';
	VerifySplit(new Array('h','\t','\0','\0','o'), 'h\t\0\0o', '');
	
	sCat = 'h\t\0\0\\';
	VerifySplit(new Array('h','\t','\0','\0','\\'), 'h\t\0\0\\', '');
	
	sCat = '\"\t\0\0\\';
	VerifySplit(new Array('\"','\t','\0','\0','\\'), '\"\t\0\0\\', '');
/*****************************************************************************/


    apEndTest();

}


split001();


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
