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


var iTestID = 53761;


function verify(sAct, sExp, sMes, bugNum){
	if (bugNum == null) bugNum = "";
	if (sAct != sExp)
        apLogFailInfo(sMes, sExp, sAct, bugNum);
}

function retTryFinally(){
	try{return "in try";}
	finally{return "in finally";}
}

function throwFuncExp(){
	var a = new Object();
	return a();
}

function throwObjExp(){
	var a;
	return a.undef;
}

function throwOverflow(){
	return throwOverflow();
}

function tcatch02() {
	var i, e, res, resNum, resDesc;
	var loc_id = apGetLocale();

    apInitTest("tCatch02: JS Runtime errors");

    //----------------------------------------------------------------------------
    apInitScenario("2.1:  No error in try - code executed");

    res = "unchanged";
	try{for(i=0; i<100; i++) res="unchanged"+i;}
	catch(e){
		apLogFailInfo("Catch block executed with no error:  2.11", "","","")
	}
	verify(res,"unchanged99","try block completed:  2.12","");

	res = "unchanged";
	try{for(i=0; i<100; i++) res="unchanged"+i;}
	catch(e){
		apLogFailInfo("Catch block executed with no error:  2.13", "","","")
	}
	finally{
		verify(res,"unchanged99","not all tries executed:  2.14","");
		res = "changed";
	}
	verify(res,"changed","finally did not execute:  2.15","");

	//----------------------------------------------------------------------------
    apInitScenario("2.2:  No error in try - empty block");
    
	res = "unchanged";
	try{}
	catch(e){
		apLogFailInfo("Catch block executed with no error:  2.21", "","","")
	}
	verify(res,"unchanged","something screwy:  2.22","");

	res = "unchanged";
	try{}
	catch(e){
		apLogFailInfo("Catch block executed with no error:  2.23", "","","")
	}
	finally{
		verify(res,"unchanged","catch block executed:  2.24","");
		res = "changed";
	}
	verify(res,"changed","finally not executed:  2.25","");

	//----------------------------------------------------------------------------
    apInitScenario("2.3:  One runtime error in try");
    
	try{throwObjExp();}
	catch(e){
			verify(e instanceof Error,true,"error identifier not error:  2.31","");
		@if(@_jscript_version<7)
			verify(e.number,"-2146823281","wrong error:  2.32","");
			if(loc_id==1033) verify(e.description,"'undef' is null or not an object","error message:  2.325","");
		@else
			verify(e.number,"-2146827864","wrong error:  2.32","");
			if(loc_id==1033) verify(e.description,"Object required","error message:  2.325","");
		@end
	}

	try{
		res = "in try";
		throwObjExp();
	}
	catch(e){
		verify(e instanceof Error,true,"error identifier not error:  2.33","");
		@if(@_jscript_version<7)
			verify(e.number,"-2146823281","wrong error:  2.34","");
			if(loc_id==1033) verify(e.description,"'undef' is null or not an object","error message:  2.345","");
		@else
			verify(e.number,"-2146827864","wrong error:  2.34","");
			if(loc_id==1033) verify(e.description,"Object required","error message:  2.345","");
		@end
	}
	finally{
		res = "in finally";
		verify(e instanceof Error,true,"e outside catch block:  2.35","");
		@if(@_jscript_version<7)
			verify(e.number,"-2146823281","wrong error:  2.36","");
			if(loc_id==1033) verify(e.description,"'undef' is null or not an object","error message:  2.365","");
		@else
			verify(e.number,"-2146827864","wrong error:  2.36","");
			if(loc_id==1033) verify(e.description,"Object required","error message:  2.365","");
		@end
	}
	verify(res,"in finally","finally did not execute:  2.38","");
	
	//----------------------------------------------------------------------------
    apInitScenario("2.4:  Five runtime errors in try");

	try{
		throwFuncExp();
		throwObjExp();
		throwObjExp();
		throwObjExp();
		throwObjExp();
		apLogFailInfo("Script shouldn't be executed:  2.401","","","");
	}
	catch(e){
		verify(e instanceof Error,true,"error object not thrown:  2.402","");
		if(loc_id == 1033) verify(e.description, "Function expected","wrong error msg:  2.403","");
		verify(e.number,"-2146823286","wrong error:  2.404","");
	}

	try{
		throwFuncExp();
		throwObjExp();
		throwObjExp();
		throwObjExp();
		throwObjExp();
		apLogFailInfo("Script shouldn't be executed:  2.405", "","","");
	}
	catch(e){
		verify(e instanceof Error,true,"error object not thrown:  2.406","");
		if(loc_id == 1033) verify(e.description,"Function expected","wrong error msg:  2.407","");
		verify(e.number,"-2146823286","wrong error:  2.408","");
	}
	finally{
		res = "in finally";
		verify(e instanceof Error,true,"error object not thrown:  2.409","");
		if(loc_id == 1033) verify(e.description, "Function expected","wrong error msg:  2.410","");
		verify(e.number,"-2146823286","wrong error:  2.411","");
	}
	verify(res,"in finally","finally block not executed:  2.412","");

	//----------------------------------------------------------------------------
    apInitScenario("2.6:  Try with cross-language error");
@if(@_win32)
if (apGlobalObj.apGetHost().indexOf("IE") != 0){
//	if(apGlobalObj.apGetHost().toLowerCase() == "vbstest"){

	try{
		var x = new ActiveXObject("scriptcontrol");
		x.language = "vbscript";
		x.addcode("function test : y = 1/0 : end function");
		try{
			x.codeobject.test();
		}catch(e){
			verify(e instanceof Error,true,"error object not thrown:  2.61","");
			if(loc_id == 1033) verify(e.description,"Division by zero","wrong error msg:  2.62","");
			@if(@_jscript_version < 7)
				verify(e.number,"-2146828277","wrong error:  2.63","");
			@else
				verify(e.number,"-2146823266","wrong error:  2.63","");
			@end
		}

		res = "unchanged";
		try{
			res = "in try";
			x.codeobject.test();
		}catch(e){
			verify(e instanceof Error,true,"error object not thrown:  2.64","");
			if(loc_id == 1033) verify(e.description,"Division by zero","wrong error msg:  2.65","");
			@if(@_jscript_version < 7)
				verify(e.number,"-2146828277","wrong error:  2.66","");
			@else
				verify(e.number,"-2146823266","wrong error:  2.66","");
			@end
		}finally{
			verify(res,"in try","try block not executed:  2.67","");
			res = "in finally";
		}
		verify(res,"in finally","finally not executed:  2.68","");

	}catch(e){apLogFailInfo("Is msscript.ocx registered:  2.69","no error","" + e,"");}

//	}
}
@end
	//----------------------------------------------------------------------------
    apInitScenario("2.7:  Try with no catch - must have finally");

	res = "unchanged";
	try{
		try{
			res = "in try";
			throwFuncExp();
		}
		finally{
			verify(res,"in try","try block not executed:  2.701","");
			res = "in finally";
		}
	}
	catch(e){
		verify(res,"in finally","finally not executed:  2.702","");
		verify(e instanceof Error,true,"error object not thrown:  2.703","");
		if(loc_id == 1033) verify(e.description,"Function expected","wrong error msg:  2.704","");
		verify(e.number,"-2146823286","wrong error:  2.705","");
		res = "in catch";
	}
	verify(res,"in catch","catch block not executed:  2.706","");
	
	res = "unchanged";
	try{
		try{
			res = "in try";
			throwFuncExp();
		}
		finally{
			verify(res,"in try","try block not executed:  2.707","");
			res = "in finally";
		}
	}
	catch(e){
		verify(res,"in finally","finally not executed:  2.708","");
		verify(e instanceof Error,true,"error object not thrown:  2.709","");
		if(loc_id == 1033) verify(e.description,"Function expected","wrong error msg:  2.710","");
		verify(e.number,"-2146823286","wrong error:  2.711","");
		res = "in catch";
	}
	finally{
		verify(res,"in catch","catch block not executed:  2.712","");
		res = "in second finally";
	}
	verify(res,"in second finally","finally not executed:  2.713","");

	//----------------------------------------------------------------------------
    apInitScenario("2.8:  Unusual code after catch");

	try{throwObjExp();}
	catch(e){}
	for(i=0; i<100; i++){
		@if(@_jscript_version<7)
			verify(e.number,"-2146823281","error data corrupted:  2.81","");
			if(loc_id==1033) verify(e.description,"'undef' is null or not an object","error msg corrupted:  2.82","");
		@else
			verify(e.number,"-2146827864","error data corrupted:  2.81","");
			if(loc_id==1033) verify(e.description,"Object required","error msg corrupted:  2.82","");
		@end
	}	//end of for

	try{throwObjExp();}
	catch(e){}
	finally{}
	for(i=0; i<100; i++){
		@if(@_jscript_version<7)
			verify(e.number,"-2146823281","error data corrupted:  2.83","");
			if(loc_id==1033) verify(e.description,"'undef' is null or not an object","error msg corrupted:  2.84","");
		@else
			verify(e.number,"-2146827864","error data corrupted:  2.83","");
			if(loc_id==1033) verify(e.description,"Object required","error msg corrupted:  2.84","");
		@end
	}	//end of for

	//--------------------------------------------------------------------------
    apInitScenario("Scenario: VS7");
	var sExp, sAct;

	sExp = "in finally";
	sAct = retTryFinally();
	if(sExp != sAct)
	    apLogFailInfo( "Scenario failed: ", sExp, sAct, 127207);

    apInitScenario("Scenario: VS7");
	resDesc = resNum = "unchanged";
	@if(@_jscript_version < 7)	
		try{switch (null.foo){}}
		catch(e){resNum=e.number;resDesc=e.description;}
		verify(resNum,"-2146823281","Wrong error:  ",157902);
		if(loc_id == 1033) verify(resDesc,"'null.foo' is null or not an object","wrong error msg",157902);
	@end

	//--------------------------------------------------------------------------
    apInitScenario("Multiple (~5) catches");
/*
	res = "unchanged";
	try{
		x = new err1();
	}
	catch(e:e instanceof String){
		verify(res,"unchanged","");
		res = "in catch1";
	}
	catch(e:e instanceof Date){
		verify(res,"unchanged","");
		res = "in catch2";
	}
	catch(e:e instanceof Number){
		verify(res,"unchanged","");
		res = "in catch3";
	}
	catch(e:e instanceof verify){
		verify(res,"unchanged","");
		res = "in catch4";
	}
	catch(e:e instanceof Error){
		verify(res,"unchanged","");
		res = "in catch5";
	}
	verify(res,"in catch5","");

	var res = "unchanged";
	try{
		x = new err1();
	}
	catch(e:e instanceof String){
		verify(res,"unchanged","");
		res = "in catch1";
	}
	catch(e:e instanceof Date){
		verify(res,"unchanged","");
		res = "in catch2";
	}
	catch(e:e instanceof Number){
		verify(res,"unchanged","");
		res = "in catch3";
	}
	catch(e:e instanceof verify){
		verify(res,"unchanged","");
		res = "in catch4";
	}
	catch(e:e instanceof Error){
		verify(res,"unchanged","");
		res = "in catch5";
	}
	finally{
		verify(res,"in catch5","");
		res = "in finally";
	}
	verify(res,"in finally","");
*/

	apEndTest();
}


tcatch02();


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
