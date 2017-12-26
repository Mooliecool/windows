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


var iTestID = 52715;


var undef;

function verify(sAc, sEx, sMes, bNum){
	if (bNum == null) bNum = ""
	if (sAc != sEx)
        apLogFailInfo(sMes, sEx, sAc, bNum);
}

function del001(){
	var obj, sAct, sExp;
    apInitTest("del001");

//----------------------------------------------------------------------------
    apInitScenario("1.1 - Delete obj props");

	obj = new Object();
	sExp = new String("Test");
	obj.a = sExp;
	verify(obj.a,sExp,"add field to obj - string","");
	delete obj.a;
	verify(obj.a,undef,"remove field from obj - string","");

	obj = new Object();
	sExp = new Number(12);
	obj.a = sExp;
	verify(obj.a,sExp,"add field to obj - number","");
	delete obj.a;
	verify(obj.a,undef,"remove field from obj - number","");

	obj = new Object();
	sExp = new Date();
	obj.a = sExp;
	verify(obj.a,sExp,"add field to obj - date","");
	delete obj.a;
	verify(obj.a,undef,"remove field from obj - date","");

	obj = new Object();
	sExp = new Object();
	obj.a = sExp;
	verify(obj.a,sExp,"add field to obj - object","");
	delete obj.a;
	verify(obj.a,undef,"remove field from obj - object","");

	obj = new Object();
	sExp = new Function("return true");
	obj.a = sExp;
	verify(obj.a,sExp,"add field to obj - function","");
	delete obj.a;
	verify(obj.a,undef,"remove field from obj - function","");

	@if(@_win32)
		obj = new Object();
		sExp = new ActiveXObject("Scripting.Dictionary");
		obj.a = sExp;
		verify(obj.a,sExp,"add field to obj - activeXobj","");
		delete obj.a;
		verify(obj.a,undef,"remove field from obj - activeXobj","");
	@end

//----------------------------------------------------------------------------
    apInitScenario("1.2 - cannot delete objs");

	sAct = sExp = new Object();
	sAct.a = "test";
	verify(sAct.a,"test","add field to obj","");
	delete sAct;
	verify(sAct,sExp,"cannot delete object",124791);
	verify(sAct.a,"test","field maintained through delete - object","");

	sAct = sExp = new Date();
	delete sAct;
	verify(sAct,sExp,"cannot delete date","");

	sAct = sExp = new Function("return true;");
	delete sAct;
	verify(sAct,sExp,"cannot delete function","");
	verify(sAct(), true,"function returns correct value after delete","");

	sExp = sAct = new String("Test");
	delete sAct;
	verify(sAct,sExp,"cannot delete string","");

	sExp = sAct = new Number(12);
	delete sAct;
	verify(sAct,sExp,"cannot delete number","");

	@if(@_win32)
		sAct = sExp = new ActiveXObject("Scripting.Dictionary");
		delete sAct;
		verify(sAct,sExp,"cannot delete activeXobj","");
	@end

//----------------------------------------------------------------------------
    apInitScenario("1.3 - return value:  false");
	sExp = false;

	obj = new Object();
	sAct = delete obj;
	verify(sAct,sExp,"return from deleted object","");

	obj = new Date();
	sAct = delete obj;
	verify(sAct,sExp,"return from deleted date","");

	obj = new Function("return true;");
	sAct = delete obj;
	verify(sAct,sExp,"return from deleted function","");

	obj = new String("Test");
	sAct = delete obj;
	verify(sAct,sExp,"return from deleted string","");

	obj = new Number(12);
	sAct = delete obj;
	verify(sAct,sExp,"return from deleted number","");

	@if(@_win32)
		obj = new ActiveXObject("Scripting.Dictionary");
		sAct = delete obj;
		verify(sAct,sExp,"return from deleted activeXobj","");
	@end
//----------------------------------------------------------------------------
    apInitScenario("1.4 - return value:  true");
	sExp = true;

	obj = new Object();
	obj.a = new String("Test");
	sAct = delete obj.a;
	verify(sAct,sExp,"return from deleted string field","");

	obj = new Object();
	obj.a = new Number(12);
	sAct = delete obj.a;
	verify(sAct,sExp,"return from deleted number field","");

	obj = new Object();
	obj.a = new Date();
	sAct = delete obj.a;
	verify(sAct,sExp,"return from deleted date field","");

	obj = new Object();
	obj.a = new Object();
	sAct = delete obj.a;
	verify(sAct,sExp,"return from deleted object field","");

	obj = new Object();
	obj.a = new Function("return true");
	sAct = delete obj.a;
	verify(sAct,sExp,"return from deleted function field","");

	@if(@_win32)
		obj = new Object();
		obj.a = new ActiveXObject("Scripting.Dictionary");
		sAct = delete obj.a;
		verify(sAct,sExp,"return from deleted activeXobj field","");
	@end

    apEndTest();
}


del001();


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
