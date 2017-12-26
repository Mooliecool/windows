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


var iTestID = 220253;


function verify(sRes, sExp, sMsg, sBug) {
	if(sBug==null) sBug="";
    if(sRes!=sExp) 
        apLogFailInfo(sMsg,sExp,sRes,sBug);
}

function ParamArray(str, ... args : String[]) {
	var i;
	var result = String(str);
	for(i=0;i<args.length;i++) result+=" "+args[i];
	return result;
}

function ParamArrayType(str, ... args : String[]) {
	return args;
}

function ParamArrayErr1(str, ... args : String[]) {
	var x,err;
	try{x=args[0];}
	catch(e){err=e}
	return err;
}

var arg2:String[];	//for scope tests later on
function ParamArrayScope1(str, ... arg2 : String[]) {
	var i;
	var result = String(str);
	for(i=0;i<arg2.length;i++) result+=" "+arg2[i];
	return result;
}

function parray01() {
	apInitTest("parray01 - Parameter array nightly tests");
	var i, g_str, undef, ret_err, g_lang = apGetLocale();

	apInitScenario("1:  Baseline & Types");
		var foo1 = ParamArray;
		var foo2 = new Function("str, ... args:String[]", "var i; var result = String(str); for(i=0;i<args.length;i++) result+=' '+args[i]; return result;");

		verify(ParamArray("Hi", "Kirk", "Mason", "Ian", "Eric"), "Hi Kirk Mason Ian Eric", "1.1:  ParamArray failed using 4 items",221514);
		verify(ParamArray("Hi"), "Hi",  "1.2:  ParamArray failed using no items", "");
		verify(ParamArrayType("Hi", "Kirk", "Mason", "Ian", "Eric"), "Kirk,Mason,Ian,Eric", "1.3:  ParamArrayType failed", "");
		verify(foo1("Hi", "Kirk", "Mason", "Ian", "Eric"), "Hi Kirk Mason Ian Eric", "1.4:  instance of ParamArray failed using 4 items", 232098);
		verify(foo1("Hi"), "Hi",  "1.5:  instance of ParamArray failed using no items", "");
		verify(foo2("Hi"), "Hi",  "1.7:  constructed ParamArray failed using no items", "");

	apInitScenario("2:  Errors");
		ret_err = undef;
		ret_err = ParamArrayErr1("Hi");

		verify(ret_err.number,-2146823266,"2.1:  out of bounds error","");
		if(g_lang==1033) verify(ret_err.description,"Index was outside the bounds of the array.","2.2:  out of bounds error msg","");
		else if(g_lang==1041) verify(ret_err.description,"Index was outside the bounds of the array.","2.2:  out of bounds error msg","");
		verify(ret_err.description,"Index was outside the bounds of the array.","2.2:  out of bounds error msg","");

	apInitScenario("3:  Scoping");
		arg2 = ["You","should","not","see","this"];
		verify(ParamArrayScope1("Hi", "Kirk", "Mason", "Ian", "Eric"), "Hi Kirk Mason Ian Eric", "3.1:  ParamArrayScope1 failed using 4 items", "");
		verify(ParamArrayScope1("Hi"), "Hi",  "3.2:  ParamArrayScope1 failed using no items", "");
		g_str = arg2[0];
		for(i=1;i<arg2.length;i++) g_str+=" "+arg2[i];
		verify(g_str,"You should not see this","3.3:  global values altered by ParamArrayScope1","");

		function ParamArrayScope2(str, ... arg2 : String[]) {
			var i;
			var result = String(str);
			for(i=0;i<arg2.length;i++) result+=" "+arg2[i];
			return result;
		}
		arg2 = ["You","should","not","see","this"];
		verify(ParamArrayScope2("Hi", "Kirk", "Mason", "Ian", "Eric"), "Hi Kirk Mason Ian Eric", "3.4:  ParamArrayScope2 failed using 4 items", "");
		verify(ParamArrayScope2("Hi"), "Hi",  "3.5:  ParamArrayScope2 failed using no items", "");
		g_str = arg2[0];
		for(i=1;i<arg2.length;i++) g_str+=" "+arg2[i];
		verify(g_str,"You should not see this","3.6:  global values altered by ParamArrayScope2","");

	apInitScenario("4.  Return control from last scenario");
    apEndTest();
}


parray01();


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
