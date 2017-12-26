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


var iTestID = 163151;


function verify(sExp, sAct, sMsg, bugNum){
	if(bugNum == null) bugNum = "";
	if(sExp != sAct){apLogFailInfo(sMsg, sExp, sAct, bugNum);}
}

var eDesc, eNum, eDesc1, eNum1, eDesc2, eNum2;

public class truck{
	public var errNum = "unchanged";
	public var errDesc = "unchanged";
	public var fee = "unchanged";
	public function doob(){
		try{eval("import shooter;");}
		catch(e){errNum=e.number; errDesc=e.description;}
	}
	public function skoob(){
		eval("var fee = 'foo';");
	}
	function floob(){
		try{eval("package stuff{public class stunt{}}");}
		catch(e){errNum=e.number; errDesc=e.description;}
	}
}


@if(!@aspx)
package shooter{
	public class foo{
		public var s = new String();
		public function sets(){
			s = "some string";
		}
	}
}
eDesc = eNum = "unchanged";
try{eval("import shooter;");}
catch(e){eNum=e.number; eDesc=e.description;}
@end

eDesc1 = eNum1 = "unchanged";
try{eval("package stuff1{public class stunt1{}}");}
catch(e){eNum1=e.number; eDesc1=e.description;}

eDesc2 = eNum2 = "unchanged";
try{eval("class stunt2{}}");}
catch(e){eNum2=e.number; eDesc2=e.description;}

var z = 0;
eval("var z = 32");

class cls2_7{
  public static function b(){return(99)}
}

function eval002(){
	var t, x;
	var loc_id = apGetLocale();
	var sv = ScriptEngineMajorVersion() + "." + ScriptEngineMinorVersion();
	apInitTest("eval002:  version 7.0+ restrictions");

//---------------------------------------------------------------------------------------------------
	apInitScenario("2.1 - cannot import package in eval - not in ASPX");
@if(!@aspx)
	verify(-2146827059,eNum,"2.101 import in an eval - global","");
	if(loc_id==1033) verify("The import statement is not valid in this context",eDesc,"2.102 error msg for import in eval","");

	eDesc = eNum = "unchanged";
	try{eval("import shooter;");}
	catch(e){eNum=e.number; eDesc=e.description;}
	verify(-2146827059,eNum,"2.103 import in an eval - local","");
	if(loc_id==1033) verify("The import statement is not valid in this context",eDesc,"2.104 error msg for import in eval","");

	eDesc = eNum = "unchanged";
	try{eval("function fum(i){if(i<1)import shooter;else fum(i-1);}fum(100);","unsafe");}
	catch(e){eNum=e.number; eDesc=e.description;}
	verify(-2146827059,eNum,"2.105 import in an eval - deeply embedded in function","");
	if(loc_id==1033) verify("The import statement is not valid in this context",eDesc,"2.106 error msg for import in eval","");
@end

	t = new truck();
	t.doob();
	verify(-2146827059,t.errNum,"2.107 import in an eval - in a class","");
	if(loc_id==1033) verify("The import statement is not valid in this context",t.errDesc,"2.108 error msg for import in eval","");

//---------------------------------------------------------------------------------------------------
	apInitScenario("2.2 - no new packages in eval");

	verify(-2146827056,eNum1,"2.201 package in an eval - global","");
	if(loc_id==1033) verify("Package declaration not allowed in this context",eDesc1,"2.202 error msg for package in eval - global","");

	eDesc = eNum = "unchanged";
	try{eval("package stuff3{public class stunt3{}}");}
	catch(e){eNum=e.number; eDesc=e.description;}
	verify(-2146827056,eNum,"2.203 package in an eval - local","");
	if(loc_id==1033) verify("Package declaration not allowed in this context",eDesc,"2.204 error msg for package in eval - local","");

	eDesc = eNum = "unchanged";
	try{eval("function fum(i){if(i<1)package stuff2{public class stunt2{}};else fum(i-1);}fum(100);","unsafe");}
	catch(e){eNum=e.number; eDesc=e.description;}
	verify(-2146827056,eNum,"2.205 package in an eval - deeply embedded in function","");
	if(loc_id==1033) verify("Package declaration not allowed in this context",eDesc,"2.206 error msg for package in eval","");

	t = new truck();
	t.floob();
	verify(-2146827056,t.errNum,"2.207 package in an eval - in a class","");
	if(loc_id==1033) verify("Package declaration not allowed in this context",t.errDesc,"2.208 error msg for package in eval","");

//---------------------------------------------------------------------------------------------------
	apInitScenario("2.3 - no new classes in eval");

	verify(-2146827286,eNum2,"2.301 class in an eval - global","");
	if(loc_id==1033) verify("Syntax error",eDesc2,"2.302 error msg for class in eval","");

	eDesc = eNum = "unchanged";
	try{eval("public class stunt4{}");}
	catch(e){eNum=e.number; eDesc=e.description;}
	verify(-2146827179,eNum,"2.303 class in an eval - local","");
	if(loc_id==1033) verify("Class definition not allowed in this context",eDesc,"2.304 error msg for package in eval - local","");

	eDesc = eNum = "unchanged";
	try{eval("function fum(i){if(i<1)public class stunt5{};else fum(i-1);}fum(100);","unsafe");}
	catch(e){eNum=e.number; eDesc=e.description;}
	verify(-2146827179,eNum,"2.305 class in an eval - deeply embedded in function","");
	if(loc_id==1033) verify("Class definition not allowed in this context",eDesc,"2.306 error msg for class in eval","");

//---------------------------------------------------------------------------------------------------
	apInitScenario("2.4 - new vars not visible outside eval");

	x = 0;
	eval("var x = 32;");
	@if(!@_fast)
		verify(32,x,"2.401 new var - local",303210);
	@else
		verify(0,x,"2.401 new var - local","");
	@end

	eval("var y = 32;");
	@if(!@_fast)
		verify(32,y,"2.402 new var - local",213492);
	@end

	var w = 0;
	eval("var w; w = 32;");
	@if(!@_fast)
		verify(32,w,"2.403 new var - local","");
	@else
		verify(0,w,"2.403 new var - local","");
	@end

	eval("var v; v = 32;");
	@if(!@_fast)
		verify(32,v,"2.404 new var - local",);
	@end

	x = 0;
	eval("function fum(i){if(i<1)var x=32;else fum(i-1);}fum(100);","unsafe");
	verify(0,x,"2.405 new var in function in eval","");

	verify(0,z,"2.406 new var - global","");

	t = new truck();
	t.skoob();
	verify("unchanged",t.fee,"2.407 new var declared in class","");

//---------------------------------------------------------------------------------------------------
	apInitScenario("2.5 - no new types in eval");

	eDesc = eNum = "unchanged";
//	function myType(){this.default=7};
//	eval("function myType(){this.default=3}");
//	try{var c :myType;}
//	catch(e){eDesc=e.description; eNum=e.number;}
//print(c.foot);
 
//---------------------------------------------------------------------------------------------------
	apInitScenario("2.6 - only public class members visible");

//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
	apInitScenario("2.7 - static members callable");
	if (cls2_7.b() != 99)
	  apLogFailInfo("failed to call static member", 99, cls2_7.b(), 301201)
//---------------------------------------------------------------------------------------------------


	apEndTest();
}


eval002();


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
