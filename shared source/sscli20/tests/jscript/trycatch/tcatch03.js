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


var iTestID = 215624;


function verify(sRes, sExp, sMsg, sBug) {
	if(sBug == null) sBug = "";
	if(sRes != sExp) apLogFailInfo (sMsg, sExp, sRes, sBug);
}

function recurse() { recurse(); }

var foo_err = new Object();
function foo() {
  try {eval("1+");}
  catch(e) {
    foo_err.number = e.number;
    foo_err.description = e.description;
  }
}

var str = "";
function test(){
  var i;
  for (i=2; i<33000; i++) str += String.fromCharCode(32);
  var bigErr = new Error(123456, str)
  throw(bigErr)
}

function tcatch03() {
	var sErr,sNum, sv, lcid;
	sv = ScriptEngineMajorVersion() + (ScriptEngineMinorVersion()/10);
	lcid = apGetLocale();

	apInitTest ("Tcatch03 - funky trycatch combos");
//-------------------------------------------------------------------------------
	apInitScenario("1. error object creation");

	var err = new Error("mase is the king");	//single param becomes err.message
	try{throw err}
	catch(e){sErr = e.description}
	verify(sErr, "mase is the king", "error not created or thrown properly","");

	err = new Error(328, "mase is the king");	//two params becomes err.number err.message
	try{throw err}
	catch(e){sErr=e.description;sNum=e.number;}
	verify(sErr,"mase is the king","error not created or thrown properly","");
	verify(sNum,328,"error not created or thrown properly","");

	var dat = new Date();
	err = new Error(dat);
	try{throw err}
	catch(e){sErr=e.description;sNum=e.number;}
	if(sv<7.0){
		verify(sErr,"","date object as error","295545");
		verify(sNum,dat.valueOf(),"error number not assigned correctly","295545");
	}else{
		verify(sErr,dat,"date object as error","295545");
		verify(sNum,0,"error number not assigned correctly","295545");
	}

//-------------------------------------------------------------------------------
	apInitScenario("3. error in a function");

	foo();
	if (sv<7.0){
		verify(foo_err.number,-2146827286,"wrong error or error not thrown inside function",64205);
		if (lcid==1033) verify(foo_err.description,"Syntax error","wrong error msg from inside function",64205);
	}else{
		verify(foo_err.number,-2146827093,"wrong error or error not thrown inside function",64205);
		if (lcid==1033) verify(foo_err.description,"Expected expression","wrong error msg from inside function",64205);
	}

//-------------------------------------------------------------------------------
	apInitScenario("4. throw error with huge description");

	try{ test()}
	catch(e){ sNum=e.number; sErr=e.description; }
	verify(sNum,"123456","wrong error from bigErr","");
	verify(sErr,str,"wrong msg from bigErr","VS7 513397");

//-------------------------------------------------------------------------------
	apInitScenario("5. return control from last scenario");
	apEndTest();
}


tcatch03();


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
