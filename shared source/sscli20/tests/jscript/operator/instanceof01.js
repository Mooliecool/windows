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


var iTestID = 227808;

/* -------------------------------------------------------------------------
  Test: 	instanceof 01.js
   
  Product:	JScript
 
  Area:		Control
 
  Purpose:	check instanceof for CC Run.
 
  Scenarios:	
		
  Notes: 	
 ---------------------------------------------------------------------------
  
 
	[0] 	qiongou -Created 11/27/2001
 -------------------------------------------------------------------------*/

/*---------------------------------*/
import System;

var sAct="";
var sExp = "";
var sActErr = "";
var sExpErr = "";
var sErrThr = "";

class A { }
class B extends A {}

expando class E 
{
	var a : Array = [0,1,2];
	expando function f() {return "this is expando fucntion"}
}

//***********************************
function instanceof01()
{
	apInitTest("");
	
	apInitScenario ("1. instanceof Date Object");
	var d1: Date = new Date
	sAct = d1 instanceof Date;
	if (sAct != true) 
		apLogFailInfo ("1. instanceof Date Object",true ,sAct,"");

	apInitScenario ("2. instanceof Number Object");	
	var n2 = new Number(1234);
	sAct = n2 instanceof Number;
	if (sAct != true) 
		apLogFailInfo ("2. instanceof Number Object",true ,sAct,"");

	
	apInitScenario ("3. instanceof DateTime Object");	
	var d3: DateTime = DateTime.Now;
	sAct = d3 instanceof DateTime;
	if (sAct != true) 
		apLogFailInfo ("3. instanceof DateTime Object",true ,sAct,"");

	apInitScenario ("4. instanceof function Object");
	function square(x : int) : int {
   		return x*x
	}
	function bracket(s : String) : String{
   		return("[" + s + "]");
	}
	var f4 = new square;
	sAct = f4 instanceof square;
	if (sAct != true) 
		apLogFailInfo ("4. instanceof function Object",true ,sAct,"");
	sAct = f4 instanceof bracket;
	if (sAct != false ) 
		apLogFailInfo ("4. instanceof function Object",true ,sAct,"");
	
	apInitScenario ("5. instanceof Object");
	var o5 = new Object;
	sAct = o5 instanceof Object;
	if (sAct != true) 
		apLogFailInfo ("5. instanceof Object",true ,sAct,"");

	apInitScenario ("6. instanceof RegExp");
	var re: RegExp = new RegExp("d(b+)(d)","ig");
	var arr6: Array = re.exec("cdbBdbsbdbdz");
@if (!@_fast)
	 print(RegExp.$1);
	print(RegExp.$2);
	print(RegExp.$3);
	print("$1" in RegExp);  //slow mode only
@end
	sAct = (re instanceof RegExp);
	if (sAct != true) 
		apLogFailInfo ("6. instanceof RegExp",true ,sAct,"");


	apInitScenario ("7. instanceof class")
	var a: A = new A();
	sAct = (a instanceof A);
	if (sAct != true) 
		apLogFailInfo ("7. instanceof class",true ,sAct,"");



	apInitScenario ("8. instanceof sub class")
	var b: B = new B();
	sAct = (b instanceof B);
	if (sAct != true) 
		apLogFailInfo ("8.1 instanceof sub class",true ,sAct,"");

	sAct = (b instanceof A);
	if (sAct != true) 
		apLogFailInfo ("8.2 instanceof sub class",true ,sAct,"");

	
	apInitScenario ("9. instanceof null Object")	
	var o9 : Object;
	sAct = (o9 instanceof Object);
	if (sAct != false) 
		apLogFailInfo ("9. instanceof null Object",false ,sAct,"");
	
	apInitScenario ("10. instanceof expando class")	
	var e: E = new E();
	sAct = (e instanceof E);
	if (sAct != true) 
		apLogFailInfo ("10. instanceof expando class",true ,sAct,"");

	apInitScenario ("11. instanceof in eval");
	var sr = false; //""
	var s11 = "var str = new E();"+
		  "sr = str instanceof E" ;
	eval(s11);
	if (sr != true)
		apLogFailInfo ("11. instanceof in eval",true ,sAct,"");

	apEndTest ();

}

	

instanceof01();


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
