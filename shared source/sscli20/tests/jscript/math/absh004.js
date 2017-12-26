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


var iTestID = 191769;

//////////////////////////////////////////////////////////////////////////////

@if(!@aspx)
	import System
@end


function absh004() {

    var booleanTyped   : Boolean = Boolean(true);
    var booleanObj     : Object  = Boolean(true);

    var sbyteTyped     : SByte   = SByte(-50);
    var sbyteObj       : Object  = SByte(-50);
    var byteTyped      : Byte    = Byte(50);
    var byteObj        : Object  = Byte(50);

    var int16Typed     : Int16   = Int16(-50);
    var int16Obj       : Object  = Int16(-50);
    var uint16Typed    : UInt16  = UInt16(50);
    var uint16Obj      : Object  = UInt16(50);

    var int32Typed     : Int32   = Int32(-50);
    var int32Obj       : Object  = Int32(-50);
    var uint32Typed    : UInt32  = UInt32(50);
    var uint32Obj      : Object  = UInt32(50);

    var int64Typed     : Int64   = Int64(-50);
    var int64Obj       : Object  = Int64(-50);
    var uint64Typed    : UInt64  = UInt64(50);
    var uint64Obj      : Object  = UInt64(50);

    var singleTyped    : Single  = Single(-50);
    var singleObj      : Object  = Single(-50);

    var doubleTyped    : Double  = Double(-50);
    var doubleObj      : Object  = Double(-50);

    var numberTyped    : Number  = Number(-50);
    var numberObj      : Object  = Number(-50);

    var numobjTyped    : Number  = new Number(-50);
    var numobjObj      : Object  = new Number(-50);

    var strTyped       : String  = String("2");
    var strObj         : Object  = String("2");

    var charTyped      : Char    = Char("\u0032");
    var charObj        : Object  = Char("\u0032");


    apInitTest("absh004")
    apInitScenario("Testing for compilation errors")


    try{eval(' System.Math.Abs(booleanObj); throw(0)')}catch(e){if (e.number != -2146827105) apLogFailInfo("booleanObj failed", "", "", "")} 
    if (System.Math.Abs(sbyteTyped) != 50) apLogFailInfo("sbyteTyped failed", "", "", "")
    try{eval(' System.Math.Abs(sbyteObj); throw(0)')}catch(e){if (e.number != -2146827105) apLogFailInfo("sbyteObj failed", "", "", "")} 
    if (System.Math.Abs(byteTyped) != 50) apLogFailInfo("byteTyped failed", "", "", "")
    try{eval(' System.Math.Abs(byteObj); throw(0)')}catch(e){if (e.number != -2146827105) apLogFailInfo("byteObj failed", "", "", "")} 
    if (System.Math.Abs(int16Typed) != 50) apLogFailInfo("int16Typed failed", "", "", "")
    try{eval(' System.Math.Abs(int16Obj); throw(0)')}catch(e){if (e.number != -2146827105) apLogFailInfo("int16Obj failed", "", "", "")} 
    if (System.Math.Abs(uint16Typed) != 50) apLogFailInfo("uint16Typed failed", "", "", "")
    try{eval(' System.Math.Abs(uint16Obj); throw(0)')}catch(e){if (e.number != -2146827105) apLogFailInfo("uint16Obj failed", "", "", "")} 

    if (System.Math.Abs(int32Typed) != 50) apLogFailInfo("int32Typed failed", "", "", "")
    try{eval(' System.Math.Abs(int32Obj); throw(0)')}catch(e){if (e.number != -2146827105) apLogFailInfo("int32Obj failed", "", "", "")} 
    if (System.Math.Abs(uint32Typed) != 50) apLogFailInfo("uint32Typed failed", "", "", "")
    try{eval(' System.Math.Abs(uint32Obj); throw(0)')}catch(e){if (e.number != -2146827105) apLogFailInfo("uint32Obj failed", "", "", "")} 

    if (System.Math.Abs(int64Typed) != 50) apLogFailInfo("int64Typed failed", "", "", "")
    try{eval(' System.Math.Abs(int64Obj); throw(0)')}catch(e){if (e.number != -2146827105) apLogFailInfo("int64Obj failed", "", "", "")} 
    if (System.Math.Abs(uint64Typed) != 50) apLogFailInfo("uint64Typed failed", "", "", "")
    try{eval(' System.Math.Abs(uint64Obj); throw(0)')}catch(e){if (e.number != -2146827105) apLogFailInfo("uint64Obj failed", "", "", "")} 

    if (System.Math.Abs(singleTyped) != 50) apLogFailInfo("singleTyped failed", "", "", "")
    try{eval(' System.Math.Abs(singleObj); throw(0)')}catch(e){if (e.number != -2146827105) apLogFailInfo("singleObj failed", "", "", "")} 
    if (System.Math.Abs(doubleTyped) != 50) apLogFailInfo("doubleTyped failed", "", "", "")
    try{eval(' System.Math.Abs(doubleObj); throw(0)')}catch(e){if (e.number != -2146827105) apLogFailInfo("doubleObj failed", "", "", "")} 

    if (System.Math.Abs(numberTyped) != 50) apLogFailInfo("numberTyped failed", "", "", "")
    try{eval(' System.Math.Abs(numberObj); throw(0)')}catch(e){if (e.number != -2146827105) apLogFailInfo("numberObj failed", "", "", "")} 
    if (System.Math.Abs(numobjTyped) != 50) apLogFailInfo("numobjTyped failed", "", "", "")
    try{eval(' System.Math.Abs(numobjObj); throw(0)')}catch(e){if (e.number != -2146827105) apLogFailInfo("numobjObj failed", "", "", "")} 

    try{eval(' System.Math.Abs(strTyped); throw(0)')}catch(e){if (e.number != -2146827105) apLogFailInfo("strTyped failed", "", "", "")}
    try{eval(' System.Math.Abs(strObj); throw(0)')}catch(e){if (e.number != -2146827105) apLogFailInfo("strObj failed", "", "", "")} 
    if (System.Math.Abs(charTyped) != 50) apLogFailInfo("charTyped failed", "", "", "")
    try{eval(' System.Math.Abs(charObj); throw(0)')}catch(e){if (e.number != -2146827105) apLogFailInfo("charObj failed", "", "", "")} 

    //if (System.Math.Abs() != 50) apLogFailInfo(" failed", "", "", "")



    apEndTest();

}


absh004();


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
