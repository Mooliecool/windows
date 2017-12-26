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


var iTestID = 52917;


function FuncRetVal() { return 655.35; }
function myObjVal() { return 123321; }

@if(!@aspx)
	function myObject() { this.retVal = myObjVal(); }
@else
	expando function myObject() { this.retVal = myObjVal(); }
@end

function mult02() {
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

    apInitTest("mult02");

    apInitScenario("numvar *= numvar");
    res = numvar; res *= numvar;
    expected = 18671041;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar *= numvar");
    res = strvar; res *= numvar;
    expected = 246297;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] *= numvar");
    res = ary[3]; res *= numvar;
    expected = 56777.94;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() *= numvar");
    res = FuncRetVal(); res *= numvar;
    expected = 2831767.35;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() *= numvar");
    res = myDate.getDate(); res *= numvar;
    expected = 133951
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar *= numvar");
    res = boolvar; res *= numvar;
    expected = 4321;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar *= strvar");
    res = numvar; res *= strvar;
    expected = 246297
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar *= strvar");
    res = strvar; res *= strvar;
    expected = 3249
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] *= strvar");
    res = ary[3]; res *= strvar;
    expected = 748.98
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() *= strvar");
    res = FuncRetVal(); res *= strvar;
    expected = 37354.95;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() *= strvar");
    res = myDate.getDate(); res *= strvar;
    expected = 1767
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar *= strvar");
    res = boolvar; res *= strvar;
    expected = 57;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar *= ary[3]");
    res = numvar; res *= ary[3];
    expected = 56777.94
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar *= ary[3]");
    res = strvar; res *= ary[3];
    expected = 748.98
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] *= ary[3]");
    res = ary[3]; res *= ary[3];
    expected = 172.6596
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() *= ary[3]");
    res = FuncRetVal(); res *= ary[3];
    expected = 8611.299
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() *= ary[3]");
    res = myDate.getDate(); res *= ary[3];
    expected = 407.34
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar *= ary[3]");
    res = boolvar; res *= ary[3];
    expected = 13.14
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar *= FuncRetVal()");
    res = numvar; res *= FuncRetVal();
    expected = 2831767.35;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar *= FuncRetVal()");
    res = strvar; res *= FuncRetVal();
    expected = 37354.95
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] *= FuncRetVal()");
    res = ary[3]; res *= FuncRetVal();
    expected = 8611.299
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() *= FuncRetVal()");
    res = FuncRetVal(); res *= FuncRetVal();
    expected = 429483.6225
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() *= FuncRetVal()");
    res = myDate.getDate(); res *= FuncRetVal();
    expected = 20315.85
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar *= FuncRetVal()");
    res = boolvar; res *= FuncRetVal();
    expected = 655.35
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar *= myDate.getDate()");
    res = numvar; res *= myDate.getDate();
    expected = 133951
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar *= myDate.getDate()");
    res = strvar; res *= myDate.getDate();
    expected = 1767
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] *= myDate.getDate()");
    res = ary[3]; res *= myDate.getDate();
    expected = 407.34
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() *= myDate.getDate()");
    res = FuncRetVal(); res *= myDate.getDate();
    expected = 20315.85
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() *= myDate.getDate()");
    res = myDate.getDate(); res *= myDate.getDate();
    expected = 961
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar *= myDate.getDate()");
    res = boolvar; res *= myDate.getDate();
    expected = 31;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar *= boolvar");
    res = numvar; res *= boolvar;
    expected = 4321;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar *= boolvar");
    res = strvar; res *= boolvar;
    expected = 57;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] *= boolvar");
    res = ary[3]; res *= boolvar;
    expected = 13.14;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() *= boolvar");
    res = FuncRetVal(); res *= boolvar;
    expected = 655.35;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() *= boolvar");
    res = myDate.getDate(); res *= boolvar;
    expected = 31;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar *= boolvar");
    res = boolvar; res *= boolvar;
    expected = 1;
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar *= myObj.retVal");
	@cc_on
	@if (@_jscript_version < 7)
    res = numvar; res *= myObj.retVal;
	@else
    res = numvar; res *= 123321;
	@end
    expected = 532870041;
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("strvar *= myObj.retVal");
	@if (@_jscript_version < 7)
    res = strvar; res *= myObj.retVal;
	@else
    res = strvar; res *= 123321;
	@end
    expected = 7029297
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("ary[3] *= myObj.retVal");
	@if (@_jscript_version < 7)
    res = ary[3]; res *= myObj.retVal;
	@else
    res = ary[3]; res *= 123321;
	@end
    expected = 1620437.94
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("FuncRetVal() *= myObj.retVal");
	@if (@_jscript_version < 7)
    res = FuncRetVal(); res *= myObj.retVal;
	@else
    res = FuncRetVal(); res *= 123321;
	@end
    expected = 80818417.35
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("myDate.getDate() *= myObj.retVal");
	@if (@_jscript_version < 7)
    res = myDate.getDate(); res *= myObj.retVal;
	@else
    res = myDate.getDate(); res *= 123321;
	@end
    expected = 3822951
    if (doVerify(expected,res)) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("boolvar *= myObj.retVal");
	@if (@_jscript_version < 7)
    res = boolvar; res *= myObj.retVal;
	@else
    res = boolvar; res *= 123321;
	@end
    expected = 123321
    if (expected != res) {
    	apLogFailInfo("wrong return value",expected,res,"");
    }
    if (typeof(res) != "number") {
    	apLogFailInfo("wrong return type","number",res,"");
    }

    apInitScenario("numvar *= emptystrvar");
    res = numvar; res *= emptystrvar;
    expected = 0;
    if (expected != res) {
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

mult02();


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
