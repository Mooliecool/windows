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


var iTestID = 53687;

// test different types being evaluated

// Added to handle strict mode in JScript 7.0
@cc_on 
   @if (@_fast)  
      var numvar, infvar, numvar0, strvar, emptystrvar, boolvar, nullvar, objvar, funcvar, casestrvar;
    
@end       

function switch01()
{
   // Added to handle strict mode in JScript 7.0
   @cc_on 
      @if (@_fast)  
         var a, objvar2, funcvar2;
       
   @end       


numvar = 4321;
infvar = Number.POSITIVE_INFINITY;
numvar0 = 0;
strvar = "4321";
emptystrvar = "";
casestrvar = "case";
boolvar = true;
nullvar = null;
objvar = new Object();
objvar.test = 4321;
funcvar = new Function("var i = 7;");


apInitTest("switch01 - different types being evaluated");

//------------------------------- number type
apInitScenario("1. Number type");
a = 0;
switch ( numvar ) {
	case "4321": apLogFailInfo("wrong case", numvar, "'4321'", ""); break;
	case null: apLogFailInfo("wrong case", numvar, "null", ""); break;
	case true: apLogFailInfo("wrong case", numvar, "true", ""); break;
	case 432: apLogFailInfo("wrong case", numvar, "432", "");break;
	case 4321: a++; break;
}
if ( a != 1 ) apLogFailInfo( "wrong count of 'a'", 1, a, "");

a = 0;
switch ( infvar ) {
	case "4321": apLogFailInfo("wrong case", infvar, "'4321'", ""); break;
	case null: apLogFailInfo("wrong case", infvar, "null", ""); break;
	case true: apLogFailInfo("wrong case", infvar, "true", ""); break;
	case 432: apLogFailInfo("wrong case", infvar, "432", ""); break;
	case 4321: apLogFailInfo("wrong case", infvar, "4321", ""); break;
	case Number.POSITIVE_INFINITY: a++; break;
}
if ( a != 1 ) apLogFailInfo( "wrong count of 'a'", 1, a, "");

a = 0;
switch ( numvar0 ) {
	case "4321": apLogFailInfo("wrong case", numvar0, "'4321'", ""); break;
	case null: apLogFailInfo("wrong case", numvar0, "null", ""); break;
	case true: apLogFailInfo("wrong case", numvar0, "true", ""); break;
	case 432: apLogFailInfo("wrong case", numvar0, "432", ""); break;
	case 4321: apLogFailInfo("wrong case", numvar0, "4321", ""); break;
	case 0: a++; break;
}
if ( a != 1 ) apLogFailInfo( "wrong count of 'a'", 1, a, "");


//-------------------------------- string type
apInitScenario("2. String type");
a = 0;
switch ( strvar ) {
	case 4321: apLogFailInfo("wrong case", strvar, "4321", ""); break;
	case null: apLogFailInfo("wrong case", strvar, "null", ""); break;
	case true: apLogFailInfo("wrong case", strvar, "true", ""); break;
	case "432": apLogFailInfo("wrong case", strvar, "'432'", ""); break;
	case "4321": a++; break;
}
if ( a != 1 ) apLogFailInfo( "wrong count of 'a'", 1, a, "");

a = 0;
switch ( emptystrvar ) {
	case 4321: apLogFailInfo("wrong case", emptystrvar, "4321", ""); break;
	case null: apLogFailInfo("wrong case", emptystrvar, "null", ""); break;
	case true: apLogFailInfo("wrong case", emptystrvar, "true", ""); break;
	case "432": apLogFailInfo("wrong case", emptystrvar, "'432'", ""); break;
	case "4321": apLogFailInfo("wrong case", emptystrvar, "'4321'", ""); break;
	case "": a++; break;
}
if ( a != 1 ) apLogFailInfo( "wrong count of 'a'", 1, a, "");

a = 0;
switch ( casestrvar ) {
	case 4321: apLogFailInfo("wrong case", casestrvar, "4321", ""); break;
	case null: apLogFailInfo("wrong case", casestrvar, "null", ""); break;
	case true: apLogFailInfo("wrong case", casestrvar, "true", ""); break;
	case "432": apLogFailInfo("wrong case", casestrvar, "'432'", ""); break;
	case "4321": apLogFailInfo("wrong case", casestrvar, "'4321'", ""); break;
	case "case": a++; break;
}
if ( a != 1 ) apLogFailInfo( "wrong count of 'a'", 1, a, "");


//-------------------------------- boolean type
apInitScenario("3. Boolean type");
a = 0;
switch ( boolvar ) {
	case "4321": apLogFailInfo("wrong case", boolvar, "'4321'", ""); break;
	case null: apLogFailInfo("wrong case", boolvar, "null", ""); break;
	case 4321: apLogFailInfo("wrong case", boolvar, "4321", ""); break;
	case false: apLogFailInfo("wrong case", boolvar, "false", ""); break;
	case true: a++; break;
}
if ( a != 1 ) apLogFailInfo( "wrong count of 'a'", 1, a, "");


//-------------------------------- null type
apInitScenario("4. Null type");
a = 0;
switch ( nullvar ) {
	case "4321": apLogFailInfo("wrong case", nullvar, "'4321'", ""); break;
	case 4321: apLogFailInfo("wrong case", nullvar, "4321", ""); break;
	case false: apLogFailInfo("wrong case", nullvar, "false", ""); break;
	case null: a++; break;
}
if ( a != 1 ) apLogFailInfo( "wrong count of 'a'", 1, a, "");


//-------------------------------- Object type
apInitScenario("5. Object type");
a = 0;
objvar2 = objvar;
switch ( objvar ) {
	case "4321": apLogFailInfo("wrong case", objvar, "'4321'", ""); break;
	case null: apLogFailInfo("wrong case", objvar, "null", ""); break;
	case 4321: apLogFailInfo("wrong case", objvar, "4321", ""); break;
	case true: apLogFailInfo("wrong case", objvar, "true", ""); break;
	case objvar2: a++; break;
}
if ( a != 1 ) apLogFailInfo( "wrong count of 'a'", 1, a, "");

a = 0;
switch ( new Object ) {
	case "4321": apLogFailInfo("wrong case", "new Object", "'4321'", ""); break;
	case null: apLogFailInfo("wrong case", "new Object", "null", ""); break;
	case 4321: apLogFailInfo("wrong case", "new Object", "4321", ""); break;
	case true: apLogFailInfo("wrong case", "new Object", "true", ""); break;
	case new Object: a--; break;
	default: a++; break;
}
if ( a != 1 ) apLogFailInfo( "wrong count of 'a'", 1, a, "");

a = 0;
funcvar2 = funcvar;
switch ( funcvar ) {
	case "4321": apLogFailInfo("wrong case", funcvar, "'4321'", ""); break;
	case null: apLogFailInfo("wrong case", funcvar, "null", ""); break;
	case 4321: apLogFailInfo("wrong case", funcvar, "4321", ""); break;
	case true: apLogFailInfo("wrong case", funcvar, "true", ""); break;
	case new Function("var i = 7;"): a--; break;
	case funcvar2: a++; break;
}
if ( a != 1 ) apLogFailInfo( "wrong count of 'a'", 1, a, "");


apEndTest();
}




switch01();


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
