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


var iTestID = 230466;

///////////////////////////////////////////////////////////////////////////////////
//import System



//          this tescase tests the &= compound assignment op using combinations
//          of hard typed variables

// Scenarios:

// 1) untyped &= untyped
// 2) untyped &= boolean
// 3) untyped &= Number
// 4) untyped &= String
// 5) untyped &= Object
// 6) untyped &= Date
// 7) untyped &= Array
// 8) untyped &= function
// 9) untyped &= RegExp
// 10) untyped &= Error
// 11) untyped &= byte
// 12) untyped &= char
// 13) untyped &= short
// 14) untyped &= int
// 15) untyped &= long
// 16) untyped &= float






function asgnand01() {
    var tmp,res,expected,emptyVar;

    var uvar = 4321;
    var boolvar:boolean = true;
    var numvar:Number = 3213.321;
    var strvar:String = "this is a string";
    var obvar:Object = new Object;
    var datevar:Date = new Date(0);
    var arrvar:Array = new Array;
    arrvar[0] = 2.2;
    //arrvar[3] = 3.3;
    //arrvar[4] = 4.4;
    function funvar(){return 4};
    var revar:RegExp = /\d{2}-\d{2}/g;
    var errvar:Error = new Error;
    var bytevar:byte = 50;
    var charvar:char = "g";
    var shrtvar:short = 4321;
    var intvar:int = 1234;
    var longvar:long = 321321321321;
    var flovar:float = 1.25;
    var strTM:String = -2146828275;
    var strNE:String = "no errors...";

    apInitTest("AsgnAnd01");


///////////////////////////////////////////////////////////////////
/////////////////  untyped  ///////////////////////////////////////
///////////////////////////////////////////////////////////////////
    apInitScenario("untyped &= untyped");
    uvar = 1234;
    uvar &= -4321;
    expected = 1042
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }


    apInitScenario("untyped &= boolean");
    uvar = 1234;
    boolvar = true;
    uvar &= boolvar;                        // boolean changes to 1
    expected = int(0);
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }

    uvar = 1235;
    boolvar = true;
    uvar &= boolvar;                        // boolean changes to 1
    expected = int(1);
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }

    apInitScenario("untyped &= Number");
    uvar = 1234;
    numvar = 4321;
    uvar &= numvar;
    expected = Number(192);
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }
    uvar = 1234;
    numvar = 4321.1;
    uvar &= numvar;
    expected = Number(192);
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }


    apInitScenario("untyped &= String");
    uvar = 1234;
    strvar = "4321";
    uvar &= strvar;
    expected = 192
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }


    apInitScenario("untyped &= Object");
    uvar = 1234;
    obvar = 4321;
    uvar &= obvar;
    expected = int(192)
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }


    apInitScenario("untyped &= Date");
    uvar = 1234;
    datevar = new Date(4321);
    uvar &= datevar;
    expected = 192
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }

    
    apInitScenario("untyped &= Array");
    uvar = 1234;
    // arrvar initialized above
    uvar &= arrvar;
    expected = 2
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }


    apInitScenario("untyped &= function");
    uvar = 1234;
    // funvar initialized above
    uvar &= funvar;
    expected =0
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }


    apInitScenario("untyped &= RegExp");
    uvar = 1234;
    revar = /\d{2}-\d{2}/g
    uvar &= revar;
    expected = 0
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }


    apInitScenario("untyped &= Error");
    uvar = 1234;
    errvar = new Error(123);
    uvar &= errvar;
    expected = 0
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }


    apInitScenario("untyped &= byte");
    uvar = 1235;
    bytevar = 1;
    uvar &= bytevar;
    expected = 1
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }


    apInitScenario("untyped &= char");
    uvar = 81;
    charvar = "C";
    uvar &= charvar;
    expected = 65
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }


    apInitScenario("untyped &= short");
    uvar = 255;
    shrtvar = 15;
    uvar &= shrtvar;
    expected = 15
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }


    apInitScenario("untyped &= int");
    uvar = 1234567;
    intvar = 1234567890
    uvar &= intvar;
    expected = 1180290
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }

    uvar = 1234567;
    intvar = -1
    uvar &= intvar;
    expected = 1234567;
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }

    uvar = -1234567;
    intvar = -1
    uvar &= intvar;
    expected = -1234567;
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }

    uvar = -2147483648;
    intvar = -2147483648;
    uvar &= intvar;
    expected = -2147483648;
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }

    uvar = 2147483903;
    intvar = 128;
    uvar &= intvar;
    expected = 128;
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }


    apInitScenario("untyped &= long");
    uvar = 1234;
    longvar = 12344321;
    uvar &= longvar;
    expected = int(1024)
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }
    uvar = 536870913;
    longvar = 1073741825;
    uvar &= longvar;
    expected = 1
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }


    apInitScenario("untyped &= float");
    uvar = 1234;
    flovar = 4321;
    uvar &= flovar;
    expected = int(192);
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }
    uvar = 1234;
    flovar = 4321.9;
    uvar &= flovar;
    expected = 192;
    if (doVerify(expected,uvar)) {
        apLogFailInfo("wrong return value",expected,uvar,"");
    }






    apEndTest();
}


function doVerify(a,b) {
  var delta = 1e-5;  
  if (a === b) { return 0; }

  if (typeof(a) == typeof(b) && a==b) return 0
  if (typeof(a) == "number" && typeof(b) == "number"){
    if (a<b && (a+delta>b)) return 0;
    if (a>b && (a<b+delta)) return 0;
  }
  return 1;
}


asgnand01();


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
