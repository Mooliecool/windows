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


var iTestID = 52919;


function FuncRetVal() { return 655.35; }
function myObjVal() { return 123321; }


@if(!@aspx)
	function myObject() { this.toString = myObjVal; }
@else
	expando function myObject() { this.toString = myObjVal; }
@end

function divi02() {
    var tmp,res,expected,emptyVar;

    var myDate = new Date();
    myDate.setTime(0); // this is Wed Dec 31 16:00:00 1969

    var numvar = 4321;
    var strvar = "57";
    var boolvar = true; // new Boolean(true);
    var emptystrvar = "";

    var ary = new Array();
    ary[2] = 12.14;
    ary[3] = 13.14;
    ary[4] = 15.14;

    var myObj = new myObject();
    var myStr = new String("112");

    apInitTest("divi02");

    apInitScenario("numvar / numvar");
    res = numvar; res /= numvar;
    expected = 1;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar / numvar");
    res = strvar; res /= numvar;
    expected = 1.319139e-2;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] / numvar");
    res = ary[3]; res /= numvar;
    expected = 3.04096274e-3
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() / numvar");
    res = FuncRetVal(); res /= numvar;
    expected = .15166628095
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() / numvar");
    res = myDate.getDate(); res /= numvar;
    expected = 7.174265e-3
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar / numvar");
    res = boolvar; res /= numvar;
    expected = 2.314279e-4
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar / strvar");
    res = numvar; res /= strvar;
    expected = 75.80702
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar / strvar");
    res = strvar; res /= strvar;
    expected = 1
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] / strvar");
    res = ary[3]; res /= strvar;
    expected = .230526315789
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() / strvar");
    res = FuncRetVal(); res /= strvar;
    expected = 11.4973684210526
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() / strvar");
    res = myDate.getDate(); res /= strvar;
    expected = .5438597
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar / strvar");
    res = boolvar; res /= strvar;
    expected = 1.754386e-2
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar / ary[3]");
    res = numvar; res /= ary[3];
    expected = 328.843226788432
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar / ary[3]");
    res = strvar; res /= ary[3];
    expected = 4.337899543379
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] / ary[3]");
    res = ary[3]; res /= ary[3];
    expected = 1
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() / ary[3]");
    res = FuncRetVal(); res /= ary[3];
    expected = 49.8744292237443
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() / ary[3]");
    res = myDate.getDate(); res /= ary[3];
    expected = 2.35920852359209
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar / ary[3]");
    res = boolvar; res /= ary[3];
    expected = 0.076103500761035
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar / FuncRetVal()");
    res = numvar; res /= FuncRetVal();
    expected = 6.59342336156252
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar / FuncRetVal()");
    res = strvar; res /= FuncRetVal();
    expected = 8.69764248111696e-2
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] / FuncRetVal()");
    res = ary[3]; res /= FuncRetVal();
    expected = 2.00503547722591e-2
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() / FuncRetVal()");
    res = FuncRetVal(); res /= FuncRetVal();
    expected = 1
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() / FuncRetVal()");
    res = myDate.getDate(); res /= FuncRetVal();
    expected = 4.73029678797589e-2
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar / FuncRetVal()");
    res = boolvar; res /= FuncRetVal();
    expected = 1.52590218955954e-3
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar / myDate.getDate()");
    res = numvar; res /= myDate.getDate();
    expected = 139.3871
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar / myDate.getDate()");
    res = strvar; res /= myDate.getDate();
    expected = 1.83871
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] / myDate.getDate()");
    res = ary[3]; res /= myDate.getDate();
    expected = .423870967741936
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() / myDate.getDate()");
    res = FuncRetVal(); res /= myDate.getDate();
    expected = 21.1403225806452
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() / myDate.getDate()");
    res = myDate.getDate(); res /= myDate.getDate();
    expected = 1
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar / myDate.getDate()");
    res = boolvar; res /= myDate.getDate();
    expected = 3.225806e-2
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar / boolvar");
    res = numvar; res /= boolvar;
    expected = 4321
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar / boolvar");
    res = strvar; res /= boolvar;
    expected = 57;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] / boolvar");
    res = ary[3]; res /= boolvar;
    expected = 13.14;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() / boolvar");
    res = FuncRetVal(); res /= boolvar;
    expected = 655.35;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() / boolvar");
    res = myDate.getDate(); res /= boolvar;
    expected = 31;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar / boolvar");
    res = boolvar; res /= boolvar;
    expected = 1;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar / myObj.retVal");
    res = numvar; res /= myObj.retVal;
    expected = .035038638999035
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar / myObj.retVal");
    res = strvar; res /= myObj.retVal;
    expected = 4.62208383000462e-4
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] / myObj.retVal");
    res = ary[3]; res /= myObj.retVal;
    expected = 1.06551195660207e-4
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() / myObj.retVal");
    res = FuncRetVal(); res /= myObj.retVal;
    expected = 5.31418006665531e-3
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() / myObj.retVal");
    res = myDate.getDate(); res /= myObj.retVal;
    expected = 2.51376489000251e-4
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar / myObj.retVal");
    res = boolvar; res /= myObj.retVal;
    expected = 8.10891900000811e-6
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("emptystrvar /= numvar");
    res = emptystrvar; res /= numvar;
    expected = 0;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apEndTest();
}

function doVerify(a,b) {
  if (a == b) { return 0; }
  var epsilon = a;
  if (b < a) epsilon = a;
  epsilon /= 1e6;
  var result = a - b;
  if (result < 0) result *= -1;

  if (result > epsilon) { return 1; } // failure!
  return 0; // success
}


divi02();


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
