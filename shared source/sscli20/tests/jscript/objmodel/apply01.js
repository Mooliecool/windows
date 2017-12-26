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


var iTestID = 79878;


/* apply01.js - test the apply() function of JScript
	this test case is similar to call01.js
*/

function verify(sRes, sExp, sMsg, sBug) {
    if (sRes != sExp) 
        apLogFailInfo (sMsg, sExp, sRes, sBug);
}


@if(!@aspx)
	function add_x(x) {	
		return x + this.n;
	}
	function add_xy(x, y) {
		return x + y + this.n;
	}
	function adder() {
		this.add_x = add_x;
		this.add_xy = add_xy;
		this.assignAddername = assignAddername;
	}
	function getAddername() {
		return this.name
	}
	function assignAddername(name) {
		return (this.name = name);
	}
@else
	expando function add_x(x) {	
		return x + this.n;
	}
	expando function add_xy(x, y) {
		return x + y + this.n;
	}
	expando function adder() {
		this.add_x = add_x;
		this.add_xy = add_xy;
		this.assignAddername = assignAddername;
	}
	expando function getAddername() {
		return this.name
	}
	expando function assignAddername(name) {
		return (this.name = name);
	}
@end

function apply01() {

	apInitTest ("Function.apply tests");
	
	apInitScenario("1. Use apply function with different arguments");
	
		var a = new adder();
		a.n = 10;
		verify (a.add_x(5), 15, "call a.add_x(5) directly", "");
		verify (a.add_x.apply(a, [12]), 22, "call a.add_xy via apply()", "");

		verify (a.add_xy(3, 5), 18, "call a.add_xy(3, 5) directly", "");
		verify (a.add_xy.apply(a, [10, 5]), 25, "call a.add_xy via apply()", "");
		
		a.name = "Addername";
		a.getAddername = getAddername;
		verify (a.getAddername.apply(a, []), "Addername", "calling apply with empty array", "");

		verify (a.assignAddername.apply(a, ["newAddername"]), "newAddername", "calling apply with a string arg in array", "");

	apInitScenario("2  Large sparse array");
		var size = 0x80000000
              @if(@_jscript_version >= 7)		
                var err:Error = null
              @else
                var err = null
              @end
		do{
			try{
				function f() {}
				var a = new Array(size);
				f.apply(null, a);
				apLogFailInfo("Error expected","Out of memory","",340725);
				err = new Error()
			}catch(e){
				err = e
			}
			size >>>= 1;
		}while(err.description != "Out of memory")
		if (err.description != "Out of memory") 
			apLogFailInfo("Error expected","Out of memory","",340725);
	apInitScenario("3  control returned from previous scenario");

	apEndTest();
}


apply01();


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
