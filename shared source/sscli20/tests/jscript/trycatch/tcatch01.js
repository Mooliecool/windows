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


var iTestID = 53760;


function verify(sAct, sExp, sMes, bugNum){
	if (bugNum == null) bugNum = "";
	if (sAct != sExp)
        apLogFailInfo(sMes, sExp, sAct, bugNum);
}

function throwFuncExp1(){
@if(@_fast)
	var obj = new TypeError();
	obj.number = -2146823286;
	obj.message = obj.description = "Function expected";
	throw obj;
@else
	return a();
@end
}

function throwFuncExp2(){
	var a = new Object();
	return a();
}

function throwFuncExp3(){
	var a;
	return a();
}

function throwObjExp1(){
	var a;
	return a.undef;
}

function throwObjExp2(){
@if(@_fast)
	var obj = new ReferenceError();
	obj.number = -2146823279;
	obj.message = obj.description = "'a' is undefined";
	throw obj;
@else
	return a.undef;
@end
}

function throwUndef(){
@if(@_fast)
	var obj = new ReferenceError();
	obj.number = -2146823279;
	obj.message = obj.description = "'undef' is undefined";
	throw obj;
@else
	return undef;
@end
}

function throwNum(){
	throw 123.456;
}
function throwString(){
	throw "Test string";
}

@if(!@aspx)
	function custObj(){
		this.prop1 = "test";
	}
@else
	expando function custObj(){
		this.prop1 = "test";
	}
@end

function tcatch01(){
	var resNum, resDesc;
	var loc_id = apGetLocale();

	apInitTest("tCatch01: Acceptance level tests for TryCatch");

	//----------------------------------------------------------------------------
	apInitScenario("tCatch-1.1: throw standard runtime errors");

	resNum = resDesc = "unchanged";
	try{throwUndef();}
	catch(e){
		if(e instanceof Error){
			resNum = e.number;
			resDesc = e.description;
		}
	}
	verify(resNum,-2146823279,"Wrong error:  1.101","");
	if(loc_id == 1033) verify(resDesc,"'undef' is undefined","Not correct error:  1.102","");

	resNum = resDesc = "unchanged";
	try{throwObjExp1();}
	catch(e){
		if(e instanceof Error){
			resNum = e.number;
			resDesc = e.description;
		}
	}

	@if(@_jscript_version < 7)
		verify(resNum,-2146823281,"Wrong error:  1.103","");
		if(loc_id == 1033) verify(resDesc,"'undef' is null or not an object","Not correct error:  1.104","");
	@else
		verify(resNum,-2146827864,"Wrong error:  1.103","");
		if(loc_id == 1033) verify(resDesc,"Object required","Not correct error:  1.104","");
	@end

	resNum = resDesc = "unchanged";
	try{throwObjExp2();}
	catch(e){
		if(e instanceof Error){
			resNum = e.number;
			resDesc = e.description;
		}
	}
	verify(resNum,-2146823279,"Wrong error:  1.105","");
	if(loc_id == 1033) verify(resDesc,"'a' is undefined","Not correct error:  1.106","");

	resNum = resDesc = "unchanged";
	try{throwFuncExp1();}
	catch(e){
		if(e instanceof Error){
			resNum = e.number;
			resDesc = e.description;
		}
	}
	@if(@_jscript_version < 7)
		verify(resNum,-2146823281,"Wrong error:  1.107","");
		if(loc_id == 1033) verify(resDesc,"Object expected","Not correct error:  1.108","");
	@else
		verify(resNum,-2146823286,"Wrong error:  1.107","");
		if(loc_id == 1033) verify(resDesc,"Function expected","Not correct error:  1.108","");
	@end

	resNum = resDesc = "unchanged";
	try{throwFuncExp2();}
	catch(e){
		if(e instanceof Error){
			resNum = e.number;
			resDesc = e.description;
		}
	}
	verify(resNum,-2146823286,"Wrong error:  1.109","");
	if(loc_id == 1033) verify(resDesc,"Function expected","Not correct error:  1.110","");

	resNum = resDesc = "unchanged";
	try{throwFuncExp3();}
	catch(e){
		if(e instanceof Error){
			resNum = e.number;
			resDesc = e.description;
		}
	}
	@if(@_jscript_version < 7)
		verify(resNum,-2146823281,"Wrong error:  1.111","");
		if(loc_id == 1033) verify(resDesc,"Object expected","Not correct error:  1.112","");
	@else
		verify(resNum,-2146823286,"Wrong error:  1.111","");
		if(loc_id == 1033) verify(resDesc,"Function expected","Not correct error:  1.112","");
	@end

	//----------------------------------------------------------------------------
    apInitScenario("tCatch-1.2: throw non errors");
  
	try{throwNum();}
	catch(e){
		if(e instanceof Error){
			apLogFailInfo( "Error - not member of error:  1.21","","","");
		}
		verify(e,123.456,"Wrong value for error:  1.22","");
	}

	try{throwString();}
	catch(e){
		if(e instanceof Error){
			apLogFailInfo( "Error - not member of error:  1.23","","","");
		}
		verify(e,"Test string","Wrong value for error:  1.24","");
	}

	try{throw new custObj();}
	catch(e){
		if(e instanceof Error){
			apLogFailInfo( "Error - not member of error:  1.25","","","");
		}
		verify(e.prop1,"test","Wrong value for error:  1.26","");
	}

	//----------------------------------------------------------------------------
    apInitScenario("tCatch-1.3: nested try catch");

	resNum = resDesc = "unchanged";
	try{
		try{
			try{throwObjExp1();}
			catch(e){
				resNum = e.number;
				resDesc = e.description;
			}

			@if(@_jscript_version < 7)
				verify(resNum,-2146823281,"Wrong error:  1.31","");
				if(loc_id == 1033) verify(resDesc,"'undef' is null or not an object","Not correct error:  1.32","");
			@else
				verify(resNum,-2146827864,"Wrong error:  1.31","");
				if(loc_id == 1033) verify(resDesc,"Object required","Not correct error:  1.32","");
			@end
			try{throwNum();}
			catch(e){
				if(e instanceof Error){
					apLogFailInfo( "Error - not member of error:  1.33","","","");
				}
				verify(e,123.456,"Wrong value for error:  1.34","");
			}
		}
		catch(e){
			apLogFailInfo( "Error - caught in wrong nesting:  1.35","","","");
		}
	}
	catch(e){
		apLogFailInfo( "Error - caught in wrong nesting:  1.36","","","");
	}

	//----------------------------------------------------------------------------
	apEndTest();
}


tcatch01();


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
