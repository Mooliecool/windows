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


var iTestID = 53764;

@if(@_fast)
  var i;
@end
var undef;
function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "Scenario failed: "+sMes, sExp, sAct, "");
}
function cObj() { this.test = "pass"; }

function tcatch06() 
{
    apInitTest("tCatch06: Instanceof tests");

    //----------------------------------------------------------------------------
	apInitScenario("Instanceof with null string");

	verify("" instanceof String, false,"1");
	verify(new String("") instanceof String, true,"2");
	verify(new String("") instanceof Object, true,"3");
	//verify(new String("") instanceof new String(""), false,"4");
	//----------------------------------------------------------------------------
	apInitScenario("Instanceof with a string");

	verify("asd" instanceof String, false,"1");
	verify(new String("asd") instanceof String, true,"2");
	verify(new String("asd") instanceof Object, true,"3");
	//verify(new String("asd") instanceof new String("asd"), false,"4");

	//----------------------------------------------------------------------------
	apInitScenario("Instanceof with a number");

	verify(123 instanceof Number, false,"1");
	verify(new Number(123) instanceof Number, true,"2");
	verify(new Number(123) instanceof Object, true,"3");
	//verify(new Number(123) instanceof new Number(123), false,"4");

	//----------------------------------------------------------------------------
	apInitScenario("Instanceof with a date");

	verify(new Date(0) instanceof Date, true,"1");
	verify(new Date(0) instanceof Object, true,"2");
	//verify(new Date(0) instanceof new Date(0), false,"3");

	//----------------------------------------------------------------------------
	apInitScenario("Instanceof with a custom object");
@if(@_fast)
	verify(new cObj() instanceof cObj, true,"1");
	verify(new cObj() instanceof Object, true,"2");
	//verify(new cObj(0) instanceof new cObj(0), false,"3");
@else
	verify(new cObj(0) instanceof cObj, true,"1");
	verify(new cObj(0) instanceof Object, true,"2");
	//verify(new cObj(0) instanceof new cObj(0), false,"3");
@end

	//----------------------------------------------------------------------------
	apInitScenario("Instanceof with an activeX object");

@if(@_win32)
if (apGlobalObj.apGetHost().indexOf("IE") != 0){
//if(apGlobalObj.apGetHost().toLowerCase() == "vbstest"){

       try{
	  var x =  new ActiveXObject("ScriptControl");
	  verify(x instanceof cObj, false,"");
	  verify(x instanceof Object, false,"");
	  verify(x instanceof ActiveXObject,true,"");
       }catch (e){
         apLogFailInfo ("Is msscript.ocx registered","no error","" + e,"");
       }

//}
}
@end

	//----------------------------------------------------------------------------
	apInitScenario("~1000 instanceof calls");
@if(@_win16)
	var depth = 10;
@else
	var depth = 100;
@end
	for(i=0; i < depth; i++)
	{
		verify("" instanceof String, false,"1");
		verify(new String("") instanceof String, true,"2");
		verify(new String("") instanceof Object, true,"3");
		//verify(new String("") instanceof new String(""), false,"4");
		verify("asd" instanceof String, false,"1");
		verify(new String("asd") instanceof String, true,"2");
		verify(new String("asd") instanceof Object, true,"3");
		//verify(new String("asd") instanceof new String("asd"), false,"4");
@if(@_fast)
		verify(new cObj() instanceof cObj, true,"1");
		verify(new cObj() instanceof Object, true,"2");
@else
		verify(new cObj(0) instanceof cObj, true,"1");
		verify(new cObj(0) instanceof Object, true,"2");
@end
	}	

	apEndTest();

}

tcatch06();


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
